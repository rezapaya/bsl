// bslma_instrumentedallocator.t.cpp                                  -*-C++-*-
#include <bslma_instrumentedallocator.h>

#include <bsls_alignmentutil.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <iostream>
#include <sstream>

using namespace BloombergLP;
using namespace bslma;

// ============================================================================
//                                TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 ---------
// The component under test implements a memory allocator
// that keeps track of the number of bytes it allocates that are currently in
// use and the total number of bytes it has allocated.  Most testing goes into
// making sure that the correct number of bytes are being counted.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] InstrumentedAllocator(bslma::Allocator *basicAllocator = 0);
// [ 4] InstrumentedAllocator(const char       *name,
//                            bslma::Allocator *basicAllocator = 0);
// [ 2] ~InstrumentedAllocator();
//
// MANIPULATORS
// [ 2] allocate(size_type size);
// [ 2] deallocate(void *address);
//
// ACCESSORS
// [ 3] numBytesInUse() const;
// [ 3] numBytesAllocated() const;
// [ 7] print() const;
// [ 4] name() const;
// ----------------------------------------------------------------------------
// OTHER CONCERNS
//
// [ 5] alignment of allocated memory
// [ 6] negative testing for 'deallocate'
// [ 8] deallocate/allocate null test
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 9] USAGE EXAMPLE
// ----------------------------------------------------------------------------

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i) 
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                           TEST HELPER FUNCTIONS
// ----------------------------------------------------------------------------

namespace {

size_t alignmentFromSize(size_t size)
{
    if (size == 0) return 1;

    size_t maxAlignment = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;
    size_t alignmentMask = 1;

    while ((alignmentMask < maxAlignment) && (size & ~alignmentMask) != 0) {
        // clear the current bit and move to the next
        size &= ~alignmentMask;
        alignmentMask <<= 1;
    }

    // Loop postcondition: size >= maxAlignment or all but the highest bit of
    // size are cleared.

    ASSERT(size >= maxAlignment || (size & (size - 1)) == 0);

    return size >= maxAlignment ? maxAlignment : size;
}

}  // close unnamed namespace

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
/// Example 1: Tracking the amount of dynamically allocated memory.
/// ---------------------------------------------------------------
//
// In the following example, we demonstrate how the instrumented allocator is
// used to know the amount of dynamic memory allocated by a container we will
// create called DoubleStack.

class DoubleStack {
  private:
    // NOT IMPLEMENTED
    DoubleStack(const DoubleStack&, bslma::Allocator);

    // DATA
    enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };

    double           *d_stack_p;     // dynamically allocated array
                                     // (d_capacity elements)

    int               d_capacity;    // physical capacity of this stack
                                     // (in elements)

    int               d_length;      // logical index of next
                                     // available stack element

    bslma::Allocator *d_allocator_p; // holds (but doesn't own)
                                     // object

    // PRIVATE MANIPULATORS
    void increaseSize();
        // Increase the capacity by at least one element.

  public:
    // CREATORS
    explicit
    DoubleStack(bslma::Allocator *basicAllocator = 0);
        // Create a stack for doubles.   Optionally, specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    ~DoubleStack();
        // Delete this object.

    // MANIPULATORS
    void push(double value);
        // Add 'value' to the top of the stack.

    void pop();
        // Remove the element at the top of the stack.
};

DoubleStack::DoubleStack(bslma::Allocator *basicAllocator)
: d_capacity(INITIAL_SIZE)
, d_length(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    ASSERT(d_allocator_p);
    d_stack_p = (double *) 
                       d_allocator_p->allocate(d_capacity * sizeof *d_stack_p);
}

inline
DoubleStack::~DoubleStack()
{
    // CLASS INVARIANTS
    ASSERT(d_allocator_p);
    ASSERT(d_stack_p);
    ASSERT(0 <= d_length);
    ASSERT(0 <= d_capacity);
    ASSERT(d_length <= d_capacity);

    d_allocator_p->deallocate(d_stack_p);
}

void DoubleStack::push(double value)
{
    if (d_length >= d_capacity) {
        increaseSize();
    }
    d_stack_p[d_length++] = value;
}

static inline
void reallocate(double          **array,
                int               newSize,
                int               length,
                bslma::Allocator *basicAllocator)
    // Reallocate memory in the specified 'array' to the specified
    // 'newSize' using the specified 'basicAllocator'.  The specified
    // 'length' number of leading elements are preserved.  Since the
    //  class invariant requires that the physical capacity of the
    // container may grow but never shrink; the behavior is undefined
    // unless length <= newSize.
{
    ASSERT(array);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(basicAllocator);
    ASSERT(length <= newSize);        // enforce class invariant

    double *tmp = *array;             // support exception neutrality
    *array = (double *) basicAllocator->allocate(newSize * sizeof **array);

    // COMMIT POINT

    memcpy(*array, tmp, length * sizeof **array);
    basicAllocator->deallocate(tmp);
}

