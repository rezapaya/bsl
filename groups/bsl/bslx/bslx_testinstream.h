// bslx_testinstream.h                                                -*-C++-*-
#ifndef INCLUDED_BSLX_TESTINSTREAM
#define INCLUDED_BSLX_TESTINSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enable unexternalization of fundamental types with identification.
//
//@CLASSES:
//  bslx::TestInStream: byte-array-based input stream class
//
//@SEE_ALSO: bslx_testoutstream
//
//@DESCRIPTION: This component implements a byte-array-based data
// stream object that provides platform-independent input methods
// ("unexternalization") on values, and arrays of values, of
// fundamental types, and on 'bsl::string'.
//
// This component is intended to be used in conjunction with the
// 'bslx_testoutstream' externalization component.  Each input method
// of 'bslx::TestInStream' reads either a value or a homogeneous array
// of values of a fundamental type, in a format that was written by
// the corresponding 'bslx::TestOutStream' method.  In general, the
// user of this component cannot rely on being able to read data that
// was written by any mechanism other than 'bslx::TestOutStream'.
//
// The supported types and required content are listed in the table
// below.  All of the fundamental types in the table may be input as
// scalar values or as homogeneous arrays.  'bsl::string' is input as
// a logical scalar string.  Note that 'Int64' and 'Uint64' denote
// 'bsls::Types::Int64' and 'bsls::Types::Uint64', which in turn are
// 'typedef' names for the signed and unsigned 64-bit integer types,
// respectively, on the host platform:
//..
//      C++ TYPE          REQUIRED CONTENT OF ANY PLATFORM NEUTRAL FORMAT
//      --------          -----------------------------------------------
//      Int64             least significant 64 bits (signed)
//      Uint64            least significant 64 bits (unsigned)
//      int               least significant 32 bits (signed)
//      unsigned int      least significant 32 bits (unsigned)
//      short             least significant 16 bits (signed)
//      unsigned short    least significant 16 bits (unsigned)
//      char              least significant  8 bits (platform dependent)
//      signed char       least significant  8 bits (signed)
//      unsigned char     least significant  8 bits (unsigned)
//      double            IEEE standard 8-byte floating-point value
//      float             IEEE standard 4-byte floating-point value
//
//      bsl::string       BDE STL implementation of the STL string class
//..
// This component also supports compact streaming in of integer types.  In
// particular, 64-bit values can be streamed in from 40, 48, 56, or 64-bit
// values, and 32-bit values can be streamed in from 24 and 32-bit values
// (consistent with what has been written to the stream, of course).  Note
// that, for signed types, the sign is preserved for all streamed-in values.
//
// Note that input streams can be *invalidated* explicitly and queried for
// *validity* and *emptiness*.  Reading from an initially invalid stream has no
// effect.  Attempting to read beyond the end of a stream will automatically
// invalidate the stream.  Whenever an inconsistent value is detected, the
// stream should be invalidated explicitly.
//
///Externalization and Value Semantics
///-----------------------------------
// The concept of "externalization" in this component is intentionally
// specialized to support streaming the *values* of entities atomically.  In
// particular, streaming an array of a specific length is considered to be an
// atomic operation.  It is therefore an error, for example, to stream out an
// array of 'int' of length three and then stream in those three 'int' values
// as three scalar 'int' variables.  Similarly, it is an error to stream out an
// array of length *L* and stream back in an array of length other than *L*.
//
///Input Limit
///-----------
// If exceptions are enabled at compile time, the test input stream can be
// configured to throw an exception after a specified number of input requests
// is exceeded.  If the input limit is less than zero (default), then the
// stream never throws an exception.  Note that a non-negative input limit is
// decremented after each input attempt, and throws only when the current input
// limit transitions from 0 to -1; no additional exceptions will be thrown
// until the input limit is again reset to a non-negative value.
//
///Version Checking
///----------------
// By default, 'bslx::TestInStream' assumes that the first unit of data in the
// stream is of type 'char' and represents the version number of an object to
// be (un)externalized.  When the first input method is invoked on a
// 'bslx::TestInStream', the stream checks for the existence of this version
// number and that it has a value greater than 0.  This automatic version
// checking helps to ensure that an object's externalization operation is
// correctly devised to include its version number as the first unit of data in
// the stream.  For data streams where automatic checking of version number is
// not desired (e.g., testing corrupted streams), this checking can be disabled
// by invoking the 'setSuppressVersionCheck' method with an argument of 1.
//
///Exception Test Macros
///---------------------
// This component also provides a pair of macros, 'BEGIN_BSLX_EXCEPTION_TEST'
// and 'END_BSLX_EXCEPTION_TEST'.  These macros can be used for testing
// exception safety of classes and their methods when 'bslx' streaming is
// involved.
//
// A 'bslx::TestInStream' named 'testInStream' *must* *be* *defined* *in*
// *scope* in order to use this pair of macros.  Note that if
// exception-handling is disabled (i.e., if 'BDE_BUILD_TARGET_EXC' is not
// defined), then the macros simply print the following:
//..
//          BSLX EXCEPTION TEST -- (NOT ENABLED) --
//..
// When exception-handling is enabled, the macros will set the 'testInStream's
// input stream size limit to 0, 'try' the code being tested, 'catch' any
// exceptions thrown due to input stream limit reached, and keep increasing the
// input limit until the code being tested completed successfully.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:
///- - - - -
// This 'bslx_testinstream' component (in conjunction with the companion
// 'bslx_testoutstream' component) is most useful in testing the 'bslx'
// externalization functionality of other components.  The following code
// exhibits the use of the 'bslx' test streams in conjunction with the
// 'BEGIN_BSLX_EXCEPTION_TEST' and 'END_BSLX_EXCEPTION_TEST' macros in
// performing a through test of the 'bslxStreamIn' and 'bslxStreamOut'
// functions of a hypothetical 'mydoublearray' component.
//
// The test program shown uses the standard 'bde' test methodology, and as such
// contains several useful constructs and conventions that are not fully
// described here.  Among them are:
//
// The 'ASSERT' and various 'LOOP_ASSERT' macros, which generate specific error
// reports if their last argument evaluates to zero.
//
// The 'g' function, which takes a 'char *' argument in specialized input
// language and generates an array with a value corresponding to the input
// specification.
//
// The 'verbose' flag is generated from a command-line argument, and indicates
// whether to generate non-error status messages.
//
// Various printing macros such as 'P()', 'P_()', and such, which format their
// arguments as appropriate status messages.
//
// 'Obj' is the standard 'typedef' for the object under test, in this case
// 'class MyDoubleArray'.
//
// Note that "\$" must be replaced by "\" in the preprocessor macro definitions
// that follow.  The "$" symbols are present in this header file to avoid a
// diagnostic elicited by some compilers (e.g., "warning: multi-line comment"):
//..
//  // myDoubleArray.t.cpp
//  #include <mydoublearray.h>
//  #include <bslx_testinstream.h>
//  #include <bslx_testinstreamexception.h>
//  // ...
//
//  typedef MyDoubleArray MyObj;
//  typedef double Element;
//
//  const Element VALUES[] = { 1.5, 2.5, 3.5, 4.75, -5.25 };
//
//  const Element &V0 = VALUES[0], &VA = V0,  // V0, V1, ... are used in
//                &V1 = VALUES[1], &VB = V1,  // conjunction with VALUES array.
//                &V2 = VALUES[2], &VC = V2,
//                &V3 = VALUES[3], &VD = V3,  // VA, VB, ... are used in
//                &V4 = VALUES[4], &VE = V4;  // conjunction with 'g' and 'gg'.
//
//  const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
//
//  // ...
//
//  switch (test) { case 0:
//  // ...
//
//    case 10: {
//      // --------------------------------------------------------------------
//      // TESTING STREAMING FUNCTIONALITY:
//      //   First perform a trivial direct test of the 'bslxStreamOut' and
//      //   'bslxStreamIn' methods (the rest of the testing will use the
//      //   stream operators).
//      //
//      //   VALID STREAMS
//      //     Use 'g' to construct a set, S, of unique 'MyDoubleArray'
//      //     objects with substantial and varied differences in value.  Using
//      //     all combinations (u,v) in the cross product S X S, stream the
//      //     value of v (into a temporary copy of) u and assert u == v.
//      //
//      //   EXCEPTION NEUTRALITY
//      //     Perform the same test as described above for VALID STREAMS,
//      //     except that during the stream-in operation, configure the
//      //     'bslx::TestInStream' object to throw an exception for every
//      //     input method invoked within the 'streamIn' method.  Use purify
//      //     to verify that no memory is leaked.
//      //
//      //   EMPTY AND INVALID STREAMS
//      //     For each u in S, create a copy and attempt to stream into it
//      //     from an empty and then invalid stream.  Verify after each try
//      //     that the object is unchanged and that the stream is invalid.
//      //
//      //   INCOMPLETE (BUT OTHERWISE VALID) DATA
//      //     Write 3 distinct 'MyDoubleArray' objects to an output stream
//      //     buffer of total length N.  For each partial stream length from
//      //     0 to N - 1, construct an input stream and attempt to read into
//      //     objects initialized with distinct values.  Verify values of
//      //     objects that are either successfully modified or left entirely
//      //     unmodified, and that the stream became invalid immediately after
//      //     the first incomplete read.  Finally ensure that each object
//      //     streamed into is in some valid state by assigning it a distinct
//      //     new value and testing for equality.
//      //
//      //   CORRUPTED DATA
//      //     Use the underlying stream package to simulate an instance of a
//      //     typical valid (control) stream and verify that it can be
//      //     streamed in successfully.  Then for each data field in the
//      //     stream (beginning with the version number), provide one or more
//      //     similar tests with that data field corrupted.  After each test,
//      //     verify that the object is in some valid state after streaming,
//      //     and that the input stream has gone invalid.
//      //
//      // Testing:
//      //   bslx::InStream& streamIn(bslx::InStream& stream);
//      //   bslx::OutStream& streamOut(bslx::OutStream& stream) const;
//      //   Ensure streamIn is exception neutral
//      // --------------------------------------------------------------------
//
//      if (verbose) cout << endl
//                        << "Testing Streaming Functionality" << bsl::endl
//                        << "===============================" << bsl::endl;
//      // To corrupt the output stream of any BDE object for testing purposes,
//      // replace the first byte of the stream with the value M1 that follows:
//      char M1 = 0xff;
//
//      static const char *SPECS[] = {
//          "",     "A",     "B",      "CC",      "DE",       "ABC",
//          "BCDE", "CDEAB", "DEABCD", "EABCDEA", "ABCDEABC", "BCDEABCDE",
//          0}; // Null string required as last element.
//
//      const int VERSION = 1;
//
//      if (verbose) {
//          bsl::cout
//              << "\tTrivial direct test of "
//              << "bslxStreamIn and bslxStreamOut methods."
//              << bsl::endl;
//      }
//      {
//          const MyObj X(h("ABC"));
//          TestOutStream out;              X.bslxStreamOut(out, VERSION);
//
//          const char *const PD  = out.data();
//          const int         NPB = out.length();
//          TestInStream in(PD, NPB);
//          ASSERT(in);                          ASSERT(!in.isEmpty());
//          in.setSuppressVersionCheck(1);
//
//          MyObj t(h("DE"));                    ASSERT(X != t);
//          t.bslxStreamIn(in, VERSION);         ASSERT(X == t);
//          ASSERT(in);                          ASSERT(in.isEmpty());
//      }
//
//      if (verbose) {
//          bsl::cout << "\tThorough test of bslx stream functions."
//                    << bsl::endl;
//      }
//      {
//          for (int i = 0; SPECS[i]; ++i) {
//              const MyObj X(h(SPECS[i]));
//              TestOutStream out;
//              OutStreamFunctions::streamOut(out, X, VERSION);
//              const char *const PD  = out.data();
//                                              LOOP_ASSERT(i, *PD != M1);
//              const int         NPB = out.length();
//
//              // Verify that each new value overwrites every old value
//              // and that the input stream is emptied, but remains valid.
//
//              for (int j = 0; SPECS[j]; ++j) {
//                  TestInStream in(PD, NPB);
//                  in.setSuppressVersionCheck(1);
//                  LOOP2_ASSERT(i,j,in);   LOOP2_ASSERT(i,j,!in.isEmpty());
//
//                  MyObj t(h(SPECS[j]));
//                  LOOP2_ASSERT(i, j, X == t == (i == j));
//                  InStreamFunctions::streamIn(in, t, VERSION);
//                  LOOP2_ASSERT(i, j, X == t);
//                  LOOP2_ASSERT(i, j, in); LOOP2_ASSERT(i,j,in.isEmpty());
//              }
//          }
//      }
//
//      if (verbose) {
//          bsl::cout
//              << "\tTest streamIn method for exception neutrality."
//              << bsl::endl;
//      }
//      {
//          for (int i = 0; SPECS[i]; ++i) {
//              const MyObj X(h(SPECS[i]));
//              TestOutStream out;
//              OutStreamFunctions::streamOut(out, X, VERSION);
//              const char *const PD  = out.data();
//                                              LOOP_ASSERT(i, *PD != M1);
//              const int         NPB = out.length();
//
//              // Verify that each new value overwrites every old value
//              // and that the input stream is emptied, but remains valid.
//
//              for (int j = 0; SPECS[j]; ++j) {
//                  TestInStream testInStream(PD, NPB);
//                  TestInStream& in = testInStream;
//                  in.setSuppressVersionCheck(1);
//                  LOOP2_ASSERT(i,j,in);   LOOP2_ASSERT(i,j,!in.isEmpty());
//
//                  MyObj t(h(SPECS[j]));
//                  LOOP2_ASSERT(i, j, X == t == (i == j));
//                  BEGIN_BSLX_EXCEPTION_TEST { in.reset();
//                      InStreamFunctions::streamIn(in, t, VERSION);
//                  } END_BSLX_EXCEPTION_TEST
//                  LOOP2_ASSERT(i, j, X == t);
//                  LOOP2_ASSERT(i, j, in); LOOP2_ASSERT(i,j,in.isEmpty());
//              }
//          }
//      }
//
//      if (verbose) {
//          bsl::cout << "\tOn empty and invalid streams." << bsl::endl;
//      }
//      {
//          TestOutStream out;
//          const char *const  PD = out.data();
//          const int         NPB = out.length();
//          ASSERT(0 == NPB);
//
//          for (int i = 0; SPECS[i]; ++i) {
//              TestInStream in(PD, NPB);
//              in.setSuppressVersionCheck(1);
//              LOOP_ASSERT(i, in);            LOOP_ASSERT(i, in.isEmpty());
//
//              // Ensure that reading from an empty or invalid input stream
//              // leaves the stream invalid and the target object unchanged.
//
//              const MyObj X(h(SPECS[i])); MyObj t(X); LOOP_ASSERT(i, X == t);
//              InStreamFunctions::streamIn(in, t, VERSION);
//              LOOP_ASSERT(i, !in);                    LOOP_ASSERT(i, X == t);
//              InStreamFunctions::streamIn(in, t, VERSION);
//              LOOP_ASSERT(i, !in);                    LOOP_ASSERT(i, X == t);
//          }
//      }
//
//      if (verbose) {
//          bsl::cout
//              << "\tOn incomplete (but otherwise valid) data."
//              << bsl::endl;
//      }
//      {
//          const MyObj W1 = h("A"),       X1 = h("CDCD"),   Y1 = h("BB");
//          const MyObj W2 = h("BCBCBCB"), X2 = h("ADEAD"),  Y2 = h("CABDE");
//          const MyObj W3 = h("DEEDDE"),  X3 = h("ABABAB"), Y3 = h("C");
//
//          TestOutStream out;
//          OutStreamFunctions::streamOut(out, X1, VERSION);
//          const int NPB1 = out.length();
//          OutStreamFunctions::streamOut(out, X2, VERSION);
//          const int NPB2 = out.length();
//          OutStreamFunctions::streamOut(out, X3, VERSION);
//          const int NPB  = out.length();
//          const char *const PD = out.data(); ASSERT(*PD != M1);
//
//          for (int i = 0; i < NPB; ++i) {
//              TestInStream in(PD,i);  in.setQuiet(!veryVerbose);
//              in.setSuppressVersionCheck(1);
//              LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
//              MyObj t1(W1), t2(W2), t3(W3);
//
//              if (i < NPB1) {
//                  InStreamFunctions::streamIn(in, t1, VERSION);
//                  LOOP_ASSERT(i, !in);
//                  if (0 == i) {
//                                       LOOP_ASSERT(i, W1 == t1);
//                  }
//                  InStreamFunctions::streamIn(in, t2, VERSION);
//                  LOOP_ASSERT(i, !in); LOOP_ASSERT(i, W2 == t2);
//                  InStreamFunctions::streamIn(in, t3, VERSION);
//                  LOOP_ASSERT(i, !in); LOOP_ASSERT(i, W3 == t3);
//              }
//              else if (i < NPB2) {
//                  InStreamFunctions::streamIn(in, t1, VERSION);
//                  LOOP_ASSERT(i,  in); LOOP_ASSERT(i, X1 == t1);
//                  InStreamFunctions::streamIn(in, t2, VERSION);
//                  LOOP_ASSERT(i, !in);
//                  if (NPB1 == i) {
//                                       LOOP_ASSERT(i, W2 == t2);
//                  }
//                  InStreamFunctions::streamIn(in, t3, VERSION);
//                  LOOP_ASSERT(i, !in); LOOP_ASSERT(i, W3 == t3);
//              }
//              else {
//                  InStreamFunctions::streamIn(in, t1, VERSION);
//                  LOOP_ASSERT(i,  in); LOOP_ASSERT(i, X1 == t1);
//                  InStreamFunctions::streamIn(in, t2, VERSION);
//                  LOOP_ASSERT(i,  in); LOOP_ASSERT(i, X2 == t2);
//                  InStreamFunctions::streamIn(in, t3, VERSION);
//                  LOOP_ASSERT(i, !in);
//                  if (NPB2 == i) {
//                                       LOOP_ASSERT(i, W3 == t3);
//                  }
//              }
//
//                              LOOP_ASSERT(i, Y1 != t1);
//              t1 = Y1;        LOOP_ASSERT(i, Y1 == t1);
//
//                              LOOP_ASSERT(i, Y2 != t2);
//              t2 = Y2;        LOOP_ASSERT(i, Y2 == t2);
//
//                              LOOP_ASSERT(i, Y3 != t3);
//              t3 = Y3;        LOOP_ASSERT(i, Y3 == t3);
//          }
//      }
//
//      if (verbose) bsl::cout << "\tOn corrupted data." << bsl::endl;
//
//      const MyObj W = h("");            // default value
//      const MyObj X = h("DCB");         // original value
//      const MyObj Y = h("ABCDE");       // new value
//
//      if (verbose) {
//          bsl::cout << "\t\tGood stream (for control)."
//                    << bsl::endl;
//      }
//      {
//          const int length  = 5;
//
//          TestOutStream out;    // Stream out "new" value.
//          OutStreamFunctions::streamOut(out, length, 0);
//          out.putArrayFloat64(VALUES, 5);
//          const char *const PD  = out.data();
//          const int         NPB = out.length();
//
//          MyObj t(X);    ASSERT(W != t);   ASSERT(X == t);    ASSERT(Y != t);
//          TestInStream in(PD, NPB); ASSERT(in);
//          in.setSuppressVersionCheck(1);
//          InStreamFunctions::streamIn(in, t, VERSION);  ASSERT(in);
//                         ASSERT(W != t);   ASSERT(X != t);    ASSERT(Y == t);
//      }
//
//      if (verbose) {
//          bsl::cout << "\t\tBad version." << bsl::endl;
//      }
//      {
//          const char version = -1; // too small
//          const int  length  = 5;
//
//          TestOutStream out;    // Stream out "new" value.
//          OutStreamFunctions::streamOut(out, length, 0);
//          out.putArrayFloat64(VALUES, 5);
//          const char *const PD  = out.data();
//          const int         NPB = out.length();
//
//          MyObj t(X);    ASSERT(W != t);    ASSERT(X == t);   ASSERT(Y != t);
//          TestInStream in(PD, NPB);  in.setQuiet(!veryVerbose);
//          in.setSuppressVersionCheck(1);
//          ASSERT(in);
//          InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
//                         ASSERT(W != t);    ASSERT(X == t);   ASSERT(Y != t);
//      }
//      {
//          const char version = 100; // too large
//          const int  length  = 5;
//
//          TestOutStream out;    // Stream out "new" value.
//          OutStreamFunctions::streamOut(out, length, 0);
//          out.putArrayFloat64(VALUES, 5);
//          const char *const PD  = out.data();
//          const int         NPB = out.length();
//
//          MyObj t(X);    ASSERT(W != t);    ASSERT(X == t);   ASSERT(Y != t);
//          TestInStream in(PD, NPB);
//          in.setSuppressVersionCheck(1);
//          ASSERT(in);
//          InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
//                         ASSERT(W != t);    ASSERT(X == t);   ASSERT(Y != t);
//      }
//
//      if (verbose) bsl::cout << "\t\tBad length." << bsl::endl;
//      {
//          const int length = -1; // too small
//
//          TestOutStream out;    // Stream out "new" value.
//          OutStreamFunctions::streamOut(out, length, 0);
//          out.putArrayFloat64(VALUES, 5);
//          const char *const PD  = out.data();
//          const int         NPB = out.length();
//
//          MyObj t(X);    ASSERT(W != t);   ASSERT(X == t);    ASSERT(Y != t);
//          TestInStream in(PD, NPB);
//          in.setSuppressVersionCheck(1);
//          ASSERT(in);
//          InStreamFunctions::streamIn(in, t, VERSION); ASSERT(!in);
//                         ASSERT(W != t);   ASSERT(X == t);    ASSERT(Y != t);
//      }
//    } break;
//    // ...
//  }
//..

