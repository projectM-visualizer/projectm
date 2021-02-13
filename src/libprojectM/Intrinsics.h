//
//  Intrinsics.h
//  libprojectM
//
//  Created by Mischa Spiegelmock on 2/13/21.
//

#ifndef PRJMIntrinsics_h
#define PRJMIntrinsics_h

// try to detect if we have SSE2 (intel only)
#ifdef __SSE2__
#define _HAS_SSE2_ 1
#endif

#ifdef __arm64__
#undef _HAS_SSE2_
#endif

#endif /* PRJMIntrinsics_h */
