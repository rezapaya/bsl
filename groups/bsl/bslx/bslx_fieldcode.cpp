// bslx_fieldcode.cpp                                                 -*-C++-*-
#include <bslx_fieldcode.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslx_fieldcode_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {
namespace bslx {

                        // ===============
                        // class FieldCode
                        // ===============

// CLASS METHODS
const char *FieldCode::toAscii(Type code)
{
    switch (code) {
      case(BSLX_INT8)    : return "INT8";
      case(BSLX_UINT8)   : return "UINT8";
      case(BSLX_INT16)   : return "INT16";
      case(BSLX_UINT16)  : return "UINT16";
      case(BSLX_INT24)   : return "INT24";
      case(BSLX_UINT24)  : return "UINT24";
      case(BSLX_INT32)   : return "INT32";
      case(BSLX_UINT32)  : return "UINT32";
      case(BSLX_INT40)   : return "INT40";
      case(BSLX_UINT40)  : return "UINT40";
      case(BSLX_INT48)   : return "INT48";
      case(BSLX_UINT48)  : return "UINT48";
      case(BSLX_INT56)   : return "INT56";
      case(BSLX_UINT56)  : return "UINT56";
      case(BSLX_INT64)   : return "INT64";
      case(BSLX_UINT64)  : return "UINT64";
      case(BSLX_FLOAT32) : return "FLOAT32";
      case(BSLX_FLOAT64) : return "FLOAT64";
      case(BSLX_INVALID) : return "INVALID";
      default: return "(* UNKNOWN *)";
    }
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, FieldCode::Type rhs)
{
    return stream << FieldCode::toAscii(rhs);
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
