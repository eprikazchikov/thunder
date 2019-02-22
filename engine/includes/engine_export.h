#ifndef ENGINE_EXPORT_H
#define ENGINE_EXPORT_H

#ifdef _WIN32
// mingw and visual studio
#define DLL_EXPORT __declspec(dllexport)
#define DLL_IMPORT __declspec(dllimport)
#define DLL_DEPREC __declspec(deprecated)
#else
// gcc or clang on non-windows platforms
#define DLL_EXPORT __attribute__((visibility("default")))
#define DLL_IMPORT __attribute__((visibility("default")))
#define DLL_DEPREC __attribute__ ((__deprecated__))
#endif

#ifdef ENGINE_STATIC_DEFINE
#  define ENGINE_EXPORT
#  define ENGINE_NO_EXPORT
#else
#  ifndef ENGINE_EXPORT
#    ifdef engine_editor_EXPORTS
        /* We are building this library */
#      define ENGINE_EXPORT DLL_EXPORT
#    else
        /* We are using this library */
#      define ENGINE_EXPORT DLL_IMPORT
#    endif
#  endif

#  ifndef ENGINE_NO_EXPORT
#    define ENGINE_NO_EXPORT 
#  endif
#endif

#ifndef ENGINE_DEPRECATED
#  define ENGINE_DEPRECATED DLL_DEPREC
#endif

#ifndef ENGINE_DEPRECATED_EXPORT
#  define ENGINE_DEPRECATED_EXPORT ENGINE_EXPORT ENGINE_DEPRECATED
#endif

#ifndef ENGINE_DEPRECATED_NO_EXPORT
#  define ENGINE_DEPRECATED_NO_EXPORT ENGINE_NO_EXPORT ENGINE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef ENGINE_NO_DEPRECATED
#    define ENGINE_NO_DEPRECATED
#  endif
#endif

#endif /* ENGINE_EXPORT_H */
