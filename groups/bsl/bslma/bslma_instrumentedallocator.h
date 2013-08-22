// bslma_instrumentedallocator.h                                     -*-C++-*-
#ifndef INCLUDED_BSLMA_INSTRUMENTEDALLOCATOR
#define INCLUDED_BSLMA_INSTRUMENTEDALLOCATOR

//@PURPOSE: Provide a memory allocator that counts the bytes it allocates.
//
//@CLASSES:
// bslma::InstrumentedAllocator: counting allocator
//
//@SEE_ALSO: bslma_allocator, bslma_testallocator, bael_countingallocator
//
//@DESCRIPTION: This component provides a special-purpose instrumented
// allocator that implements the bslma::Allocator protocol.  It keeps track of
// the number of bytes that have been allocated and the number that were
// allocated and are currently in use.  This allocator is thread-aware.  It
// also provides a basic check for allocate/deallocate mismatches.
//..
//  ,-----------------------------.
// (  bslma::InstrumentedAllocator )
//  `-----------------------------'
//                 |             numBytesInUse
//                 |             numBytesAllocated
//                 |             print
//                 |             name
//                 |             ctor/dtor
//                 V
//      ,---------------------.
//     (   bslma::Allocator    )
//      `---------------------'
//                               allocate
//                               deallocate
//..
///Counting Bytes
///-----------
// The byte counts maintained by bslma::InstrumentedAllocator are set to 0 when
// constructed and increase with each call to 'allocate' by 'size'.
// Each call to deallocate decreases the numByteInUse by the same amount by
// which the byte count was increased on the matching allocate call.  The
// number of currently used bytes is returned by 'numBytesInUse' and the total
// number of bytes that were allocated is returned by 'numBytesAllocated'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Tracking the amount of dynamically allocated memory.
///---------------------------------------------------------------
// In the following example, we demonstrate how the instrumented allocator is
// used to know the amount of dynamic memory allocated by a container we will
// create called 'DoubleStack'.
//..
//  class DoubleStack {
//    private:
//      // NOT IMPLEMENTED
//      DoubleStack(const DoubleStack&);
//
//      // DATA
//      enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };
//
//      double           *d_stack_p;     // dynamically allocated array
//                                       // (d_capacity elements)
//
//      int               d_capacity;    // physical capacity of this stack
//                                       // (in elements)
//
//      int               d_length;      // logical index of next
//                                       // available stack element
//
//      bslma::Allocator *d_allocator_p; // holds (but doesn't own)
//                                       // object
//
//      // PRIVATE MANIPULATORS
//      void increaseSize();
//          // Increase the capacity by at least one element.
//
//    public:
//      // CREATORS
//      explicit
//      DoubleStack(bslma::Allocator *basicAllocator = 0);
//          // Create a stack for doubles.   Optionally, specify a 
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      ~DoubleStack();
//          // Delete this object.
//
//      // MANIPULATORS
//      void push(double value);
//          // Add 'value' to the top of the stack.
//
//      void pop();
//          // Remove the element at the top of the stack.
//  };
//
//  DoubleStack::DoubleStack(bslma::Allocator *basicAllocator)
//  : d_capacity(INITIAL_SIZE)
//  , d_length(0)
//  , d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//      assert(d_allocator_p);
//      d_stack_p = (double *)
//                     d_allocator_p->allocate(d_capacity * sizeof *d_stack_p);
//  }
//
//  DoubleStack::~DoubleStack()
//  {
//      // CLASS INVARIANTS
//      assert(d_allocator_p);
//      assert(d_stack_p);
//      assert(0 <= d_length);
//      assert(0 <= d_capacity);
//      assert(d_length <= d_capacity);
//
//      d_allocator_p->deallocate(d_stack_p);
//  }
//
//  inline
//  void DoubleStack::push(double value)
//  {
//      if (d_length >= d_capacity) {
//          increaseSize();
//      }
//      d_stack_p[d_length++] = value;
//  }
//
//  static inline
//  void reallocate(double          **array,
//                  int               newSize,
//                  int               length,
//                  bslma::Allocator *basicAllocator)
//      // Reallocate memory in the specified 'array' to the specified
//      // 'newSize' using the specified 'basicAllocator'.  The specified
//      // 'length' number of leading elements are preserved.  Since the
//      //  class invariant requires that the physical capacity of the
//      // container may grow but never shrink; the behavior is undefined
//      // unless 'length' <= 'newSize'.
//  {
//      assert(array);
//      assert(1 <= newSize);
//      assert(0 <= length);
//      assert(basicAllocator);
//      assert(length <= newSize);        // enforce class invariant
//
//      double *tmp = *array;             // support exception neutrality
//      *array = (double *) basicAllocator->allocate(newSize * sizeof **array);
//
//      // COMMIT POINT
//
//      memcpy(*array, tmp, length * sizeof **array);
//      basicAllocator->deallocate(tmp);
//  }
//
//  void DoubleStack::increaseSize()
//  {
//      int proposedNewSize = d_capacity * GROW_FACTOR; // reallocate can throw
//
//      assert(proposedNewSize > d_length);
//      reallocate(&d_stack_p, proposedNewSize, d_length, d_allocator_p);
//      d_capacity = proposedNewSize;                   // we're committed
//  }
//..
// Now, InstrumentedAllocator can be passed into the object and we can then see
// how the object is allocating memory.
//..
//  InstrumentedAllocator inst("DoubleStack Allocator");
//  DoubleStack stack(&inst);
//  stack.push(1);
//
//  assert(inst.numBytesInUse() == 8);
//  assert(inst.numBytesAllocated() == 8);
//
//  stack.push(2);
//
//  assert(inst.numBytesInUse() == 16);
//  assert(inst.numBytesAllocated() == 24);
//
//  stack.push(3);
//
//  assert(inst.numBytesInUse() == 32);
//  assert(inst.numBytesAllocated() == 56);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

