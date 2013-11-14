// bdlma_multipool.cpp                                                -*-C++-*-
#include <bdlma_multipool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_multipool_cpp,"$Id$ $CSID$")

#include <bdlma_bufferedsequentialallocator.h>  // for testing only

#include <bslma_autodestructor.h>
#include <bslma_deallocatorproctor.h>
#include <bslma_default.h>

#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

#include <bsl_new.h>

namespace BloombergLP {
namespace bdlma {

// TYPES
enum {
    DEFAULT_NUM_POOLS      = 10,  // default number of pools

    DEFAULT_MAX_CHUNK_SIZE = 32,  // default maximum number of blocks per chunk

    MIN_BLOCK_SIZE         =  8   // minimum block size (in bytes)
};

                      // ---------------
                      // class Multipool
                      // ---------------

// PRIVATE MANIPULATORS
void Multipool::initialize(bsls::BlockGrowth::Strategy growthStrategy,
                           int                         maxBlocksPerChunk)
{
    BSLS_ASSERT(1 <= maxBlocksPerChunk);

    d_maxBlockSize = MIN_BLOCK_SIZE;

    d_pools_p = static_cast<Pool *>(
                      d_allocator_p->allocate(d_numPools * sizeof *d_pools_p));

    bslma::DeallocatorProctor<bslma::Allocator> autoPoolsDeallocator(
                                                                d_pools_p,
                                                                d_allocator_p);
    bslma::AutoDestructor<Pool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) Pool(d_maxBlockSize + sizeof(Header),
                                 growthStrategy,
                                 maxBlocksPerChunk,
                                 d_allocator_p);

        d_maxBlockSize *= 2;
        BSLS_ASSERT(d_maxBlockSize > 0);
    }

    d_maxBlockSize /= 2;

    autoDtor.release();
    autoPoolsDeallocator.release();
}

void Multipool::initialize(
                        const bsls::BlockGrowth::Strategy *growthStrategyArray,
                        int                                maxBlocksPerChunk)
{
    BSLS_ASSERT(growthStrategyArray);
    BSLS_ASSERT(1 <= maxBlocksPerChunk);

    d_maxBlockSize = MIN_BLOCK_SIZE;

    d_pools_p = static_cast<Pool *>(
                      d_allocator_p->allocate(d_numPools * sizeof *d_pools_p));

    bslma::DeallocatorProctor<bslma::Allocator> autoPoolsDeallocator(
                                                                d_pools_p,
                                                                d_allocator_p);
    bslma::AutoDestructor<Pool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) Pool(d_maxBlockSize + sizeof(Header),
                                 growthStrategyArray[i],
                                 maxBlocksPerChunk,
                                 d_allocator_p);

        d_maxBlockSize *= 2;
        BSLS_ASSERT(d_maxBlockSize > 0);
    }

    d_maxBlockSize /= 2;

    autoDtor.release();
    autoPoolsDeallocator.release();
}

void Multipool::initialize(bsls::BlockGrowth::Strategy  growthStrategy,
                           const int                   *maxBlocksPerChunkArray)
{
    BSLS_ASSERT(maxBlocksPerChunkArray);

    d_maxBlockSize = MIN_BLOCK_SIZE;

    d_pools_p = static_cast<Pool *>(
                      d_allocator_p->allocate(d_numPools * sizeof *d_pools_p));

    bslma::DeallocatorProctor<bslma::Allocator> autoPoolsDeallocator(
                                                                d_pools_p,
                                                                d_allocator_p);
    bslma::AutoDestructor<Pool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) Pool(d_maxBlockSize + sizeof(Header),
                                 growthStrategy,
                                 maxBlocksPerChunkArray[i],
                                 d_allocator_p);

        d_maxBlockSize *= 2;
        BSLS_ASSERT(d_maxBlockSize > 0);
    }

    d_maxBlockSize /= 2;

    autoDtor.release();
    autoPoolsDeallocator.release();
}

void Multipool::initialize(
                     const bsls::BlockGrowth::Strategy *growthStrategyArray,
                     const int                         *maxBlocksPerChunkArray)
{
    BSLS_ASSERT(growthStrategyArray);
    BSLS_ASSERT(maxBlocksPerChunkArray);

    d_maxBlockSize = MIN_BLOCK_SIZE;

    d_pools_p = static_cast<Pool *>(
                      d_allocator_p->allocate(d_numPools * sizeof *d_pools_p));

    bslma::DeallocatorProctor<bslma::Allocator> autoPoolsDeallocator(
                                                                d_pools_p,
                                                                d_allocator_p);
    bslma::AutoDestructor<Pool> autoDtor(d_pools_p, 0);

    for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
        new (d_pools_p + i) Pool(d_maxBlockSize + sizeof(Header),
                                 growthStrategyArray[i],
                                 maxBlocksPerChunkArray[i],
                                 d_allocator_p);

        d_maxBlockSize *= 2;
        BSLS_ASSERT(d_maxBlockSize > 0);
    }

    d_maxBlockSize /= 2;

    autoDtor.release();
    autoPoolsDeallocator.release();
}

