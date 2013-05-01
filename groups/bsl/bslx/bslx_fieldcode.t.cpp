// bslx_fieldcode.t.cpp                                               -*-C++-*-

#include <bslx_fieldcode.h>

#include <bsl_iostream.h>
#include <bsl_strstream.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script
using namespace bslx;

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a component that consists of an enumeration and its helper
// functions.  We must ensure that the helper functions 'toAscii' and
// 'operator<<' generate the expected format for each enumerator in the
// enumeration, as well as for values outside the set of valid enumerators.  We
// achieve this goal by creating a table of test vectors consisting of varying
// enumerator value and the corresponding expected value.  We iterate over the
// table and verify that the helper functions generate the expected output
// format.
//-----------------------------------------------------------------------------
// [1] const char *toAscii(code);
// [1] ostream& operator<<(stream, code);
//=============================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//==========================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//--------------------------------------------------------------------------

typedef FieldCode Obj;

//==========================================================================
//                              MAIN PROGRAM
//--------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // BASIC TEST:
        //   Iterate over a set of table-generated test vectors of varying
        //   enumerator value and verify that 'toAscii' and 'operator<<'
        //   generate the expected format.
        //
        // Testing:
        //   const char *toAscii(code);
        //   ostream& operator<<(stream, code);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BASIC TEST" << endl
                                  << "==========" << endl;

        struct {
            int d_line;
            Obj::Type d_code;
            const char *d_exp;
        } DATA[] = {
           // line    code                   expected output
           // ----    ---------              ---------------
            { L_,     Obj::BSLX_INT8,        "INT8"          },
            { L_,     Obj::BSLX_UINT8,       "UINT8"         },
            { L_,     Obj::BSLX_INT16,       "INT16"         },
            { L_,     Obj::BSLX_UINT16,      "UINT16"        },
            { L_,     Obj::BSLX_INT24,       "INT24"         },
            { L_,     Obj::BSLX_UINT24,      "UINT24"        },
            { L_,     Obj::BSLX_INT32,       "INT32"         },
            { L_,     Obj::BSLX_UINT32,      "UINT32"        },
            { L_,     Obj::BSLX_INT40,       "INT40"         },
            { L_,     Obj::BSLX_UINT40,      "UINT40"        },
            { L_,     Obj::BSLX_INT48,       "INT48"         },
            { L_,     Obj::BSLX_UINT48,      "UINT48"        },
            { L_,     Obj::BSLX_INT56,       "INT56"         },
            { L_,     Obj::BSLX_UINT56,      "UINT56"        },
            { L_,     Obj::BSLX_INT64,       "INT64"         },
            { L_,     Obj::BSLX_UINT64,      "UINT64"        },
            { L_,     Obj::BSLX_FLOAT32,     "FLOAT32"       },
            { L_,     Obj::BSLX_FLOAT64,     "FLOAT64"       },
            { L_,     Obj::BSLX_INVALID,     "INVALID"       },
            { L_,     (Obj::Type)  1,        "(* UNKNOWN *)" },
            { L_,     (Obj::Type) -1,        "(* UNKNOWN *)" },
            { L_,     (Obj::Type)  0,        "(* UNKNOWN *)" },
            { L_,     (Obj::Type) 10,        "(* UNKNOWN *)" }
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "Testing 'toAscii'." << endl;
        {
            for (int i = 0 ; i < NUM_DATA; ++i) {
                const int LINE =       DATA[i].d_line;
                const Obj::Type CODE = DATA[i].d_code;
                const char *EXP =      DATA[i].d_exp;

                const char *res = Obj::toAscii(CODE);
                if (veryVerbose) { cout << '\t'; P_(i); P_(CODE); P(res); }
                LOOP2_ASSERT(LINE, i, strlen(EXP) == strlen(res));
                LOOP2_ASSERT(LINE, i, 0 == strcmp(EXP, res));
            }
        }

        if (verbose) cout << "Testing 'operator<<'." << endl;
        {
            const int SIZE = 100;
            char buf[SIZE];
            const char XX = (char) 0xFF;  // Value for an unset char.
            char mCtrl[SIZE];     memset(mCtrl, XX, SIZE);
            const char *CTRL = mCtrl;

            for (int i = 0 ; i < NUM_DATA; ++i) {
                const int LINE =       DATA[i].d_line;
                const Obj::Type CODE = DATA[i].d_code;
                const char *EXP =      DATA[i].d_exp;

                memset(buf, XX, SIZE);
                ostrstream out(buf, SIZE);
                out << CODE << ends;

                const int SZ = strlen(EXP) + 1;
                if (veryVerbose) { cout << '\t'; P_(i); P(buf); }
                LOOP2_ASSERT(LINE, i, XX == buf[SIZE - 1]);
                LOOP2_ASSERT(LINE, i, 0 == memcmp(buf, EXP, SZ));
                LOOP2_ASSERT(LINE, i,
                             0 == memcmp(buf + SZ, CTRL + SZ, SIZE - SZ));
            }
        }

        if (verbose) cout << "Verifying value of 'LENGTH'." << endl;
        ASSERT(19 == Obj::BSLX_LENGTH);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

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
