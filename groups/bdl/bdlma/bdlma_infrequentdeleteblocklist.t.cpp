// bdlma_infrequentdeleteblocklist.t.cpp                              -*-C++-*-
#include <bdlma_infrequentdeleteblocklist.h>

#include <bdls_testutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// A 'bdlma::InfrequentDeleteBlockList' is a mechanism (i.e., having state but
// no value) that is used as a memory manager to manage a singly-linked list of
// dynamically allocated blocks of arbitrary (non-zero) size.  The primary
// concerns are that 'bdlma::InfrequentDeleteBlockList' (1) returns
// maximally-aligned memory blocks of the expected size from the object
// allocator, and (2) returns memory blocks back to the object allocator via
// the 'release' method (the 'deallocate' method has no effect), and upon
// destruction.  We make heavy use of the 'bslma::TestAllocator' to ensure that
// these concerns are satisfied.
//-----------------------------------------------------------------------------
// [ 2] bdlma::InfrequentDeleteBlockList(bslma::Allocator *ba = 0);
// [ 3] ~bdlma::InfrequentDeleteBlockList();
// [ 2] void *allocate(int size);
// [ 4] void deallocate(void *address);
// [ 3] void release();
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ *] CONCERN: There is no temporary allocation from any allocator.
// [ 2] CONCERN: Precondition violations are detected when enabled.

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BDLS_TESTUTIL_ASSERT
#define LOOP_ASSERT  BDLS_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BDLS_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BDLS_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BDLS_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BDLS_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BDLS_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BDLS_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BDLS_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BDLS_TESTUTIL_ASSERTV

#define Q   BDLS_TESTUTIL_Q   // Quote identifier literally.
#define P   BDLS_TESTUTIL_P   // Print identifier and value.
#define P_  BDLS_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BDLS_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BDLS_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                               GLOBAL TYPEDEF
//-----------------------------------------------------------------------------

typedef bdlma::InfrequentDeleteBlockList Obj;

//=============================================================================
//                   HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// This type was copied from 'bdlma_infrequentdeleteblocklist.h' for testing
// purposes.

struct Block {
    Block                               *d_next_p;
    bsls::AlignmentUtil::MaxAlignedType  d_memory;  // force alignment
};

