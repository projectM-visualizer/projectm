/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2004 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */
/**
 * $Id: wipemalloc.c,v 1.1.1.1 2005/12/23 18:05:05 psperl Exp $
 *
 * Clean memory allocator
 */

#include "wipemalloc.h"
#include <assert.h>

 void *wipemalloc( size_t count )
 {
    void *mem = malloc( count );
    if ( mem != NULL ) {
        memset( mem, 0, count );
      } else {
        printf( "wipemalloc() failed to allocate %d bytes\n", (int)count );
      }
    return mem;
 }

/** Safe memory deallocator */
 void wipefree( void *ptr ) {
    if ( ptr != NULL ) {
        free( ptr );
      }
  }

void *wipe_aligned_alloc( size_t align, size_t size )
{
#if TARGET_OS_MAC
    // only support powers of 2 for align
    assert( (align & (align-1)) == 0 );
    void *allocated = malloc(size + align - 1 + sizeof(void*));
    if (allocated == NULL)
    {
        printf( "wipe_aligned_malloc() failed to allocate %d bytes\n", (int)size );
        return NULL;
    }
    void *ret = (void*) (((size_t)allocated + sizeof(void*) + align -1) & ~(align-1));
    *((void**)((size_t)ret - sizeof(void*))) = allocated;
    return ret;
#else
    void *mem = aligned_alloc( align, size );
    if ( mem != NULL ) {
        memset( mem, 0, size );
      } else {
        printf( "wipe_aligned_alloc() failed to allocate %d bytes\n", (int)size );
      }
    return mem;
#endif
}

void wipe_aligned_free( void *p )
{
#if TARGET_OS_MAC
    if (p != NULL)
    {
        void *allocated = *((void**)((size_t)p - sizeof(void*)));
        free(allocated);
    }
#else
    if (p != NULL)
        free(p);
#endif
}
