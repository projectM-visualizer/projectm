// #include <omp.h>
#include <unistd.h>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <emscripten/html5.h>
#include <projectM-4/playlist.h>
#include <projectM-4/projectM.h>
#include <emscripten/html5_webgl.h>
#define GL_GLEXT_PROTOTYPES
#define GL_FRAGMENT_PRECISION_HIGH
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLES3/gl31.h>
// #include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>

#define GL_CONTEXT_PROFILE_MASK 0x9126
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#define GL_CONTEXT_CORE_PROFILE_BIT 0x00000001
#define CONTEXT_FLAG_NO_ERROR_BIT_KHR 0x00000008
#define GL_ANISOTROPIC_FILTER 0x3000
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <cstdint>      // For int16_t, int32_t
#include <vector>       // Easier temporary buffer management than raw pointers
#include <limits>       // For INT32_MAX
#include <cmath>        // For std::max (optional, can use INT32_MAX directly)

using namespace emscripten;

typedef struct {
projectm_handle projectm_engine;
projectm_playlist_handle playlist;
EM_BOOL loading;
} AppData;

projectm_handle pm;
AppData app_data;
projectm_playlist_handle playlist={};

void load_preset_callback_example(bool is_hard_cut, unsigned int index,void* user_data) {
// AppData* app_data = (AppData*)user_data;
projectm_playlist_handle playlist = app_data.playlist;
uint32_t indx = projectm_playlist_play_next(playlist, false);
return;
}

void load_preset_callback_done(bool is_hard_cut, unsigned int index,void* user_data) {
// AppData* app_data = (AppData*)user_data;
emscripten_resume_main_loop();
app_data.loading=EM_FALSE;
return;
}

void projectm_pcm_add_float_from_js_array_wrapper(
uintptr_t pm_handle_value,
emscripten::val js_audio_array_val,
unsigned int num_samples_per_channel,
int channels_enum_value) {
// projectm_handle current_pm_handle = reinterpret_cast<projectm_handle>(pm_handle_value);
projectm_handle current_pm_handle = app_data.projectm_engine;
if (!current_pm_handle) {
        fprintf(stderr, "Error: projectM handle is null in from_js_array_wrapper.\n");
        return;
    }
    // Convert the JavaScript array (expected to be Float32Array) to std::vector<float>
    // This performs a copy of the data.
    std::vector<float> cpp_audio_buffer = emscripten::vecFromJSArray<float>(js_audio_array_val);
    // Optional: Sanity checks
    if (channels_enum_value <= 0 || num_samples_per_channel == 0) {
        fprintf(stderr, "Error: Invalid channel count (%d) or samples_per_channel (%u).\n",
                channels_enum_value, num_samples_per_channel);
        return;
    }
        size_t expected_total_elements = static_cast<size_t>(num_samples_per_channel) * static_cast<size_t>(channels_enum_value);
    if (cpp_audio_buffer.size() != expected_total_elements) {
        fprintf(stderr, "Error: Audio data size mismatch. Expected %zu elements, got %zu elements from JS array.\n",
                expected_total_elements, cpp_audio_buffer.size());
        return;
    }
    // Call the original projectM function with data from the C++ vector
    projectm_pcm_add_float(current_pm_handle, cpp_audio_buffer.data(), num_samples_per_channel, static_cast<projectm_channels>(channels_enum_value));

}

