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
    if ( mem != NULL )
    {
        memset( mem, 0, count );
    }
    else
    {
        printf( "wipemalloc() failed to allocate %d bytes\n", (int)count );
    }
    return mem;
}


/** Safe memory deallocator */
void wipefree( void *ptr )
{
    if ( ptr != NULL )
        free( ptr );
}


void *wipe_aligned_alloc( size_t align, size_t size )
{
    void *mem = NULL;

#if HAVE_ALIGNED_ALLOC==1 && !__APPLE__

    mem = aligned_alloc( align, size );

#elif HAVE_POSIX_MEMALIGN==1

    if (posix_memalign(&mem, align, size))
      mem = NULL;

#else

    // only support powers of 2 for align
    assert( (align & (align-1)) == 0 );
    assert( (size % align) == 0 );
    void *allocated = malloc(size + align - 1 + sizeof(void*));
    if (allocated)
    {
        mem = (void*) (((size_t)allocated + sizeof(void*) + align -1) & ~(align-1));
        ((void**)mem)[-1] = allocated;
    }

#endif

    if (mem)
    {
        memset( mem, 0, size );
    }
    else
    {
        printf( "wipe_aligned_alloc() failed to allocate %d bytes\n", (int)size );
    }
    return mem;
}


void wipe_aligned_free( void *p )
{
#if HAVE_ALIGNED_ALLOC==1 || HAVE_POSIX_MEMALIGN==1

    if (p != NULL)
        free(p);

#else

    if (p != NULL)
    {
        void *allocated = ((void**)p)[-1];
        free(allocated);
    }

#endif
}
