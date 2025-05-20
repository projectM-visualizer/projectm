#include <unistd.h>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/html5.h>
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
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <cstdint>      // For int16_t, int32_t
#include <vector>       // Easier temporary buffer management than raw pointers
#include <limits>       // For INT32_MAX
#include <cmath>        // For std::max (optional, can use INT32_MAX directly)

using namespace emscripten;

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
projectm_handle pm;
SDL_AudioDeviceID dev;
struct{Uint8* snd;int pos;Uint32 slen;SDL_AudioSpec spec;}wave;

void cls_aud(){
if(dev!=0){
SDL_PauseAudioDevice(dev,SDL_TRUE);
SDL_CloseAudioDevice(dev);
dev=0;
return;
}
}

void qu(int rc){
SDL_Quit();
return;
}

void opn_aud(){
dev=SDL_OpenAudioDevice(NULL,SDL_FALSE,&wave.spec,NULL,0);
if(!dev){
SDL_FreeWAV(wave.snd);
}
SDL_PauseAudioDevice(dev,SDL_FALSE);
return;
}

void SDLCALL bfr(void* userdata, Uint8* stm, int len) {
    // --- Start with safety checks and handling for when pm is null ---
if (!pm) {
        // Simplified SDL playback logic when pm is not ready
if (!wave.snd || wave.slen == 0) {
SDL_memset(stm, 0, len); // Silence for SDL
} else {
            // Copy wave data for SDL playback only (loop to handle wrap-around)
int bytes_to_copy;
while (len > 0) {
int remaining_in_wav = wave.slen - wave.pos;
bytes_to_copy = std::min(len, remaining_in_wav); // Copy whichever is smaller
if (bytes_to_copy > 0) {
SDL_memcpy(stm, wave.snd + wave.pos, bytes_to_copy);
stm += bytes_to_copy;
wave.pos += bytes_to_copy;
len -= bytes_to_copy;
}
                // Check for wrap-around *after* potential copy
if (wave.pos >= wave.slen) {
wave.pos = 0; // Wrap around
if (len == 0) break; // Avoid infinite loop if len was exactly remaining_in_wav
}
}
}
return; // Exit early if pm is not valid
}
    // --- PM is valid, process audio for SDL and projectM ---
if (!wave.snd || wave.slen == 0) {
SDL_memset(stm, 0, len); // Silence for SDL
        // Optionally feed silence to projectM
return;
}
    // Determine properties based on the loaded wave spec
int bytes_per_sample = SDL_AUDIO_BITSIZE(wave.spec.format) / 8;
int channels = wave.spec.channels;
if (bytes_per_sample == 0 || channels == 0) {
fprintf(stderr, "Error in bfr: Invalid audio format (bytes_per_sample=%d, channels=%d)\n", bytes_per_sample, channels);
SDL_memset(stm, 0, len); // Fill SDL buffer with silence on error
return;
}
    // int bytes_per_frame = bytes_per_sample * channels; // Bytes for one sample across all channels
Uint8* wptr_current_sdl_chunk_start = stm; // Keep track of where current SDL chunk starts
    // Process audio in chunks, handling wrap-around for wave.snd buffer
while (len > 0) {
int remaining_in_wav = wave.slen - wave.pos; // Bytes remaining in wave buffer from current pos
int current_chunk_len = std::min(len, remaining_in_wav); // Bytes to process in this iteration
if (current_chunk_len <= 0) { // Should only happen if len is 0 or wave.pos is wrong
if (wave.pos >= wave.slen) { // Handle wrap immediately if needed
wave.pos = 0;
continue; // Re-evaluate remaining_in_wav and current_chunk_len
} else {
fprintf(stderr, "Error in bfr: current_chunk_len is zero or negative (%d). Breaking loop.\n", current_chunk_len);
break; // Avoid potential infinite loop
}
}
        // Pointer to the start of the current audio data chunk in wave.snd
Uint8* wptr_wav_chunk_start = wave.snd + wave.pos;
        // 1. Copy chunk to SDL stream buffer for playback
SDL_memcpy(wptr_current_sdl_chunk_start, wptr_wav_chunk_start, current_chunk_len);
        // 2. Feed the *SAME CHUNK* data (from wptr_wav_chunk_start) to projectM
if (pm && current_chunk_len > 0) {
unsigned int total_samples_in_chunk = current_chunk_len / bytes_per_sample;
unsigned int samples_per_channel_in_chunk = total_samples_in_chunk / channels;
if (samples_per_channel_in_chunk > 0) {
                // Choose projectM function based on format
switch (wave.spec.format) {
case AUDIO_F32LSB: // Handle potential endianness if needed, SDL might abstract this
case AUDIO_F32MSB:
                        // case AUDIO_F32:    // Catch-all for float
projectm_pcm_add_float(pm,reinterpret_cast<const float*>(wptr_wav_chunk_start),samples_per_channel_in_chunk,(projectm_channels)channels);
break;
case AUDIO_S16LSB:
case AUDIO_S16MSB:
// case AUDIO_S16:
projectm_pcm_add_int16(pm,reinterpret_cast<const int16_t*>(wptr_wav_chunk_start),samples_per_channel_in_chunk,(projectm_channels)channels);
break;
case AUDIO_U8:
                        // For uint8, projectM expects bytes per channel if mono, total samples if stereo?
                        // Let's assume the uint8 function also takes samples per channel count like others
projectm_pcm_add_uint8(pm,wptr_wav_chunk_start,samples_per_channel_in_chunk,(projectm_channels)channels);
break;
case AUDIO_S32LSB:
case AUDIO_S32MSB:
                        // case AUDIO_S32:
{ // Create a scope for temporary buffer
                        // Convert S32 samples to Float samples [-1.0, 1.0]
const int32_t* s32_samples = reinterpret_cast<const int32_t*>(wptr_wav_chunk_start);
                        // Use std::vector for RAII (automatic memory management)
std::vector<float> float_buffer(total_samples_in_chunk);
                        // Conversion constant (using INT32_MAX from <limits>)
const float max_s32_val = static_cast<float>(std::numeric_limits<int32_t>::max()); // 2147483647.0f
for (unsigned int i = 0; i < total_samples_in_chunk; ++i) {
float_buffer[i] = static_cast<float>(s32_samples[i]) / max_s32_val;
                            // Optional clamping in case of rounding errors, though division should handle it
                            // float_buffer[i] = std::max(-1.0f, std::min(1.0f, float_buffer[i]));
}
                        // Feed the converted float buffer to projectM
projectm_pcm_add_float(pm,float_buffer.data(),samples_per_channel_in_chunk,(projectm_channels)channels);
                        // vector float_buffer goes out of scope and automatically cleans up memory
}
break;
default:
                        // Use the format code directly in the warning
fprintf(stderr, "Warning in bfr: Unhandled audio format %d. Audio not sent to projectM.\n", wave.spec.format);
break;
}
}
}
        // 3. Advance pointers and remaining length for next iteration
wave.pos += current_chunk_len;
wptr_current_sdl_chunk_start += current_chunk_len; // Advance SDL buffer pointer
len -= current_chunk_len;
        // Check for wrap-around *after* processing the chunk
if (wave.pos >= wave.slen) {
wave.pos = 0; // Wrap around
}
} // End while(len > 0)
}


