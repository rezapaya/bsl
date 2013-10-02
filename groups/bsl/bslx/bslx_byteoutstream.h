// bslx_byteoutstream.h                                               -*-C++-*-
#ifndef INCLUDED_BSLX_BYTEOUTSTREAM
#define INCLUDED_BSLX_BYTEOUTSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enable externalization of fundamental types to byte representation.
//
//@CLASSES:
//  bslx::ByteOutStream: byte-array-based output stream class
//
//@SEE_ALSO: bslx_byteinstream
//
//@DESCRIPTION: This component implements a byte-array-based output
// stream class that provides platform-independent output methods
// ("externalization") on values, and arrays of values, of fundamental
// types, and on 'bsl::string'.
//
// This component is intended to be used in conjunction with the
// 'bslx_byteinstream' "unexternalization" component.  Each output
// method of 'bslx::ByteOutStream' writes a value or a homogeneous
// array of values to an internally managed buffer.  The values are
// formatted to be readable by the corresponding 'bslx::ByteInStream'
// method. In general, the user cannot rely on any other mechanism to
// read data written by 'bslx::ByteOutStream' unless that mechanism
// explicitly states its ability to do so.
//
// The supported types and required content are listed in the table
// below.  All of the fundamental types in the table may be output as
// scalar values or as homogeneous arrays.  'bsl::string' is output as
// a logical scalar string.  Note that 'Int64' and 'Uint64' denote
// 'bsls::Types::Int64' and 'bsls::Types::Uint64', which in turn are
// 'typedef' names for the signed and unsigned 64-bit integer types,
// respectively, on the host platform.
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
// format.
//
// Note that output streams can be *invalidated* explicitly and queried for
// *validity*.  Writing to an initially invalid stream has no effect.  Whenever
// an output operation fails, the stream should be invalidated explicitly.
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
///Example 1
///- - - - -
// The 'bslx::ByteOutStream' can be used to externalize values in a
// platform-neutral way.  Writing out fundamental C++ types and
// 'bsl::string' requires no additional work on the part of the
// client; the client can simply use the stream directly. The
// following code serializes a few representative values using a
// 'bslx::ByteOutStream', and then writes the contents of the 'bslx'
// stream's buffer to 'stdout':
//..
//  int main(int argc, char **argv)
//  {
//      // Create a stream and write out some values.
//      bslx::ByteOutStream stream;
//      outStream.putInt32(1);
//      outStream.putInt32(2);
//      outStream.putInt8('c');
//      outStream.putString(bsl::string("hello"));
//
//      // Verify the results on 'stdout'.
//      const char *theChars = outStream.data();
//      int length = outStream.length();
//      for (int i = 0; i < length; ++i) {
//          if(bsl::isalnum(static_cast<unsigned char>(theChars[i]))) {
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
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 1
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 2
//  nextByte (char): c
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
// See the 'bslx_byteinstream' component usage example for a more practical
// example of using 'bslx' streams with a value-semantic type.

#ifndef INCLUDED_BSLX_BYTESTREAMIMPUTIL
#include <bslx_bytestreamimputil.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace bslx {

                         // ===================
                         // class ByteOutStream
                         // ===================

class ByteOutStream {
    // This class provides output methods to externalize values, and C-style
    // arrays of values, of the fundamental integral and floating-point types,
    // as well as 'bsl::string' values, in a documented format.

    // DATA
    bsl::vector<char> d_buffer;     // byte buffer to write to
    int               d_validFlag;  // stream validity flag, 'true' if stream
                                    // is in valid state, 'false' otherwise

    // FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream&, const ByteOutStream&);

    // MANIPULATORS
    void validate();
        // Put this input stream in an valid state. This function
        // has no effect if this stream is already valid.

    // NOT IMPLEMENTED
    ByteOutStream(const ByteOutStream&);
    ByteOutStream& operator=(const ByteOutStream&);
    bool operator==(const ByteOutStream&) const;
    bool operator!=(const ByteOutStream&) const;

  public:
    // CREATORS
    explicit ByteOutStream(bslma::Allocator *basicAllocator = 0);
        // Create an empty output byte stream.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used to supply memory.

    explicit ByteOutStream(int              initialCapacity,
                           bslma::Allocator *basicAllocator = 0);
        // Create an empty output byte stream having an initial buffer capacity
        // of at least the specified 'initialCapacity'.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used to supply memory.