EGLDisplay display;
EGLSurface surface;
EGLConfig eglconfig=NULL;
EGLContext ctxegl;
EGLint config_size,major,minor,atb_pos;
EGLint numSamples;
EGLint numSamplesNV;
EGLint numBuffersNV;
EGLint numGreen;
EGLint numRed;
EGLint numBlue;
EGLint numAlpha;
EGLint numDepth;
EGLint numStencil;
EGLint numBuffer;
EGLint numMBuffers;
EGLint colorSpace;
EGLint colorFormat;
EMSCRIPTEN_WEBGL_CONTEXT_HANDLE gl_ctx;
EM_JS(void, js_initialize_audio_system_and_worklet_cpp, (uintptr_t pm_handle_for_addpcm), {
    const engineHandleForPM = pm_handle_for_addpcm;

    if (typeof window.projectMAudioContext_Global_Cpp === 'undefined' || !window.projectMAudioContext_Global_Cpp) {
        try {
            window.projectMAudioContext_Global_Cpp = new (window.AudioContext || window.webkitAudioContext)();
            console.log("JS: Web Audio context created (from Cpp module).");
        } catch (e) {
            console.error("JS: Web Audio API (from Cpp module) not supported or failed to init:", e);
            return;
        }
    }
    const audioContext = window.projectMAudioContext_Global_Cpp;
    if (audioContext.state === 'suspended') {
        audioContext.resume().catch(e => console.warn("JS: AudioContext (from Cpp module) resume might require user gesture.", e));
    }

    async function setupWorkletInternal() {
        try {
            if (window.projectMWorkletNode_Global_Cpp) {
                try { window.projectMWorkletNode_Global_Cpp.port.postMessage({ type: 'stopPlayback' }); } catch(e){}
                try { window.projectMWorkletNode_Global_Cpp.disconnect(); } catch(e){}
                window.projectMWorkletNode_Global_Cpp = null;
            }
            
            // Ensure 'projectm_audio_processor.js' is in the same directory as your main HTML/JS,
            // or provide the correct path.
            await audioContext.audioWorklet.addModule('projectm_audio_processor.js');
            console.log("JS: AudioWorklet 'projectm_audio_processor.js' module added (for Cpp module).");

            window.projectMWorkletNode_Global_Cpp = new AudioWorkletNode(audioContext, 'projectm-audio-processor');
            const workletNode = window.projectMWorkletNode_Global_Cpp;

            workletNode.port.onmessage = (event) => {
                if (event.data.type === 'pcmData') {
                    // 'Module' is the Emscripten instance of THIS C++ module (projectM_emscripten.cpp)
                    if (Module && Module.projectm_pcm_add_float && engineHandleForPM !== 0) {
                        Module.projectm_pcm_add_float( 
                            engineHandleForPM,
                            event.data.audioData, 
                            event.data.samplesPerChannel,
                            event.data.channelsForPM
                        );
                    }
                } else if (event.data.type === 'playbackEnded') {
                    console.log("JS: Worklet reported playback ended.");
                    // You could trigger C++ logic here if needed, e.g. Module.ccall('handle_song_end', null);
                }
            };

            workletNode.connect(audioContext.destination);
            console.log("JS: AudioWorkletNode (for Cpp module) created, connected, listener set up.");
        } catch (err) {
            console.error('JS: Error setting up AudioWorklet (for Cpp module):', err);
        }
    }
    setupWorkletInternal();
});

EM_JS(void, js_load_wav_into_worklet_cpp, (const char* path_in_vfs, bool loop, bool start_playing), {
    const filePath = UTF8ToString(path_in_vfs);
    if (!window.projectMAudioContext_Global_Cpp || !window.projectMWorkletNode_Global_Cpp) {
        console.error("JS: Audio system or worklet not initialized (Cpp module). Call js_initialize_audio_system_and_worklet_cpp first.");
        return;
    }
    if (window.projectMAudioContext_Global_Cpp.state === 'suspended') {
        window.projectMAudioContext_Global_Cpp.resume().then(() => {
             js_load_wav_into_worklet_cpp(path_in_vfs, loop, start_playing); // Try again after resume
        }).catch(e => console.error("JS: Error resuming context for WAV load (Cpp module):", e));
        return;
    }

    if (!FS.analyzePath(filePath).exists) {
         console.error("JS: WAV file for worklet (Cpp module) not found in VFS: " + filePath);
         return;
    }
    try {
        const fileDataUint8Array = FS.readFile(filePath);
        const audioDataArrayBuffer = fileDataUint8Array.buffer.slice(
            fileDataUint8Array.byteOffset, fileDataUint8Array.byteOffset + fileDataUint8Array.byteLength
        );
        console.log("JS: Decoding audio data '" + filePath + "' for worklet (Cpp module).");
        window.projectMAudioContext_Global_Cpp.decodeAudioData(audioDataArrayBuffer)
            .then(function(decodedBuffer) {
                console.log("JS: Audio data decoded. Sending AudioBuffer to worklet (Cpp module).");
                window.projectMWorkletNode_Global_Cpp.port.postMessage({
                    type: 'loadWavData',
                    audioBuffer: decodedBuffer,
                    loop: loop,
                    startPlaying: start_playing
                });
            }).catch(function(err) {
                console.error("JS: Error decoding WAV '" + filePath + "' for worklet (Cpp module):", err);
            });
    } catch (e) {
        console.error("JS: Error reading file from VFS for worklet '" + filePath + " (Cpp module)':", e);
    }
});

