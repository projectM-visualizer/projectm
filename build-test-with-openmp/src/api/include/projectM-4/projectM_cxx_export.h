
#ifndef PROJECTM_CXX_EXPORT_H
#define PROJECTM_CXX_EXPORT_H

#ifdef PROJECTM_CXX_STATIC_DEFINE
#  define PROJECTM_CXX_EXPORT
#  define PROJECTM_CXX_NO_EXPORT
#else
#  ifndef PROJECTM_CXX_EXPORT
#    ifdef projectM_api_EXPORTS
        /* We are building this library */
#      define PROJECTM_CXX_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define PROJECTM_CXX_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef PROJECTM_CXX_NO_EXPORT
#    define PROJECTM_CXX_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef PROJECTM_CXX_DEPRECATED
#  define PROJECTM_CXX_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef PROJECTM_CXX_DEPRECATED_EXPORT
#  define PROJECTM_CXX_DEPRECATED_EXPORT PROJECTM_CXX_EXPORT PROJECTM_CXX_DEPRECATED
#endif

#ifndef PROJECTM_CXX_DEPRECATED_NO_EXPORT
#  define PROJECTM_CXX_DEPRECATED_NO_EXPORT PROJECTM_CXX_NO_EXPORT PROJECTM_CXX_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef PROJECTM_CXX_NO_DEPRECATED
#    define PROJECTM_CXX_NO_DEPRECATED
#  endif
#endif

#endif /* PROJECTM_CXX_EXPORT_H */
