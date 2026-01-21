
#ifndef PROJECTM_EXPORT_H
#define PROJECTM_EXPORT_H

#ifdef PROJECTM_STATIC_DEFINE
#  define PROJECTM_EXPORT
#  define PROJECTM_NO_EXPORT
#else
#  ifndef PROJECTM_EXPORT
#    ifdef projectM_api_EXPORTS
        /* We are building this library */
#      define PROJECTM_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define PROJECTM_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef PROJECTM_NO_EXPORT
#    define PROJECTM_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef PROJECTM_DEPRECATED
#  define PROJECTM_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef PROJECTM_DEPRECATED_EXPORT
#  define PROJECTM_DEPRECATED_EXPORT PROJECTM_EXPORT PROJECTM_DEPRECATED
#endif

#ifndef PROJECTM_DEPRECATED_NO_EXPORT
#  define PROJECTM_DEPRECATED_NO_EXPORT PROJECTM_NO_EXPORT PROJECTM_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef PROJECTM_NO_DEPRECATED
#    define PROJECTM_NO_DEPRECATED
#  endif
#endif

#endif /* PROJECTM_EXPORT_H */
