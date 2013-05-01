// bslx_bytestreamimputil.h                                           -*-C++-*-
#ifndef INCLUDED_BSLX_BYTESTREAMIMPUTIL
#define INCLUDED_BSLX_BYTESTREAMIMPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Support platform-independent representation of fundamental types.
//
//@CLASSES:
//   bslx::ByteStreamImpUtil: namespace for put/get externalization functions
//
//@SEE_ALSO: bslx_byteinstream, bslx_byteoutstream
//
//@DESCRIPTION: This component provides a byte-array-based implementation for a
// suite of externalization and unexternalization functions used to convert
// values (and arrays of values) of the following fundamental integer and
// floating-point types:
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
//..
// In addition to basic streaming methods, where each streamed instance of a
// fundamental type occupies the same number of bytes in the stream
// (regardless of its value), this component provides an interface
// for efficient streaming of integer types.  In particular, 64-bit values
// can be streamed as 40, 48, 56, or 64-bit values, and 32-bit values can be
// streamed as 24 and 32-bit values.  Externalized integers are written and
// assumed to be in two's complement, 'Big Endian' (i.e., network byte order)
// format.  Floating point formats are described below.
//
///Note on Function Naming and Interface
///-------------------------------------
// The names and interfaces of the methods of
// 'bslx::ByteStreamImpUtil' follow a systematic fashion explained
// below.  This makes it easier to guess the name and signature of the
// intended function.  In what follows, 'buffer' is always of type
// 'char *' or 'const char *' depending on whether it is used as an
// input or an output, and 'variable' and 'value' are of a type that
// depends on the name of the method and intended width, with
// 'variable' used as an output, while 'value' is used as an input.
//
// Here are the 'get...' methods for integral and floating-point scalar types:
//..
//   Name                         Type of 'variable'           Notes
//   ----                         ------------------           -----
//   getIntNN(variable, buffer)   bsls::Types::Int64 *         NN=64,56,48,40
//                                int *                        NN=32,24
//                                short *                      NN=16
//                                char *                       NN=8
//                                signed char *                NN=8
//                                unsigned char *              NN=8
//
//   getUintNN(variable, buffer)  bsls::Types::Uint64 *        NN=64,56,48,40
//                                int *                        NN=32,24
//                                unsigned short *             NN=16
//
//   getFloatNN(variable, buffer) double *                     NN=64
//                                float *                      NN=32
//..
// Here are the 'put...' methods for scalar types.  Note that there is no
// 'putUintNN' since 'putIntNN' applies equally to unsigned 'NN'-bit values
// (through a conversion to a signed value):
//..
//   Name                         Type of 'value'              Notes
//   ----                         ---------------              -----
//   putIntNN(buffer, value)      bsls::Types::Int64           NN=64,56,48,40
//                                int                          NN=32,24,16,8
//
//   putFloatNN(buffer, value)    double                       NN=64
//                                float                        NN=32
//..
// Here are the 'getArray...' array methods for integral and floating-point
// scalar array types:
//..
//   Name                         Type of 'values'             Notes
//   ----                         ----------------             -----
//   getArrayIntNN(values,        bsls::Types::Int64 *         NN=64,56,48,40
//                 buffer,        int *                        NN=32,24
//                 numValues)     short *                      NN=16
//                                char *                       NN=8
//                                signed char *                NN=8
//                                unsigned char *              NN=8
//
//   getArrayUintNN(values,       bsls::Types::Uint64 *        NN=64,56,48,40
//                  buffer,       unsigned int *               NN=32,24
//                  numValues)    unsigned short *             NN=16
//
//   getArrayFloatNN(values,      double *                     NN=64
//                   buffer,      float *                      NN=32
//                   numValues)
//..
// Finally, the 'putArray...' array methods follow.  Note that this time, there
// is an overload for unsigned types, but that the function name is still
// 'putArrayInt...' for arrays of both signed and unsigned integrals:
//..
//   Name                         Type of 'values'             Notes
//   ----                         ---------------              -----
//   putArrayIntNN(buffer,        const bsls::Types::Int64 *   NN=64,56,48,40
//                 values,        const bsls::Types::Uint64 *  NN=64,56,48,40
//                 numValues)     const int *                  NN=32,24
//                                const unsigned int *         NN=32,24
//                                const short *                NN=16
//                                const unsigned short *       NN=16
//                                const char *                 NN=8
//                                const signed char *          NN=8
//                                const unsigned char *        NN=8
//
//   putArrayFloatNN(values,      const double *               NN=64
//                   buffer,      const float *                NN=32
//                   numValues)
//..
///IEEE 754 Double-Precision Format
///--------------------------------
// A 'double' is assumed to be *at* *least* 64 bits in size.  The externalized
// byte representation of a 64-bit floating-point value is defined to conform
// to the IEEE double-precision format illustrated below.  If the native
// representation of a 64-bit floating point value does not match this
// format, a conversion process to and from this format is performed.
// This conversion may (of course) be lossy:
//..
//  sign bit    11-bit exponent             52-bit mantissa
//    /        /                           /
//  +-+-----------+----------------------------------------------------+
//  |s|e10......e0|m51...............................................m0|
//  +-+-----------+----------------------------------------------------+
//  LSB                                                              MSB
//..
///IEEE 754 Single-Precision Format
///--------------------------------
// A 'float' is assumed to be *at* *least* 32 bits in size.  The externalized
// byte representation of a 32-bit floating-point value is defined to conform
// to the IEEE single-precision format illustrated below.  If the native
// representation of a 32-bit floating point value does not match this
// format, a conversion process to and from this format is performed.
// This conversion may (of course) be lossy:
//..
//  sign bit    8-bit exponent        23-bit mantissa
//     /       /                     /
//    +-+--------+-----------------------+
//    |s|e7....e0|m22..................m0|
//    +-+--------+-----------------------+
//    LSB                              MSB
//..
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1
///- - - - -
// The 'bslx::ByteStreamImpUtil' component can be used stand-alone to save
// and restore a platform neutral representation of fundamental data to and
// from a byte buffer:
//..
//  class MyPoint {
//      int d_x;
//      int d_y;
//
//      MyPoint(const MyPoint&);             // not used in example
//      MyPoint& operator=(const MyPoint&);  // not used in example
//
//    public:
//      MyPoint() : d_x(0), d_y(0) {}
//      MyPoint(int x, int y) : d_x(x), d_y(y) {}
//
//      // ...
//
//      void setX(int x) { d_x = x; }
//      void setY(int y) { d_y = y; }
//
//      int x() const { return d_x; }
//      int y() const { return d_y; }
//  };
//
//  inline bool operator==(const MyPoint& lhs, const MyPoint& rhs)  {
//      return lhs.x() == rhs.x() && lhs.y() == rhs.y();
//  }
//
//  inline bool operator!=(const MyPoint& lhs, const MyPoint& rhs)  {
//      return lhs.x() != rhs.x() || lhs.y() != rhs.y();
//  }
//
//  int savePoint(char *buffer, int bufferLength, const MyPoint& point)
//  {
//      enum { INSUFFICIENT_SPACE = -1, SUCCESS = 0 };
//      if (bufferLength < 4 + 4) {
//          return INSUFFICIENT_SPACE;
//      }
//      bslx::ByteStreamImpUtil::putInt32(buffer + 0, point.x());
//      bslx::ByteStreamImpUtil::putInt32(buffer + 4, point.y());
//      return SUCCESS;
//  }
//
//  int restorePoint(MyPoint *point, const char *buffer, int bufferLength)
//  {
//      enum { SUCCESS = 0, INSUFFICIENT_DATA = 1 };
//      if (bufferLength < 4 + 4) {
//          return INSUFFICIENT_DATA;
//      }
//
//      int result;
//      bslx::ByteStreamImpUtil::getInt32(&result, buffer + 0);
//      point->setX(result);
//      bslx::ByteStreamImpUtil::getInt32(&result, buffer + 4);
//      point->setY(result);
//      return SUCCESS;
//  }
//
//  void assignPointTheHardWay(MyPoint *dstPoint, const MyPoint& srcPoint)
//  {
//      assert(dstPoint);
//
//      char buffer[sizeof srcPoint];
//
//      int s;
//      s = savePoint(buffer, sizeof buffer - 1, srcPoint); assert(0 != s);
//      s = savePoint(buffer, sizeof buffer,     srcPoint); assert(0 == s);
//
//      s = restorePoint(dstPoint, buffer, sizeof buffer - 1); assert(0 != s);
//      s = restorePoint(dstPoint, buffer, sizeof buffer    ); assert(0 == s);
//
//      assert(*dstPoint == srcPoint);
//  }
//
//  void f()
//  {
//      MyPoint x(3, 2);
//      MyPoint y;                     assert(x != y);  assert(!(x == y));
//      assignPointTheHardWay(&y, x);  assert(x == y);  assert(!(x != y));
//  }
//..

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>           // for 'memcpy'
#endif

