// bslma_newdeleteallocator.cpp                                       -*-C++-*-
#include <bslma_newdeleteallocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_atomicoperations.h>
#include <bsls_objectbuffer.h>

#include <new>

namespace BloombergLP {

typedef bsls::ObjectBuffer<bslma::NewDeleteAllocator>
                                        bslma_NewDeleteAllocator_Singleton;
    // A 'bslma_NewDeleteAllocator_Singleton' is a buffer with the right size
    // and alignment to hold a 'bslma::NewDeleteAllocator' object.

static bslma_NewDeleteAllocator_Singleton g_newDeleteAllocatorSingleton;
    // 'g_newDeleteAllocatorSingleton' is a global static buffer to hold the
    // singleton.

static bsls::AtomicOperations::AtomicTypes::Pointer
                                         g_newDeleteAllocatorSingleton_p = {0};
    // 'g_newDeleteAllocatorSingleton_p' is a global static pointer to the
    // singleton, which is *statically* initialized to 0.

static inline
bslma::NewDeleteAllocator *
initSingleton(bslma_NewDeleteAllocator_Singleton *address)
    // Construct a 'bslma::NewDeleteAllocator' at the specified 'address' in a
    // thread-safe way, and return 'address'.
{
    // Thread-safe initialization of singleton:
    //
    // A 'bslma::NewDeleteAllocator' contains no data members but does contain
    // a vtbl pointer.  During construction, the vtbl pointer is first set the
    // base class's vtbl before it is set to its final, derived-class value.
    // If two threads try to initialize the same singleton, the one that
    // finishes first may be in for a rude awakening as the second thread
    // temporarily changes the vtbl ptr to point to the base class's vtbl.
    //
    // We solve this problem by initializing the singleton on the stack and
    // then bit-copying it into its final location.  The stack initialization
    // is safe because each thread has its own stack.  The bit-copy is safe
    // because the only thing that is being copied is the vtbl pointer, which
    // is the same for all threads -- i.e., in case of a race, the second
    // thread copies identically the same data over the results of the first
    // thread's copy.

    bslma_NewDeleteAllocator_Singleton stackTemp;
    void *v = new(stackTemp.buffer()) bslma::NewDeleteAllocator();

    // Note that 'bsls::ObjectBuffer<T>' copy-assignment is a bit-wise copy.
    // Also, it's imperative to use 'v' here instead of the 'stackTemp' object
    // itself even though they point to the same object in memory, because that
    // creates a data dependency between the construction of the
    // 'NewDeleteAllocator' and this copy-assignment.  Without this dependency
    // the construction of the 'NewDeleteAllocator' can be reordered past the
    // copy-assignment or optimized out (as observed for Solaris optimized
    // builds)
    *address = *(static_cast<bslma_NewDeleteAllocator_Singleton *>(v));
    return &address->object();
}

namespace bslma {

                        // ------------------------
                        // class NewDeleteAllocator
                        // ------------------------

// CLASS METHODS
NewDeleteAllocator& NewDeleteAllocator::singleton()
{
    // This initialization is not guaranteed to happen once, but repeated
    // initialization will be safe (see the comment above).

    if (!bsls::AtomicOperations::getPtrAcquire(
                                           &g_newDeleteAllocatorSingleton_p)) {
        bsls::AtomicOperations::setPtrRelease(
                &g_newDeleteAllocatorSingleton_p,
                initSingleton(&g_newDeleteAllocatorSingleton));
    }

    return *static_cast<bslma::NewDeleteAllocator *>(
                const_cast<void *>(
                    bsls::AtomicOperations::getPtrRelaxed(
                                           &g_newDeleteAllocatorSingleton_p)));
}

// CREATORS
NewDeleteAllocator::~NewDeleteAllocator()
{
}

// MANIPULATORS
void *NewDeleteAllocator::allocate(size_type size)
{
    return 0 == size ? 0 : ::operator new(size);
}

}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
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