    ~ByteOutStream();
        // Destroy this object.

    // MANIPULATORS
    void invalidate();
        // Put this output stream in an invalid state.  This function has no
        // effect if this stream is already invalid.  Note that this function
        // should be called whenever a write operation to this stream fails.

    ByteOutStream& putLength(int length);
        // Format the specified non-negative 'length' to this output stream and
        // return a reference to this modifiable stream.
        //
        // If 'length' is less than 128, then write the least significant byte
        // of 'length'.  Otherwise, write the least significant four bytes of
        // 'length' in network byte order, but with the most significant output
        // bit set to 1.

    ByteOutStream& putVersion(int version);
        // Format the specified non-negative 'version' to this output stream
        // and return a reference to this modifiable stream.
        //
        // Write the least significant byte of 'version'.

    ByteOutStream& putInt64(bsls::Types::Int64 value);
        // Format the least significant 64 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant eight bytes of 'value' in network byte
        // order.

    ByteOutStream& putUint64(bsls::Types::Int64 value);
        // Format the least significant 64 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant eight bytes of 'value' in network byte
        // order.

    ByteOutStream& putInt56(bsls::Types::Int64 value);
        // Format the least significant 56 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant seven bytes of 'value' in network byte
        // order.

    ByteOutStream& putUint56(bsls::Types::Int64 value);
        // Format the least significant 56 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant seven bytes of 'value' in network byte
        // order.

    ByteOutStream& putInt48(bsls::Types::Int64 value);
        // Format the least significant 48 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant six bytes of 'value' in network byte
        // order.

    ByteOutStream& putUint48(bsls::Types::Int64 value);
        // Format the least significant 48 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant six bytes of 'value' in network byte
        // order.

    ByteOutStream& putInt40(bsls::Types::Int64 value);
        // Format the least significant 40 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant five bytes of 'value' in network byte
        // order.

    ByteOutStream& putUint40(bsls::Types::Int64 value);
        // Format the least significant 40 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant five bytes of 'value' in network byte
        // order.

    ByteOutStream& putInt32(int value);
        // Format the least significant 32 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant four bytes of 'value' in network byte
        // order.

    ByteOutStream& putUint32(unsigned int value);
        // Format the least significant 32 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant four bytes of 'value' in network byte
        // order.

    ByteOutStream& putInt24(int value);
        // Format the least significant 24 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant three bytes of 'value' in network byte
        // order.

    ByteOutStream& putUint24(int value);
        // Format the least significant 24 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant three bytes of 'value' in network byte
        // order.

    ByteOutStream& putInt16(int value);
        // Format the least significant 16 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant two bytes of 'value' in network byte
        // order.

    ByteOutStream& putUint16(int value);
        // Format the least significant 16 bits of the specified 'value' to
        // this output stream and return a reference to this modifiable stream.
        //
        // Write the least significant two bytes of 'value' in network byte
        // order.

    ByteOutStream& putInt8(int value);
        // Format the least significant 8 bits of the specified 'value' to this
        // output stream and return a reference to this modifiable stream.
        //
        // Write the least significant byte of 'value'.

    ByteOutStream& putUint8(int value);
        // Format the least significant 8 bits of the specified 'value' to this
        // output stream and return a reference to this modifiable stream.
        //
        // Write the least significant byte of 'value'.

    ByteOutStream& putFloat64(double value);
        // Format the most significant 64 bits in the specified 'value' to this
        // output stream and return a reference to this modifiable stream.
        // Note that, for non-IEEE-conforming platforms, the "most significant
        // 64 bits" of 'value' might not be contiguous, and that this operation
        // may be lossy.
        //
        // Format 'value' as an IEEE double-precision floating point number and
        // write those eight bytes in network byte order.

    ByteOutStream& putFloat32(float value);
        // Format the most significant 32 bits in the specified 'value' to this
        // output stream and return a reference to this modifiable stream.
        // Note that, for non-IEEE-conforming platforms, the "most significant
        // 32 bits" of 'value' might not be contiguous, and that this operation
        // may be lossy.
        //
        // Format 'value' as an IEEE single-precision floating point number and
        // write those four bytes in network byte order.

    ByteOutStream& putString(const bsl::string& value);
        // Format the specified 'value' to this output stream and return
        // a reference to this modifiable stream.
        //
        // Write 'value.length()' using the 'putLength' format, then
        // write each character of 'value' as a one-byte integer.