namespace BloombergLP {

namespace bslx {

                         // ========================
                         // struct ByteStreamImpUtil
                         // ========================

struct ByteStreamImpUtil {
    // This utility class provides a namespace for a suite of pure
    // procedures that facilitate the (un)externalization of values
    // (and C-style arrays of values) of the fundamental integral and
    // floating-point types in a data-independent, platform-neutral
    // representation.

    // TYPES
    enum {
        // Enumeration is used to enumerate CPU-independent size of
        // data types in wire format.  (Might not be the same as size
        // of data types in memory.)
        SIZEOF_INT64   = 8,
        SIZEOF_INT56   = 7,
        SIZEOF_INT48   = 6,
        SIZEOF_INT40   = 5,
        SIZEOF_INT32   = 4,
        SIZEOF_INT24   = 3,
        SIZEOF_INT16   = 2,
        SIZEOF_INT8    = 1,
        SIZEOF_FLOAT64 = 8,
        SIZEOF_FLOAT32 = 4
    };

  private:
    // CLASS METHODS
    template<class TYPE>
    static
    void putIntImpl(char *buffer, TYPE value, int numBytes);
        // Format, as the specified 'numBytes', two's compliment integer (in
        // network order), the least significant 'numBytes' * 8-bits of the
        // specified 'value' (in host byte order) to the specified 'buffer'.

    template<class TYPE>
    static
    void getIntImpl(TYPE *variable, const char *buffer, int numBytes);
        // Read the specified 'numBytes' from the specified 'buffer' as a
        // 'numBytes' * 8-bits 'TYPE' value (in network byte order) and store
        // that value in the specified 'variable' (in host byte order).

  public:
    // CLASS METHODS
    static void putInt64(char *buffer, bsls::Types::Int64 value);
        // Format, as an eight-byte, two's complement integer (in network byte
        // order), the least significant 64 bits of the specified 'value' (in
        // host byte order) to the specified byte 'buffer'.  Note that this
        // function applies equally to unsigned 64-bit values.

    static void putInt56(char *buffer, bsls::Types::Int64 value);
        // Format, as a seven-byte, two's complement integer (in network byte
        // order), the least significant 56 bits of the specified 'value' (in
        // host byte order) to the specified byte 'buffer'.  Note that this
        // function applies equally to unsigned 64-bit values.

    static void putInt48(char *buffer, bsls::Types::Int64 value);
        // Format, as a six-byte, two's complement integer (in network byte
        // order), the least significant 48 bits of the specified 'value' (in
        // host byte order) to the specified byte 'buffer'.  Note that this
        // function applies equally to unsigned 64-bit values.

    static void putInt40(char *buffer, bsls::Types::Int64 value);
        // Format, as a five-byte, two's complement integer (in network byte
        // order), the least significant 40 bits of the specified 'value' (in
        // host byte order) to the specified byte 'buffer'.  Note that this
        // function applies equally to unsigned 64-bit values.

    static void putInt32(char *buffer, int value);
        // Format, as a four-byte, two's complement integer (in network byte
        // order), the least significant 32 bits of the specified 'value' (in
        // host byte order) to the specified byte 'buffer'.  Note that this
        // function applies equally to unsigned 32-bit values.