static inline
int roundUp(int x, int y)
    // Round up the specified 'x' to the nearest whole integer multiple of the
    // specified 'y'.  The behavior is undefined unless '0 <= x' and '0 < y'.
{
    return (x + y - 1) / y * y;
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// A 'bdlma::InfrequentDeleteBlockList' object is commonly used to supply
// memory to more elaborate memory managers that distribute parts of each
// (larger) allocated memory block supplied by the
// 'bdlma::InfrequentDeleteBlockList' object.  The 'my_StrPool' memory pool
// manager shown below requests relatively large blocks of memory from its
// 'bdlma::InfrequentDeleteBlockList' member object and distributes memory
// chunks of varying sizes from each block on demand:
//..
    // my_strpool.h

    class my_StrPool {

        // DATA
        char           *d_block_p;    // current memory block

        int             d_blockSize;  // size of current memory block

        int             d_cursor;     // offset to next available byte in block

        bdlma::InfrequentDeleteBlockList
                        d_blockList;  // supplies managed memory blocks

      private:
        // PRIVATE MANIPULATORS
        void *allocateBlock(int numBytes);
            // Request a memory block of at least the specified 'numBytes' size
            // and allocate the initial 'numBytes' from this block.  Return the
            // address of the allocated memory.

      private:
        // NOT IMPLEMENTED
        my_StrPool(const my_StrPool&);
        my_StrPool& operator=(const my_StrPool&);

      public:
        // CREATORS
        explicit
        my_StrPool(bslma::Allocator *basicAllocator = 0);
            // Create a string pool.  Optionally specify a 'basicAllocator'
            // used to supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.

        ~my_StrPool();
            // Destroy this object and release all associated memory.

        // MANIPULATORS
        void *allocate(int size);
            // Return the address of a contiguous block of memory of the
            // specified 'size' (in bytes).  If 'size' is 0, no memory is
            // allocated and 0 is returned.  The behavior is undefined unless
            // 'size >= 0'.

        void release();
            // Release all memory currently allocated through this object.
    };

    // MANIPULATORS
    inline
    void my_StrPool::release()
    {
        d_blockList.release();
        d_block_p = 0;
    }

    // ...

    // my_strpool.cpp

    enum {
        INITIAL_SIZE  = 128,  // initial block size

        GROWTH_FACTOR =   2,  // multiplicative factor by which to grow block

        THRESHOLD     = 128   // size beyond which an individual block may be
                              // allocated if it doesn't fit in current block
    };

    // PRIVATE MANIPULATORS
    void *my_StrPool::allocateBlock(int numBytes)
    {
        ASSERT(0 < numBytes);

        if (THRESHOLD < numBytes) { // Alloc separate block if above threshold.
            return reinterpret_cast<char *>(
                                    d_blockList.allocate(numBytes));  // RETURN
        }

        if (d_block_p) {  // Don't increase block size if no current block.
            d_blockSize *= GROWTH_FACTOR;
        }
        d_block_p = reinterpret_cast<char*>(d_blockList.allocate(d_blockSize));
        d_cursor  = numBytes;

        return d_block_p;
    }

    // CREATORS
    my_StrPool::my_StrPool(bslma::Allocator *basicAllocator)
    : d_block_p(0)
    , d_blockSize(INITIAL_SIZE)
    , d_cursor(0)
    , d_blockList(basicAllocator)  // the blocklist knows about 'bslma_default'
    {
    }

    my_StrPool::~my_StrPool()
    {
        ASSERT(INITIAL_SIZE <= d_blockSize);
        ASSERT(d_block_p || (0 <= d_cursor && d_cursor <= d_blockSize));
    }

    // MANIPULATORS
    void *my_StrPool::allocate(int size)
    {
        if (0 == size) {
            return 0;                                                 // RETURN
        }

        if (d_block_p && size + d_cursor <= d_blockSize) {
            char *p = d_block_p + d_cursor;
            d_cursor += size;
            return p;                                                 // RETURN
        }
        else {
            return allocateBlock(size);
        }
    }
//..
// In the code shown above, the 'my_StrPool' memory manager allocates from its
// 'bdlma::InfrequentDeleteBlockList' member object an initial memory block of
// size 'INITIAL_SIZE'.  This size is multiplied by 'GROWTH_FACTOR' each time a
// depleted memory block is replaced by a newly-allocated block.  The
// 'allocate' method distributes memory from the current memory block
// piecemeal, except when the requested size (1) is not available in the
// current block, or (2) exceeds the 'THRESHOLD', in which case a separate
// memory block is allocated and returned.  When the 'my_StrPool' memory
// manager is destroyed, its 'bdlma::InfrequentDeleteBlockList' member object
// is also destroyed, which in turn automatically deallocates all of its
// managed memory blocks.

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator(veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 5: {
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
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        bslma::TestAllocator a(veryVeryVerbose);

        if (verbose) cout << "Testing 'my_StrPool'." << endl;
        {
            my_StrPool strPool(&a);

            const int SIZE[] = { 0, 10, 127, 128, 129, 1000 };
            const int NUM_DATA = sizeof SIZE / sizeof *SIZE;
            for (int di = 0; di < NUM_DATA; ++di) {
                void *p = strPool.allocate(SIZE[di]);
                if (veryVerbose) { T_; P_(SIZE[di]); P(p); }
                if (SIZE[di]) {
                    LOOP_ASSERT(di, p);
                } else {
                    LOOP_ASSERT(di, !p);
                }
            }
        }
        ASSERT(0 == a.numBytesInUse());
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING DEALLOCATE
        //   Ensure that 'deallocate' has no effect.
        //
        // Concerns:
        //: 1 Calling 'deallocate' with a non-null address has no effect.
        //:
        //: 2 Calling 'deallocate' with a null address has no effect.
        //
        // Plan:
        //: 1 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 2 Create a 'bslma::TestAllocator' object 'oa'.
        //:
        //: 3 Use the default constructor and 'oa' to create a modifiable 'Obj'
        //:   'mX'.
        //:
        //: 4 Allocate a couple of blocks from 'mX', and deallocate each block
        //:   immediately following its allocation; verify that neither block
        //:   is released to the object allocator.  (C-1)
        //:
        //: 5 Perform a separate test to verify that 'mX.deallocate(0)' has no
        //:   effect.  (C-2)
        //
        // Testing:
        //   void deallocate(void *address);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING DEALLOCATE" << endl
                                  << "==================" << endl;

        if (verbose) cout << "\nTesting 'deallocate'." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                Obj mX(&oa);               ASSERT(0 == oa.numBlocksInUse());

                void *p = mX.allocate(5);  ASSERT(1 == oa.numBlocksInUse());
                mX.deallocate(p);          ASSERT(1 == oa.numBlocksInUse());

                p = mX.allocate(64);       ASSERT(2 == oa.numBlocksInUse());
                mX.deallocate(p);          ASSERT(2 == oa.numBlocksInUse());
            }
                                           ASSERT(0 == oa.numBlocksInUse());
        }

        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                Obj mX(&oa);               ASSERT(0 == oa.numBlocksInUse());

                mX.deallocate(0);          ASSERT(0 == oa.numBlocksInUse());

                mX.allocate(5);            ASSERT(1 == oa.numBlocksInUse());
                mX.deallocate(0);          ASSERT(1 == oa.numBlocksInUse());
            }
                                           ASSERT(0 == oa.numBlocksInUse());
        }

        ASSERT(0 == da.numBlocksTotal());

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // DTOR & RELEASE
        //   Ensure that both the destructor and the 'release' method release
        //   all memory allocated from the object allocator.
        //
        // Concerns:
        //: 1 All memory allocated from the object allocator is released at
        //:   destruction.
        //:
        //: 2 All memory allocated from the object allocator is released by
        //:   the 'release' method.
        //:
        //: 3 Additional allocations can be made from the object following
        //:   a call to 'release'.
        //
        // Plan:
        //: 1 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 2 For 'N' in the range '[0 .. 4]':
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the default constructor and 'oa' to create a modifiable
        //:     'Obj' 'mX'.
        //:
        //:   3 Allocate 'N' blocks from 'mX' and verify that the requested
        //:     number of blocks were allocated from 'oa'.
        //:
        //:   4 Allow 'mX' to go out of scope and verify that the destructor
        //:     released all memory back to 'oa'.  (C-1)
        //:
        //:  3 Repeat P-2 except invoke 'release' on 'mX' following P-2.3 and
        //:    verify that 'release' released all memory back to 'oa'.  (C-2)
        //:
        //:  4 Following each call to 'release', allocate 'N' additional blocks
        //:    from 'mX' and verify that the requested number of blocks were
        //:    allocated from 'oa'.  (C-3)
        //
        // Testing:
        //   ~bdlma::InfrequentDeleteBlockList();
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DTOR & RELEASE" << endl
                                  << "==============" << endl;

        const int DATA[]   = { 1, 16, 256, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nTesting destructor." << endl;
        {
            for (int ti = 0; ti <= NUM_DATA; ++ti) {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(&oa);

                    for (int tj = 0; tj < ti; ++tj) {
                        const int SIZE = DATA[tj];

                        void *p = mX.allocate(SIZE);
                        if (veryVerbose) { T_; P_(SIZE); P(p); }
                    }
                    LOOP_ASSERT(ti, ti == oa.numBlocksInUse());
                }

                LOOP_ASSERT(ti, 0 == oa.numBlocksInUse());
            }
        }

        if (verbose) cout << "\nTesting 'release'." << endl;
        {
            for (int ti = 0; ti <= NUM_DATA; ++ti) {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj mX(&oa);

                for (int tj = 0; tj < ti; ++tj) {
                    const int SIZE = DATA[tj];

                    void *p = mX.allocate(SIZE);
                    if (veryVerbose) { T_; P_(SIZE); P(p); }
                }
                LOOP_ASSERT(ti, ti == oa.numBlocksInUse());

                mX.release();

                LOOP_ASSERT(ti,  0 == oa.numBlocksInUse());

                for (int tj = 0; tj < ti; ++tj) {
                    const int SIZE = DATA[tj];

                    void *p = mX.allocate(SIZE);
                    if (veryVerbose) { T_; P_(SIZE); P(p); }
                }
                LOOP_ASSERT(ti, ti == oa.numBlocksInUse());
            }
        }

        ASSERT(0 == da.numBlocksTotal());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR & ALLOCATE
        //   Ensure that we can use the default constructor to create an
        //   object, and that we can 'allocate' memory from that object.
        //
        // Concerns:
        //: 1 If an allocator is NOT supplied to the default constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 2 If an allocator IS supplied to the default constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 3 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 4 The default constructor allocates no memory.
        //:
        //: 5 Any memory allocation is from the object allocator.
        //:
        //: 6 Memory blocks returned by 'allocate' are of at least the
        //:   requested size (in bytes).
        //:
        //: 7 Memory blocks returned by 'allocate' are maximally aligned.
        //:
        //: 8 Calling 'allocate' with 0 returns 0 and has no effect on any
        //:   allocator.
        //:
        //: 9 There is no temporary allocation from any allocator.
        //:
        //:10 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using a loop-based approach, default-construct three distinct
        //:   objects, in turn, but configured differently: (a) without passing
        //:   an allocator, (b) passing a null allocator address explicitly,
        //:   and (c) passing the address of a test allocator distinct from the
        //:   default.  For each of these three iterations:  (C-1..5, 9)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     as the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object
        //:     'mX', with its object allocator configured appropriately (see
        //:     P-2); use a distinct test allocator for the object's footprint.
        //:
        //:   3 Use the appropriate test allocators to verify that no memory
        //:     is allocated by the default constructor.  (C-4)
        //:
        //:   4 Allocate a couple of blocks from 'mX' and verify that all
        //:     memory is allocated from the object allocator.  (C-1..3, 5)
        //:
        //:   5 Verify that no temporary memory is allocated from any
        //:     allocator.  (C-9)
        //:
        //:   6 Verify that all object memory is released when the object is
        //:     destroyed.  Note that the destructor is fully tested in case 3.
        //:
        //: 2 Create a 'bslma::TestAllocator' object and install it as the
        //:   current default allocator.
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of positive allocation request sizes.
        //:
        //: 4 For each row 'R' (representing an allocation request size, 'S')
        //:   in the table described in P-3:  (C-5..7, 9)
        //:
        //:   1 Create a 'bslma::TestAllocator' object 'oa'.
        //:
        //:   2 Use the default constructor and 'oa' to create a modifiable
        //:     'Obj' 'mX'.
        //:
        //:   3 Invoke 'mX.allocate(S)'.
        //:
        //:   4 Verify that the returned memory address is non-null and that
        //:     the memory was allocated from the object allocator.  (C-5)
        //:
        //:   5 Verify that the actual size of the allocated block recorded by
        //:     the allocator is equal to the expected size, and that the
        //:     returned memory address is properly offset to reserve room for
        //:     the memory block header.  (C-6)
        //:
        //:   6 Verify that the returned memory address is maximally aligned.
        //:     (C-7)
        //:
        //:   7 Verify that no temporary memory is allocated from any
        //:     allocator.  (C-9)
        //:
        //: 5 Perform a separate test to verify that 'mX.allocate(0)' returns 0
        //:   and has no effect on any allocator.  (C-8)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered.  (C-10)
        //
        // Testing:
        //   bdlma::InfrequentDeleteBlockList(bslma::Allocator *ba = 0);
        //   void *allocate(int size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DEFAULT CTOR & ALLOCATE" << endl
                                  << "=======================" << endl;

        if (verbose) cout << "\nTesting constructor." << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) Obj();
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objPtr = new (fa) Obj(0);
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objPtr = new (fa) Obj(&sa);
                objAllocatorPtr = &sa;
              } break;
              default: {
                LOOP_ASSERT(CONFIG, !"Bad allocator config.");
              } break;
            }
            LOOP_ASSERT(CONFIG, sizeof(Obj) == fa.numBytesInUse());

            Obj&                   mX = *objPtr;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify no allocation from the object allocator.

            LOOP2_ASSERT(CONFIG, oa.numBlocksTotal(),
                         0 == oa.numBlocksTotal());

            // Verify no allocation from the non-object allocator.

            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());

            // Verify we can allocate from the object.

            void *p = mX.allocate(1);
            LOOP2_ASSERT(CONFIG, p, p);
            LOOP2_ASSERT(CONFIG,  oa.numBlocksTotal(),
                         1 ==  oa.numBlocksTotal());
            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());

            p = mX.allocate(3001);
            LOOP2_ASSERT(CONFIG, p, p);
            LOOP2_ASSERT(CONFIG,  oa.numBlocksTotal(),
                         2 ==  oa.numBlocksTotal());
            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            LOOP2_ASSERT(CONFIG,  fa.numBlocksInUse(),
                         0 ==  fa.numBlocksInUse());
            LOOP2_ASSERT(CONFIG,  oa.numBlocksInUse(),
                         0 ==  oa.numBlocksInUse());
            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());
        }

        typedef bsls::AlignmentUtil U;
        const int HDRSZ = sizeof(Block) -
                                       bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

        if (veryVerbose) { T_; P_(HDRSZ); P(U::BSLS_MAX_ALIGNMENT); }

        struct {
            int d_line;  // line number
            int d_size;  // memory request size
        } DATA[] = {
            //LINE   SIZE
            //----   ---------------
            { L_,    1               },
            { L_,    2               },
            { L_,    4               },
            { L_,    8               },
            { L_,    16              },
            { L_,    HDRSZ           },
            { L_,    5 * HDRSZ -  0  },
            { L_,    5 * HDRSZ -  1  },
            { L_,    5 * HDRSZ -  2  },
            { L_,    5 * HDRSZ -  3  },
            { L_,    5 * HDRSZ -  4  },
            { L_,    5 * HDRSZ -  5  },
            { L_,    5 * HDRSZ -  6  },
            { L_,    5 * HDRSZ -  7  },
            { L_,    5 * HDRSZ -  8  },
            { L_,    5 * HDRSZ -  9  },
            { L_,    5 * HDRSZ - 10  },
            { L_,    5 * HDRSZ - 11  },
            { L_,    5 * HDRSZ - 12  },
            { L_,    5 * HDRSZ - 13  },
            { L_,    5 * HDRSZ - 14  },
            { L_,    5 * HDRSZ - 15  },
            { L_,    5 * HDRSZ - 16  }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting 'allocate'." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE = DATA[ti].d_line;
            const int SIZE = DATA[ti].d_size;

            const int EXP_SZ = roundUp(SIZE + HDRSZ, U::BSLS_MAX_ALIGNMENT);

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);
            void *p = mX.allocate(SIZE);  LOOP2_ASSERT(LINE, ti, p);

            const void *EXP_P = (char *)oa.lastAllocatedAddress() + HDRSZ;

            int numBytes = oa.lastAllocatedNumBytes();

            int offset = U::calculateAlignmentOffset(p, U::BSLS_MAX_ALIGNMENT);

            if (veryVerbose) {
                T_; P_(SIZE); P_(numBytes); P_(EXP_SZ);
                P_(p); P_(EXP_P); P(offset);
            }

            LOOP2_ASSERT(LINE, ti, EXP_P  == p);
            LOOP2_ASSERT(LINE, ti, EXP_SZ == numBytes);
            LOOP2_ASSERT(LINE, ti,      0 == offset);
            LOOP2_ASSERT(LINE, ti,      0 == da.numBlocksTotal());
            LOOP2_ASSERT(LINE, ti,      1 == oa.numBlocksTotal());
        }

        if (verbose) cout << "\nTesting 'allocate(0)'." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);
            void *p = mX.allocate(0);

            ASSERT(0 == p);
            ASSERT(0 == oa.numBlocksTotal());
            ASSERT(0 == da.numBlocksTotal());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX;

            if (veryVerbose) cout << "\t'allocate(size < 0)'" << endl;
            {
                ASSERT_SAFE_PASS(mX.allocate( 1));
                ASSERT_SAFE_PASS(mX.allocate( 0));
                ASSERT_SAFE_FAIL(mX.allocate(-1));
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create a modifiable object 'mX'.
        //: 2 Allocate a block 'b1' from 'mX'.
        //: 3 Deallocate block 'b1' (with no effect).
        //: 4 Allocate blocks 'b2' and 'b3' from 'mX'.
        //: 5 Invoke the release method on 'mX'.
        //: 6 Allocate a block 'b4' from 'mX'.
        //: 7 Allow 'mX' to go out of scope.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        {
            // 1
            Obj mX(&oa);                   ASSERT(0 == oa.numBlocksInUse());

            // 2
            void *p = mX.allocate(16);     ASSERT(1 == oa.numBlocksInUse());

            // 3
            mX.deallocate(p);              ASSERT(1 == oa.numBlocksInUse());

            // 4
            mX.allocate(8);                ASSERT(2 == oa.numBlocksInUse());
            mX.allocate(32);               ASSERT(3 == oa.numBlocksInUse());

            // 5
            mX.release();                  ASSERT(0 == oa.numBlocksInUse());

            // 6
            mX.allocate(1);                ASSERT(1 == oa.numBlocksInUse());
        }   // 7
                                           ASSERT(0 == oa.numBlocksInUse());

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

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