#ifndef INCLUDED_BSLX_TESTINSTREAMEXCEPTION
#include <bslx_testinstreamexception.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace bslx {

                         // ==================
                         // class TestInStream
                         // ==================

class TestInStream {
    // This class provides input methods to unexternalize values (and C-style
    // arrays of values) of fundamental types from their byte representations.
    // By default, each input method also verifies the stream version (but
    // verification may be disabled via the 'setSuppressVersionCheck' method)
    // and the input value type.  By default, if invalid data is detected,
    // error messages are displayed on 'stdout'; this error reporting may be
    // disables via the 'setQuiet' method.  Note that attempting to read beyond
    // the end of a stream will automatically invalidate the stream.  The class
    // supports validity testing by providing the method
    // 'bool isValid() const'.

    // DATA
    bsl::vector<char> d_buffer;      // bytes to be unexternalized
    int               d_validFlag;   // indicate stream validity
    int               d_quietFlag;   // flag for "quiet" mode
    int               d_suppressVersionCheckFlag;
                                     // flag for "suppress-version-check" mode
    int               d_inputLimit;  // number of input op's before exception
    int               d_cursor;      // index of the next byte to be
                                     // extracted from this stream

    // FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream&, const TestInStream&);

    // MANIPULATORS
    void validate();
        // Put this input stream in an valid state. This function
        // has no effect if this stream is already valid.