    static void putInt24(char *buffer, int value);
        // Format, as a three-byte, two's complement integer (in network byte
        // order), the least significant 24 bits of the specified 'value' (in
        // host byte order) to the specified byte 'buffer'.  Note that this
        // function applies equally to unsigned 32-bit values.

    static void putInt16(char *buffer, int value);
        // Format, as a two-byte, two's complement integer (in network byte
        // order), the least significant 16 bits of the specified 'value' (in
        // host byte order) to the specified byte 'buffer'.  Note that this
        // function applies equally to unsigned 32-bit values.

    static void putInt8(char *buffer, int value);
        // Format, as a one-byte, two's complement integer (in network byte
        // order), the least significant 8 bits of the specified 'value' (in
        // host byte order) to the specified byte 'buffer'.  Note that this
        // function applies equally to unsigned 32-bit values.

    static void putFloat64(char *buffer, double value);
        // Format, as an 8-byte IEEE double-precision floating point number
        // (in network byte order), the most significant 8-bytes worth of
        // information in the specified 'value' (in host byte order) to the
        // specified byte 'buffer'.  Note that for non-conforming platforms,
        // this operation may be lossy.

    static void putFloat32(char *buffer, float value);
        // Format, as a 4-byte IEEE double-precision floating point number (in
        // network byte order), the most significant 4-bytes worth of
        // information in the specified 'value' (in host byte order) to the
        // specified byte 'buffer'.  Note that for non-conforming platforms,
        // this operation may be lossy.

    static void getInt64(bsls::Types::Int64 *variable,
                         const char         *buffer);
        // Read eight bytes from the specified 'buffer' as a 64-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended if 'variable' holds more than 64 bits
        // on this platform.

    static void getUint64(bsls::Types::Uint64 *variable,
                          const char          *buffer);
        // Read eight bytes from the specified 'buffer' as a 64-bit unsigned
        // integer (in network byte order) and store that value in the
        // specified 'variable' (in host byte order).  Note that the value will
        // be zero-extended if 'variable' holds more than 64 bits on this
        // platform.

    static void getInt56(bsls::Types::Int64 *variable,
                         const char         *buffer);
        // Read seven bytes from the specified 'buffer' as a 56-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended to the size of the 'variable'
        // on this platform.

    static void getUint56(bsls::Types::Uint64 *variable,
                          const char          *buffer);
        // Read seven bytes from the specified 'buffer' as a 56-bit unsigned
        // integer (in network byte order) and store that value in the
        // specified 'variable' (in host byte order).  Note that the value will
        // be zero-extended to the size of the 'variable' on this platform.

    static void getInt48(bsls::Types::Int64 *variable,
                         const char         *buffer);
        // Read six bytes from the specified 'buffer' as a 48-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended to the size of the 'variable'
        // on this platform.

    static void getUint48(bsls::Types::Uint64 *variable,
                          const char          *buffer);
        // Read six bytes from the specified 'buffer' as a 48-bit unsigned
        // integer (in network byte order) and store that value in the
        // specified 'variable' (in host byte order).  Note that the value will
        // be zero-extended to the size of the 'variable' on this platform.

    static void getInt40(bsls::Types::Int64 *variable,
                         const char         *buffer);
        // Read five bytes from the specified 'buffer' as a 40-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended to the size of the 'variable'
        // on this platform.

    static void getUint40(bsls::Types::Uint64 *variable,
                          const char          *buffer);
        // Read five bytes from the specified 'buffer' as a 40-bit unsigned
        // integer (in network byte order) and store that value in the
        // specified 'variable' (in host byte order).  Note that the value will
        // be zero-extended to the size of the 'variable' on this platform.

    static void getInt32(int *variable, const char *buffer);
        // Read four bytes from the specified 'buffer' as a 32-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended if 'variable' holds more than 32 bits
        // on this platform.

    static void getUint32(unsigned int *variable, const char *buffer);
        // Read four bytes from the specified 'buffer' as a 32-bit unsigned
        // integer (in network byte order) and store that value in the
        // specified 'variable' (in host byte order).  Note that the value will
        // be zero-extended if 'variable' holds more than 32 bits on this
        // platform.

    static void getInt24(int *variable, const char *buffer);
        // Read three bytes from the specified 'buffer' as a 24-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended to the size of the 'variable'
        // on this platform.

    static void getUint24(unsigned int *variable, const char *buffer);
        // Read three bytes from the specified 'buffer' as a 24-bit unsigned
        // integer (in network byte order) and store that value in the
        // specified 'variable' (in host byte order).  Note that the value will
        // be zero-extended to the size of the 'variable' on this platform.

    static void getInt16(short *variable, const char *buffer);
        // Read two bytes from the specified 'buffer' as a 16-bit, two's
        // complement integer (in network byte order) and store that value
        // in the specified 'variable' (in host byte order).  Note that the
        // value will be sign-extended if 'variable' holds more than 16 bits
        // on this platform

    static void getUint16(unsigned short *variable, const char *buffer);
        // Read two bytes from the specified 'buffer' as a 16-bit unsigned
        // integer (in network byte order) and store that value in the
        // specified 'variable' (in host byte order).  Note that the value will
        // be zero-extended if 'variable' holds more than 16 bits on this
        // platform.

    static void getInt8(char          *variable, const char *buffer);
    static void getInt8(signed char   *variable, const char *buffer);
    static void getInt8(unsigned char *variable, const char *buffer);
        // Read one byte from the specified 'buffer' as an 8-bit, two's
        // complement integer and store that value in the specified 'variable'.

    static void getFloat64(double *variable, const char *buffer);
        // Read eight bytes from the specified 'buffer' as a standard IEEE
        // double-precision floating-point number (in network byte order) and
        // store that value in the specified 'variable' (in host byte order).
        // Note that for non-conforming platforms, this operation may be lossy.

    static void getFloat32(float *variable, const char *buffer);
        // Read four bytes from the specified 'buffer' as a standard IEEE
        // single-precision floating-point number (in network byte order) and
        // store that value in the specified 'variable' (in host byte order).
        // Note that for non-conforming platforms, this operation may be lossy.

