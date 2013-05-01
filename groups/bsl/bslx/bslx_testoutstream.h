// bslx_testoutstream.h                                               -*-C++-*-
#ifndef INCLUDED_BSLX_TESTOUTSTREAM
#define INCLUDED_BSLX_TESTOUTSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enable externalization of fundamental types with identification.
//
//@CLASSES:
//  bslx::TestOutStream: byte-array-based output stream class
//
//@SEE_ALSO: bslx_testinstream
//
//@DESCRIPTION: This component implements a byte-array-based output
// stream class that provides platform-independent output methods
// ("externalization") on values, and arrays of values, of fundamental
// types, and on 'bsl::string'.
//
// This component is intended to be used in conjunction with the
// 'bslx_testinstream' "unexternalization" component.  Each output method of
// 'bslx::TestOutStream' writes a value or a homogeneous array of values to an
// internally managed buffer.  The values are formatted to be readable by the
// corresponding 'bslx::TestInStream' method.  In general, the user cannot rely
// on any other mechanism to read data written by 'bslx::TestOutStream' unless
// that mechanism explicitly states its ability to do so.
//
// The supported types and required content are listed in the table
// below.  All of the fundamental types in the table may be output as
// scalar values or as homogeneous arrays.  'bsl::string' is output as
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
// This component also supports compact streaming of integer types.  In
// particular, 64-bit integers can be streamed as 40, 48, 56, or 64-bit values,
// and 32-bit integers can be streamed as 24 or 32-bit values, at the user's
// discretion.  In all cases, the least significant bytes of the fundamental
// integer type are written to the stream.  Note that, for signed types, this
// truncation may not preserve the sign of the streamed value; it is the user's
// responsibility to choose output methods appropriate to the data.
//
// Note that the values are stored in 'Big Endian' (i.e., network byte order)
// format.  Also note that output streams can be *invalidated* explicitly and
// queried for *validity*.  Writing to an initially invalid stream has no
// effect.  Whenever an output operation fails, the stream should be
// invalidated explicitly.
//
///EXTERNALIZATION AND VALUE SEMANTICS
///-----------------------------------
// The concept of "externalization" in this component is intentionally
// specialized to support streaming the *values* of entities atomically.  In
// particular, streaming an array of a specific length is considered to be an
// atomic operation.  It is therefore an error, for example, to stream out an
// array of 'int' of length three and then stream in those three 'int' values
// as three scalar 'int' variables.  Similarly, it is an error to stream out an
// array of length *L* and stream back in an array of length other than *L*.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:
///- - - - -
// The 'bslx::TestOutStream can be used to externalize values in a
// platform-neutral way.  Writing out fundamental C++ types and
// 'bsl::string' requires no additional work on the part of the
// client; the client can simply use the stream directly.  The
// following code serializes a few representative values using a
// 'bslx::TestOutStream', and then writes the contents of the
// 'bslx' stream's buffer to 'stdout':
//..
//  int main(int argc, char **argv)
//  {
//      // Create a stream and write out some values.
//      bslx::TestOutStream stream;
//      stream.putInt32(1);
//      stream.putInt32(2);
//      stream.putInt8('c');
//      stream.putString(bsl::string("hello"));
//
//      // Verify the results on 'stdout'.
//      const char *theChars = outStream.data();
//      int length = outStream.length();
//      for (int i = 0; i < length; ++i) {
//          if (bsl::isalnum(static_cast<unsigned char>(theChars[i]))) {
//              cout << "nextByte (char): " << theChars[i] << endl;
//          }
//          else {
//              cout << "nextByte (int): " << (int)theChars[i] << endl;
//          }
//      }
//      return 0;
//  }
//..
// Executing the above code results in the following output:
//..
//  nextByte (int): 6
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 1
//  nextByte (int): 6
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 2
//  nextByte (int): 0
//  nextByte (char): c
//  nextByte (int): 0
//  nextByte (int): 5
//  nextByte (int): 0
//  nextByte (int): 5
//  nextByte (char): h
//  nextByte (char): e
//  nextByte (char): l
//  nextByte (char): l
//  nextByte (char): o
//..
// Note the numeric value 5 prepended to the string literal "hello".  This is
// because 'const char *' types are picked up by the template specialization
// for type 'bsl::string', which outputs the string length before emitting the
// characters.
//
// See the 'bslx_testinstream' component usage example for a more practical
// example of using test streams with a value-semantic type.

#ifndef INCLUDED_BSLX_BYTEOUTSTREAM
#include <bslx_byteoutstream.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace bslx {

                         // ===================
                         // class TestOutStream
                         // ===================

class TestOutStream {
    // This class implements output methods to externalize fundamental types
    // and their associated type identification data in byte representation.
    // It stores the accumulated result in network byte order format.

    // DATA
    ByteOutStream d_imp;                 // byte out stream implementation
    int           d_makeNextInvalidFlag; // if 'true' set to output invalid
                                         // data for next operation

    // FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream&, const TestOutStream&);