    // PRIVATE MANIPULATORS
    void checkArray(FieldCode::Type code,
                    int             scalarSize,
                    int             numElements);
        // Verify the validity of the field code and array length, and
        // the sufficiency of data at the current cursor position in
        // the internal stream buffer. Extract the field code at the
        // cursor position from the buffer. If the field code does not
        // correspond to the specified 'code', then mark this stream
        // as invalid, and if the quiet flag is zero print an error
        // message. Otherwise, advance the cursor by the size of the
        // field code and extract the array length.  If the length
        // does not correspond to the specified 'numElements', then
        // mark this stream as invalid, and if the quiet flag is zero
        // print an error message. Otherwise, advance the cursor by
        // the size of the array length, and verify that the buffer
        // contains sufficient bytes for the specified
        // 'scalarSize * numElements'. If there are too few bytes in
        // the buffer, then mark this stream as invalid.  The behavior
        // is undefined unless '0 < scalarSize' and '0 <= numElements'.
        // Note that if this stream is invalid, this function has no
        // effect.  Also note that error messages are not printed for
        // insufficient data in the buffer.

    void checkScalar(FieldCode::Type code,
                     int             scalarSize);
        // Verify the validity of the field code and the sufficiency
        // of data at the current cursor position in the internal
        // stream buffer.  Extract the field code at the cursor
        // position from the buffer.  If the field code does not
        // correspond to the specified 'code', then mark this stream
        // as invalid and, if the quiet flag is zero, print an error
        // message.  Otherwise, advance the cursor position by the
        // size of the field code, and verify that the buffer contains
        // sufficient bytes for the specified 'scalarSize'.  If there
        // are too few bytes, then this stream is marked as invalid.
        // The behavior is undefined unless '0 < scalarSize'.  Note
        // that if this stream is invalid, this function has no
        // effect.  Also note that error messages are not printed for
        // insufficient data in the buffer.