    static void putArrayInt64(char                      *buffer,
                              const bsls::Types::Int64  *values,
                              int                        numValues);
    static void putArrayInt64(char                      *buffer,
                              const bsls::Types::Uint64 *values,
                              int                        numValues);
        // Format, as consecutive eight-byte, two's complement
        // integers (in network byte order), the least significant 64
        // bits of each of the specified 'numValues' leading entires
        // in the specified 'values' (in host byte order) to the
        // specified 'buffer'.  The behavior is undefined unless
        // '0 <= numValues'.

    static void putArrayInt56(char                      *buffer,
                              const bsls::Types::Int64  *values,
                              int                        numValues);
    static void putArrayInt56(char                      *buffer,
                              const bsls::Types::Uint64 *values,
                              int                        numValues);
        // Format, as consecutive seven-byte, two's complement
        // integers (in network byte order), the least significant 56
        // bits of each of the specified 'numValues' leading entires
        // in the specified 'values' (in host byte order) to the
        // specified 'buffer'.  The behavior is undefined unless
        // '0 <= numValues'.

    static void putArrayInt48(char                      *buffer,
                              const bsls::Types::Int64  *values,
                              int                        numValues);
    static void putArrayInt48(char                      *buffer,
                              const bsls::Types::Uint64 *values,
                              int                        numValues);
        // Format, as consecutive six-byte, two's complement integers
        // (in network byte order), the least significant 48 bits of
        // each of the specified 'numValues' leading entires in the
        // specified 'values' (in host byte order) to the specified
        // 'buffer'.  The behavior is undefined unless '0 <= numValues'.

    static void putArrayInt40(char                      *buffer,
                              const bsls::Types::Int64  *values,
                              int                        numValues);
    static void putArrayInt40(char                      *buffer,
                              const bsls::Types::Uint64 *values,
                              int                        numValues);
        // Format, as consecutive five-byte, two's complement integers
        // (in network byte order), the least significant 40 bits of
        // each of the specified 'numValues' leading entires in the
        // specified 'values' (in host byte order) to the specified
        // 'buffer'.  The behavior is undefined unless '0 <= numValues'.

    static void putArrayInt32(char               *buffer,
                              const int          *values,
                              int                 numValues);
    static void putArrayInt32(char               *buffer,
                              const unsigned int *values,
                              int                 numValues);
        // Format, as consecutive four-byte, two's complement integers
        // (in network byte order), the least significant 32 bits of
        // each of the specified 'numValues' leading entires in the
        // specified 'values' (in host byte order) to the specified
        // 'buffer'.  The behavior is undefined unless '0 <= numValues'.

    static void putArrayInt24(char               *buffer,
                              const int          *values,
                              int                 numValues);
    static void putArrayInt24(char               *buffer,
                              const unsigned int *values,
                              int                 numValues);
        // Format, as consecutive three-byte, two's complement
        // integers (in network byte order), the least significant 24
        // bits of each of the specified 'numValues' leading entires
        // in the specified 'values' (in host byte order) to the
        // specified 'buffer'.  The behavior is undefined unless
        // '0 <= numValues'.

    static void putArrayInt16(char                 *buffer,
                              const short          *values,
                              int                   numValues);
    static void putArrayInt16(char                 *buffer,
                              const unsigned short *values,
                              int                   numValues);
        // Format, as consecutive two-byte, two's complement integers
        // (in network byte order), the least significant 16 bits of
        // each of the specified 'numValues' leading entires in the
        // specified 'values' (in host byte order) to the specified
        // 'buffer'.  The behavior is undefined unless '0 <= numValues'.

    static void putArrayInt8(char                *buffer,
                             const char          *values,
                             int                  numValues);
    static void putArrayInt8(char                *buffer,
                             const signed char   *values,
                             int                  numValues);
    static void putArrayInt8(char                *buffer,
                             const unsigned char *values,
                             int                  numValues);
        // Format, as consecutive one-byte, two's complement integers,
        // each of the specified 'numValues' leading entires in the
        // specified 'values' to the specified 'buffer'.  The behavior
        // is undefined unless '0 <= numValues'.

    static void putArrayFloat64(char         *buffer,
                                const double *values,
                                int           numValues);
        // Format, as consecutive 8-byte IEEE double-precision
        // floating point numbers (in network byte order), the most
        // significant 8-bytes worth of information in each of the
        // specified 'numValues' leading entries in the specified
        // 'values' (in host byte order) to the specified byte
        // 'buffer'.  The behavior is undefined unless '0 <= numValues'.
        // Note that for non-conforming platforms, this operation may
        // be lossy.

    static void putArrayFloat32(char        *buffer,
                                const float *values,
                                int          numValues);
        // Format, as consecutive 4-byte IEEE double-precision
        // floating point numbers (in network byte order), the most
        // significant 4-bytes worth of information in each of the
        // specified 'numValues' leading entries in the specified
        // 'values' (in host byte order) to the specified byte
        // 'buffer'.  The behavior is undefined unless '0 <= numValues'.
        // Note that for non-conforming platforms, this operation may
        // be lossy.

    static void getArrayInt64(bsls::Types::Int64 *values,
                              const char         *buffer,
                              int                 numValues);
        // Read the specified 'numValues' eight-byte, two's complement
        // integers (in network byte order) from the specified
        // 'buffer' and store these values sequentially in the
        // specified 'values' (in host byte order).  The behavior is
        // undefined unless '0 <= numValues'.  Note that each element
        // value will be sign-extended if 'values' holds more than 64
        // bits on this platform.

    static void getArrayUint64(bsls::Types::Uint64 *values,
                               const char          *buffer,
                               int                  numValues);
        // Read the specified 'numValues' eight-byte unsigned integers
        // (in network byte order) from the specified 'buffer' and
        // store these values sequentially in the specified 'values'
        // (in host byte order).  The behavior is undefined unless
        // '0 <= numValues'.  Note that each element value will be
        // zero-extended if 'values' holds more than 64 bits on this
        // platform.

