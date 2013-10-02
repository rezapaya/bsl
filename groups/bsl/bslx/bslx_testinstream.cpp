// bslx_testinstream.cpp                                              -*-C++-*-
#include <bslx_testinstream.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslx_testinstream_cpp,"$Id$ $CSID$")

#include <bslx_bytestreamimputil.h>

#ifdef FOR_TESTING_ONLY
#include <bslx_testoutstream.h>             // for testing only
#include <bslx_testinstreamexception.h>     // for testing only
#endif

#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_cstdio.h>


namespace BloombergLP {
namespace bslx {

namespace {

typedef ByteStreamImpUtil Util;
    // 'Util' is an alias for type 'ByteStreamImpUtil' that provides a
    // suite of pure procedures that facilitate the unexternalization
    // of values of the fundamental integral and floating-point types
    // in a data-independent, platform-neutral representation.

enum {
    // Enumeration is used to enumerate size of auxiliary data in
    // bytes streamed in from 'TestInStream'.
    SIZEOF_CODE    = Util::SIZEOF_INT8,   // size of field code value
    SIZEOF_VERSION = Util::SIZEOF_INT8,   // size of version value
    SIZEOF_ARRLEN  = Util::SIZEOF_INT32   // size of array length value
};
}  // close unnamend namespace

                        // ------------------
                        // class TestInStream
                        // ------------------

// STATIC METHODS
void TestInStream::checkScalar(FieldCode::Type  code, int scalarSize)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT(0 < scalarSize);

    if (!isValid()) {
        return;                                                     // RETURN
    }

    invalidate();

    if (length() - cursor() >= SIZEOF_CODE) {
        unsigned char codeFound;  // 'unsigned' to support type values >= 128
        Util::getInt8(&codeFound, d_buffer.begin() + cursor());
        d_cursor += SIZEOF_CODE;
        if (codeFound != code) {
            if (!d_quietFlag) {
                bsl::fprintf(stderr,
                             "*** TestInStream: Expecting type %d"
                             ", but found type %d ***\n",
                             code,
                             (FieldCode::Type) codeFound);
            }
            return;                                                 // RETURN
        }
    }
    else {
        // Incomplete stream.
        return;                                                     // RETURN
    }

    if (length() - cursor() < scalarSize) {
        // Incomplete stream.
        return;                                                     // RETURN
    }

    validate();
}

void TestInStream::checkArray(FieldCode::Type  code,
                              int              scalarSize,
                              int              numElements)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT(0 < scalarSize);
    BSLS_ASSERT(0 <= numElements);

    if (!isValid()) {
        return;                                                     // RETURN
    }

    invalidate();

    if (length() - cursor() >= SIZEOF_CODE) {
        unsigned char codeFound;  // 'unsigned' to support type values >= 128
        Util::getInt8(&codeFound, d_buffer.begin() + cursor());
        d_cursor += SIZEOF_CODE;
        if (codeFound != code) {
            if (!d_quietFlag) {
                bsl::fprintf(stderr,
                             "*** TestInStream: Expecting type %d, "
                             "but found type %d ***\n",
                             code,
                             (FieldCode::Type) codeFound);
            }
            return;                                                 // RETURN
        }
    }
    else {
        // Incomplete stream.
        return;                                                     // RETURN
    }

    if (length() - cursor() >= SIZEOF_ARRLEN) {
        int numElemFound;
        Util::getInt32(&numElemFound, d_buffer.begin() + cursor());
        d_cursor += SIZEOF_ARRLEN;
        if (numElemFound != numElements) {
            if (!d_quietFlag) {
                bsl::fprintf(stderr,
                             "*** TestInStream: Expecting array length %d"
                             ", but found length %d ***\n",
                             numElements,
                             numElemFound);
            }
            return;                                                 // RETURN
        }
    }
    else {
        // Incomplete stream.
        return;                                                     // RETURN
    }

    if (length() - cursor() < scalarSize * numElements) {
        // Incomplete stream.
        return;                                                     // RETURN
    }

    validate();
}