    void checkVersion();
        // Verify that the leading 'SIZEOF_CODE + SIZEOF_VERSION'
        // bytes in the internal stream buffer, constitute a valid
        // version code (field code and value).  Extract the field
        // code for the version at byte position 0.  If the field code
        // is not equal to 'FieldCode::INT8', then mark this stream as
        // invalid, if the quiet flag is zero, print an error message.
        // Otherwise, extract the value of the version that follows
        // the field code.  If version is less than or equal to zero,
        // then mark this stream as invalid, if quiet flag is zero,
        // print an error message. Note that if this stream is invalid
        // or the suppress version check flag is non-zero, this
        // function has no effect.  Also note that error messages are
        // not printed for insufficient data in the buffer.

    void throwExceptionIfInputLimitExhausted(const FieldCode::Type& code);
        // Decrement internal input limit value. If after this
        // decrement the input limit becomes negative, an exception
        // object 'TestInStreamException' is created and initialized
        // with the specified data type 'code', and then exception is
        // thrown.  Note that if exception-handling is disabled (i.e.,
        // if 'BDE_BUILD_TARGET_EXC' is not defined), then the
        // function does nothing.

    // NOT DEFINED
    TestInStream(const TestInStream&);
    TestInStream& operator=(const TestInStream&);

    bool operator==(const TestInStream&) const;
    bool operator!=(const TestInStream&) const;

