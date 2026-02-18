#ifdef __EMSCRIPTEN__

#include "DynamicLibrary.hpp"

#include <string>

namespace libprojectM {
namespace Renderer {
namespace Platform {

auto DynamicLibrary::Open(const char* const* /*names*/, std::string& /*reason*/) -> bool
{
    return false;
}

void DynamicLibrary::Close()
{
}

auto DynamicLibrary::GetSymbol(const char* /*name*/) const -> void*
{
    return nullptr;
}

auto DynamicLibrary::FindGlobalSymbol(const char* /*name*/) -> void*
{
    return nullptr;
}

} // namespace Platform
} // namespace Renderer
} // namespace libprojectM

#endif // __EMSCRIPTEN__
