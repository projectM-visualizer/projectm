/**
 * @file MilkdropShaderPreprocess.hpp
 * @brief GL-free, pure-text assembly of the input passed to the HLSL preprocessor.
 *
 * Extracted from MilkdropShader::PreprocessPresetShader so the exact string handed to
 * HLSLParser::ApplyPreprocessor can be produced without a MilkdropShader instance, a GL
 * context, textures, or PresetState. This is the shared foundation for a runtime
 * shader-transpile cache and a build-time (host) precompute tool: both call this function
 * and must get byte-identical output.
 */
#pragma once

#include "MilkdropShader.hpp"

#include <string>

namespace libprojectM {
namespace MilkdropPreset {

/**
 * @brief Assembles the pure-text shader source that is later passed to the HLSL
 *        preprocessor (HLSLParser::ApplyPreprocessor).
 *
 * This is a pure function of (shader type, preset shader body): it renames the preset's
 * shader entry point to PS(), strips unsupported sampler_state{} blocks, injects the
 * per-type variable declarations, trims trailing text after the shader's main function,
 * and prepends the static preset shader header plus the type-specific #defines. It does
 * NOT touch GL, Renderer::Shader, textures, or PresetState.
 *
 * The output is byte-for-byte identical to what MilkdropShader::PreprocessPresetShader
 * historically produced in-place.
 *
 * @param type The preset shader type (warp or composite).
 * @param presetShaderBody The raw preset shader body (as loaded from the preset).
 * @return The fully assembled shader source ready for the HLSL preprocessor.
 * @throws Renderer::ShaderException If the body is empty or malformed (missing
 *         "shader_body", opening brace, or closing brace) — same messages/type as before.
 */
std::string AssembleApplyPreprocessorInput(MilkdropShader::ShaderType type,
                                           const std::string& presetShaderBody);

/**
 * @brief Computes a version-salted, collision-safe cache key for a preprocessed-HLSL
 *        lookup, from the exact bytes handed to HLSLParser::ApplyPreprocessor.
 *
 * GL-free and side-effect-free, so the runtime transpile path and a future build-time
 * precompute tool compute byte-identical keys for identical inputs.
 *
 * Collision safety: the key embeds a compile-time salt (bumped whenever the static shader
 * header, the hlslparser, or the assembly transform changes — invalidating stale entries),
 * the input length, and two independent 64-bit FNV-1a hashes seeded with different offset
 * bases. That is ~128 bits of effective hash plus an exact length check, so accidental
 * collisions are astronomically unlikely (and a length mismatch alone rules most out).
 * The value stored under a key is deterministic text, so even in the impossible event of a
 * full collision the only effect would be reusing a valid-but-wrong transpile — never a
 * crash. Callers may treat a hit as authoritative.
 *
 * @param applyPreprocessorInput The exact string that would be passed to ApplyPreprocessor
 *        (i.e. the return value of AssembleApplyPreprocessorInput).
 * @return A printable ASCII cache key.
 */
std::string ComputePreprocessCacheKey(const std::string& applyPreprocessorInput);

/**
 * @brief Returns the compile-time version salt embedded in every cache key.
 *
 * Exposed so a build-time precompute tool can stamp the exact same salt into its resource
 * header without duplicating the constant (the tool and ComputePreprocessCacheKey must not
 * drift). Bumping the salt in the .cpp invalidates every stale precomputed/runtime entry.
 */
std::string PreprocessCacheSalt();

} // namespace MilkdropPreset
} // namespace libprojectM