EM_JS(void, js_control_worklet_playback_cpp, (bool playCommand) => { // Renamed param for clarity
    if (window.projectMWorkletNode_Global_Cpp) {
        if (playCommand) {
            // To ensure context is active before sending start message
            if (window.projectMAudioContext_Global_Cpp && window.projectMAudioContext_Global_Cpp.state === 'suspended') {
                window.projectMAudioContext_Global_Cpp.resume().then(() => {
                    console.log("JS: Audio context resumed, sending startPlayback to worklet.");
                    window.projectMWorkletNode_Global_Cpp.port.postMessage({ type: 'startPlayback' });
                }).catch(e => console.error("JS: Failed to resume context for playback start.", e));
            } else {
                 window.projectMWorkletNode_Global_Cpp.port.postMessage({ type: 'startPlayback' });
            }
        } else {
            window.projectMWorkletNode_Global_Cpp.port.postMessage({ type: 'stopPlayback' });
        }
    } else {
        console.warn("JS: Worklet node not ready for playback control (Cpp module).");
    }
});

// Your C++ functions to be called by JS (e.g., from UI events) or other C++
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void cpp_initialize_audio_and_load_song(const char* song_path_in_vfs) {
        if (!app_data.projectm_engine) {
            fprintf(stderr, "C++: ProjectM engine not initialized before audio setup!\n");
            return;
        }
        printf("C++: Initializing Web Audio system and loading song: %s\n", song_path_in_vfs);
        js_initialize_audio_system_and_worklet_cpp(reinterpret_cast<uintptr_t>(app_data.projectm_engine));
        js_load_wav_into_worklet_cpp(song_path_in_vfs, true, true); // loop=true, start_playing=true
    }

    EMSCRIPTEN_KEEPALIVE
    void cpp_play_audio() {
        printf("C++: Requesting Web Audio play.\n");
        js_control_worklet_playback_cpp(true);
    }

    EMSCRIPTEN_KEEPALIVE
    void cpp_stop_audio() {
        printf("C++: Requesting Web Audio stop.\n");
        js_control_worklet_playback_cpp(false);
    }

    // Your 'pl' function, now re-purposed for Web Audio via C++
    // This is what your JS EM_ASM calls via Module.ccall('pl')
    EMSCRIPTEN_KEEPALIVE
    void pl() {
        printf("C++: pl() called, initializing and loading default song via Web Audio.\n");
        cpp_initialize_audio_and_load_song("/snd/sample.wav");
    }

} // extern "C"

void renderLoop(){
if(app_data.loading==EM_TRUE){return;}
// glClearColor( 1.0, 1.0, 1.0, 0.0 );
glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
projectm_opengl_render_frame(pm);
eglSwapBuffers(display,surface);
return;
}

void start_render(int size){
glClearColor( 1.0, 1.0, 1.0, 0.0 );
glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
//printf("Setting window size: %i\n", size);
glViewport(0,0,8192,8192);  //  viewport/scissor after UsePrg runs at full resolution
glViewport(0,0,size,size);  //  viewport/scissor after UsePrg runs at full resolution
glEnable(GL_SCISSOR_TEST);
glScissor(0,0,size,size);
glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT,GL_NICEST);
glHint(GL_GENERATE_MIPMAP_HINT,GL_NICEST);
glDisable(GL_DITHER);
glFrontFace(GL_CW);
glCullFace(GL_BACK);
app_data.loading=EM_FALSE;
projectm_set_window_size(pm,size,size);
emscripten_set_main_loop((void (*)())renderLoop,0,0);
emscripten_set_main_loop_timing(2,1);
}

void _on_preset_switch_failed(const char *preset_filename, const char *message, void *user_data) {
printf("Preset switch failed (%s): %s\n", preset_filename, message);
return;
}

// C++ name mangling means that “normal” C++ functions cannot be called; the
// function must either be defined in a .c file or be a C++ function defined
// with extern "C".
// https://emscripten.org/docs/api_reference/preamble.js.html#calling-compiled-c-functions-from-javascript
extern "C" {

void add_audio_data(uint8_t* data, int len) {
projectm_pcm_add_uint8(pm, data, len, PROJECTM_MONO);
return;
}
}

void on_preset_switch_requested(bool is_hard_cut, void* user_data) {
printf("projectM is requesting a preset switch (hard_cut: %s)!\n", is_hard_cut ? "true" : "false");
// EM_ASM({
// const randIndex = Math.floor(Math.random()*25);
// Module.loadPresetFile('/presets/preset_'+randIndex+'.milk');
// });
    emscripten_pause_main_loop();
app_data.loading=EM_TRUE;
char *str = (char*)EM_ASM_PTR({
const randIndex = Math.floor(Math.random()*25);
var jsString = '/presets/preset_'+randIndex+'.milk';
var lengthBytes = lengthBytesUTF8(jsString)+1;
return stringToNewUTF8(jsString);
});
// projectm_load_preset_file(pm, str, false);
AppData* app_datas = (AppData*)user_data;
projectm_playlist_add_preset(app_datas->playlist,str,false);
// projectm_playlist_handle playlist = app_data.playlist;
uint32_t indx = projectm_playlist_play_next(app_data.playlist,true);
return;
}

