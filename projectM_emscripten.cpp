#include <emscripten/bind.h>
#include <emscripten/html5.h>
#include <projectM-4/projectM.h>

using namespace emscripten;

EMSCRIPTEN_WEBGL_CONTEXT_HANDLE gl_ctx;
projectm_handle pm;

void _on_preset_switch_failed(const char *preset_filename, const char *message, void *user_data) {
    printf("Preset switch failed (%s): %s\n", preset_filename, message);
}

// C++ name mangling means that “normal” C++ functions cannot be called; the
// function must either be defined in a .c file or be a C++ function defined
// with extern "C".
// https://emscripten.org/docs/api_reference/preamble.js.html#calling-compiled-c-functions-from-javascript
extern "C" {
    void add_audio_data(uint8_t* data, int len) {
        if (!pm) return;
        projectm_pcm_add_uint8(pm, data, len, PROJECTM_MONO);
    }
}

int init() {
    if (pm) return 0;

    // initialize WebGL context attributes
    // https://emscripten.org/docs/api_reference/html5.h.html#c.EmscriptenWebGLContextAttributes
    EmscriptenWebGLContextAttributes webgl_attrs;
    emscripten_webgl_init_context_attributes(&webgl_attrs);
    //webgl_attrs.alpha = false;
    webgl_attrs.majorVersion = 2;
    webgl_attrs.minorVersion = 0;

    gl_ctx = emscripten_webgl_create_context("#my-canvas", &webgl_attrs);

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
    projectm_set_window_size(pm, width, height);
}

// https://emscripten.org/docs/api_reference/bind.h.html#_CPPv419EMSCRIPTEN_BINDINGS4name
// https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html
EMSCRIPTEN_BINDINGS(projectm_bindings) {
    function("destruct", &destruct);
    function("init", &init);
    function("loadPresetFile", &load_preset_file);
    function("renderFrame", &render_frame);
    function("setWindowSize", &set_window_size);
}