namespace BloombergLP {

namespace bslma {

                          // ===========================
                          // class InstrumentedAllocator
                          // ===========================

class InstrumentedAllocator : public Allocator {
    // This allocator keeps some statistics on the memory it allocates.  These
    // statistics include the number of bytes that it allocated that are
    // currently in use and the total number of bytes it has allocated.

  private:
    // NOT IMPLEMENTED
    InstrumentedAllocator(const InstrumentedAllocator&);
    InstrumentedAllocator& operator=(const InstrumentedAllocator&);

    // DATA
    const char       *d_name_p;            // optional name of the allocator

    bsls::AtomicInt64 d_numBytesInUse;     // number of bytes currently
                                           // allocated from this object

    bsls::AtomicInt64 d_numBytesAllocated; // cumulative number of bytes ever
                                           // allocated from this object

    bslma::Allocator *d_allocator_p;       // memory allocator (held,not owned)

  public:
    // CREATORS
    explicit
    InstrumentedAllocator(bslma::Allocator *basicAllocator = 0);
        // Create an instrumented allocator that keeps track of the number of
        // bytes that have been and are currently allocated.  Optionally,
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    explicit
    InstrumentedAllocator(const char       *name,
                          bslma::Allocator *basicAllocator = 0);
        // Create an instrumented allocator that keeps track of the number of
        // bytes that have been and are currently allocated.  This allocator
        // will then be referred to as 'name' in 'print'.  Optionally,
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    // ~InstrumentedAllocator();
        // Destroy this context object.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    virtual void *allocate(size_type size);
        // Return a newly-allocated block of memory of the specified positive
        // 'size' (in bytes).  Invokes the 'allocate' method of the allocator 
        // supplied at construction, increment the number of currently 
        // (and cumulatively) allocated bytes by 'size'.

    virtual void deallocate(void *address);
        // Return the memory block at the specified 'address' back to this
        // allocator.  If 'address' is 0, this function has no effect (e.g., on
        // allocation/deallocation statistics).  Decrement the number of 
        // currently allocated bytes by the size (in bytes) originally 
        // requested for this block.  The behavior is undefined unless 
        // 'address' was returned by 'allocate'.

    // ACCESSORS
    bsls::Types::Int64 numBytesInUse() const;
        // Return the number of bytes currently in use that were allocated by
        // this allocator.  Note that 'numBytesInUse' <= 'numBytesAllocated'.

    bsls::Types::Int64 numBytesAllocated() const;
        // Return the total number of bytes that were allocated by this
        // allocator.  Note that 'numBytesInUse' <= 'numBytesAllocated'.

    const char *name() const;
        // Return the name of the allocator given to it at construction.  If
        // a name was not given the name of "0" will be given as a default.

                                  // Aspects

    template <class OutputStream>
    OutputStream& print(OutputStream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Print the number of bytes currently in use and the number of bytes
        // that have been allocated to the specified 'stream' in an easy to
        // read format.  'level' and 'spacesPerLevel' arguments have no effect.
};

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // ---------------------------
                          // class InstrumentedAllocator
                          // ---------------------------
// CREATORS
inline
InstrumentedAllocator::InstrumentedAllocator(Allocator *basicAllocator)
: d_name_p("0")
, d_numBytesInUse(0)
, d_numBytesAllocated(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
InstrumentedAllocator::InstrumentedAllocator(const char *name,
                                             Allocator  *basicAllocator)
: d_name_p(name)
, d_numBytesInUse(0)
, d_numBytesAllocated(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

// ACCESSORS
inline
bsls::Types::Int64 InstrumentedAllocator::numBytesInUse() const
{
    return d_numBytesInUse.load();
}

inline
bsls::Types::Int64 InstrumentedAllocator::numBytesAllocated() const
{
    return d_numBytesAllocated.load();
}

inline
const char *InstrumentedAllocator::name() const
{
    return d_name_p;
}

                                // Aspects

template <class OutputStream>
OutputStream& InstrumentedAllocator::print(OutputStream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    if (level != 0 || spacesPerLevel != 4) {
        stream << "Level and spacesPerLevel will have no effect.";
    }

    stream << "--------------------------------------------------------\n"
           << "            InstrumentedAllocator Statistics            \n"
           << "--------------------------------------------------------\n"
           << "Name: "                  << name()                  << "\n"
           << "Bytes in use: "          << numBytesInUse()         << "\n"
           << "Total bytes allocated: " << numBytesAllocated()     << "\n";

    return stream;
}

}  // close package namespace

}  // close enterprise namespace

#endif

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