    static void getArrayInt56(bsls::Types::Int64 *values,
                              const char         *buffer,
                              int                 numValues);
        // Read the specified 'numValues' seven-byte unsigned integers
        // (in network byte order) from the specified 'buffer' and
        // store these values sequentially in the specified 'values'
        // (in host byte order).  The behavior is undefined unless
        // '0 <= numValues'.  Note that each element value will be
        // sign-extended to the size of the array element on this
        // platform.

    static void getArrayUint56(bsls::Types::Uint64 *values,
                               const char          *buffer,
                               int                  numValues);
        // Read the specified 'numValues' seven-byte, two's complement
        // integers (in network byte order) from the specified
        // 'buffer' and store these values sequentially in the
        // specified 'values' (in host byte order).  The behavior is
        // undefined unless '0 <= numValues'.  Note that each element
        // value will be zero-extended to the size of the array
        // element on this platform.

    static void getArrayInt48(bsls::Types::Int64 *values,
                              const char         *buffer,
                              int                 numValues);
        // Read the specified 'numValues' six-byte, two's complement
        // integers (in network byte order) from the specified
        // 'buffer' and store these values sequentially in the
        // specified 'values' (in host byte order).  The behavior is
        // undefined unless '0 <= numValues'.  Note that each element
        // value will be sign-extended to the size of the array
        // element on this platform.

    static void getArrayUint48(bsls::Types::Uint64 *values,
                               const char          *buffer,
                               int                  numValues);
        // Read the specified 'numValues' six-byte, two's complement
        // integers (in network byte order) from the specified
        // 'buffer' and store these values sequentially in the
        // specified 'values' (in host byte order).  The behavior is
        // undefined unless '0 <= numValues'.  Note that each element
        // value will be zero-extended to the size of the array
        // element on this platform.

    static void getArrayInt40(bsls::Types::Int64 *values,
                              const char         *buffer,
                              int                 numValues);
        // Read the specified 'numValues' five-byte, two's complement
        // integers (in network byte order) from the specified
        // 'buffer' and store these values sequentially in the
        // specified 'values' (in host byte order).  The behavior is
        // undefined unless '0 <= numValues'.  Note that each element
        // value will be sign-extended to the size of the array
        // element on this platform.

    static void getArrayUint40(bsls::Types::Uint64 *values,
                               const char          *buffer,
                               int                  numValues);
        // Read the specified 'numValues' five-byte, two's complement
        // integers (in network byte order) from the specified
        // 'buffer' and store these values sequentially in the
        // specified 'values' (in host byte order).  The behavior is
        // undefined unless '0 <= numValues'.  Note that each element
        // value will be zero-extended to the size of the array
        // element on this platform.

    static void getArrayInt32(int *values, const char *buffer, int numValues);
        // Read the specified 'numValues' four-byte, two's complement
        // integers (in network byte order) from the specified
        // 'buffer' and store these values sequentially in the
        // specified 'values' (in host byte order).  The behavior is
        // undefined unless '0 <= numValues'.  Note that each element
        // value will be zero-extended to the size of the array
        // element on this platform.

    static void getArrayUint32(unsigned int *values,
                               const char   *buffer,
                               int           numValues);
        // Read the specified 'numValues' four-byte, two's complement
        // integers (in network byte order) from the specified
        // 'buffer' and store these values sequentially in the
        // specified 'values' (in host byte order).  The behavior is
        // undefined unless '0 <= numValues'.  Note that each element
        // value will be zero-extended to the size of the array
        // element on this platform.

    static void getArrayInt24(int        *values,
                              const char *buffer,
                              int         numValues);
        // Read the specified 'numValues' three-byte, two's complement
        // integers (in network byte order) from the specified
        // 'buffer' and store these values sequentially in the
        // specified 'values' (in host byte order).  The behavior is
        // undefined unless '0 <= numValues'.  Note that each element
        // value will be zero-extended to the size of the array
        // element on this platform.

    static void getArrayUint24(unsigned int *values,
                               const char   *buffer,
                               int           numValues);
        // Read the specified 'numValues' three-byte, two's complement
        // integers (in network byte order) from the specified
        // 'buffer' and store these values sequentially in the
        // specified 'values' (in host byte order).  The behavior is
        // undefined unless '0 <= numValues'.  Note that each element
        // value will be zero-extended to the size of the array
        // element on this platform.

    static void getArrayInt16(short      *values,
                              const char *buffer,
                              int         numValues);
        // Read the specified 'numValues' two-byte, two's complement
        // integers (in network byte order) from the specified
        // 'buffer' and store these values sequentially in the
        // specified 'values' (in host byte order).  The behavior is
        // undefined unless '0 <= numValues'.  Note that each element
        // value will be zero-extended to the size of the array
        // element on this platform.

    static void getArrayUint16(unsigned short *values,
                               const char     *buffer,
                               int             numValues);
        // Read the specified 'numValues' two-byte, two's complement
        // integers (in network byte order) from the specified
        // 'buffer' and store these values sequentially in the
        // specified 'values' (in host byte order).  The behavior is
        // undefined unless '0 <= numValues'.  Note that each element
        // value will be zero-extended to the size of the array
        // element on this platform.

    static void getArrayInt8(char          *values,
                             const char    *buffer,
                             int            numValues);
    static void getArrayInt8(signed char   *values,
                             const char    *buffer,
                             int            numValues);
    static void getArrayInt8(unsigned char *values,
                             const char    *buffer,
                             int            numValues);
        // Read the specified 'numValues' one-byte, two's complement
        // integers from the specified 'buffer' and store these values
        // sequentially in the the specified 'values'.  The behavior
        // is undefined unless '0 <= numValues'.

    static void getArrayFloat64(double     *values,
                                const char *buffer,
                                int         numValues);
        // Read the specified 'numValues' eight-byte, IEEE
        // double-precision floating-point numbers (in network byte
        // order) from the specified 'buffer' and store these values
        // sequentially in the specified 'values' (in host byte
        // order).  The behavior is undefined unless '0 <= numValues'.
        // Note that for non-conforming platforms, this operation may
        // be lossy.