  public:
    // CREATORS
    explicit TestInStream(bslma::Allocator *basicAllocator = 0);
        // Create an empty input byte stream.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The constructed
        // object is useless until a buffer is set with the 'reload' method.

    TestInStream(const char       *buffer,
                 int               numBytes,
                 bslma::Allocator *basicAllocator = 0);
        // Create an input byte stream containing the specified initial
        // 'numBytes' from (a copy of) the specified 'buffer'.  Optionally
        // specify the 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '0 <= numBytes' and
        // 'numBytes' is less than or equal to the length of 'buffer'.

    ~TestInStream();
        // Destroy this input byte stream.

    // MANIPULATORS
    void setQuiet(int flag);
        // Set this stream's quiet mode to the specified 'flag'.  If 'flag' is
        // non-zero, then quiet mode is turned ON, and no error message will be
        // written to the standard output.  If 'flag' is zero, then quiet mode
        // is turned OFF.  Note that quiet mode is turned OFF by default.

    void setSuppressVersionCheck(int flag);
        // Set this stream's suppress-version-check mode to the specified
        // 'flag'.  If 'flag' is non-zero, then suppress-version-check mode is
        // turned ON, and this stream will not check for the existence of
        // version information when reading.  If 'flag' is zero, then
        // suppress-version-check mode is turned OFF, and the version
        // information will be checked.  Note that suppress-version-check mode
        // is turned OFF by default.

    void setInputLimit(int limit);
        // Set the number of input operations allowed on this stream to the
        // specified 'limit' before an exception is thrown.  If 'limit' is less
        // than 0, no exception is to be thrown.  By default, no exception is
        // scheduled.

    void seek(int offset);
        // Set the index of the next byte to be extracted from this stream to
        // the specified 'offset' and validate this stream if it is currently
        // invalid.  The behavior is undefined unless offset is less than or
        // equal to the number of bytes in this stream.

