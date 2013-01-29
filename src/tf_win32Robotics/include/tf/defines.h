#ifndef _TF_DEFINES_H_
#define _TF_DEFINES_H_

#ifdef WIN32
#  ifdef tf_EXPORTS
#    define TF_EXPORT __declspec(dllexport)
#  else
#    define TF_EXPORT __declspec(dllimport)
#  endif // tf_EXPORTS
#else // WIN32
#  define TF_EXPORT
#endif // WIN32

#endif //_TF_DEFINES_H_