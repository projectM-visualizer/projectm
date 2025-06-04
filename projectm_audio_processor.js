// projectm_audio_processor.js
class ProjectMAudioWorkletProcessor extends AudioWorkletProcessor {
    constructor(options) {
        super(options);
        this.mainAudioBuffer = null; // Will hold the full AudioBuffer from the WAV
        this.playhead = 0;           // Current sample frame position in this.mainAudioBuffer
        this.looping = true;         // Default to looping
        this.isPlaying = false;
        this.outputChannels = 0;     // Determined from output buffer

        this.port.onmessage = (event) => {
            if (event.data.type === 'loadWavData') {
                this.mainAudioBuffer = event.data.audioBuffer; // This is an AudioBuffer object
                this.playhead = event.data.startPlaying ? 0 : -1; // Reset or keep paused
                this.looping = event.data.loop !== undefined ? event.data.loop : true;
                this.isPlaying = event.data.startPlaying || false;
                if (this.isPlaying) {
                    console.log('[Worklet] AudioBuffer received, playback started/resumed.');
                } else {
                    console.log('[Worklet] AudioBuffer received, playback paused.');
                }
            } else if (event.data.type === 'stopPlayback') {
                this.isPlaying = false;
                 console.log('[Worklet] Playback stopped by main thread.');
            } else if (event.data.type === 'startPlayback') {
                if (this.mainAudioBuffer) { // Only start if buffer is loaded
                    this.isPlaying = true;
                    this.playhead = event.data.playheadPosition || 0; // Allow starting from specific point
                    console.log('[Worklet] Playback (re)started by main thread.');
                } else {
                    console.log('[Worklet] Cannot start playback, no audio buffer loaded.');
                }
            } else if (event.data.type === 'setLoop') {
                this.looping = event.data.loop;
                 console.log('[Worklet] Loop set to: ' + this.looping);
            }
        };
    }

    process(inputs, outputs, parameters) {
        if (!this.isPlaying || !this.mainAudioBuffer) {
            // Output silence if not playing or no buffer
            for (const outputChannel of outputs[0]) {
                outputChannel.fill(0);
            }
            return true; // Keep processor alive
        }

        const outputBuffer = outputs[0]; // Assuming first output
        this.outputChannels = outputBuffer.length; // Number of output channels (e.g., 2 for stereo)
        const samplesToProcess = outputBuffer[0].length; // e.g., 128 samples (block size)

        // Prepare a buffer to send to ProjectM (e.g., first channel, or mixdown)
        // We send a copy of what's being played in this block.
        let pcmDataForProjectM = new Float32Array(samplesToProcess);

        for (let i = 0; i < samplesToProcess; i++) {
            if (this.playhead >= this.mainAudioBuffer.length) { // End of buffer
                if (this.looping) {
                    this.playhead = 0; // Loop
                } else {
                    this.isPlaying = false; // Stop
                    // Fill rest of current output block with silence
                    for (let ch = 0; ch < this.outputChannels; ch++) {
                        for (let j = i; j < samplesToProcess; j++) {
                            outputBuffer[ch][j] = 0;
                        }
                    }
                    // Send any partially filled pcmDataForProjectM before breaking
                    if (i > 0) {
                         this.port.postMessage({
                            type: 'pcmData',
                            audioData: pcmDataForProjectM.slice(0, i), // Send only filled part
                            samplesPerChannel: i, // Number of valid samples in this partial block
                            channelsForPM: 1 // Assuming we send one channel to PM
                        });
                    }
                    return true; // End processing for this block
                }
            }

            // Playback: copy from mainAudioBuffer to outputBuffer
            for (let ch = 0; ch < this.outputChannels; ch++) {
                // Use source channel if available, else duplicate channel 0 (mono to stereo)
                const sourceChannelData = this.mainAudioBuffer.numberOfChannels > ch
                                        ? this.mainAudioBuffer.getChannelData(ch)
                                        : this.mainAudioBuffer.getChannelData(0);
                outputBuffer[ch][i] = sourceChannelData[this.playhead];
            }

            // For addPCM: copy the first channel's data (or mixdown if you prefer)
            pcmDataForProjectM[i] = outputBuffer[0][i];
            this.playhead++;
        }

        // Post the fully processed block of PCM data to the main thread
        this.port.postMessage({
            type: 'pcmData',
            audioData: pcmDataForProjectM, // Float32Array (already a copy of outputL for this block)
            samplesPerChannel: samplesToProcess,
            channelsForPM: 1 // Example: sending mono (left channel) to ProjectM
        });

        return true; // Keep processor alive
    }
}

registerProcessor('projectm-audio-processor', ProjectMAudioWorkletProcessor);