    void reset();
        // Set the index of the next byte to be extracted from this stream to 0
        // (i.e., the beginning of the stream) and validate this stream if it
        // is currently invalid.

    void reload(const char *buffer, int numBytes);
        // Replace this stream's content with (a copy of) the specified initial
        // 'numBytes' from the specified 'buffer' and set the index of the next
        // byte to be extracted from this stream to 0 (i.e., the beginning of
        // the stream).  Also validate this stream if it is currently invalid.
        // The behavior is undefined unless '0 <= numBytes'.

    TestInStream& getLength(int& variable);
        // Verify the type of the next value in this stream, consume that 8-bit
        // unsigned integer or 32-bit signed integer value representing a
        // length (see the package group level documentation) into the
        // specified 'variable' if its type is appropriate, and return a
        // reference to this modifiable stream dependent on the most
        // significant bit of the next byte in the stream; if it is 0 consume
        // an 8-bit unsigned integer, otherwise a 32-bit signed integer after
        // setting this bit to zero.  If the type is incorrect, then this
        // stream is marked invalid and the value of 'variable' is unchanged.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.

    TestInStream& getString(bsl::string& str);
        // Consume a string from this input stream, place that value in the
        // specified 'str', and return a reference to this modifiable stream.
        // If this stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'variable' is undefined.
        // The string must be prefaced by a non-negative integer indicating the
        // number of characters composing the string.  Behavior is undefined if
        // the length indicator is not greater than 0.

    TestInStream& getVersion(int& variable);
        // Verify the type of the next value in this stream, consume that 8-bit
        // unsigned integer value representing a version (see the package group
        // level documentation) into the specified 'variable' if its type is
        // appropriate, and return a reference to this modifiable stream.  If
        // the type is incorrect, then this stream is marked invalid and the
        // value of 'variable' is unchanged.  If this stream is initially
        // invalid, this operation has no effect.  If this function otherwise
        // fails to extract a valid value, this stream is marked invalid and
        // the value of 'variable' is undefined.

    TestInStream& getInt64(bsls::Types::Int64& variable);
        // Verify the type of the next value in this stream, consume that
        // 64-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    TestInStream& getUint64(bsls::Types::Uint64& variable);
        // Verify the type of the next value in this stream, consume that
        // 64-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    TestInStream& getInt56(bsls::Types::Int64& variable);
        // Verify the type of the next value in this stream, consume that
        // 56-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    TestInStream& getUint56(bsls::Types::Uint64& variable);
        // Verify the type of the next value in this stream, consume that
        // 56-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    TestInStream& getInt48(bsls::Types::Int64& variable);
        // Verify the type of the next value in this stream, consume that
        // 48-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    TestInStream& getUint48(bsls::Types::Uint64& variable);
        // Verify the type of the next value in this stream, consume that
        // 48-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    TestInStream& getInt40(bsls::Types::Int64& variable);
        // Verify the type of the next value in this stream, consume that
        // 40-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    TestInStream& getUint40(bsls::Types::Uint64& variable);
        // Verify the type of the next value in this stream, consume that
        // 40-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    TestInStream& getInt32(int& variable);
        // Verify the type of the next value in this stream, consume that
        // 32-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    TestInStream& getUint32(unsigned int& variable);
        // Verify the type of the next value in this stream, consume that
        // 32-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    TestInStream& getInt24(int& variable);
        // Verify the type of the next value in this stream, consume that
        // 24-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    TestInStream& getUint24(unsigned int& variable);
        // Verify the type of the next value in this stream, consume that
        // 24-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    TestInStream& getInt16(short& variable);
        // Verify the type of the next value in this stream, consume that
        // 16-bit signed integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    TestInStream& getUint16(unsigned short& variable);
        // Verify the type of the next value in this stream, consume that
        // 16-bit unsigned integer value into the specified 'variable' if its
        // type is appropriate, and return a reference to this modifiable
        // stream.  If the type is incorrect, then this stream is marked
        // invalid and the value of 'variable' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is marked
        // invalid and the value of 'variable' is undefined.

    TestInStream& getInt8(char&        variable);
    TestInStream& getInt8(signed char& variable);
        // Verify the type of the next value in this stream, consume that 8-bit
        // signed integer value into the specified 'variable' if its type is
        // appropriate, and return a reference to this modifiable stream.  If
        // the type is incorrect, then this stream is marked invalid and the
        // value of 'variable' is unchanged.  If this stream is initially
        // invalid, this operation has no effect.  If this function otherwise
        // fails to extract a valid value, this stream is marked invalid and
        // the value of 'variable' is undefined.

    TestInStream& getUint8(char&          variable);
    TestInStream& getUint8(unsigned char& variable);
        // Verify the type of the next value in this stream, consume that 8-bit
        // unsigned integer value into the specified 'variable' if its type is
        // appropriate, and return a reference to this modifiable stream.  If
        // the type is incorrect, then this stream is marked invalid and the
        // value of 'variable' is unchanged.  If this stream is initially
        // invalid, this operation has no effect.  If this function otherwise
        // fails to extract a valid value, this stream is marked invalid and
        // the value of 'variable' is undefined.

    TestInStream& getFloat64(double& variable);
        // Verify the type of the next value in this stream, consume that IEEE
        // double-precision (8-byte) floating-point value into the specified
        // 'variable' if its type is appropriate, and return a reference to
        // this modifiable stream.  If the type is incorrect, then this stream
        // is marked invalid and the value of 'variable' is unchanged.  If this
        // stream is initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'variable' is undefined.  Note that
        // for non-conforming platforms, this operation may be lossy.