// PRIVATE ACCESSORS
int Multipool::findPool(int size) const
{
    BSLS_ASSERT_SAFE(0    <= size);
    BSLS_ASSERT_SAFE(size <= d_maxBlockSize);

    int accumulator = ((size + MIN_BLOCK_SIZE - 1) >> 3) * 2 - 1;

    accumulator |= accumulator >> 16;
    accumulator |= accumulator >>  8;
    accumulator |= accumulator >>  4;
    accumulator |= accumulator >>  2;
    accumulator |= accumulator >>  1;

    unsigned input = accumulator;

#if defined(BSLS_PLATFORM_CMP_GNU)
    return __builtin_popcount(input) - 1;
#else
    input -= (input >> 1) & 0x55555555;

    {
        const int mask = 0x33333333;
        input = ((input >> 2) & mask) + (input & mask);
    }

    input = ((input >>  4) + input) & 0x0f0f0f0f;
    input =  (input >>  8) + input;
    input =  (input >> 16) + input;

    return (input & 0x000000ff) - 1;
#endif
}

// CREATORS
Multipool::Multipool(bslma::Allocator *basicAllocator)
: d_numPools(DEFAULT_NUM_POOLS)
, d_blockList(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    initialize(bsls::BlockGrowth::BSLS_GEOMETRIC, DEFAULT_MAX_CHUNK_SIZE);
}

Multipool::Multipool(int               numPools,
                     bslma::Allocator *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(1 <= numPools);

    initialize(bsls::BlockGrowth::BSLS_GEOMETRIC, DEFAULT_MAX_CHUNK_SIZE);
}

Multipool::Multipool(bsls::BlockGrowth::Strategy  growthStrategy,
                     bslma::Allocator            *basicAllocator)
: d_numPools(DEFAULT_NUM_POOLS)
, d_blockList(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    initialize(growthStrategy, DEFAULT_MAX_CHUNK_SIZE);
}

Multipool::Multipool(int                          numPools,
                     bsls::BlockGrowth::Strategy  growthStrategy,
                     bslma::Allocator            *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(1 <= numPools);

    initialize(growthStrategy, DEFAULT_MAX_CHUNK_SIZE);
}

Multipool::Multipool(int                                numPools,
                     const bsls::BlockGrowth::Strategy *growthStrategyArray,
                     bslma::Allocator                  *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(1 <= numPools);
    BSLS_ASSERT(growthStrategyArray);

    initialize(growthStrategyArray, DEFAULT_MAX_CHUNK_SIZE);
}

Multipool::Multipool(int                          numPools,
                     bsls::BlockGrowth::Strategy  growthStrategy,
                     int                          maxBlocksPerChunk,
                     bslma::Allocator            *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(1 <= numPools);
    BSLS_ASSERT(1 <= maxBlocksPerChunk);

    initialize(growthStrategy, maxBlocksPerChunk);
}

Multipool::Multipool(int                                numPools,
                     const bsls::BlockGrowth::Strategy *growthStrategyArray,
                     int                                maxBlocksPerChunk,
                     bslma::Allocator                  *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(1 <= numPools);
    BSLS_ASSERT(growthStrategyArray);
    BSLS_ASSERT(1 <= maxBlocksPerChunk);

    initialize(growthStrategyArray, maxBlocksPerChunk);
}

Multipool::Multipool(int                          numPools,
                     bsls::BlockGrowth::Strategy  growthStrategy,
                     const int                   *maxBlocksPerChunkArray,
                     bslma::Allocator            *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(1 <= numPools);
    BSLS_ASSERT(maxBlocksPerChunkArray);

    initialize(growthStrategy, maxBlocksPerChunkArray);
}

Multipool::Multipool(int                                numPools,
                     const bsls::BlockGrowth::Strategy *growthStrategyArray,
                     const int                         *maxBlocksPerChunkArray,
                     bslma::Allocator                  *basicAllocator)
: d_numPools(numPools)
, d_blockList(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(1 <= numPools);
    BSLS_ASSERT(growthStrategyArray);
    BSLS_ASSERT(maxBlocksPerChunkArray);

    initialize(growthStrategyArray, maxBlocksPerChunkArray);
}

Multipool::~Multipool()
{
    BSLS_ASSERT(d_pools_p);
    BSLS_ASSERT(1 <= d_numPools);
    BSLS_ASSERT(1 <= d_maxBlockSize);
    BSLS_ASSERT(d_allocator_p);

    d_blockList.release();
    for (int i = 0; i < d_numPools; ++i) {
        d_pools_p[i].release();
        d_pools_p[i].~Pool();
    }
    d_allocator_p->deallocate(d_pools_p);
}

// MANIPULATORS
void *Multipool::allocate(int size)
{
    BSLS_ASSERT(1 <= size);

    if (size <= d_maxBlockSize) {
        const int pool = findPool(size);
        Header *p = static_cast<Header *>(d_pools_p[pool].allocate());
        p->d_header.d_poolIdx = pool;
        return p + 1;
    }

    // The requested size is large and will not be pooled.

    Header *p = static_cast<Header *>(
                                  d_blockList.allocate(size + sizeof(Header)));
    p->d_header.d_poolIdx = -1;
    return p + 1;
}

void Multipool::deallocate(void *address)
{
    BSLS_ASSERT(address);

    Header *h = static_cast<Header *>(address) - 1;

    const int pool = h->d_header.d_poolIdx;

    if (-1 == pool) {
        d_blockList.deallocate(h);
    }
    else {
        d_pools_p[pool].deallocate(h);
    }
}

void Multipool::release()
{
    for (int i = 0; i < d_numPools; ++i) {
        d_pools_p[i].release();
    }
    d_blockList.release();
}

void Multipool::reserveCapacity(int size, int numBlocks)
{
    BSLS_ASSERT(1    <= size);
    BSLS_ASSERT(size <= d_maxBlockSize);
    BSLS_ASSERT(0    <= numBlocks);

    const int pool = findPool(size);
    d_pools_p[pool].reserveCapacity(numBlocks);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
