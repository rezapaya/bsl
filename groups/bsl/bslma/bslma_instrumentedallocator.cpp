// bslma_instrumentedallocator.cpp                                    -*-C++-*-
#include <bslma_instrumentedallocator.h>

#include <bsls_alignmentutil.h>

namespace BloombergLP {
namespace bslma {

                        // ---------------------------
                        // class InstrumentedAllocator
                        // ---------------------------
namespace {

const Allocator::size_type offset =  bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;
const Allocator::size_type matchingNumber = 0xFEEDF00D; // magic number

}  // close unnamed namespace

// MANIPULATORS
void *InstrumentedAllocator::allocate(size_type size)
{
    if (0 == size) {
        return 0;                                                     // RETURN
    }

    // Rounding of size for best alignment
    
    size_type paddedSize = 
                          bsls::AlignmentUtil::roundUpToMaximalAlignment(size);

    // Adding some memory for metadata (size and matching number)

    size_type totalSize = paddedSize + 2*offset;

    void *address = d_allocator_p->allocate(totalSize);  // can throw

    size_type *matchingNum    = static_cast<size_type *>(address);
    size_type *allocatedSize  = reinterpret_cast<size_type *>
                                       (static_cast<char *>(address) + offset);
    void *returnBlock         = static_cast<char *>(address) + 2*offset;

    // Update the number of bytes allocated

    d_numBytesAllocated += size;
    d_numBytesInUse += size;

    // Set the metadata

    *matchingNum   = matchingNumber;
    *allocatedSize = size;

    return returnBlock;
}

void InstrumentedAllocator::deallocate(void *address)
{
    if (address) {
        address = static_cast<char *>(address) - 2*offset;
        size_type matchingNum =  *static_cast<size_type *>(address);

        // Making sure that address was returned by 'allocate'

        if (matchingNum != matchingNumber) {
            BSLS_ASSERT(0 && "'deallocate' called with invalid 'address'");
        }

        else {
            address = static_cast<char *>(address) + offset;
            size_type size  = *static_cast<size_type *>(address);

            // Double checking that you previously allocated this memory

            BSLS_ASSERT(d_numBytesInUse >= size);

            // Update number of bytes in use

            d_numBytesInUse -= size;

            address = static_cast<char *>(address) - offset;
            d_allocator_p->deallocate(address);
        }
    }
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
