#ifndef _STD_MKNCC
#define _STD_MKNCC

// note: this is endian-incompatible with win32's MAKEFOURCC
// otoh, it shows up nicely in a debug register ;)

#define MK4CC(a, b, c, d) ( (((unsigned long)a)<<24)|(((unsigned long)b)<<16)|(((unsigned long)c)<<8)|((unsigned long)d) )
#define MK3CC(b, c, d) ( (((unsigned long)b)<<16)|(((unsigned long)c)<<8)|((unsigned long)d) )
#define MK2CC(c, d) ( (((unsigned long)c)<<8)|((unsigned long)d) )

#endif