EM_JS(void,getShader,(int num),{
var pth=document.querySelector('#milkPath').innerHTML;
console.log('Getting preset: '+pth);
const ff=new XMLHttpRequest();
ff.open('GET',pth,true);
ff.responseType='arraybuffer';
document.querySelector('#stat').innerHTML='Downloading Shader';
document.querySelector('#stat').style.backgroundColor='yellow';
ff.addEventListener("load",function(){
let sarrayBuffer=ff.response;
if(sarrayBuffer){
let sfil=new Uint8ClampedArray(sarrayBuffer);
FS.writeFile("/presets/preset_"+num+".milk",sfil);
setTimeout(function(){
   // Module.loadPresetFile("/presets/preset.milk");
document.querySelector('#stat').innerHTML='Downloaded Shader';
document.querySelector('#stat').style.backgroundColor='blue';
},50);
}
});
ff.send(null);
});

int init() {

EM_ASM({
FS.mkdir('/presets');
FS.mkdir('/textures');
FS.mkdir('/snd');
document.querySelector('#scanvas').height=window.innerHeight;
document.querySelector('#scanvas').width=window.innerHeight;
document.querySelector('#mcanvas').height=window.innerHeight;
document.querySelector('#mcanvas').width=window.innerHeight;
var $sngs=[];
var $shds=[];
var $texs=[];

function textures(xml){
const nparser=new DOMParser();
const htmlDocs=nparser.parseFromString(xml.responseText,'text/html');
const preList=htmlDocs.getElementsByTagName('pre')[0].getElementsByTagName('a');
$texs[0]=preList.length;
console.log('scanned: '+$texs[0]+' textures.');
for(var i=5;i<preList.length;i++){
var txxt=preList[i].href;
let currentOrigin = window.location.origin;
let pathName = currentOrigin+'/textures';
let lastSlashIndex = pathName.lastIndexOf('/');
let basePath = pathName.substring(0, lastSlashIndex);
txxt=txxt.replace(currentOrigin,'');
$texs[i]=basePath+'/textures'+txxt;
console.log('$texs['+i+']: ',$texs[i]);
const ff=new XMLHttpRequest();
ff.open('GET',$texs[i],true);
ff.responseType='arraybuffer';
document.querySelector('#stat').innerHTML='Downloading Texture';
document.querySelector('#stat').style.backgroundColor='yellow';
ff.addEventListener("load",function(){
let sarrayBuffer=ff.response;
if(sarrayBuffer){
let sfil=new Uint8ClampedArray(sarrayBuffer);
FS.writeFile("/textures/"+txxt,sfil);
console.log('got texture: '+txxt+' / '+$texs[i]);
setTimeout(function(){
document.querySelector('#stat').innerHTML='Downloaded Texture';
document.querySelector('#stat').style.backgroundColor='blue';
},500);
}
});
ff.send(null);
}};

function scanTextures(){
const nxhttp=new XMLHttpRequest();
nxhttp.onreadystatechange=function(){
if(this.readyState==4&&this.status==200){
console.log('scanning textures.');
textures(this);
}};
nxhttp.open('GET','textures/',true);
nxhttp.send();
}

function shds(xml){
const nparser=new DOMParser();
const htmlDocs=nparser.parseFromString(xml.responseText,'text/html');
const preList=htmlDocs.getElementsByTagName('pre')[0].getElementsByTagName('a');
$shds[0]=preList.length;
for(var i=1;i<preList.length;i++){
var txxt=preList[i].href;
let pathName = 'https://glsl.1ink.us/milk';
let currentOrigin = window.location.origin;
let lastSlashIndex = pathName.lastIndexOf('/');
let basePath = pathName.substring(0, lastSlashIndex);
txxt=txxt.replace(currentOrigin,'');
$shds[i]=basePath+'/milk'+txxt;
}
console.log('Scanned '+$shds[0]+' Shaders.');
setTimeout(function(){
getShaders();
},2500);
};

function scanShaders(){
const nxhttp=new XMLHttpRequest();
nxhttp.onreadystatechange=function(){
if(this.readyState==4&&this.status==200){
shds(this);
}};
nxhttp.open('GET','https://glsl.1ink.us/milk/',true);
nxhttp.send();
}

function sngs(xml){
const nparser=new DOMParser();
const htmlDocs=nparser.parseFromString(xml.responseText,'text/html');
const preList=htmlDocs.getElementsByTagName('pre')[0].getElementsByTagName('a');
$sngs[0]=preList.length;
for(var i=1;i<preList.length;i++){
var txxt=preList[i].href;
let pathName = window.location.pathname;
let currentOrigin = window.location.origin;
let lastSlashIndex = pathName.lastIndexOf('/');
let basePath = pathName.substring(0, lastSlashIndex + 1);
txxt=txxt.replace(currentOrigin,'');
$sngs[i]=basePath+'songs/'+txxt;
// $sngs[i]=currentOrigin+txxt;
}};

function scanSongs(){
const nxhttp=new XMLHttpRequest();
nxhttp.onreadystatechange=function(){
if(this.readyState==4&&this.status==200){
sngs(this);
}};
nxhttp.open('GET','songs/',true);
nxhttp.send();
}

const fll=new BroadcastChannel('file');
fll.addEventListener('message',ea=>{
const fill=new Uint8Array(ea.data.data);
FS.writeFile('/snd/sample.wav',fill);
setTimeout(function(){
Module.ccall('pl', null, []);
},500);
const shutDown=new BroadcastChannel('shutDown');
shutDown.postMessage({data:222});
});

function getShader(pth,fname){
const ff=new XMLHttpRequest();
ff.open('GET',pth,true);
ff.responseType='arraybuffer';
document.querySelector('#stat').innerHTML='Downloading Shader';
document.querySelector('#stat').style.backgroundColor='yellow';
ff.addEventListener("load",function(){
let sarrayBuffer=ff.response;
if(sarrayBuffer){
let sfil=new Uint8ClampedArray(sarrayBuffer);
FS.writeFile(fname,sfil);
console.log('got preset: '+fname);
        // Module.loadPresetFile(fname);
document.querySelector('#stat').innerHTML='Downloaded Shader';
document.querySelector('#stat').style.backgroundColor='blue';
const presetFileNameToLoad = fname;
console.log("JS: Attempting to load pre-downloaded: " + presetFileNameToLoad);
try {
const content = FS.readFile(presetFileNameToLoad, { encoding: 'utf8' });
console.log("JS: Content of " + presetFileNameToLoad + " (first 200 chars):", content.substring(0,200));
if (content.length === 0) {
console.error("JS: File " + presetFileNameToLoad + " is EMPTY!");
}
} catch (e) {
console.error("JS: Failed to read file " + presetFileNameToLoad + " from FS:", e);
    // Don't proceed to Module.loadPresetFile if it can't be read or is empty
return;
}
}
});
ff.send(null);
}

function snd(){
const randSong=Math.floor(($sngs[0]-5)*Math.random());
const songSrc=$sngs[randSong+5];
document.querySelector('#track').src=songSrc;
const sng=new BroadcastChannel('sng');
sng.postMessage({data:songSrc});
};

document.querySelector('#musicBtn').addEventListener('click',function(){
window.open('./flac');
setTimeout(function(){
snd();
},1450);
});

document.querySelector('#milkBtn').addEventListener('click',function(){
for (var i=0;i<25;i++){
var randShd=Math.floor(($shds[0]-5)*Math.random());
var milkSrc=$shds[randShd+5];
document.querySelector('#milkPath').innerHTML=milkSrc;
Module.getShader(i);
   // Module.loadPresetFile('/presets/preset_'+randShd+'.milk');
console.log('Got '+'/presets/preset_'+i+'.milk from '+milkSrc+'.');
}
/*
Module.setWindowSize(window.innerHeight,window.innerHeight);
const randShd=Math.floor(($shds[0]-5)*Math.random());
const milkSrc=$shds[randShd+5];
console.log('Got shader: '+milkSrc);
document.querySelector('#milkPath').innerHTML=milkSrc;
Module.getShader();
// var randShd=Math.floor(25*Math.random());
// Module.loadPresetFile('/presets/preset_'+randShd+'.milk');
// Module.loadPresetFile("/presets/preset.milk");
*/
});

function getShaders(){
for (var i=0;i<25;i++){
var randShd=Math.floor(($shds[0]-5)*Math.random());
var milkSrc=$shds[randShd+5];
document.querySelector('#milkPath').innerHTML=milkSrc;
Module.getShader(i);
// Module.loadPresetFile('/presets/preset_'+randShd+'.milk');
console.log('Got '+'/presets/preset_'+i+'.milk from '+milkSrc+'.');
}
Module.addPath();
setTimeout(function(){
Module.startRender(window.innerHeight);
},500);
}
var pth=document.querySelector('#milkPath').innerHTML;
// Module.getShader();

scanTextures();
scanSongs();
scanShaders();
js_init_web_audio(); 

document.querySelector('#meshSize').addEventListener('change', (event) => {
let meshValue = event.target.value;
// Split the value into two numbers
let values = meshValue.split(',').map(Number);
console.log('Setting Mesh:', values[0], values[1]);
Module.setMesh(values[0], values[1]);
});

//  const meshValue = document.querySelector('#meshSize').value;
   // Split the value into two numbers
// const values = meshValue.split(',').map(Number);
// console.log('Setting Mesh:', values[0], values[1]);
// Module.setMesh(values[0], values[1]);
});


    if (pm) return 0;
    // initialize WebGL context attributes
    // https://emscripten.org/docs/api_reference/html5.h.html#c.EmscriptenWebGLContextAttributes
    EmscriptenWebGLContextAttributes webgl_attrs;
    emscripten_webgl_init_context_attributes(&webgl_attrs);
    webgl_attrs.majorVersion = 2;
    webgl_attrs.minorVersion = 0;
    webgl_attrs.alpha = EM_TRUE;
    webgl_attrs.stencil = EM_TRUE;
    webgl_attrs.depth = EM_TRUE;
    webgl_attrs.antialias = EM_TRUE;
    webgl_attrs.premultipliedAlpha=EM_TRUE;
    webgl_attrs.preserveDrawingBuffer=EM_FALSE;
    webgl_attrs.enableExtensionsByDefault=EM_TRUE;
    webgl_attrs.powerPreference=EM_WEBGL_POWER_PREFERENCE_HIGH_PERFORMANCE;
    display=eglGetDisplay(EGL_DEFAULT_DISPLAY);
    PFNEGLGETCONFIGATTRIBPROC eglGetConfigAttribHI = reinterpret_cast<PFNEGLGETCONFIGATTRIBPROC>(eglGetProcAddress("eglGetConfigAttribHI"));
    eglInitialize(display,&major,&minor);
    eglGetConfigAttrib(display,eglconfig,EGL_SAMPLES,&numSamples);
    eglGetConfigAttrib(display,eglconfig,EGL_COVERAGE_BUFFERS_NV,&numSamplesNV);
    eglGetConfigAttrib(display,eglconfig,EGL_SAMPLE_BUFFERS,&numMBuffers);
    eglGetConfigAttrib(display,eglconfig,EGL_RED_SIZE,&numRed);
    eglGetConfigAttrib(display,eglconfig,EGL_GREEN_SIZE,&numGreen);
    eglGetConfigAttrib(display,eglconfig,EGL_BLUE_SIZE,&numBlue);
    eglGetConfigAttrib(display,eglconfig,EGL_ALPHA_SIZE,&numAlpha);
    eglGetConfigAttrib(display,eglconfig,EGL_DEPTH_SIZE,&numDepth);
    eglGetConfigAttrib(display,eglconfig,EGL_STENCIL_SIZE,&numStencil);
    eglGetConfigAttrib(display,eglconfig,EGL_BUFFER_SIZE,&numBuffer);
    eglGetConfigAttrib(display,eglconfig,EGL_COVERAGE_BUFFERS_NV,&numBuffersNV);
    eglGetConfigAttrib(display,eglconfig,EGL_GL_COLORSPACE,&colorSpace);
    eglGetConfigAttrib(display,eglconfig,EGL_COLOR_FORMAT_HI,&colorFormat);
    static EGLint ctx_att[]={
            EGL_CONTEXT_CLIENT_TYPE,EGL_OPENGL_ES_API,
            EGL_CONTEXT_CLIENT_VERSION,3,
            EGL_CONTEXT_MAJOR_VERSION_KHR,3,
            EGL_CONTEXT_MINOR_VERSION_KHR,0,
// EGL_CONTEXT_FLAGS_KHR,EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR,
            EGL_CONTEXT_PRIORITY_LEVEL_IMG,EGL_CONTEXT_PRIORITY_REALTIME_NV,
// EGL_CONTEXT_PRIORITY_LEVEL_IMG,EGL_CONTEXT_PRIORITY_HIGH_IMG,
            EGL_NONE
    };
    static EGLint att_lst2[]={
            EGL_GL_COLORSPACE_KHR,colorSpace,
            EGL_NONE
    };
    static EGLint att_lst[]={
            EGL_COLOR_COMPONENT_TYPE_EXT,EGL_COLOR_COMPONENT_TYPE_FLOAT_EXT,
// EGL_COLOR_COMPONENT_TYPE_EXT,EGL_COLOR_COMPONENT_TYPE_FIXED_EXT,
// EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR,EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
// EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR,EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR,
            EGL_RENDERABLE_TYPE,EGL_OPENGL_ES3_BIT,
// EGL_RENDERABLE_TYPE,EGL_OPENGL_BIT,  // EGL 1.5 needed  (WASM cannot Window surface)
// EGL_RENDERABLE_TYPE,EGL_NONE,
// EGL_CONFORMANT,EGL_OPENGL_BIT,
// EGL_CONFORMANT,EGL_NONE,
//  EGL_CONFIG_CAVEAT,EGL_NONE,
            EGL_CONTEXT_OPENGL_ROBUST_ACCESS_EXT,EGL_TRUE,
            EGL_CONTEXT_OPENGL_NO_ERROR_KHR,EGL_TRUE,
// EGL_DEPTH_ENCODING_NV,EGL_DEPTH_ENCODING_NONLINEAR_NV,
// EGL_RENDER_BUFFER,EGL_TRIPLE_BUFFER_NV,
            EGL_RENDER_BUFFER,EGL_QUADRUPLE_BUFFER_NV, //   available in OpenGL
// EGL_SURFACE_TYPE,EGL_MULTISAMPLE_RESOLVE_BOX_BIT,
            EGL_SURFACE_TYPE,EGL_SWAP_BEHAVIOR_PRESERVED_BIT|EGL_MULTISAMPLE_RESOLVE_BOX_BIT,
            EGL_MULTISAMPLE_RESOLVE,EGL_MULTISAMPLE_RESOLVE_BOX,
//  EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE,EGL_TRUE, // EGL 1.5 "...the context will only support OpenGL ES 3.0 and later features."
            EGL_COLOR_FORMAT_HI,colorFormat, //  available in OpenGL
// EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY,EGL_NO_RESET_NOTIFICATION,
// EGL_NATIVE_RENDERABLE,EGL_TRUE,
            EGL_COLOR_BUFFER_TYPE,EGL_RGB_BUFFER,
            EGL_LUMINANCE_SIZE,0, // available in OpenGL
            EGL_RED_SIZE,numRed,
            EGL_GREEN_SIZE,numGreen,
            EGL_BLUE_SIZE,numBlue,
            EGL_ALPHA_SIZE,numAlpha,
            EGL_DEPTH_SIZE,numDepth,
            EGL_STENCIL_SIZE,numStencil,
            EGL_BUFFER_SIZE,numBuffer,
            EGL_COVERAGE_BUFFERS_NV,numBuffersNV, // available in GLES 3.1
            EGL_COVERAGE_SAMPLES_NV,numSamplesNV,
            EGL_SAMPLE_BUFFERS,numMBuffers,
            EGL_SAMPLES,numSamples,
            EGL_NONE
    };
    eglChooseConfig(display,att_lst,&eglconfig,1,&config_size);
    ctxegl=eglCreateContext(display,eglconfig,EGL_NO_CONTEXT,ctx_att);
    surface=eglCreateWindowSurface(display,eglconfig,(NativeWindowType)0,att_lst2);
// eglBindAPI(EGL_OPENGL_ES_API);
    eglBindAPI(EGL_OPENGL_API);
    gl_ctx = emscripten_webgl_create_context("#mcanvas", &webgl_attrs);
    if (!gl_ctx) {
        fprintf(stderr, "Failed to create WebGL context\n");
        return 1;
    }
    EMSCRIPTEN_RESULT em_res = emscripten_webgl_make_context_current(gl_ctx);
    eglMakeCurrent(display,surface,surface,ctxegl);
    emscripten_webgl_make_context_current(gl_ctx);
    glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT,GL_NICEST);
    glHint(GL_GENERATE_MIPMAP_HINT,GL_NICEST);
    if (em_res != EMSCRIPTEN_RESULT_SUCCESS) {
        fprintf(stderr, "Failed to activate the WebGL context for rendering\n");
        return 1;
    }
    // These are probably redundant since all GL extensions are enabled by default
    // https://github.com/emscripten-core/emscripten/blob/1b01a9ef2b60184eb70616bbb294cf33d011bbb2/src/settings.js#L481
    // https://emscripten.org/docs/api_reference/html5.h.html#c.EmscriptenWebGLContextAttributes.enableExtensionsByDefault
    //
    // enable floating-point texture support for motion vector grid
    // https://github.com/projectM-visualizer/projectm/blob/master/docs/emscripten.rst#initializing-emscriptens-opengl-context
    // https://emscripten.org/docs/api_reference/html5.h.html#c.emscripten_webgl_enable_extension
    emscripten_webgl_enable_extension(gl_ctx, "OES_texture_float");
    // projectM uses half-float textures for the motion vector grid to store
    // the displacement of the previous frame's warp mesh. WebGL 2.0 sadly
    // doesn't support this texture format by default (while OpenGL ES 3 does)
    // so we have to enable the following WebGL extensions.
    emscripten_webgl_enable_extension(gl_ctx, "OES_texture_half_float");
    emscripten_webgl_enable_extension(gl_ctx, "OES_texture_half_float_linear");
    emscripten_webgl_enable_extension(gl_ctx,"EXT_color_buffer_float"); // GLES float
    emscripten_webgl_enable_extension(gl_ctx,"EXT_float_blend"); // GLES float