    // NOT IMPLEMENTED
    TestOutStream(const TestOutStream&);
    TestOutStream& operator=(const TestOutStream&);
    bool operator==(const TestOutStream&) const;
    bool operator!=(const TestOutStream&) const;

  public:
    // CREATORS
    explicit TestOutStream(bslma::Allocator *basicAllocator = 0);
        // Create an empty output byte stream.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~TestOutStream();
        // Destroy this object.

    // MANIPULATORS
    void invalidate();
        // Put this output stream in an invalid state.  This function has no
        // effect if this stream is already invalid.  Note that this function
        // should be called whenever a write operation to this stream fails.

    void makeNextInvalid();
        // Make the next output operation format invalid data to this output
        // stream.  The invalid data can be detected by a corresponding
        // 'TestInStream' object.

    TestOutStream& putLength(int numValues);
        // Format the specified 'numValues' as a 8-bit unsigned integer if
        // 'numValues' is less than 128, otherwise as a 32-bit integer to this
        // output stream with its most significant bit set and return a
        // reference to this modifiable stream.

    TestOutStream& putString(const bsl::string& str);
        // Format as an array of 8-bit integers the specified 'str' to this
        // output stream and return a reference to this modifiable stream.

    TestOutStream& putVersion(int version);
        // Format the specified 'version' as an 8-bit unsigned integer to this
        // output stream and return a reference to this modifiable stream.

    TestOutStream& putInt64(bsls::Types::Int64 value);
        // Format as a signed integer the least significant 64 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putUint64(bsls::Types::Int64 value);
        // Format as an unsigned integer the least significant 64 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putInt56(bsls::Types::Int64 value);
        // Format as a signed integer the least significant 56 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putUint56(bsls::Types::Int64 value);
        // Format as an unsigned integer the least significant 56 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putInt48(bsls::Types::Int64 value);
        // Format as a signed integer the least significant 48 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putUint48(bsls::Types::Int64 value);
        // Format as an unsigned integer the least significant 48 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putInt40(bsls::Types::Int64 value);
        // Format as a signed integer the least significant 40 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putUint40(bsls::Types::Int64 value);
        // Format as an unsigned integer the least significant 40 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putInt32(int value);
        // Format as a signed integer the least significant 32 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putUint32(unsigned int value);
        // Format as an unsigned integer the least significant 32 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putInt24(int value);
        // Format as a signed integer the least significant 24 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putUint24(int value);
        // Format as an unsigned integer the least significant 24 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putInt16(int value);
        // Format as a signed integer the least significant 16 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putUint16(int value);
        // Format as an unsigned integer the least significant 16 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putInt8(int value);
        // Format as a signed integer the least significant 8 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putUint8(int value);
        // Format as an unsigned integer the least significant 8 bits of the
        // specified 'value' to this output stream and return a reference to
        // this modifiable stream.

    TestOutStream& putFloat64(double value);
        // Format as an IEEE double-precision floating point number the most
        // significant 8 bytes in the specified 'value' to this output stream
        // and return a reference to this modifiable stream.  Note that, for
        // non-conforming platforms, the "most significant 8 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

    TestOutStream& putFloat32(float value);
        // Format as an IEEE single-precision floating point number the most
        // significant 4 bytes in the specified 'value' to this output stream
        // and return a reference to this modifiable stream.  Note that, for
        // non-conforming platforms, the "most significant 4 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

    TestOutStream& putArrayInt64(const bsls::Types::Int64 *values,
                                 int                       numValues);
        // Format as an atomic sequence of signed integers the least
        // significant 64 bits of each of the specified 'numValues' leading
        // entries in the specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= numValues'.

    TestOutStream& putArrayUint64(const bsls::Types::Uint64 *values,
                                  int                        numValues);
        // Format as an atomic sequence of unsigned integers the least
        // significant 64 bits of each of the specified 'numValues' leading
        // entries in the specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= numValues'.

    TestOutStream& putArrayInt56(const bsls::Types::Int64 *values,
                                 int                       numValues);
        // Format as an atomic sequence of signed integers the least
        // significant 56 bits of each of the specified 'numValues' leading
        // entries in the specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= numValues'.

    TestOutStream& putArrayUint56(const bsls::Types::Uint64 *values,
                                  int                        numValues);
        // Format as an atomic sequence of unsigned integers the least
        // significant 56 bits of each of the specified 'numValues' leading
        // entries in the specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= numValues'.

    TestOutStream& putArrayInt48(const bsls::Types::Int64 *values,
                                 int                       numValues);
        // Format as an atomic sequence of signed integers the least
        // significant 48 bits of each of the specified 'numValues' leading
        // entries in the specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= numValues'.

    TestOutStream& putArrayUint48(const bsls::Types::Uint64 *values,
                                  int                        numValues);
        // Format as an atomic sequence of unsigned integers the least
        // significant 48 bits of each of the specified 'numValues' leading
        // entries in the specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= numValues'.