    ByteOutStream& putArrayInt64(const bsls::Types::Int64 *values,
                                 int                       numValues);
        // Format as an atomic sequence the least significant 64 bits
        // of each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.
        //
        // For each element of 'values', write the least significant
        // eight bytes in network byte order.

    ByteOutStream& putArrayUint64(const bsls::Types::Uint64 *values,
                                  int                        numValues);
        // Format as an atomic sequence the least significant 64 bits
        // of each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.
        //
        // For each element of 'values', write the least significant
        // eight bytes in network byte order.

    ByteOutStream& putArrayInt56(const bsls::Types::Int64 *values,
                                 int                       numValues);
        // Format as an atomic sequence the least significant 56 bits
        // of each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.
        //
        // For each element of 'values', write the least significant
        // seven bytes in network byte order.

    ByteOutStream& putArrayUint56(const bsls::Types::Uint64 *values,
                                  int                        numValues);
        // Format as an atomic sequence the least significant 56 bits
        // of each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.
        //
        // For each element of 'values', write the least significant
        // seven bytes in network byte order.

    ByteOutStream& putArrayInt48(const bsls::Types::Int64 *values,
                                 int                       numValues);
        // Format as an atomic sequence the least significant 48 bits
        // of each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.
        //
        // For each element of 'values', write the least significant
        // six bytes in network byte order.

    ByteOutStream& putArrayUint48(const bsls::Types::Uint64 *values,
                                  int                        numValues);
        // Format as an atomic sequence the least significant 48 bits
        // of each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.
        //
        // For each element of 'values', write the least significant
        // six bytes in network byte order.

    ByteOutStream& putArrayInt40(const bsls::Types::Int64 *values,
                                 int                       numValues);
        // Format as an atomic sequence the least significant 40 bits
        // of each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.
        //
        // For each element of 'values', write the least significant
        // five bytes in network byte order.

    ByteOutStream& putArrayUint40(const bsls::Types::Uint64 *values,
                                  int                        numValues);
        // Format as an atomic sequence the least significant 40 bits
        // of each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.
        //
        // For each element of 'values', write the least significant
        // five bytes in network byte order.

    ByteOutStream& putArrayInt32(const int *values, int numValues);
        // Format as an atomic sequence the least significant 32 bits
        // of each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.
        //
        // For each element of 'values', write the least significant
        // four bytes in network byte order.

    ByteOutStream& putArrayUint32(const unsigned int *values, int numValues);
        // Format as an atomic sequence the least significant 32 bits
        // of each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.
        //
        // For each element of 'values', write the least significant
        // four bytes in network byte order.

    ByteOutStream& putArrayInt24(const int *values, int numValues);
        // Format as an atomic sequence the least significant 24 bits
        // of each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.
        //
        // For each element of 'values', write the least significant
        // three bytes in network byte order.

    ByteOutStream& putArrayUint24(const unsigned int *values, int numValues);
        // Format as an atomic sequence the least significant 24 bits
        // of each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.
        //
        // For each element of 'values', write the least significant
        // three bytes in network byte order.

    ByteOutStream& putArrayInt16(const short *values, int numValues);
        // Format as an atomic sequence the least significant 16 bits
        // of each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.
        //
        // For each element of 'values', write the least significant
        // two bytes in network byte order.

    ByteOutStream& putArrayUint16(const unsigned short *values, int numValues);
        // Format as an atomic sequence the least significant 16 bits
        // of each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.
        //
        // For each element of 'values', write the least significant
        // two bytes in network byte order.

    ByteOutStream& putArrayInt8(const char        *values, int numValues);
    ByteOutStream& putArrayInt8(const signed char *values, int numValues);
        // Format as an atomic sequence the least significant 8 bits
        // of each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.
        //
        // Write each byte of 'values' in order.

    ByteOutStream& putArrayUint8(const char          *values, int numValues);
    ByteOutStream& putArrayUint8(const unsigned char *values, int numValues);
        // Format as an atomic sequence the least significant 8 bits
        // of each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.
        //
        // Write each byte of 'values' in order.

    ByteOutStream& putArrayFloat64(const double *values, int numValues);
        // Format as an atomic sequence the most significant 64 bits
        // in each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.  Note that, for
        // non-IEEE-conforming platforms, the "most significant 64 bits"
        // might not be contiguous, and that this operation may be lossy.
        //
        // For each element of 'values', format the value as an IEEE
        // double-precision floating point number and write those
        // eight bytes in network byte order.