void plt(){
char flnm[24];
SDL_FreeWAV(wave.snd);
SDL_SetMainReady();
if (SDL_Init(SDL_INIT_AUDIO)<0){
qu(1);
}
SDL_strlcpy(flnm,"/snd/sample.wav",sizeof(flnm));
if(SDL_LoadWAV(flnm,&wave.spec,&wave.snd,&wave.slen)==NULL){
qu(1);
}
wave.spec.samples = 8192; // Try 2048 or 4096
wave.pos=0;
wave.spec.callback=bfr;
opn_aud();
return;
}

void renderLoop(){
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

void pl(){
plt();
return;
}

void add_audio_data(uint8_t* data, int len) {
projectm_pcm_add_uint8(pm, data, len, PROJECTM_MONO);
return;
}
}

void on_preset_switch_requested(bool is_hard_cut, void* user_data) {
printf("projectM is requesting a preset switch (hard_cut: %s)!\n", is_hard_cut ? "true" : "false");
EM_ASM({
const randIndex = Math.floor(Math.random()*25);
Module.loadPresetFile('/presets/preset_'+randIndex+'.milk');
});
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

            function pll(){
Module.ccall('pl');
            }

            const fll=new BroadcastChannel('file');
            fll.addEventListener('message',ea=>{
const fill=new Uint8Array(ea.data.data);
FS.writeFile('/snd/sample.wav',fill);
setTimeout(function(){pll();},500);
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
setTimeout(function(){
    Module.startRender(window.innerHeight);
},500);
            }

            var pth=document.querySelector('#milkPath').innerHTML;
// Module.getShader();

            scanTextures();
            scanSongs();
            scanShaders();

 
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

    if (!pm) {
        fprintf(stderr, "Failed to create projectM handle\n");
        return 1;
    }

    // configure projectM
    const char* texture_search_paths[] = {"textures"};
    projectm_set_texture_search_paths(pm, texture_search_paths, 1);
    projectm_set_fps(pm, 60);
    projectm_set_soft_cut_duration(pm, 17);
    projectm_set_preset_switch_failed_event_callback(pm, &_on_preset_switch_failed, nullptr);

    projectm_set_preset_switch_requested_event_callback(pm, &on_preset_switch_requested, nullptr);

    printf("projectM initialized\n");
    return 0;
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

// https://emscripten.org/docs/api_reference/bind.h.html#_CPPv419EMSCRIPTEN_BINDINGS4name
// https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html
EMSCRIPTEN_BINDINGS(projectm_bindings) {
        function("destruct", &destruct);
        function("init", &init);
        function("loadPresetFile", &load_preset_file);
        function("renderFrame", &render_frame);
        function("startRender", &start_render);
        function("setWindowSize", &set_window_size);
        function("setMesh", &set_mesh);
        function("getShader", &getShader);
}

int main(){
    init();
    return 1;
}
