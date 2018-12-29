//
// Created by matthew on 12/28/18.
//

#ifndef _PARAMDEF_H
#define _PARAMDEF_H

/* Debug level, zero for none */
#define PARAM_DEBUG 0

#define P_CREATE 1
#define P_NONE 0

#define P_TYPE_BOOL 0
#define P_TYPE_INT 1
#define P_TYPE_DOUBLE 2
#define P_TYPE_STRING 3

#define P_FLAG_NONE 0
#define P_FLAG_READONLY 1
#define P_FLAG_USERDEF (1 << 1)
#define P_FLAG_QVAR (1 << 2)
#define P_FLAG_TVAR (1 << 3)
#define P_FLAG_ALWAYS_MATRIX (1 << 4)
#define P_FLAG_PER_PIXEL (1 << 6)
#define P_FLAG_PER_POINT (1 << 7)
#define P_FLAG_RGB (1 << 8)

#endif //_PARAMDEF_H