pm = projectm_create();

app_data.projectm_engine = pm;

js_initialize_audio_system_and_worklet_cpp(reinterpret_cast<uintptr_t>(pm));

// projectm_playlist_settings playlist_settings;
// memset(&playlist_settings, 0, sizeof(projectm_playlist_settings)); // Initialize to zero
// playlist_settings.load_preset_callback = load_preset_callback_example;
// playlist_settings.user_data_load_preset = &app_data; // Pass our AppData to the callback
    printf("Playlist settings configured.\n");

    // --- 3. Create a Playlist ---

playlist = projectm_playlist_create(pm);

printf("Playlist created successfully.\n");

app_data.playlist = playlist;

const char * loc="/presets/";
projectm_playlist_add_path(playlist,loc,true,true);
// printf("Added /presets/ to playlist successfully.\n");

projectm_playlist_set_preset_switched_event_callback(playlist,&load_preset_callback_done,&app_data);

// projectm_playlist_set_preset_switched_event_callback(projectm_playlist_handle instance,
//                                                          projectm_playlist_preset_switched_event callback,
//                                                          void* user_data)

    if (!pm) {
        fprintf(stderr, "Failed to create projectM handle\n");
        return 1;
    }
    // configure projectM
    const char* texture_search_paths[] = {"textures"};
    projectm_set_texture_search_paths(pm, texture_search_paths, 1);
    projectm_set_fps(pm, 60);
    projectm_set_soft_cut_duration(pm, 17);
    projectm_playlist_set_shuffle(playlist,true);
    projectm_set_preset_switch_failed_event_callback(pm, &_on_preset_switch_failed, nullptr);
    projectm_set_preset_switch_requested_event_callback(pm, &on_preset_switch_requested, &app_data);
