/*
 * Builds the stb_image implementation from an external stb_image.h, e.g. from vcpkg's stb port.
 * Only used if ENABLE_SYSTEM_STB_IMAGE is set, otherwise stb_image.c with the vendored copy is built.
 */
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