void DoubleStack::increaseSize()
{
    int proposedNewSize = d_capacity * GROW_FACTOR; // reallocate can throw

    ASSERT(proposedNewSize > d_length);
    reallocate(&d_stack_p, proposedNewSize, d_length, d_allocator_p);
    d_capacity = proposedNewSize;                   // we're committed
}

// ============================================================================
//                                MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test    = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;

    switch (test) { case 0:
      case 9: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

// InstrumentedAllocator can be passed into the object and we can then see
// how the object is allocating memory.

InstrumentedAllocator inst("DoubleStack Allocator");
DoubleStack stack(&inst);
stack.push(1);

ASSERT(inst.numBytesInUse() == 8);
ASSERT(inst.numBytesAllocated() == 8);

stack.push(2);

ASSERT(inst.numBytesInUse() == 16);
ASSERT(inst.numBytesAllocated() == 24);

stack.push(3);

ASSERT(inst.numBytesInUse() == 32);
ASSERT(inst.numBytesAllocated() == 56);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // ALLOCATE / DEALLOCATE NULL TEST
        //   Testing the deallocation/deallocation of 0.
        //
        // Concerns:
        //: 1 That invoking 'allocate' with 0 size and 'deallocate' on 0
        //:   address succeeds.
        //
        // Plan:
        //: 1 Create a 'InstrumentedAllocator' on the stack.  Then invoke
        //:   'allocate' with 0 size and 'deallocate' on 0 address.
        //
        // Testing:
        //   void *allocate(size_type size)   // allocate 0
        //   void deallocate(void *address)   // deallocate 0
        // --------------------------------------------------------------------
                     
        if (verbose) printf("\nALLOCATE / DEALLCOATE NULL TEST"
                            "\n===============================\n");

        // With page after return block being protected

        {
            if (verbose) printf("\nCreating allocator\n");

            InstrumentedAllocator obj;

            if (verbose) printf("\nTesting allocate 0\n");

            void *address = obj.allocate(0);
            ASSERT(0 == address);

            if (verbose) printf("\nTesting deallocate 0\n");

            obj.deallocate(address);
        }
      } break;
      case 7: {
        // -----------------------------------------------------------------
        // TESTING PRINT FUNCTION
        //
        // Concerns:
        //: 1 'print' function outputs the allocator properties into the output
        //:   stream.
        //
        // Plan:
        //: 1 Use 'allocate' to allocate some memory.  Then use 'print' to
        //:   print the allocator properties into the output 'stringstream' and
        //:   verify that the output matches what's expected.
        //
        // Testing:
        //   print() const;
        // -----------------------------------------------------------------

        if (verbose) printf("\nTESTING PRINT FUNCTION"
                            "\n======================\n");

        InstrumentedAllocator obj;
        const InstrumentedAllocator& cobj = obj;

        void *address = obj.allocate(101);
        void *address2 = obj.allocate(202);
        obj.deallocate(address2);

        std::ostringstream out;
        std::ostringstream& outRef = cobj.print(out);

        if (veryVerbose) printf("%d", outRef.str());

        const char * expected =
            "--------------------------------------------------------\n"
            "            InstrumentedAllocator Statistics            \n"
            "--------------------------------------------------------\n"
            "Name: 0\n"
            "Bytes in use: 101\n"
            "Total bytes allocated: 303\n";

        ASSERT(outRef.str() == expected);

        obj.deallocate(address);
      } break;
      case 6: {
        // -------------------------------------------------------------------
        // NEGATIVE TESTING DEALLOCATE
        //   Negative testing 'deallocate' preconditions.
        //
        // Concerns:
        //: 1 'deallocate' asserts on undefined behavior when the address
        //:   passed to 'deallocate' was not originally returned by 'allocate'.
        //
        // Plan:
        //: 1 Pass an address to the local stack object to 'deallocate' and
        //:   verify that the 'deallocate' precondition assertion is triggered
        //:   because the magic number that indicates a valid address can't be
        //:   found.
        // -------------------------------------------------------------------

        if (verbose) printf("\nNEGATIVE TESTING DEALLOCATE"
                            "\n===========================\n");

        bsls::AssertFailureHandlerGuard guard(
                                            &bsls::AssertTest::failTestDriver);

        InstrumentedAllocator obj;
        void *correctAddress = obj.allocate(8);

        ASSERT(obj.numBytesInUse() == 8);

        int stackObject = 0;
        ASSERT_FAIL(obj.deallocate(&stackObject));

        ASSERT(obj.numBytesInUse() == 8);

        if (verbose) obj.print(std::cout);
        ASSERT_PASS(obj.deallocate(correctAddress));

        ASSERT(obj.numBytesInUse() == 0);

        if (verbose) obj.print(std::cout);

      } break;
      case 5: {
        // -----------------------------------------------------------------
        // TESTING MEMORY ALIGNMENT
        //   Testing the alignment of memory allocated with the
        //   'InstrumentedAllocator'.
        //
        // Concerns:
        //: 1 Memory address returned from 'allocate' is aligned according to
        //:   C++ strict alignment rules.  I.e. the alignment needs to be
        //:   suitable for any fundamental type that can fit into the allocated
        //:   memory block.
        //
        // Plan:
        //: 1 Use 'allocate' to obtain memory blocks of different sizes and
        //:   verify that the allocated blocks meet the alignment requirements
        //:   by comparing the alignment with the strict alignment algorithm.
        // -----------------------------------------------------------------

        if (verbose) printf("\nTESTING MEMORY ALIGNMENT"
                            "\n========================\n");

        InstrumentedAllocator obj("alignment");

        for (size_t n = 0; n < 0xffff; ++n) {
            void *address = obj.allocate(n);
            size_t alignment = alignmentFromSize(n);

            ASSERT((alignment & (alignment - 1)) == 0);
            ASSERT((reinterpret_cast<size_t>(address) & (alignment - 1)) == 0);

            obj.deallocate(address);
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING NAMED CONSTRUCTOR AND NAME ACCESS
        //   Testing 'bslma::InstrumentedAllocator' constructed with a name.
        //   Ensures that the name is accessible through the 'name' function.
        //
        // Concerns:
        //: 1 Allocator is constructed correctly when given a name on
        //:   construction.
        //:
        //: 2 'name' returns the name of the allocator passed to it's
        //:   constructor.
        //
        // Plan:
        //: 1 Construct an instrumented allocator with a constructor taking a
        //:   name parameter.
        //:
        //: 2 Use 'name' and verify that its output is what we expect.
        //
        // Testing:
        //   bslma::InstrumentedAllocator(const char*       name,
        //                                bslma::Allocator *basicAllocator);
        //   const char *name() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST NAMED CONSTRUCTOR AND NAME ACCESS"
                            "\n======================================\n");

        if (veryVerbose) printf("\tWith an empty name\n");

        {
            const char *NAME   = "";

            InstrumentedAllocator obj(NAME);

            ASSERT(0 == obj.numBytesInUse());
            ASSERT(0 == obj.numBytesAllocated());

            ASSERT(0 == std::strcmp(NAME, obj.name()));
        }

        if (veryVerbose) printf("\tWith a non-empty name\n");

        {
            const char *NAME   = "Test Name";

            InstrumentedAllocator obj(NAME);

            ASSERT(0 == obj.numBytesInUse());
            ASSERT(0 == obj.numBytesAllocated());

            ASSERT(0 == std::strcmp(NAME, obj.name()));
        }
      } break;
      case 3: {
        // ------------------------------------------------------------------
        // TESTING ACCESSORS
        //   Testing accessors that return the number of bytes in use and the
        //   number of allocated bytes.
        //
        // Concerns:
        //: 1 'numBytesInUse' returns the amount of currently alive memory
        //:   allocated with 'allocate' function.
        //: 2 'numBytesAllocated' returns the maximum amount of memory
        //:   allocated with 'allocate' function.
        //
        // Plan:
        //: 1 Use 'allocate' to request memory blocks of increasing size and
        //:   verify that 'numBytesInUse' returns the expected number of
        //:   bytes.  Then use 'deallocate' to free the allocated memory blocks
        //:   and verify that 'numBytesInUse' goes to zero.
        //: 2 Use 'allocate' to request memory blocks of increasing size and
        //:   verify that 'numBytesAllocated' returns the expected number of
        //:   bytes.  Then use 'deallocate' to free the allocated memory blocks
        //:   and verify that 'numBytesAllocated' doesn't decrease.
        //
        // Testing:
        //   numBytesInUse() const;
        //   numBytesAllocated() const;
        // ------------------------------------------------------------------

        if (verbose) printf("\nTESTING ACCESSORS"
                            "\n=================\n");

        InstrumentedAllocator obj;
        const InstrumentedAllocator& cobj = obj;
        const size_t allocationLimit = 0x1000;
        char * memoryBlocks[allocationLimit];
        size_t inUse = 0;

        for (size_t n = 1; n != allocationLimit; ++n) {
            // allocate a memory block
            memoryBlocks[n] = static_cast<char *>(obj.allocate(n));
            ASSERT(memoryBlocks[n] != NULL);

            inUse += n;
            ASSERT(cobj.numBytesInUse() == inUse);
            ASSERT(cobj.numBytesAllocated() == inUse);
        }

        if (veryVerbose) obj.print(std::cout);

        // deallocate memory blocks
        size_t total = inUse;
        for (size_t n = 1; n != allocationLimit; ++n) {
            obj.deallocate(memoryBlocks[n]);

            inUse -= n;
            ASSERT(cobj.numBytesInUse() == inUse);
            ASSERT(cobj.numBytesAllocated() == total);
        }

        ASSERT(inUse == 0);
        ASSERT(cobj.numBytesInUse() == 0);

      } break;
      case 2: {
        // ------------------------------------------------------------------
        // TESTING ALLOCATE AND DEALLOCATE
        //   Testing allocating and deallocating the memory with the
        //   'InstrumentedAllocator'.
        //
        // Concerns:
        //: 1 Allocating memory with 'allocate' returns valid memory blocks
        //:   that can be read and written to.
        //: 2 Memory blocks allocated with 'allocate() can be deallocated
        //:   with 'deallocate'
        //
        // Plan:
        //: 1 Use 'allocate' to allocate memory blocks of different sizes
        //:   and initialize that memory with some data.
        //: 2 Use 'deallocate' to deallocate memory allocated with
        //:   'allocate'.
        //
        // Testing:
        //   void *allocate(size_type size);
        //   void deallocate(void *address);
        // ------------------------------------------------------------------

        if (verbose) printf("\nTESTING ALLOCATE AND DEALLOCATE"
                            "\n===============================\n");

        InstrumentedAllocator obj;
        const size_t allocationLimit = 0x1000;
        char *memoryBlocks[allocationLimit];

        for (size_t n = 1; n != allocationLimit; ++n) {
            // allocate a memory block
            memoryBlocks[n] = static_cast<char *>(obj.allocate(n));
            ASSERT(memoryBlocks[n] != NULL);

            // initialize memory block
            for (size_t i = 0; i != n; ++i) {
                memoryBlocks[n][i] = char(i);
                ASSERT(memoryBlocks[n][i] == char(i));
            }
        }

        if (veryVerbose) obj.print(std::cout);

        // deallocate memory blocks
        for (size_t n = 0; n != allocationLimit; ++n) {
            obj.deallocate(memoryBlocks[n]);
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:    testing in subsequent test cases.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC TEST"
                            "\n==========\n");

        {
            // empty object
            InstrumentedAllocator obj;
            ASSERT(obj.numBytesInUse() == 0);
            ASSERT(obj.numBytesAllocated() == 0);

            if (verbose) obj.print(std::cout);

            void *address = obj.allocate(16);

            if (verbose) obj.print(std::cout);

            obj.deallocate(address);
        }

        {
            // given an allocator
            const char *name = "Allocator for basic test";
            bslma::Allocator *allocator = 0;
            InstrumentedAllocator obj(name, allocator);
            ASSERT(obj.numBytesInUse() == 0);
            ASSERT(obj.numBytesAllocated() == 0);

            void *add1 = obj.allocate(8);
            void *add2 = obj.allocate(16);
            obj.allocate(32);
            ASSERT(obj.numBytesInUse() == 56);
            ASSERT(obj.numBytesAllocated() == 56);

            obj.deallocate(add1);
            obj.deallocate(add2);

            if (verbose) obj.print(std::cout);

            ASSERT(obj.numBytesInUse() == 32);
            ASSERT(obj.numBytesAllocated() == 56);

            obj.allocate(8);
            void *add5 = obj.allocate(16);
            void *add6 = obj.allocate(32);
            ASSERT(obj.numBytesInUse() == 88);
            ASSERT(obj.numBytesAllocated() == 112);

            if (verbose) obj.print(std::cout);

            obj.deallocate(add5);
            obj.deallocate(add6);
            ASSERT(obj.numBytesInUse() == 40);
            ASSERT(obj.numBytesAllocated() == 112);
        }
      } break;
      default: {
          printf("WARNING: CASE %d NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
// Copyright (c) 2013 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