void TestInStream::checkVersion()
{
    if (!isValid() || d_suppressVersionCheckFlag) {
        return;                                                     // RETURN
    }

    invalidate();

    if (length() >= SIZEOF_CODE) {
        // 'unsigned' to support type values >= 128
        unsigned char codeFound = d_buffer[0];
        if (codeFound != FieldCode::BSLX_INT8 &&
            codeFound != FieldCode::BSLX_UINT8) {
            // Wrong type.
            if (!d_quietFlag) {
                bsl::fprintf(stderr,
                             "*** TestInStream: Expecting version of type"
                             " %d, but found type %d ***\n",
                             FieldCode::BSLX_INT8,
                             (FieldCode::Type) codeFound);
            }
            return;                                                 // RETURN
        }
    }
    else {
        // Incomplete stream.
        return;                                                     // RETURN
    }

    if (length() - SIZEOF_CODE >= SIZEOF_VERSION) {
        char version = d_buffer[SIZEOF_CODE];
        if (version <= 0) {
            // Invalid version.
            if (!d_quietFlag) {
                bsl::fprintf(stderr,
                             "*** [TestInStream] Invalid version:"
                             " %d ***\n", (int) version);
            }
        }
    }
    else {
        // Incomplete stream.
        return;                                                     // RETURN
    }

    validate();
}



// CREATORS
TestInStream::TestInStream(bslma::Allocator *basicAllocator)
: d_buffer(basicAllocator)
, d_validFlag(1)
, d_quietFlag(0)
, d_suppressVersionCheckFlag(0)
, d_inputLimit(-1)
, d_cursor(0)
{
}

TestInStream::TestInStream(const char       *buffer,
                           int               numBytes,
                           bslma::Allocator *basicAllocator)
: d_buffer(buffer, buffer + numBytes, basicAllocator)
, d_validFlag(1)
, d_quietFlag(0)
, d_suppressVersionCheckFlag(0)
, d_inputLimit(-1)
, d_cursor(0)
{
   BSLS_ASSERT_SAFE(0 <= numBytes);
}

TestInStream::~TestInStream()
{
}

// MANIPULATORS
TestInStream& TestInStream::getLength(int& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    if (length() - cursor() < SIZEOF_CODE + Util::SIZEOF_INT8) {
        invalidate(); // Incomplete stream.  Invalidate silently.
    }
    else {
        if (127 < (unsigned char)d_buffer[cursor() + SIZEOF_CODE]) {
            // If 'length > 127',
            // 'length' is stored as 4 bytes with top-bit set.
            if (isValid()) {
                getInt32(variable);
                variable ^= (1 << 31);  // Clear top bit.
            }
        }
        else {
            // If 'length <= 127', 'length' is stored as one byte.
            if (isValid()) {
                char tmp;
                getInt8(tmp);
                variable = tmp;
            }
        }
    }
    return *this;
}

TestInStream& TestInStream::getString(bsl::string& str)
{
    int length;
    getLength(length);
    if (isValid()) {
        str.resize(length);
        getArrayUint8(&str.front(), length);
    }
    return *this;
}

TestInStream& TestInStream::getVersion(int& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    if (isValid()) {
        unsigned char tmp;
        getUint8(tmp);
        if (isValid()) {
            variable = tmp;
        }
    }
    return *this;
}

TestInStream& TestInStream::getInt64(bsls::Types::Int64& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_INT64;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_INT64);
    if (isValid()) {
        Util::getInt64(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_INT64;
    }
    return *this;
}

TestInStream& TestInStream::getUint64(bsls::Types::Uint64& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_UINT64;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_INT64);
    if (isValid()) {
        Util::getUint64(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_INT64;
    }
    return *this;
}

TestInStream& TestInStream::getInt56(bsls::Types::Int64& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_INT56;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_INT56);
    if (isValid()) {
        Util::getInt56(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_INT56;
    }
    return *this;
}

TestInStream& TestInStream::getUint56(bsls::Types::Uint64& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_UINT56;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_INT56);
    if (isValid()) {
        Util::getUint56(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_INT56;
    }
    return *this;
}

TestInStream& TestInStream::getInt48(bsls::Types::Int64& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_INT48;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_INT48);
    if (isValid()) {
        Util::getInt48(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_INT48;
    }
    return *this;
}

TestInStream& TestInStream::getUint48(bsls::Types::Uint64& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_UINT48;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_INT48);
    if (isValid()) {
        Util::getUint48(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_INT48;
    }
    return *this;
}

TestInStream& TestInStream::getInt40(bsls::Types::Int64& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_INT40;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_INT40);
    if (isValid()) {
        Util::getInt40(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_INT40;
    }
    return *this;
}