    TestInStream& getFloat32(float& variable);
        // Verify the type of the next value in this stream, consume that IEEE
        // single-precision (4-byte) floating-point value into the specified
        // 'variable' if its type is appropriate, and return a reference to
        // this modifiable stream.  If the type is incorrect, then this stream
        // is marked invalid and the value of 'variable' is unchanged.  If this
        // stream is initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'variable' is undefined.  Note that
        // for non-conforming platforms, this operation may be lossy.

    TestInStream& getArrayInt64(bsls::Types::Int64 *values, int numValues);
        // Verify the type of the next value in this stream, consume that
        // 64-bit signed integer array value into the specified 'values' of the
        // specified 'numValues' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'values' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'values' is undefined.  The behavior is undefined unless
        // '0 <= numValues'.

    TestInStream& getArrayUint64(bsls::Types::Uint64 *values, int numValues);
        // Verify the type of the next value in this stream, consume that
        // 64-bit signed integer array value into the specified 'values' of the
        // specified 'numValues' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'values' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'values' is undefined.  The behavior is undefined unless
        // '0 <= numValues'.

    TestInStream& getArrayInt56(bsls::Types::Int64 *values, int numValues);
        // Verify the type of the next value in this stream, consume that
        // 56-bit signed integer array value into the specified 'values' of the
        // specified 'numValues' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'values' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'values' is undefined.  The behavior is undefined unless
        // '0 <= numValues'.

    TestInStream& getArrayUint56(bsls::Types::Uint64 *values, int numValues);
        // Verify the type of the next value in this stream, consume that
        // 56-bit signed integer array value into the specified 'values' of the
        // specified 'numValues' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'values' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'values' is undefined.  The behavior is undefined unless
        // '0 <= numValues'.

    TestInStream& getArrayInt48(bsls::Types::Int64 *values, int numValues);
        // Verify the type of the next value in this stream, consume that
        // 48-bit signed integer array value into the specified 'values' of the
        // specified 'numValues' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'values' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'values' is undefined.  The behavior is undefined unless
        // '0 <= numValues'.

    TestInStream& getArrayUint48(bsls::Types::Uint64 *values, int numValues);
        // Verify the type of the next value in this stream, consume that
        // 48-bit signed integer array value into the specified 'values' of the
        // specified 'numValues' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'values' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'values' is undefined.  The behavior is undefined unless
        // '0 <= numValues'.

    TestInStream& getArrayInt40(bsls::Types::Int64 *values, int numValues);
        // Verify the type of the next value in this stream, consume that
        // 40-bit signed integer array value into the specified 'values' of the
        // specified 'numValues' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'values' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'values' is undefined.  The behavior is undefined unless
        // '0 <= numValues'.

    TestInStream& getArrayUint40(bsls::Types::Uint64 *values, int numValues);
        // Verify the type of the next value in this stream, consume that
        // 40-bit signed integer array value into the specified 'values' of the
        // specified 'numValues' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'values' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'values' is undefined.  The behavior is undefined unless
        // '0 <= numValues'.

    TestInStream& getArrayInt32(int *values, int numValues);
        // Verify the type of the next value in this stream, consume that
        // 32-bit signed integer array value into the specified 'values' of the
        // specified 'numValues' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'values' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'values' is undefined.  The behavior is undefined unless
        // '0 <= numValues'.

    TestInStream& getArrayUint32(unsigned int *values, int numValues);
        // Verify the type of the next value in this stream, consume
        // that 32-bit unsigned integer array value into the specified
        // 'values' of the specified 'numValues' if its type and
        // length are appropriate, and return a reference to this
        // modifiable stream.  If the type is incorrect, then this
        // stream is marked invalid and the value of 'values' is
        // unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails
        // to extract a valid value, this stream is marked invalid and
        // the value of 'values' is undefined.  The behavior is
        // undefined unless '0 <= numValues'.

    TestInStream& getArrayInt24(int *values, int numValues);
        // Verify the type of the next value in this stream, consume that
        // 24-bit signed integer array value into the specified 'values' of the
        // specified 'numValues' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'values' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'values' is undefined.  The behavior is undefined unless
        // '0 <= numValues'.

    TestInStream& getArrayUint24(unsigned int *values, int numValues);
        // Verify the type of the next value in this stream, consume
        // that 24-bit unsigned integer array value into the specified
        // 'values' of the specified 'numValues' if its type and
        // length are appropriate, and return a reference to this
        // modifiable stream.  If the type is incorrect, then this
        // stream is marked invalid and the value of 'values' is
        // unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails
        // to extract a valid value, this stream is marked invalid and
        // the value of 'values' is undefined.  The behavior is
        // undefined unless '0 <= numValues'.

    TestInStream& getArrayInt16(short *values, int numValues);
        // Verify the type of the next value in this stream, consume that
        // 16-bit signed integer array value into the specified 'values' of the
        // specified 'numValues' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'values' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'values' is undefined.  The behavior is undefined unless
        // '0 <= numValues'.

    TestInStream& getArrayUint16(unsigned short *values, int numValues);
        // Verify the type of the next value in this stream, consume
        // that 16-bit unsigned integer array value into the specified
        // 'values' of the specified 'numValues' if its type and
        // length are appropriate, and return a reference to this
        // modifiable stream.  If the type is incorrect, then this
        // stream is marked invalid and the value of 'values' is
        // unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails
        // to extract a valid value, this stream is marked invalid and
        // the value of 'values' is undefined.  The behavior is
        // undefined unless '0 <= numValues'.

    TestInStream& getArrayInt8(char        *values, int numValues);
    TestInStream& getArrayInt8(signed char *values, int numValues);
        // Verify the type of the next value in this stream, consume that 8-bit
        // signed integer array value into the specified 'values' of the
        // specified 'numValues' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'values' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'values' is undefined.  The behavior is undefined unless
        // '0 <= numValues'.

