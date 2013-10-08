// bslx_testoutstream.cpp                                             -*-C++-*-
#include <bslx_testoutstream.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslx_testoutstream_cpp,"$Id$ $CSID$")

#include <bslx_fieldcode.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_ostream.h>

// CONSTANTS

namespace BloombergLP {
namespace bslx {

                        // -------------------
                        // class TestOutStream
                        // -------------------

// CREATORS
TestOutStream::TestOutStream(bslma::Allocator *basicAllocator)
: d_imp(basicAllocator)
, d_makeNextInvalidFlag(0)
{
}

TestOutStream::~TestOutStream()
{
}

// MANIPULATORS
TestOutStream& TestOutStream::putLength(int length)
{
    BSLS_ASSERT(0 <= length);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
        d_imp.putInt8(code);
        d_imp.putInt32(length);
    }
    else if (length > 127) {
        code = FieldCode::BSLX_INT32;
        d_imp.putInt8(code);
        d_imp.putInt32(length | (1 << 31));
    }
    else {
        code = FieldCode::BSLX_INT8;
        d_imp.putInt8(code);
        d_imp.putInt8(length);
    }
    return *this;
}

TestOutStream& TestOutStream::putVersion(int version)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_UINT8;
    }
    d_imp.putInt8(code);
    unsigned char tmp = version;
    d_imp.putInt8(tmp);
    return *this;
}

TestOutStream& TestOutStream::putInt64(bsls::Types::Int64 value)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_INT64;
    }
    d_imp.putInt8(code);
    d_imp.putInt64(value);
    return *this;
}

TestOutStream& TestOutStream::putUint64(bsls::Types::Int64 value)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_UINT64;
    }
    d_imp.putInt8(code);
    d_imp.putInt64(value);
    return *this;
}

TestOutStream& TestOutStream::putInt56(bsls::Types::Int64 value)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_INT56;
    }
    d_imp.putInt8(code);
    d_imp.putInt56(value);
    return *this;
}

TestOutStream& TestOutStream::putUint56(bsls::Types::Int64 value)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_UINT56;
    }
    d_imp.putInt8(code);
    d_imp.putInt56(value);
    return *this;
}

TestOutStream& TestOutStream::putInt48(bsls::Types::Int64 value)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_INT48;
    }
    d_imp.putInt8(code);
    d_imp.putInt48(value);
    return *this;
}

TestOutStream& TestOutStream::putUint48(bsls::Types::Int64 value)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_UINT48;
    }
    d_imp.putInt8(code);
    d_imp.putInt48(value);
    return *this;
}

TestOutStream& TestOutStream::putInt40(bsls::Types::Int64 value)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_INT40;
    }
    d_imp.putInt8(code);
    d_imp.putInt40(value);
    return *this;
}

TestOutStream& TestOutStream::putUint40(bsls::Types::Int64 value)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_UINT40;
    }
    d_imp.putInt8(code);
    d_imp.putInt40(value);
    return *this;
}

TestOutStream& TestOutStream::putInt32(int value)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_INT32;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(value);
    return *this;
}

TestOutStream& TestOutStream::putUint32(unsigned int value)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_UINT32;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(value);
    return *this;
}

TestOutStream& TestOutStream::putInt24(int value)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_INT24;
    }
    d_imp.putInt8(code);
    d_imp.putInt24(value);
    return *this;
}

TestOutStream& TestOutStream::putUint24(int value)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_UINT24;
    }
    d_imp.putInt8(code);
    d_imp.putInt24(value);
    return *this;
}

TestOutStream& TestOutStream::putInt16(int value)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_INT16;
    }
    d_imp.putInt8(code);
    d_imp.putInt16(value);
    return *this;
}

TestOutStream& TestOutStream::putUint16(int value)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_UINT16;
    }
    d_imp.putInt8(code);
    d_imp.putInt16(value);
    return *this;
}

TestOutStream& TestOutStream::putInt8(int value)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_INT8;
    }
    d_imp.putInt8(code);
    d_imp.putInt8(value);
    return *this;
}

TestOutStream& TestOutStream::putUint8(int value)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_UINT8;
    }
    d_imp.putInt8(code);
    d_imp.putInt8(value);
    return *this;
}

