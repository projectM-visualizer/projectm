#pragma once

#include <array>

namespace libprojectM {
namespace Renderer {
namespace Platform {

#ifdef _WIN32

// Windows GL/EGL library name candidates

constexpr std::array<const char*, 3> kNativeEglNames = {
    "libEGL.dll",
    "EGL.dll",
    nullptr};

constexpr std::array<const char*, 2> kNativeGlNames = {
    "opengl32.dll",
    nullptr};

constexpr std::array<const char*, 5> kNativeGlesNames = {
    "libGLESv3.dll",
    "GLESv3.dll",
    "libGLESv2.dll",
    "GLESv2.dll",
    nullptr};

/**
 * EGL names for global module lookup.
 */
static constexpr std::array<const char*, 6> kAllEglModuleNames = {
    "libEGL.dll",
    "EGL.dll",
    "libGLESv2.dll",
    "GLESv2.dll",
    "libGLESv3.dll",
    "GLESv3.dll"};

#elif defined(__APPLE__)

// macOS GL/EGL library name candidates
// macOS native OpenGL uses CGL (OpenGL.framework). ANGLE (and other portability layers).
// commonly provide EGL/GLES dylibs in the application bundle / @rpath.

constexpr std::array<const char*, 6> kNativeEglNames = {
    "@rpath/libEGL.dylib",
    "@rpath/libEGL.1.dylib",
    "libEGL.dylib",
    "libEGL.1.dylib",
    "EGL",
    nullptr};

constexpr std::array<const char*, 2> kNativeGlNames = {
    "/System/Library/Frameworks/OpenGL.framework/OpenGL",
    nullptr};

constexpr std::array<const char*, 7> kNativeGlesNames = {
    "@rpath/libGLESv3.dylib",
    "@rpath/libGLESv2.dylib",
    "@rpath/libGLESv2_with_capture.dylib",
    "libGLESv3.dylib",
    "libGLESv2.dylib",
    "libGLESv2_with_capture.dylib",
    nullptr};

#elif defined(__ANDROID__)

// Android EGL + GLES (no desktop libGL / GLX) library name candidates

#ifndef USE_GLES
#error "Android builds require USE_GLES to be defined. Desktop GL is not supported on Android."
#endif

constexpr std::array<const char*, 2> kNativeEglNames = {
    "libEGL.so",
    nullptr};

constexpr std::array<const char*, 3> kNativeGlesNames = {
    "libGLESv3.so",
    "libGLESv2.so",
    nullptr};

#elif !defined(__EMSCRIPTEN__)

// Unix GL/EGL library name candidates

constexpr std::array<const char*, 3> kNativeEglNames = {
    "libEGL.so.1",
    "libEGL.so",
    nullptr};

/**
 * Linux / GLES:
 * Prefer libGLESv3/libGLESv2 sonames. Core GLES entry points are expected
 * to be available as library exports. eglGetProcAddress is not guaranteed
 * to return core symbols unless EGL_KHR_get_all_proc_addresses (or its
 * client variant) is advertised.
 */
constexpr std::array<const char*, 6> kNativeGlesNames = {
    "libGLESv3.so.3",
    "libGLESv3.so",
    "libGLESv2.so.2",
    "libGLESv2.so.1",
    "libGLESv2.so",
    nullptr};

/**
 * Linux / GLVND note:
 * Many modern distributions use GLVND, which splits OpenGL entry points (libOpenGL) from
 * window-system glue such as GLX (libGLX). Prefer GLVND-facing libs first, but keep legacy
 * libGL names in the candidate list for compatibility with older or non-GLVND stacks.
 */
constexpr std::array<const char*, 6> kNativeGlNames = {
    "libOpenGL.so.1", // GLVND OpenGL dispatcher (core gl* entry points)
    "libOpenGL.so.0", // older GLVND soname
    "libGL.so.1",     // legacy/compat umbrella (often provided by GLVND)
    "libGL.so.0",     // sometimes shipped as .so.0
    "libGL.so",
    nullptr};

constexpr std::array<const char*, 3> kNativeGlxNames = {
    "libGLX.so.1", // GLVND GLX dispatcher (glXGetProcAddress*)
    "libGLX.so.0", // older GLVND soname
    nullptr};

#endif

} // namespace Platform
} // namespace Renderer
} // namespace libprojectM