    TestInStream& getArrayUint8(char          *values, int numValues);
    TestInStream& getArrayUint8(unsigned char *values, int numValues);
        // Verify the type of the next value in this stream, consume that 8-bit
        // unsigned integer array value into the specified 'values' of the
        // specified 'numValues' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type is
        // incorrect, then this stream is marked invalid and the value of
        // 'values' is unchanged.  If this stream is initially invalid, this
        // operation has no effect.  If this function otherwise fails to
        // extract a valid value, this stream is marked invalid and the value
        // of 'values' is undefined.  The behavior is undefined unless
        // '0 <= numValues'.

    TestInStream& getArrayFloat64(double *values, int numValues);
        // Verify the type of the next value in this stream, consume
        // that IEEE double-precision (8-byte) floating-point array
        // value into the specified 'values' of the specified
        // 'numValues' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type
        // is incorrect, then this stream is marked invalid and the
        // value of 'values' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'values' is
        // undefined.  The behavior is undefined unless '0 <=
        // numValues'.  Note that for non-conforming platforms, this
        // operation may be lossy.

    TestInStream& getArrayFloat32(float *values, int numValues);
        // Verify the type of the next value in this stream, consume
        // that IEEE single-precision (4-byte) floating-point array
        // value into the specified 'values' of the specified
        // 'numValues' if its type and length are appropriate, and
        // return a reference to this modifiable stream.  If the type
        // is incorrect, then this stream is marked invalid and the
        // value of 'values' is unchanged.  If this stream is
        // initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid value, this
        // stream is marked invalid and the value of 'values' is
        // undefined.  The behavior is undefined unless '0 <=
        // numValues'.  Note that for non-conforming platforms, this
        // operation may be lossy.

    void invalidate();
        // Put this input stream in an invalid state.  This function has no
        // effect if this stream is already invalid.  Note that this function
        // should be called whenever a value extracted from this stream is
        // determined to be invalid, inconsistent, or otherwise incorrect.

    // ACCESSORS
    bool isValid() const;
        // Return 'true' if this stream is valid, and 'false'
        // otherwise. An invalid stream is a stream in which
        // insufficient or invalid data was detected during an
        // extraction operation.  Note that an empty stream will be
        // valid unless an extraction attempt or explicit invalidation
        // causes it to be otherwise.

    operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream for which an output operation was
        // detected to have failed.

    bool isEmpty() const;
        // Return 'true' if this stream is empty, and 'false'
        // otherwise.  The behavior is undefined unless this stream is
        // valid.  Note that this function enables higher-level
        // components to verify that, after successfully reading all
        // expected data, no data remains.

    int length() const;
        // Return the total number of bytes stored in this stream.

    int cursor() const;
        // Return the index of the next byte to be extracted from this stream.
        // The behavior is undefined unless this stream is valid.

    bool isQuiet() const;
        // Return 'true' if this stream's quiet mode is ON, and 'false'
        // otherwise.

    bool isSuppressVersionCheck() const;
        // Return 'true' if this stream's version-checking mode is OFF, and
        // 'false' otherwise.

    int inputLimit() const;
        // Return the current number of input requests left before an exception
        // is thrown.  A negative value indicated that no exception is
        // scheduled.
};

bsl::ostream& operator<<(bsl::ostream& stream, const TestInStream& obj);
    // Write the specified 'obj' to the specified output 'stream' in some
    // reasonable (multi-line) format, and return a reference to 'stream'


// ============================================================================
//                 INLINE DEFINITIONS
// ============================================================================

                         // ------------------
                         // class TestInStream
                         // ------------------

// MANIPULATORS
inline
void TestInStream::validate()
{
    d_validFlag = true;
}

inline
void TestInStream::throwExceptionIfInputLimitExhausted(
                                                   const FieldCode::Type& code)
{
#ifdef BDE_BUILD_TARGET_EXC
    if (0 <= d_inputLimit) {
        --d_inputLimit;
        if (0 > d_inputLimit) {
            throw TestInStreamException(code);
        }
    }
#endif
}

inline
void TestInStream::setQuiet(int flag)
{
    d_quietFlag = (bool)flag;
}

inline
void TestInStream::setSuppressVersionCheck(int flag)
{
    d_suppressVersionCheckFlag = (bool)flag;
}

inline
void TestInStream::setInputLimit(int limit)
{
    d_inputLimit = limit;
}

inline
void TestInStream::invalidate()
{
    d_validFlag = 0;
}

inline
int TestInStream::length() const
{
    return d_buffer.size();
}

inline
int TestInStream::cursor() const
{
    return d_cursor;
}

inline
void TestInStream::seek(int offset)
{
    BSLS_ASSERT_SAFE(0 <= offset);

    d_cursor = offset;
    d_validFlag = 1;
}

inline
void TestInStream::reset()
{
    d_validFlag = 1;
    d_cursor = 0;
}

inline
void TestInStream::reload(const char *buffer, int numBytes)
{
    BSLS_ASSERT_SAFE(0 <= numBytes);
    invalidate();
    d_buffer.assign(buffer, buffer + numBytes);
    d_validFlag = 1;
    d_cursor = 0;
}

// ACCESSORS
inline
bool TestInStream::isValid() const
{
    return d_validFlag;
}

inline
TestInStream::operator const void *() const
{
    return isValid() ? this : 0;
}

inline
bool TestInStream::isEmpty() const
{
    return cursor() >= length();
}


inline
bool TestInStream::isQuiet() const
{
    return d_quietFlag;
}

inline
bool TestInStream::isSuppressVersionCheck() const
{
    return d_suppressVersionCheckFlag;
}

inline
int TestInStream::inputLimit() const
{
    return d_inputLimit;
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