    static void getArrayFloat32(float      *values,
                                const char *buffer,
                                int         numValues);
        // Read the specified 'numValues' four-byte, IEEE
        // double-precision floating-point numbers (in network byte
        // order) from the specified 'buffer' and store these values
        // sequentially in the specified 'values' (in host byte
        // order).  The behavior is undefined unless '0 <= numValues'.
        // Note that for non-conforming platforms, this operation may
        // be lossy.
};

// ============================================================================
//                 INLINE DEFINITIONS
// ============================================================================

                         // ------------------------
                         // struct ByteStreamImpUtil
                         // ------------------------

// CLASS METHODS
template<class TYPE>
inline
void ByteStreamImpUtil::putIntImpl(char *buffer, TYPE value, int size)
{
    TYPE v(value);
    for (int i = size - 1; i >= 0; --i) {
        buffer[i] = v;
        v >>= 8;
    }
}

template<class TYPE>
inline
void ByteStreamImpUtil::getIntImpl(TYPE       *variable,
                                   const char *buffer,
                                   int         size)
{
    for (int i = 0; i < size; ++i) {
        *variable <<= 8;
        *variable |= (unsigned char)buffer[i];
    }
}

inline
void ByteStreamImpUtil::putInt64(char *buffer, bsls::Types::Int64 value)
{
    BSLS_ASSERT_SAFE(buffer);
    putIntImpl(buffer, value, SIZEOF_INT64);
}

inline
void ByteStreamImpUtil::putInt56(char *buffer, bsls::Types::Int64 value)
{
    BSLS_ASSERT_SAFE(buffer);
    putIntImpl(buffer, value, SIZEOF_INT56);
}

inline
void ByteStreamImpUtil::putInt48(char *buffer, bsls::Types::Int64 value)
{
    BSLS_ASSERT_SAFE(buffer);
    putIntImpl(buffer, value, SIZEOF_INT48);
}

inline
void ByteStreamImpUtil::putInt40(char *buffer, bsls::Types::Int64 value)
{
    BSLS_ASSERT_SAFE(buffer);
    putIntImpl(buffer, value, SIZEOF_INT40);
}

inline
void ByteStreamImpUtil::putInt32(char *buffer, int value)
{
    BSLS_ASSERT_SAFE(buffer);
    putIntImpl(buffer, value, SIZEOF_INT32);
}

inline
void ByteStreamImpUtil::putInt24(char *buffer, int value)
{
    BSLS_ASSERT_SAFE(buffer);
    putIntImpl(buffer, value, SIZEOF_INT24);
}

inline
void ByteStreamImpUtil::putInt16(char *buffer, int value)
{
    BSLS_ASSERT_SAFE(buffer);
    putIntImpl(buffer, value, SIZEOF_INT16);
}

inline
void ByteStreamImpUtil::putInt8(char *buffer, int value)
{
    BSLS_ASSERT_SAFE(buffer);
    *buffer = value;
}

inline
void ByteStreamImpUtil::putFloat64(char *buffer, double value)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(buffer);

    typedef union Dummy {
        double             d_variable;
        bsls::Types::Int64 d_int64;
    }& T;

    putIntImpl(buffer, T(value).d_int64, SIZEOF_FLOAT64);
}

inline
void ByteStreamImpUtil::putFloat32(char *buffer, float value)
{
    BSLS_ASSERT_SAFE(buffer);

    typedef union Dummy {
        float d_variable;
        int   d_int32;
    }& T;

    putIntImpl(buffer, T(value).d_int32, SIZEOF_FLOAT32);
}

inline
void ByteStreamImpUtil::getInt64(bsls::Types::Int64 *variable,
                                 const char         *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);
    if (sizeof *variable > SIZEOF_INT64) {
        *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend
    }
    getIntImpl(variable, buffer, SIZEOF_INT64);
}

inline
void ByteStreamImpUtil::getUint64(bsls::Types::Uint64 *variable,
                                  const char          *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);
    if (sizeof *variable > SIZEOF_INT64) {
        *variable = 0;  // zero extend
    }
    getIntImpl(variable, buffer, SIZEOF_INT64);
}

inline
void ByteStreamImpUtil::getInt56(bsls::Types::Int64 *variable,
                                 const char         *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);
    *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend
    getIntImpl(variable, buffer, SIZEOF_INT56);
}

inline
void ByteStreamImpUtil::getUint56(bsls::Types::Uint64 *variable,
                                  const char          *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);
    *variable = 0;  // zero extend
    getIntImpl(variable, buffer, SIZEOF_INT56);
}

inline
void ByteStreamImpUtil::getInt48(bsls::Types::Int64 *variable,
                                 const char         *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);
    *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend
    getIntImpl(variable, buffer, SIZEOF_INT48);
}

inline
void ByteStreamImpUtil::getUint48(bsls::Types::Uint64 *variable,
                                  const char          *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);
    *variable = 0;  // zero extend
    getIntImpl(variable, buffer, SIZEOF_INT48);
}

inline
void ByteStreamImpUtil::getInt40(bsls::Types::Int64 *variable,
                                 const char         *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);
    *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend
    getIntImpl(variable, buffer, SIZEOF_INT40);
}

inline
void ByteStreamImpUtil::getUint40(bsls::Types::Uint64 *variable,
                                  const char          *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);
    *variable = 0;  // zero extend
    getIntImpl(variable, buffer, SIZEOF_INT40);
}

inline
void ByteStreamImpUtil::getInt32(int *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);
    if (sizeof *variable > SIZEOF_INT32) {
        *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend
    }
    getIntImpl(variable, buffer, SIZEOF_INT32);
}

inline
void ByteStreamImpUtil::getUint32(unsigned int *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);
    if (sizeof *variable > SIZEOF_INT32) {
        *variable = 0;  // zero extend
    }
    getIntImpl(variable, buffer, SIZEOF_INT32);
}

inline
void ByteStreamImpUtil::getInt24(int *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);
    *variable = 0x80 & buffer[0] ? -1 : 0;  // sign extend
    getIntImpl(variable, buffer, SIZEOF_INT24);
}

