#include <unistd.h>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/html5.h>
#include <projectM-4/projectM.h>

#include <GLES3/gl3.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <cstdint>      // For int16_t, int32_t
#include <vector>       // Easier temporary buffer management than raw pointers
#include <limits>       // For INT32_MAX
#include <cmath>        // For std::max (optional, can use INT32_MAX directly)

using namespace emscripten;
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
}}

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
projectm_pcm_add_float(pm,
reinterpret_cast<const float*>(wptr_wav_chunk_start),
samples_per_channel_in_chunk,
(projectm_channels)channels);
break;
case AUDIO_S16LSB:
case AUDIO_S16MSB:
                     // case AUDIO_S16:
projectm_pcm_add_int16(pm,
reinterpret_cast<const int16_t*>(wptr_wav_chunk_start),
samples_per_channel_in_chunk,
(projectm_channels)channels);
break;
case AUDIO_U8:
                         // For uint8, projectM expects bytes per channel if mono, total samples if stereo?
                         // Let's assume the uint8 function also takes samples per channel count like others
projectm_pcm_add_uint8(pm,
wptr_wav_chunk_start, // Already Uint8*
samples_per_channel_in_chunk,
(projectm_channels)channels);
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
projectm_pcm_add_float(pm,
float_buffer.data(), // Get raw pointer from vector
samples_per_channel_in_chunk,
(projectm_channels)channels);
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
wave.pos=0;
wave.spec.callback=bfr;
opn_aud();
return;
}

void renderLoop(){
projectm_opengl_render_frame(pm);
return;
}

void start_render(int size){
EM_ASM({
document.querySelector('#scanvas).height=size;
document.querySelector('#scanvas).width=size;
});
glClearColor(0.0, 0.0, 0.0, 1.0);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    printf("Setting window size: %i\n", size);
glViewport(0,0,size,size);  //  viewport/scissor after UsePrg runs at full resolution
glEnable(GL_SCISSOR_TEST);
glScissor(0,0,size,size);
glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT,GL_NICEST);
glHint(GL_GENERATE_MIPMAP_HINT,GL_NICEST);
glDisable(GL_DITHER);
glFrontFace(GL_CW);
glCullFace(GL_BACK);
projectm_set_window_size(pm,size,size);
emscripten_set_main_loop_timing(2,1);
emscripten_set_main_loop((void (*)())renderLoop,0,0);
}

void _on_preset_switch_failed(const char *preset_filename, const char *message, void *user_data) {
printf("Preset switch failed (%s): %s\n", preset_filename, message);
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
// if (!pm) return;
projectm_pcm_add_uint8(pm, data, len, PROJECTM_MONO);
// }
}

}

int init() {

EM_ASM({
FS.mkdir('/presets');
FS.mkdir('/textures');
FS.mkdir('/snd');

var $sngs=[];
var $shds=[];

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
}};

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
    console.log('got preset: '+pth);
Module.loadPresetFile("/presets/preset.milk"); 
document.querySelector('#stat').innerHTML='Downloaded Shader';
document.querySelector('#stat').style.backgroundColor='blue';
}
});
ff.send(null);
}

var pth=document.querySelector('#milkPath').innerHTML;
getShader(pth,'/presets/preset.milk');

scanSongs();
scanShaders();

function snd(){
const randSong=Math.floor(($sngs[0]-5)*Math.random());
const songSrc=$sngs[randSong+5];
document.querySelector('#track').src=songSrc;
const sng=new BroadcastChannel('sng');
sng.postMessage({data:songSrc});
Module.startRender(window.innerHeight);
};

document.querySelector('#musicBtn').addEventListener('click',function(){
window.open('./flac');
setTimeout(function(){
snd();
},1450);
});

document.querySelector('#milkBtn').addEventListener('click',function(){
Module.setWindowSize(window.innerHeight,window.innerHeight);
const randShd=Math.floor(($shds[0]-5)*Math.random());
const milkSrc=$shds[randShd+5];
console.log('Got shader: '+milkSrc);
document.querySelector('#milkPath').innerHTML=milkSrc;
getShader(milkSrc,'/presets/preset.milk');
});

document.querySelector('#meshSize').addEventListener('change', (event) => {
let selectedValue = event.target.value;
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
    //webgl_attrs.alpha = false;
    webgl_attrs.majorVersion = 2;
    webgl_attrs.minorVersion = 0;
    webgl_attrs.alpha = true;
    webgl_attrs.stencil = true;
    webgl_attrs.depth = true;
    webgl_attrs.antialias = true;
    webgl_attrs.premultipliedAlpha = false;

    gl_ctx = emscripten_webgl_create_context("#scanvas", &webgl_attrs);

    if (!gl_ctx) {
        fprintf(stderr, "Failed to create WebGL context\n");
        return 1;
    }

    EMSCRIPTEN_RESULT em_res = emscripten_webgl_make_context_current(gl_ctx);

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

    pm = projectm_create();

    if (!pm) {
        fprintf(stderr, "Failed to create projectM handle\n");
        return 1;
    }

    // configure projectM

    const char* texture_search_paths[] = {"textures"};
    projectm_set_texture_search_paths(pm, texture_search_paths, 1);

    projectm_set_fps(pm, 60);
    projectm_set_soft_cut_duration(pm, 5);
    projectm_set_preset_switch_failed_event_callback(pm, &_on_preset_switch_failed, nullptr);

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
}

int main(){
init();
// sleep(4);
// load_preset_file("/presets/preset.milk"); 
return 1;
}