TestOutStream& TestOutStream::putFloat64(double value)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_FLOAT64;
    }
    d_imp.putInt8(code);
    d_imp.putFloat64(value);
    return *this;
}

TestOutStream& TestOutStream::putFloat32(float value)
{
    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        code = FieldCode::BSLX_INVALID;
        d_makeNextInvalidFlag = 0;
    }
    else {
        code = FieldCode::BSLX_FLOAT32;
    }
    d_imp.putInt8(code);
    d_imp.putFloat32(value);
    return *this;
}

TestOutStream& TestOutStream::putArrayInt64(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_INT64;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayInt64(values, numValues);
    }

    return *this;
}

TestOutStream& TestOutStream::putArrayUint64(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_UINT64;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayUint64(values, numValues);
    }
    return *this;
}

TestOutStream& TestOutStream::putArrayInt56(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_INT56;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayInt56(values, numValues);
    }

    return *this;
}

TestOutStream& TestOutStream::putArrayUint56(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_UINT56;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayUint56(values, numValues);
    }
    return *this;
}

TestOutStream& TestOutStream::putArrayInt48(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_INT48;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayInt48(values, numValues);
    }

    return *this;
}

TestOutStream& TestOutStream::putArrayUint48(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_UINT48;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayUint48(values, numValues);
    }
    return *this;
}

TestOutStream& TestOutStream::putArrayInt40(
                                           const bsls::Types::Int64 *values,
                                           int                       numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_INT40;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayInt40(values, numValues);
    }

    return *this;
}

TestOutStream& TestOutStream::putArrayUint40(
                                          const bsls::Types::Uint64 *values,
                                          int                        numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_UINT40;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayUint40(values, numValues);
    }
    return *this;
}

TestOutStream& TestOutStream::putArrayInt32(const int *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_INT32;
    }
     d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayInt32(values, numValues);
    }

    return *this;
}

TestOutStream& TestOutStream::putArrayUint32(const unsigned int *values,
                                             int                 numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_UINT32;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayUint32(values, numValues);
    }

    return *this;
}

TestOutStream& TestOutStream::putArrayInt24(const int *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_INT24;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayInt24(values, numValues);
    }

    return *this;
}

TestOutStream& TestOutStream::putArrayUint24(const unsigned int *values,
                                             int                 numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_UINT24;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayUint24(values, numValues);
    }

    return *this;
}

TestOutStream& TestOutStream::putArrayInt16(const short *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_INT16;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayInt16(values, numValues);
    }

    return *this;
}

TestOutStream& TestOutStream::putArrayUint16(const unsigned short *values,
                                             int                   numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_UINT16;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayUint16(values, numValues);
    }

    return *this;
}

TestOutStream& TestOutStream::putArrayInt8(const char *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_INT8;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayInt8(values, numValues);
    }

    return *this;
}

TestOutStream& TestOutStream::putArrayInt8(const signed char *values,
                                           int                numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_INT8;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayInt8(values, numValues);
    }

    return *this;
}

TestOutStream& TestOutStream::putArrayUint8(const char *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_UINT8;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayUint8(values, numValues);
    }

    return *this;
}

TestOutStream& TestOutStream::putArrayUint8(const unsigned char *values,
                                            int                  numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_UINT8;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayUint8(values, numValues);
    }

    return *this;
}

TestOutStream& TestOutStream::putArrayFloat64(const double *values,
                                              int           numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_FLOAT64;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayFloat64(values, numValues);
    }

    return *this;
}

TestOutStream& TestOutStream::putArrayFloat32(const float *values,
                                              int          numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    FieldCode::Type code;
    if (d_makeNextInvalidFlag) {
        d_makeNextInvalidFlag = 0;
        code = FieldCode::BSLX_INVALID;
    }
    else {
        code = FieldCode::BSLX_FLOAT32;
    }
    d_imp.putInt8(code);
    d_imp.putInt32(numValues);
    if (0 < numValues) {
        d_imp.putArrayFloat32(values, numValues);
    }

    return *this;
}

// FREE OPERATORS

bsl::ostream& operator<<(bsl::ostream& stream, const TestOutStream& obj)
{
    return stream << obj.d_imp;
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