inline
void ByteStreamImpUtil::getUint24(unsigned int *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);
    *variable = 0;  // zero extend
    getIntImpl(variable, buffer, SIZEOF_INT24);
}

inline
void ByteStreamImpUtil::getInt16(short *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);
    if (sizeof *variable > SIZEOF_INT16) {
        *variable = (short)(0x80 & buffer[0] ? -1 : 0);  // sign extend
    }
    getIntImpl(variable, buffer, SIZEOF_INT16);
}

inline
void ByteStreamImpUtil::getUint16(unsigned short *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);
    if (sizeof *variable > SIZEOF_INT16) {
        *variable = 0;  // zero extend
    }
    getIntImpl(variable, buffer, SIZEOF_INT16);
}

inline
void ByteStreamImpUtil::getInt8(char *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);
    *variable = *buffer;
}

inline
void ByteStreamImpUtil::getInt8(signed char *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);
    getInt8((char *)variable, buffer);
}

inline
void ByteStreamImpUtil::getInt8(unsigned char *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);
    getInt8((char *)variable, buffer);
}

inline
void ByteStreamImpUtil::getFloat64(double *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);

    typedef union Dummy {
        double             d_variable;
        bsls::Types::Int64 d_int64;
    }& T;

    if (sizeof *variable > 8) {
        *variable = 0;  // zero fill mantissa
    }

    getIntImpl(&T(*variable).d_int64, buffer, SIZEOF_FLOAT64);
}

inline
void ByteStreamImpUtil::getFloat32(float *variable, const char *buffer)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(variable);

    typedef union Dummy {
        float d_variable;
        int   d_int32;
    }& T;

    if (sizeof *variable > 4) {
        *variable = 0;  // zero fill mantissa
    }

    getIntImpl(&T(*variable).d_int32, buffer, SIZEOF_FLOAT32);
}

inline
void ByteStreamImpUtil::putArrayInt64(char                      *buffer,
                                      const bsls::Types::Int64  *values,
                                      int                        numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    const bsls::Types::Int64  *end = values + numValues;
    for (; values != end; ++values) {
        putInt64(buffer, *values);
        buffer += SIZEOF_INT64;
    }
}

inline
void ByteStreamImpUtil::putArrayInt64(char                      *buffer,
                                      const bsls::Types::Uint64 *values,
                                      int                        numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    const bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt64(buffer, *values);
        buffer += SIZEOF_INT64;
    }
}

inline
void ByteStreamImpUtil::putArrayInt56(char                      *buffer,
                                      const bsls::Types::Int64  *values,
                                      int                        numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    const bsls::Types::Int64  *end = values + numValues;
    for (; values != end; ++values) {
        putInt56(buffer, *values);
        buffer += SIZEOF_INT56;
    }
}

inline
void ByteStreamImpUtil::putArrayInt56(char                      *buffer,
                                      const bsls::Types::Uint64 *values,
                                      int                        numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    const bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt56(buffer, *values);
        buffer += SIZEOF_INT56;
    }
}

inline
void ByteStreamImpUtil::putArrayInt48(char                      *buffer,
                                      const bsls::Types::Int64  *values,
                                      int                        numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    const bsls::Types::Int64  *end = values + numValues;
    for (; values != end; ++values) {
        putInt48(buffer, *values);
        buffer += SIZEOF_INT48;
    }
}

inline
void ByteStreamImpUtil::putArrayInt48(char                      *buffer,
                                      const bsls::Types::Uint64 *values,
                                      int                        numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    const bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt48(buffer, *values);
        buffer += SIZEOF_INT48;
    }
}

inline
void ByteStreamImpUtil::putArrayInt40(char                      *buffer,
                                      const bsls::Types::Int64  *values,
                                      int                        numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    const bsls::Types::Int64  *end = values + numValues;
    for (; values != end; ++values) {
        putInt40(buffer, *values);
        buffer += SIZEOF_INT40;
    }
}

inline
void ByteStreamImpUtil::putArrayInt40(char                      *buffer,
                                      const bsls::Types::Uint64 *values,
                                      int                        numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    const bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt40(buffer, *values);
        buffer += SIZEOF_INT40;
    }
}

inline
void ByteStreamImpUtil::putArrayInt32(char      *buffer,
                                      const int *values,
                                      int        numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    const int *end = values + numValues;
    for (; values != end; ++values) {
        putInt32(buffer, *values);
        buffer += SIZEOF_INT32;
    }
}

inline
void ByteStreamImpUtil::putArrayInt32(char               *buffer,
                                      const unsigned int *values,
                                      int                 numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    const unsigned int *end = values + numValues;
    for (; values != end; ++values) {
        putInt32(buffer, *values);
        buffer += SIZEOF_INT32;
    }
}

inline
void ByteStreamImpUtil::putArrayInt24(char      *buffer,
                                      const int *values,
                                      int        numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    const int *end = values + numValues;
    for (; values != end; ++values) {
        putInt24(buffer, *values);
        buffer += SIZEOF_INT24;
    }
}

inline
void ByteStreamImpUtil::putArrayInt24(char               *buffer,
                                      const unsigned int *values,
                                      int                 numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    const unsigned int *end = values + numValues;
    for (; values != end; ++values) {
        putInt24(buffer, *values);
        buffer += SIZEOF_INT24;
    }
}

inline
void ByteStreamImpUtil::putArrayInt16(char        *buffer,
                                      const short *values,
                                      int          numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    const short  *end = values + numValues;
    for (; values != end; ++values) {
        putInt16(buffer, *values);
        buffer += SIZEOF_INT16;
    }
}

inline
void ByteStreamImpUtil::putArrayInt16(char                 *buffer,
                                      const unsigned short *values,
                                      int                   numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    const unsigned short *end = values + numValues;
    for (; values != end; ++values) {
        putInt16(buffer, *values);
        buffer += SIZEOF_INT16;
    }
}

inline
void ByteStreamImpUtil::putArrayInt8(char       *buffer,
                                     const char *values,
                                     int         numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    bsl::memcpy(buffer, values, numValues);
}