TestInStream& TestInStream::getUint40(bsls::Types::Uint64& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_UINT40;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_INT40);
    if (isValid()) {
        Util::getUint40(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_INT40;
    }
    return *this;
}

TestInStream& TestInStream::getInt32(int& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_INT32;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_INT32);
    if (isValid()) {
        Util::getInt32(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_INT32;
    }
    return *this;
}

TestInStream& TestInStream::getUint32(unsigned int& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_UINT32;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_INT32);
    if (isValid()) {
        Util::getUint32(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_INT32;
    }
    return *this;
}

TestInStream& TestInStream::getInt24(int& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_INT24;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_INT24);
    if (isValid()) {
        Util::getInt24(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_INT24;
    }
    return *this;
}

TestInStream& TestInStream::getUint24(unsigned int& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_UINT24;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_INT24);
    if (isValid()) {
        Util::getUint24(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_INT24;
    }
    return *this;
}

TestInStream& TestInStream::getInt16(short& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_INT16;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_INT16);
    if (isValid()) {
        Util::getInt16(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_INT16;
    }
    return *this;
}

TestInStream& TestInStream::getUint16(unsigned short& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_UINT16;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_INT16);
    if (isValid()) {
        Util::getUint16(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_INT16;
    }
    return *this;
}

TestInStream& TestInStream::getInt8(char& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_INT8;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_INT8);
    if (isValid()) {
        Util::getInt8(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_INT8;
    }
    return *this;
}

TestInStream& TestInStream::getInt8(signed char& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_INT8;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_INT8);
    if (isValid()) {
        Util::getInt8(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_INT8;
    }
    return *this;
}

TestInStream& TestInStream::getUint8(char& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_UINT8;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_INT8);
    if (isValid()) {
        Util::getInt8(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_INT8;
    }
    return *this;
}

TestInStream& TestInStream::getUint8(unsigned char& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_UINT8;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_INT8);
    if (isValid()) {
        Util::getInt8(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_INT8;
    }
    return *this;
}

TestInStream& TestInStream::getFloat64(double& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_FLOAT64;
    throwExceptionIfInputLimitExhausted(code);
    checkScalar(code, Util::SIZEOF_FLOAT64);
    if (isValid()) {
        Util::getFloat64(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_FLOAT64;
    }
    return *this;
}

TestInStream& TestInStream::getFloat32(float& variable)
{
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_FLOAT32;
    throwExceptionIfInputLimitExhausted(code);

    checkScalar(code, Util::SIZEOF_FLOAT32);
    if (isValid()) {
        Util::getFloat32(&variable, d_buffer.begin() + cursor());
        d_cursor += Util::SIZEOF_FLOAT32;
    }

    return *this;
}

TestInStream& TestInStream::getArrayInt64(bsls::Types::Int64 *values,
                                          int                 numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_INT64;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_INT64, numValues);
    if (isValid()) {
        Util::getArrayInt64(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_INT64 * numValues;
    }
    return *this;
}

TestInStream& TestInStream::getArrayUint64(bsls::Types::Uint64 *values,
                                           int                  numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_UINT64;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_INT64, numValues);
    if (isValid()) {
        Util::getArrayUint64(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_INT64 * numValues;
    }
    return *this;
}

TestInStream& TestInStream::getArrayInt56(bsls::Types::Int64 *values,
                                          int                 numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_INT56;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_INT56, numValues);
    if (isValid()) {
        Util::getArrayInt56(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_INT56 * numValues;
    }
    return *this;
}

TestInStream& TestInStream::getArrayUint56(bsls::Types::Uint64 *values,
                                           int                  numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_UINT56;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_INT56, numValues);
    if (isValid()) {
        Util::getArrayUint56(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_INT56 * numValues;
    }
    return *this;
}

TestInStream& TestInStream::getArrayInt48(bsls::Types::Int64 *values,
                                          int                 numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_INT48;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_INT48, numValues);
    if (isValid()) {
        Util::getArrayInt48(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_INT48 * numValues;
    }
    return *this;
}

TestInStream& TestInStream::getArrayUint48(bsls::Types::Uint64 *values,
                                           int                  numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_UINT48;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_INT48, numValues);
    if (isValid()) {
        Util::getArrayUint48(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_INT48 * numValues;
    }
    return *this;
}

TestInStream& TestInStream::getArrayInt40(bsls::Types::Int64 *values,
                                          int                 numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_INT40;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_INT40, numValues);
    if (isValid()) {
        Util::getArrayInt40(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_INT40 * numValues;
    }
    return *this;
}

TestInStream& TestInStream::getArrayUint40(bsls::Types::Uint64 *values,
                                           int                  numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_UINT40;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_INT40, numValues);
    if (isValid()) {
        Util::getArrayUint40(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_INT40 * numValues;
    }
    return *this;
}

TestInStream& TestInStream::getArrayInt32(int *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);
    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_INT32;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_INT32, numValues);
    if (isValid()) {
        Util::getArrayInt32(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_INT32 * numValues;
    }
    return *this;
}

