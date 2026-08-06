// msvc_stdext_shim.h
//
// Visual Studio 2026 (MSVC toolset 14.5x, _MSC_VER >= 1950) REMOVED
// stdext::make_checked_array_iterator / stdext::make_unchecked_array_iterator
// from <iterator> (see microsoft/STL PR #5817).
//
// Qt 5.15.2 still calls them from QtCore/qlist.h (and friends) via the
// QT_MAKE_CHECKED_ARRAY_ITERATOR / QT_MAKE_UNCHECKED_ARRAY_ITERATOR macros,
// which on MSVC expand to stdext::make_..._array_iterator unconditionally with
// no way to disable them. As a result every translation unit that instantiates a
// QList fails to compile:
//     error C2653: 'stdext': is not a class or namespace name
//     error C3861: 'make_checked_array_iterator': identifier not found
//
// GitHub's hosted windows-latest and windows-2025 images both rolled to VS2026
// in mid-2026, and no VS2022 image remains, so this cannot be fixed at the
// runner level and _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING no longer helps
// (the symbols are gone, not merely deprecated). Qt itself fixed this in 5.15.17,
// but the open-source aqt archive only offers 5.15.2.
//
// This header is force-included (/FI) before every Qt header on MSVC and
// provides a minimal stand-in that returns the raw pointer, byte-identical in
// behaviour to the QT_MAKE_CHECKED_ARRAY_ITERATOR(x, N) => (x) branch Qt already
// ships on Linux/macOS. Bounds are intentionally ignored (same as that path).
//
// Guarded to _MSC_VER >= 1950 (VS2026+) so it can never collide with an older
// toolchain's real stdext::make_checked_array_iterator, which is still present on
// VS2019 / VS2022 and returns a different type.

#pragma once

// Defense-in-depth: this header is only meaningful for C++. Guard against being
// force-included into a C translation unit (the project also compiles C sources).
#if defined(__cplusplus) && defined(_MSC_VER) && _MSC_VER >= 1950

#include <cstddef>

namespace stdext {

template <typename T>
inline T* make_checked_array_iterator(T* ptr, std::size_t /*size*/, std::size_t /*index*/ = 0) noexcept
{
    return ptr;
}

template <typename T>
inline T* make_unchecked_array_iterator(T* ptr) noexcept
{
    return ptr;
}

} // namespace stdext

#endif // defined(__cplusplus) && defined(_MSC_VER) && _MSC_VER >= 1950