    TestOutStream& putArrayInt40(const bsls::Types::Int64 *values,
                                 int                       numValues);
        // Format as an atomic sequence of signed integers the least
        // significant 40 bits of each of the specified 'numValues' leading
        // entries in the specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= numValues'.

    TestOutStream& putArrayUint40(const bsls::Types::Uint64 *values,
                                  int                        numValues);
        // Format as an atomic sequence of unsigned integers the least
        // significant 40 bits of each of the specified 'numValues' leading
        // entries in the specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= numValues'.

    TestOutStream& putArrayInt32(const int *values, int numValues);
        // Format as an atomic sequence of signed integers the least
        // significant 32 bits of each of the specified 'numValues' leading
        // entries in the specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= numValues'.

    TestOutStream& putArrayUint32(const unsigned int *values, int numValues);
        // Format as an atomic sequence of unsigned integers the least
        // significant 32 bits of each of the specified 'numValues' leading
        // entries in the specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= numValues'.

    TestOutStream& putArrayInt24(const int *values, int numValues);
        // Format as an atomic sequence of signed integers the least
        // significant 24 bits of each of the specified 'numValues' leading
        // entries in the specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= numValues'.

    TestOutStream& putArrayUint24(const unsigned int *values, int numValues);
        // Format as an atomic sequence of unsigned integers the least
        // significant 24 bits of each of the specified 'numValues' leading
        // entries in the specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= numValues'.

    TestOutStream& putArrayInt16(const short *values, int numValues);
        // Format as an atomic sequence of signed integers the least
        // significant 16 bits of each of the specified 'numValues' leading
        // entries in the specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= numValues'.

    TestOutStream& putArrayUint16(const unsigned short *values, int numValues);
        // Format as an atomic sequence of unsigned integers the least
        // significant 16 bits of each of the specified 'numValues' leading
        // entries in the specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is undefined
        // unless '0 <= numValues'.

    TestOutStream& putArrayInt8(const char        *values, int numValues);
    TestOutStream& putArrayInt8(const signed char *values, int numValues);
        // Format as an atomic sequence of signed integers the 8 bits
        // of each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.

    TestOutStream& putArrayUint8(const char *values, int numValues);
    TestOutStream& putArrayUint8(const unsigned char *values, int numValues);
        // Format as an atomic sequence of unsigned integers the 8
        // bits of each of the specified 'numValues' leading entries
        // in the specified 'values' to this output stream and return
        // a reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.

    TestOutStream& putArrayFloat64(const double *values, int numValues);
        // Format as an atomic sequence of IEEE double-precision floating point
        // numbers the most significant 8 bytes in each of the specified
        // 'numValues' leading entries in the specified 'values' to this output
        // stream and return a reference to this modifiable stream.  The
        // behavior is undefined unless '0 <= numValues'.  Note that, for
        // non-conforming platforms, the "most significant 8 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

    TestOutStream& putArrayFloat32(const float *values, int numValues);
        // Format as an atomic sequence of IEEE single-precision floating point
        // numbers the most significant 4 bytes in each of the specified
        // 'numValues' leading entries in the specified 'values' to this output
        // stream and return a reference to this modifiable stream.  The
        // behavior is undefined unless '0 <= numValues'.  Note that, for
        // non-conforming platforms, the "most significant 4 bytes" might not
        // be contiguous bits, and that this operation may be lossy.

    void removeAll();
        // Remove all content in this stream.

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream for which an output operation was
        // detected to have failed.

    const char *data() const;
        // Return the address of the contiguous, non-modifiable internal memory
        // buffer of this stream.  The address will remain valid as long as
        // this array is not destroyed or modified (i.e., the current capacity
        // is not exceeded).  The behavior of accessing elements outside the
        // range '[ data() .. data() + (length() - 1) ]' is undefined.

    int length() const;
        // Return the number of bytes in this stream.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&        stream,
                         const TestOutStream& obj);
    // Write the specified 'obj' to the specified output 'stream' in some
    // reasonable (multi-line) format, and return a reference to 'stream'

// ============================================================================
//                 INLINE DEFINITIONS
// ============================================================================

                         // -------------------
                         // class TestOutStream
                         // -------------------

// MANIPULATORS
inline
void TestOutStream::invalidate()
{
    d_imp.invalidate();
}

inline
TestOutStream& TestOutStream::putString(const bsl::string& str)
{
    d_imp.putString(str);
    return *this;
}

inline
void TestOutStream::removeAll()
{
    d_imp.removeAll();
}

inline
void TestOutStream::makeNextInvalid()
{
    d_makeNextInvalidFlag = 1;
}

// ACCESSORS
inline
TestOutStream::operator const void *() const
{
    return d_imp;
}

inline
const char *TestOutStream::data() const
{
    return d_imp.data();
}

inline
int TestOutStream::length() const
{
    return d_imp.length();
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
// Copyright (c) 2013. Bloomberg Finance L.P.
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
// ----------------------------- END-OF-FILE ---------------------------------