TestInStream& TestInStream::getArrayUint32(unsigned int *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_UINT32;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_INT32, numValues);
    if (isValid()) {
        Util::getArrayUint32(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_INT32 * numValues;
    }
    return *this;
}

TestInStream& TestInStream::getArrayInt24(int *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_INT24;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_INT24, numValues);
    if (isValid()) {
        Util::getArrayInt24(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_INT24 * numValues;
    }
    return *this;
}

TestInStream& TestInStream::getArrayUint24(unsigned int *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_UINT24;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_INT24, numValues);
    if (isValid()) {
        Util::getArrayUint24(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_INT24 * numValues;
    }
    return *this;
}

TestInStream& TestInStream::getArrayInt16(short *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_INT16;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_INT16, numValues);
    if (isValid()) {
        Util::getArrayInt16(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_INT16 * numValues;
    }
    return *this;
}

TestInStream& TestInStream::getArrayUint16(unsigned short *values,
                                           int             numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_UINT16;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_INT16, numValues);
    if (isValid()) {
        Util::getArrayUint16(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_INT16 * numValues;
    }
    return *this;
}

TestInStream& TestInStream::getArrayInt8(char *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_INT8;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_INT8, numValues);
    if (isValid()) {
        Util::getArrayInt8(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_INT8 * numValues;
    }
    return *this;
}

TestInStream& TestInStream::getArrayInt8(signed char *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_INT8;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_INT8, numValues);
    if (isValid()) {
        Util::getArrayInt8(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_INT8 * numValues;
    }
    return *this;
}

TestInStream&
TestInStream::getArrayUint8(char *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_UINT8;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_INT8, numValues);
    if (isValid()) {
        Util::getArrayInt8(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_INT8 * numValues;
    }
    return *this;
}

TestInStream& TestInStream::getArrayUint8(unsigned char *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_UINT8;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_INT8, numValues);
    if (isValid()) {
        Util::getArrayInt8(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_INT8 * numValues;
    }
    return *this;
}

TestInStream& TestInStream::getArrayFloat64(double *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }
    FieldCode::Type code = FieldCode::BSLX_FLOAT64;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_FLOAT64, numValues);
    if (isValid()) {
        Util::getArrayFloat64(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_FLOAT64 * numValues;
    }
    return *this;
}

TestInStream& TestInStream::getArrayFloat32(float *values, int numValues)
{
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    if (0 == cursor()) {
        checkVersion();
    }

    FieldCode::Type code = FieldCode::BSLX_FLOAT32;
    throwExceptionIfInputLimitExhausted(code);
    checkArray(code, Util::SIZEOF_FLOAT32, numValues);
    if (isValid()) {
        Util::getArrayFloat32(values, d_buffer.begin() + cursor(), numValues);
        d_cursor += Util::SIZEOF_FLOAT32 * numValues;
    }
    return *this;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const TestInStream& obj)
{
    const int   len  = obj.d_buffer.size();
    const char *data = obj.d_buffer.begin();

    bsl::ios::fmtflags flags = stream.flags();
    stream << bsl::hex;
    for (int i = 0; i < len; i++) {
        if (0 < i && 0 != i % 8) {
            stream << ' ';
        }
        if (0 == i % 8) {  // print new line and address after 8 bytes
            stream << '\n' << bsl::setw(4) << bsl::setfill('0') << i << '\t';
        }

        char array[8];
        for (int j = 7; j >= 0; j--) {
            array[7-j] = '0' + ((data[i] >> j) & 0x01);
        }

        stream.write(array, sizeof array);
    }
    stream.flags(flags);  // reset stream format flags
    return stream;
}

}  // close packeage namespace
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
