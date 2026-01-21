// projectm_audio_processor.js
class ProjectMAudioWorkletProcessor extends AudioWorkletProcessor {
    constructor(options) {
        super(options);
        this.mainAudioBuffer = null; // Will hold the full AudioBuffer from the WAV
        this.playhead = 0;           // Current sample frame position in this.mainAudioBuffer
        this.looping = true;         // Default to looping
        this.isPlaying = false;
        this.outputChannels = 0;     // Determined from output buffer

        // Buffering for projectM (optimization)
        this.pmBufferSize = 512;     // Target batch size (reduce JS-Wasm calls)
        this.pmBuffer = new Float32Array(this.pmBufferSize);
        this.pmBufferIndex = 0;

        this.port.onmessage = (event) => {
            if (event.data.type === 'loadWavData') {
                this.mainAudioBuffer = event.data.audioBuffer; // This is an AudioBuffer object
                this.playhead = event.data.startPlaying ? 0 : -1; // Reset or keep paused
                this.looping = event.data.loop !== undefined ? event.data.loop : true;
                this.isPlaying = event.data.startPlaying || false;
                // Reset buffering state on new load
                this.pmBufferIndex = 0;
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

        for (let i = 0; i < samplesToProcess; i++) {
            if (this.playhead >= this.mainAudioBuffer.length) { // End of buffer
                if (this.looping) {
                    this.playhead = 0; // Loop
                } else {
                    this.isPlaying = false; // Stop

                    // Flush any remaining data in the PM buffer
                    if (this.pmBufferIndex > 0) {
                         this.port.postMessage({
                            type: 'pcmData',
                            audioData: this.pmBuffer.slice(0, this.pmBufferIndex),
                            samplesPerChannel: this.pmBufferIndex,
                            channelsForPM: 1
                        });
                        this.pmBufferIndex = 0;
                    }

                    // Fill rest of current output block with silence
                    for (let ch = 0; ch < this.outputChannels; ch++) {
                        for (let j = i; j < samplesToProcess; j++) {
                            outputBuffer[ch][j] = 0;
                        }
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

            // Accumulate for ProjectM
            this.pmBuffer[this.pmBufferIndex] = outputBuffer[0][i];
            this.pmBufferIndex++;

            if (this.pmBufferIndex >= this.pmBufferSize) {
                // Buffer full, send to main thread
                this.port.postMessage({
                    type: 'pcmData',
                    audioData: this.pmBuffer, // No slice needed, sending full buffer
                    samplesPerChannel: this.pmBufferSize,
                    channelsForPM: 1
                });
                this.pmBufferIndex = 0;
            }

            this.playhead++;
        }

        return true; // Keep processor alive
    }
}

registerProcessor('projectm-audio-processor', ProjectMAudioWorkletProcessor);