    ByteOutStream& putArrayFloat32(const float *values, int numValues);
        // Format as an atomic sequence the most significant 32 bits
        // in each of the specified 'numValues' leading entries in the
        // specified 'values' to this output stream and return a
        // reference to this modifiable stream.  The behavior is
        // undefined unless '0 <= numValues'.  Note that, for
        // non-IEEE-conforming platforms, the "most significant 32 bits"
        // might not be contiguous, and that this operation may be lossy.
        //
        // For each element of 'values', format the value as an IEEE
        // single-precision floating point number and write those four
        // bytes in network byte order.

    void removeAll();
        // Remove all content in this stream.

    void reserveCapacity(int newCapacity);
        // Set the internal buffer size of this stream to be at least the
        // specified 'newCapacity'.

    // ACCESSORS
    bool isValid() const;
        // Return 'true' if this stream is valid, and 'false' otherwise.
        // An invalid stream is a stream for which an output operation was
        // detected to have failed.

    operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream for which an output operation was
        // detected to have failed.

    const char *data() const;
        // Return the address of the contiguous, non-modifiable internal memory
        // buffer of this stream.  The address will remain valid as long as
        // this stream is not destroyed or modified.  The behavior of accessing
        // elements outside the range '[ data() .. data() + (length() - 1) ]'
        // is undefined.

    int length() const;
        // Return the number of bytes in this stream.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&        stream,
                         const ByteOutStream& object);
    // Write the specified 'object' to the specified output 'stream' in some
    // reasonable (multi-line) format, and return a reference to 'stream'.

// ============================================================================
//                 INLINE DEFINITIONS
// ============================================================================

                         // -------------------
                         // class ByteOutStream
                         // -------------------


// MANIPULATORS
inline
void ByteOutStream::validate()
{
    d_validFlag = true;
}

// CREATORS
inline
ByteOutStream::ByteOutStream(bslma::Allocator *basicAllocator)
: d_buffer(basicAllocator)
, d_validFlag(true)
{
}

inline
ByteOutStream::ByteOutStream(int               initialCapacity,
                             bslma::Allocator *basicAllocator)
: d_buffer(basicAllocator)
, d_validFlag(true)
{
    BSLS_ASSERT_SAFE(0 <= initialCapacity);
    d_buffer.reserve(initialCapacity);
}

inline
ByteOutStream::~ByteOutStream()
{
}

// MANIPULATORS
inline
void ByteOutStream::invalidate()
{
    d_validFlag = false;
}