// projectm_playlist_connect(app_data.playlist,app_data.projectm_engine);

    printf("projectM initialized\n");
    return 0;
}

void add_preset_path(){
const char * loc="/presets/";    
char preset_file[64]; // Buffer for the generated filename
for (int i = 0; i <= 25; ++i) {
snprintf(preset_file, sizeof(preset_file), "/presets/preset_%d.milk", i);
projectm_playlist_add_preset(app_data.playlist, preset_file, false);
}
// projectm_playlist_add_path(app_data.playlist,loc,true,true);
// projectm_set_preset_switch_requested_event_callback(app_data.projectm_engine, &on_preset_switch_requested, &app_data);
printf("Added /presets/ to playlist successfully.\n");
}

void set_mesh(int w,int h){
projectm_set_mesh_size(pm,w,h);
return;
}

void destruct() {
if (pm) projectm_destroy(pm);
pm = NULL;
if (gl_ctx) emscripten_webgl_destroy_context(gl_ctx);
gl_ctx = NULL;
}

void load_preset_file(std::string filename) {
if (!pm) return;
    // XXX: smooth_transition true does not work
    //projectm_load_preset_file(pm, filename.c_str(), true);
projectm_load_preset_file(pm, filename.c_str(), false);
}

void render_frame() {
if (!pm) return;
projectm_opengl_render_frame(pm);
}

