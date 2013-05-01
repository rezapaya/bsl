// bslx_fieldcode.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLX_FIELDCODE
#define INCLUDED_BSLX_FIELDCODE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate all supported 'bslx' externalization types.
//
//@CLASSES:
//  bslx::FieldCode: namespace for enumerating 'bslx' externalization types
//
//@DESCRIPTION: This component provides a namespace, 'bslx::FieldCode', for
// enumerating all of the supported types in the 'bslx' externalization
// package.  Functionality is provided to convert each of the enumerated
// values to its corresponding string representation and to write the string
// form to a standard 'ostream'.

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace bslx {

                         // ================
                         // struct FieldCode
                         // ================

struct FieldCode {
    // This utility class provides a namespace for enumerating all of
    // the supported types in the 'bslx' externalization package.

    enum { BSLX_OFFSET = 0xe0 };  // Value to offset each enumerator in Type

    // TYPES
    enum Type {
        // Enumeration is used to enumerate all of the supported types
        // in the 'bslx' externalization package.
        BSLX_INT8    = BSLX_OFFSET + 0,
        BSLX_UINT8   = BSLX_OFFSET + 1,
        BSLX_INT16   = BSLX_OFFSET + 2,
        BSLX_UINT16  = BSLX_OFFSET + 3,
        BSLX_INT24   = BSLX_OFFSET + 4,
        BSLX_UINT24  = BSLX_OFFSET + 5,
        BSLX_INT32   = BSLX_OFFSET + 6,
        BSLX_UINT32  = BSLX_OFFSET + 7,
        BSLX_INT40   = BSLX_OFFSET + 8,
        BSLX_UINT40  = BSLX_OFFSET + 9,
        BSLX_INT48   = BSLX_OFFSET + 10,
        BSLX_UINT48  = BSLX_OFFSET + 11,
        BSLX_INT56   = BSLX_OFFSET + 12,
        BSLX_UINT56  = BSLX_OFFSET + 13,
        BSLX_INT64   = BSLX_OFFSET + 14,
        BSLX_UINT64  = BSLX_OFFSET + 15,
        BSLX_FLOAT32 = BSLX_OFFSET + 16,
        BSLX_FLOAT64 = BSLX_OFFSET + 17,
        BSLX_INVALID = BSLX_OFFSET + 18   // last entry -- see 'LENGTH' below
    };

    enum {
        // Enumeration is used to define 'BSLX_LENGTH' to be the
        // number of (consecutively valued) enumerators in 'Type'.

        BSLX_LENGTH = BSLX_INVALID + 1 - BSLX_OFFSET
    };

    // CLASS METHODS
    static const char *toAscii(Type code);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified 'code'.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, FieldCode::Type rhs);
    // Write to the specified 'stream' the string representation exactly
    // matching the name corresponding to the specified 'rhs' value.

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