inline
ByteOutStream& ByteOutStream::putLength(int length)
{
    BSLS_ASSERT_SAFE(0 <= length);

    if (length > 127) {
        putInt32(length | (1 << 31));
    }
    else {
        putInt8(length);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putVersion(int version)
{
    return putUint8(version);
}

inline
ByteOutStream& ByteOutStream::putInt64(bsls::Types::Int64 value)
{
    char buffer[ByteStreamImpUtil::SIZEOF_INT64] = {0};

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    ByteStreamImpUtil::putInt64(buffer, value);
    bsl::copy(
        buffer,
        buffer + ByteStreamImpUtil::SIZEOF_INT64,
        bsl::back_inserter(d_buffer));
    validate();

    return *this;
}

inline
ByteOutStream& ByteOutStream::putUint64(bsls::Types::Int64 value)
{
    char buffer[ByteStreamImpUtil::SIZEOF_INT64] = {0};
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    ByteStreamImpUtil::putInt64(buffer, value);
    bsl::copy(
        buffer,
        buffer + ByteStreamImpUtil::SIZEOF_INT64,
        bsl::back_inserter(d_buffer));
    validate();

    return *this;
}

inline
ByteOutStream& ByteOutStream::putInt56(bsls::Types::Int64 value)
{
    char buffer[ByteStreamImpUtil::SIZEOF_INT56] = {0};
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    ByteStreamImpUtil::putInt56(buffer, value);
    bsl::copy(
        buffer,
        buffer + ByteStreamImpUtil::SIZEOF_INT56,
        bsl::back_inserter(d_buffer));
    validate();

    return *this;
}

inline
ByteOutStream& ByteOutStream::putUint56(bsls::Types::Int64 value)
{
    char buffer[ByteStreamImpUtil::SIZEOF_INT56] = {0};
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    ByteStreamImpUtil::putInt56(buffer, value);
    bsl::copy(
        buffer,
        buffer + ByteStreamImpUtil::SIZEOF_INT56,
        bsl::back_inserter(d_buffer));
    validate();

    return *this;
}

inline
ByteOutStream& ByteOutStream::putInt48(bsls::Types::Int64 value)
{
    char buffer[ByteStreamImpUtil::SIZEOF_INT48] = {0};
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    ByteStreamImpUtil::putInt48(buffer, value);
    bsl::copy(
        buffer,
        buffer + ByteStreamImpUtil::SIZEOF_INT48,
        bsl::back_inserter(d_buffer));
    validate();

    return *this;
}

inline
ByteOutStream& ByteOutStream::putUint48(bsls::Types::Int64 value)
{
    char buffer[ByteStreamImpUtil::SIZEOF_INT48] = {0};
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    ByteStreamImpUtil::putInt48(buffer, value);
    bsl::copy(
        buffer,
        buffer + ByteStreamImpUtil::SIZEOF_INT48,
        bsl::back_inserter(d_buffer));
    validate();

    return *this;
}

inline
ByteOutStream& ByteOutStream::putInt40(bsls::Types::Int64 value)
{
    char buffer[ByteStreamImpUtil::SIZEOF_INT40] = {0};
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    ByteStreamImpUtil::putInt40(buffer, value);
    bsl::copy(
        buffer,
        buffer + ByteStreamImpUtil::SIZEOF_INT40,
        bsl::back_inserter(d_buffer));
    validate();

    return *this;
}

inline
ByteOutStream& ByteOutStream::putUint40(bsls::Types::Int64 value)
{
    char buffer[ByteStreamImpUtil::SIZEOF_INT40] = {0};
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    ByteStreamImpUtil::putInt40(buffer, value);
    bsl::copy(
        buffer,
        buffer + ByteStreamImpUtil::SIZEOF_INT40,
        bsl::back_inserter(d_buffer));
    validate();

    return *this;
}

inline
ByteOutStream& ByteOutStream::putInt32(int value)
{
    char buffer[ByteStreamImpUtil::SIZEOF_INT32] = {0};
    ByteStreamImpUtil::putInt32(buffer, value);
    bsl::copy(
        buffer,
        buffer + ByteStreamImpUtil::SIZEOF_INT32,
        bsl::back_inserter(d_buffer));
    return *this;
}

inline
ByteOutStream& ByteOutStream::putUint32(unsigned int value)
{
    char buffer[ByteStreamImpUtil::SIZEOF_INT32] = {0};
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    ByteStreamImpUtil::putInt32(buffer, value);
    bsl::copy(
        buffer,
        buffer + ByteStreamImpUtil::SIZEOF_INT32,
        bsl::back_inserter(d_buffer));
    validate();

    return *this;
}

inline
ByteOutStream& ByteOutStream::putInt24(int value)
{
    char buffer[ByteStreamImpUtil::SIZEOF_INT24] = {0};
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    ByteStreamImpUtil::putInt24(buffer, value);
    bsl::copy(
        buffer,
        buffer + ByteStreamImpUtil::SIZEOF_INT24,
        bsl::back_inserter(d_buffer));
    validate();

    return *this;
}

inline
ByteOutStream& ByteOutStream::putUint24(int value)
{
    char buffer[ByteStreamImpUtil::SIZEOF_INT24] = {0};
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    ByteStreamImpUtil::putInt24(buffer, value);
    bsl::copy(
        buffer,
        buffer + ByteStreamImpUtil::SIZEOF_INT24,
        bsl::back_inserter(d_buffer));
    validate();

    return *this;
}

inline
ByteOutStream& ByteOutStream::putInt16(int value)
{
    char buffer[ByteStreamImpUtil::SIZEOF_INT16] = {0};
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    ByteStreamImpUtil::putInt16(buffer, value);
    bsl::copy(
        buffer,
        buffer + ByteStreamImpUtil::SIZEOF_INT16,
        bsl::back_inserter(d_buffer));
    validate();

    return *this;
}

inline
ByteOutStream& ByteOutStream::putUint16(int value)
{
    char buffer[ByteStreamImpUtil::SIZEOF_INT16] = {0};
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    ByteStreamImpUtil::putInt16(buffer, value);
    bsl::copy(
        buffer,
        buffer + ByteStreamImpUtil::SIZEOF_INT16,
        bsl::back_inserter(d_buffer));
    validate();

    return *this;
}

inline
ByteOutStream& ByteOutStream::putInt8(int value)
{
    char buffer(0);
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    ByteStreamImpUtil::putInt8(&buffer, value);
    d_buffer.push_back(buffer);
    validate();

    return *this;
}

inline
ByteOutStream& ByteOutStream::putUint8(int value)
{
    char buffer(0);
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    ByteStreamImpUtil::putInt8(&buffer, value);
    d_buffer.push_back(buffer);
    validate();

    return *this;
}

inline
ByteOutStream& ByteOutStream::putFloat64(double value)
{
    char buffer[ByteStreamImpUtil::SIZEOF_FLOAT64] = {0};
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    ByteStreamImpUtil::putFloat64(buffer, value);
    bsl::copy(
        buffer,
        buffer + ByteStreamImpUtil::SIZEOF_FLOAT64,
        bsl::back_inserter(d_buffer));
    validate();

    return *this;
}

inline
ByteOutStream& ByteOutStream::putFloat32(float value)
{
    char buffer[ByteStreamImpUtil::SIZEOF_FLOAT32] = {0};
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    ByteStreamImpUtil::putFloat32(buffer, value);
    bsl::copy(
        buffer,
        buffer + ByteStreamImpUtil::SIZEOF_FLOAT32,
        bsl::back_inserter(d_buffer));
    validate();

    return *this;
}

inline
ByteOutStream& ByteOutStream::putString(const bsl::string& value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const int length = value.length();
    putLength(length);
    putArrayUint8(value.data(), length);
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayInt64(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const bsls::Types::Int64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt64(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayUint64(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        putUint64(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayInt56(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const bsls::Types::Int64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt56(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayUint56(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        putUint56(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayInt48(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const bsls::Types::Int64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt48(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayUint48(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        putUint48(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayInt40(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const bsls::Types::Int64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt40(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayUint40(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        putUint40(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayInt32(const int *values, int numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const int *end = values + numValues;
    for (; values != end; ++values) {
        putInt32(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayUint32(const unsigned int *values,
                                             int                 numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const unsigned int *end = values + numValues;
    for (; values != end; ++values) {
        putUint32(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayInt24(const int *values, int numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const int *end = values + numValues;
    for (; values != end; ++values) {
        putInt24(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayUint24(const unsigned int *values,
                                             int                 numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const unsigned int *end = values + numValues;
    for (; values != end; ++values) {
        putUint24(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayInt16(const short *values, int numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const short *end = values + numValues;
    for (; values != end; ++values) {
        putInt16(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayUint16(const unsigned short *values,
                                             int                   numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const unsigned short *end = values + numValues;
    for (; values != end; ++values) {
        putUint16(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayInt8(const char *values, int numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const char *end = values + numValues;
    for (; values != end; ++values) {
        putInt8(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayInt8(const signed char *values,
                                           int                numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const signed char *end = values + numValues;
    for (; values != end; ++values) {
        putInt8(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayUint8(const char *values, int numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const char *end = values + numValues;
    for (; values != end; ++values) {
        putUint8(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayUint8(const unsigned char *values,
                                            int                  numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const unsigned char *end = values + numValues;
    for (; values != end; ++values) {
        putUint8(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayFloat64(const double *values,
                                              int           numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const double *end = values + numValues;
    for (; values != end; ++values) {
        putFloat64(*values);
    }
    return *this;
}

inline
ByteOutStream& ByteOutStream::putArrayFloat32(const float *values,
                                              int          numValues)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }
    const float *end = values + numValues;
    for (; values != end; ++values) {
        putFloat32(*values);
    }
    return *this;
}

inline
void ByteOutStream::removeAll()
{
    d_buffer.clear();
}

inline
void ByteOutStream::reserveCapacity(int newCapacity)
{
    BSLS_ASSERT_SAFE(0 <= newCapacity);

    d_buffer.reserve(newCapacity);
}

// ACCESSORS
inline
bool ByteOutStream::isValid() const
{
    return d_validFlag;
}

inline
ByteOutStream::operator const void *() const
{
    return isValid() ? this : 0;
}

inline
const char *ByteOutStream::data() const
{
    return d_buffer.begin();
}

inline
int ByteOutStream::length() const
{
    return d_buffer.size();
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