inline
void ByteStreamImpUtil::putArrayInt8(char              *buffer,
                                     const signed char *values,
                                     int                numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    putArrayInt8(buffer, (const char *)values, numValues);
}

inline
void ByteStreamImpUtil::putArrayInt8(char                *buffer,
                                     const unsigned char *values,
                                     int                  numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    putArrayInt8(buffer, (const char *)values, numValues);
}

inline
void ByteStreamImpUtil::putArrayFloat64(char         *buffer,
                                        const double *values,
                                        int           numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    const double *end = values + numValues;
    for (; values < end; ++values) {
        putFloat64(buffer, *values);
        buffer += SIZEOF_FLOAT64;
    }
}

inline
void ByteStreamImpUtil::putArrayFloat32(char        *buffer,
                                        const float *values,
                                        int          numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    const float *end = values + numValues;
    for (; values < end; ++values) {
        putFloat32(buffer, *values);
        buffer += SIZEOF_FLOAT32;
    }
}

inline
void ByteStreamImpUtil::getArrayInt64(bsls::Types::Int64 *values,
                                      const char         *buffer,
                                      int                 numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    const bsls::Types::Int64 *end = values + numValues;
    for (; values != end; ++values) {
        getInt64(values, buffer);
        buffer += SIZEOF_INT64;
    }
}

inline
void ByteStreamImpUtil::getArrayUint64(bsls::Types::Uint64 *values,
                                       const char          *buffer,
                                       int                  numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        getUint64(values, buffer);
        buffer += SIZEOF_INT64;
    }
}

inline
void ByteStreamImpUtil::getArrayInt56(bsls::Types::Int64 *values,
                                      const char         *buffer,
                                      int                 numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    bsls::Types::Int64 *end = values + numValues;
    for (; values != end; ++values) {
        getInt56(values, buffer);
        buffer += SIZEOF_INT56;
    }
}

inline
void ByteStreamImpUtil::getArrayUint56(bsls::Types::Uint64 *values,
                                       const char          *buffer,
                                       int                  numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        getUint56(values, buffer);
        buffer += SIZEOF_INT56;
    }
}

inline
void ByteStreamImpUtil::getArrayInt48(bsls::Types::Int64 *values,
                                      const char         *buffer,
                                      int                 numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    bsls::Types::Int64 *end = values + numValues;
    for (; values != end; ++values) {
        getInt48(values, buffer);
        buffer += SIZEOF_INT48;
    }
}

inline
void ByteStreamImpUtil::getArrayUint48(bsls::Types::Uint64 *values,
                                       const char          *buffer,
                                       int                  numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        getUint48(values, buffer);
        buffer += SIZEOF_INT48;
    }
}

inline
void ByteStreamImpUtil::getArrayInt40(bsls::Types::Int64 *values,
                                      const char         *buffer,
                                      int                 numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    bsls::Types::Int64 *end = values + numValues;
    for (; values != end; ++values) {
        getInt40(values, buffer);
        buffer += SIZEOF_INT40;
    }
}

inline
void ByteStreamImpUtil::getArrayUint40(bsls::Types::Uint64 *values,
                                       const char          *buffer,
                                       int                  numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        getUint40(values, buffer);
        buffer += SIZEOF_INT40;
    }
}

inline
void ByteStreamImpUtil::getArrayInt32(int        *values,
                                      const char *buffer,
                                      int         numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    int *end = values + numValues;
    for (; values != end; ++values) {
        getInt32(values, buffer);
        buffer += SIZEOF_INT32;
    }
}

inline
void ByteStreamImpUtil::getArrayUint32(unsigned int *values,
                                       const char   *buffer,
                                       int           numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    unsigned int *end = values + numValues;
    for (; values != end; ++values) {
        getUint32(values, buffer);
        buffer += SIZEOF_INT32;
    }
}

inline
void ByteStreamImpUtil::getArrayInt24(int        *values,
                                      const char *buffer,
                                      int         numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    int *end = values + numValues;
    for (; values != end; ++values) {
        getInt24(values, buffer);
        buffer += SIZEOF_INT24;
    }
}

inline
void ByteStreamImpUtil::getArrayUint24(unsigned int *values,
                                       const char   *buffer,
                                       int           numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    unsigned int *end = values + numValues;
    for (; values != end; ++values) {
        getUint24(values, buffer);
        buffer += SIZEOF_INT24;
    }
}

inline
void ByteStreamImpUtil::getArrayInt16(short      *values,
                                      const char *buffer,
                                      int         numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    short *end = values + numValues;
    for (; values != end; ++values) {
        getInt16(values, buffer);
        buffer += SIZEOF_INT16;
    }
}

inline
void ByteStreamImpUtil::getArrayUint16(unsigned short *values,
                                       const char     *buffer,
                                       int             numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    unsigned short *end = values + numValues;
    for (; values != end; ++values) {
        getUint16(values, buffer);
        buffer += SIZEOF_INT16;
    }
}

inline
void ByteStreamImpUtil::getArrayInt8(char       *values,
                                     const char *buffer,
                                     int         numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    bsl::memcpy(values, buffer, numValues);
}

inline
void ByteStreamImpUtil::getArrayInt8(signed char *values,
                                     const char  *buffer,
                                     int          numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    getArrayInt8((char *)values, buffer, numValues);
}

inline
void ByteStreamImpUtil::getArrayInt8(unsigned char *values,
                                     const char    *buffer,
                                     int            numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    getArrayInt8((char *)values, buffer, numValues);
}

inline
void ByteStreamImpUtil::getArrayFloat64(double     *values,
                                        const char *buffer,
                                        int         numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    double *end = values + numValues;
    for (; values != end; ++values) {
        getFloat64(values, buffer);
        buffer += SIZEOF_FLOAT64;
    }
}

inline
void ByteStreamImpUtil::getArrayFloat32(float      *values,
                                        const char *buffer,
                                        int         numValues)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    float *end = values + numValues;
    for (; values != end; ++values) {
        getFloat32(values, buffer);
        buffer += SIZEOF_INT32;
    }
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