void set_window_size(int width, int height) {
if (!pm) return;
glViewport(0,0,height,height);  //  viewport/scissor after UsePrg runs at full resolution
    // glEnable(GL_SCISSOR_TEST);
    // glScissor(0,0,height,height);
projectm_set_window_size(pm, height, height);
}

EMSCRIPTEN_BINDINGS(projectm_bindings) {
    function("destruct", &destruct);
    function("init", &init);
    function("loadPresetFile", &load_preset_file);
    function("renderFrame", &render_frame);
    function("startRender", &start_render); // Ensure this uses this module's pm
    function("setWindowSize", &set_window_size);
    function("setMesh", &set_mesh);
    function("getShader", &getShader);
    function("addPath", &add_preset_path);

    // The function JS calls to send PCM data TO this C++ module
    function("projectm_pcm_add_float", &projectm_pcm_add_float_from_js_array_wrapper);

    // C++ functions to be called from JS to control audio
    function("initializeAudioAndLoadSong", &cpp_initialize_audio_and_load_song);
    function("playAudio", &cpp_play_audio);
    function("stopAudio", &cpp_stop_audio);
    function("pl", &pl); // For compatibility if your JS already calls this
}

extern "C" {

void pl() {
initialize_audio_and_load_song("/snd/sample.wav"); // Default song
}

}

int main(){
// init();
return 0;
}
