// bslx_outstreamfunctions.t.cpp                                      -*-C++-*-

#include <bslx_outstreamfunctions.h>

#include <bsl_cstring.h>
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_ostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides a template class that has two static member
// functions, each of which invokes a method of its template parameter.  In
// addition, the component provides a number of specializations of this class
// for fundamental types and 'bsl::string'.  The testing requirements are
// fairly straightforward, and somewhat similar to the testing strategy for an
// abstract protocol.  It is sufficient to provide a test type and a test
// stream, each of which responds in a simple, observable manner when its
// various methods are called.
//-----------------------------------------------------------------------------
// template <typename TYPE> class OutStreamFunctions
//      bool isBslxVersionSupported(int version)
//      template <typename STREAM>
//      STREAM& streamOut(STREAM& stream, const TYPE& value, int version)
//
// class OutStreamFunctions<bsls::Types::Int64>
//      bool isBslxVersionSupported(int version)
//      template <typename STREAM>
//      STREAM& streamOut(STREAM& stream, const Int64& value, int version)
//
// class OutStreamFunctions<bsls::Types::Uint64>
//      bool isBslxVersionSupported(int version)
//      template <typename STREAM>
//      STREAM& streamOut(STREAM& stream, const Uint64& value, int version)
//
// class OutStreamFunctions<int>
//      bool isBslxVersionSupported(int version)
//      template <typename STREAM>
//      STREAM& streamOut(STREAM& stream, const int& value, int version)
//
// class OutStreamFunctions<unsigned int>
//      bool isBslxVersionSupported(int version)
//      template <typename STREAM>
//      STREAM& streamOut(STREAM&, const unsigned int&, int)
//
// class OutStreamFunctions<short>
//      bool isBslxVersionSupported(int version)
//      template <typename STREAM>
//      STREAM& streamOut(STREAM& stream, const short& value, int version)
//
// class OutStreamFunctions<unsigned short>
//      bool isBslxVersionSupported(int version)
//      template <typename STREAM>
//      STREAM& streamOut(STREAM&, const unsigned short&, int version)
//
// class OutStreamFunctions<bsl::string>
//      bool isBslxVersionSupported(int version)
//      template <typename STREAM>
//      STREAM& streamOut(STREAM&, const bsl::string&, int)
//
// class OutStreamFunctions<char>
//      bool isBslxVersionSupported(int version)
//      template <typename STREAM>
//      STREAM& streamOut(STREAM& stream, const char& value, int version)
//
// class OutStreamFunctions<signed char>
//      bool isBslxVersionSupported(int version)
//      template <typename STREAM>
//      STREAM& streamOut(STREAM&, const signed char&, int)
//
// class OutStreamFunctions<unsigned char>
//      bool isBslxVersionSupported(int version)
//      template <typename STREAM>
//      STREAM& streamOut(STREAM&, const unsigned char&, int)
//
// class OutStreamFunctions<double>
//      bool isBslxVersionSupported(int version)
//      template <typename STREAM>
//      STREAM& streamOut(STREAM& stream, const double& value, int version)
//
// class OutStreamFunctions<float>
//      bool isBslxVersionSupported(int version)
//      template <typename STREAM>
//      STREAM& streamOut(STREAM& stream, const float& value, int version)
//-----------------------------------------------------------------------------
// [ 1] Usage Example
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) {
            ++testStatus;
        }
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\t" << #K << ": " << K << "\n";       \
                  aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

//=============================================================================
//                   GLOBAL CONSTANTS/TYPEDEFS FOR TESTING
//=============================================================================

typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

//=============================================================================
//              Classes, functions, etc., needed for Usage Example
//-----------------------------------------------------------------------------

// In this example we illustrate the primary intended use of the templatized
// methods of this component, as well as a few trivial invocations just to show
// the syntax clearly.  To accomplish this, we exhibit three separate example
// "components": an out-stream, a value-semantic point object, and an 'enum'.
// In all cases, the component designs are very simple, with much of the
// implied functionality omitted, in order to focus attention on the key
// aspects of the functionality of *this* component.
//
// First, consider an 'enum' 'my_color' that enumerates a set of colors.
//..
 enum Color {
     RED            =  0,
     GREEN          =  1,
     BLUE           =  2
 };
//..
// Next, we consider a very special-purpose point that has as a data member its
// color.  Such a point provides an excellent opportunity for factoring, but
// since we are interested in highlighting 'bslx' streaming of various types,
// we will present a simple and unfactored design here.  In a real-world
// problem, the 'mypoint' component would be implemented differently.
//
// Note that the 'MyPoint' class in this example represents its coordinates as
// 'short' integer values; this is done to make the 'bslx' stream output byte
// pattern somewhat easier for the reader of this example to recognize when the
// output buffer is printed.
//..
 // mypoint.h

 class MyPoint {
     // This class provides a geometric point having integer coordinates and
     // an enumerated color property.

     short d_x;      // x coordinate
     short d_y;      // y coordinate
     Color d_color;  // enumerated color property

   public:
   // CLASS METHODS
   static bool isBslxVersionSupported(int version);
       // Return the most current 'bslx' streaming version number supported
       // by this class.

   // CREATORS
   MyPoint();
       // Create a valid default point.

   MyPoint(int x, int y, Color color);
       // Create a point having the specified 'x' and 'y' coordinates
       // and the specified 'color'.

   ~MyPoint();
       // Destroy this point.

   // MANIPULATORS
   // ...

   // ACCESSORS
   int x() const;
       // Return the x coordinate of this point.

   int y() const;
       // Return the y coordinate of this point.

   Color color() const;
       // Return the enumerated color of this point.

   template <class STREAM>
   STREAM& bslxStreamOut(STREAM& stream, int version) const;
       // Write this value to the specified output 'stream' using the
       // specified 'version' format and return a reference to the
       // modifiable 'stream'.  ...
 };

 // FREE OPERATORS
 inline bool operator==(const MyPoint& lhs, const MyPoint& rhs);
     // Return 'true' if the specified 'lhs' and 'rhs' points have the same
     // value and 'false' otherwise.  Two points have the same value if they
     // have the same x and y coordinates and the same color.

//..
// Representative (inline) implementations of these methods are shown below.
//..
 //                  INLINE FUNCTION DEFINITIONS

 // CLASS METHODS
 inline
 bool MyPoint::isBslxVersionSupported(int version)
 {
     return 1 == version;
 }

 // CREATORS
 inline
 MyPoint::MyPoint(int x, int y, Color color)
 : d_x(x)
 , d_y(y)
 , d_color(color)
 {
 }

 inline
 MyPoint::~MyPoint()
 {
 }

 // ...

 // MANIPULATORS
 // ...

 // ACCESSORS
 inline
 int MyPoint::x() const
 {
     return d_x;
 }

 // ...

 template <class STREAM>
 inline
 STREAM& MyPoint::bslxStreamOut(STREAM& stream, int version) const
 {
     switch (version) {
       case 1: {
         stream.putInt16(d_x);           // write the x coordinate
         stream.putInt16(d_y);           // write the y coordinate
         stream.putInt8((char) d_color); // write the color enum as one byte
       } break;
     }
     return stream;
 }

 // FREE OPERATORS
 inline bool operator==(const MyPoint& lhs, const MyPoint& rhs)
 {
     return lhs.x()     == rhs.x() &&
            lhs.y()     == rhs.y() &&
            lhs.color() == rhs.color();
 }
//..
// Finally, we will implement an extremely simple output stream that supports
// the 'bslx' documentation-only protocol.  For simplicity we will use a
// fixed-size buffer (usually a bad idea in any event, and more so here since
// the implementation knows the buffer size, but makes no effort to prevent
// overwriting that buffer), and will only show a few methods needed for this
// example.  Among the key design features *ignored* in this implementation is
// the byte order, or "endianness" of the host platform.  See other 'bslx'
// stream components for examples of properly-designed streams.
//..
 // myoutstream.h
 // ...

 class MyOutStream {
     // This class implements a limited-size fixed-buffer output stream that
     // conforms to the 'bslx' protocol for output streams.  This class is
     // suitable for demonstration purposes only.

     char d_buffer[1000]; // externalized values stored as contiguous bytes
     int  d_length;       // length of 'd_buffer' (bytes)

   public:
     // CREATORS
     MyOutStream();
         // Create an empty output stream of limited, fixed capacity.  Note
         // that this object is suitable for demonstration purposes only.

     ~MyOutStream();
        // Destroy this output byte stream.

     // MANIPULATORS
     MyOutStream& putVersion(int version);
         // Format the specified non-negative 'version' to this output
         // stream and return a reference to this modifiable stream.

     MyOutStream& putInt32(int value);
         // Format the least significant 32 bits of the specified 'value' to
         // this output stream and return a reference to this modifiable
         // stream.

     MyOutStream& putInt16(int value);
         // Format the least significant 16 bits of the specified 'value' to
         // this output stream and return a reference to this modifiable
         // stream.

     MyOutStream& putInt8(int value);
         // Format the least significant 8 bits of the specified 'value' to
         // this output stream and return a reference to this modifiable
         // stream.

     MyOutStream& putFloat64(double value);
         // Format the most significant 64 bits in the specified 'value' to
         // this output stream and return a reference to this modifiable
         // stream.  ...

     void removeAll();
         // Remove all content in this stream.

     // ACCESSORS
     inline const char *data() const;
         // Return the address of the contiguous, non-modifiable internal
         // memory buffer of this stream.

     inline int length() const;
         // Return the number of bytes in this stream.
 };

 // FREE OPERATORS
 inline
 bsl::ostream& operator<<(bsl::ostream&       stream,
                          const MyOutStream& object);
     // Write the specified 'object' to the specified output 'stream' in
     // some reasonable (multi-line) format, and return a reference to
     // 'stream'.
//..
// The relevant (inline) implementations are as follows.
//..
 //                  INLINE FUNCTION DEFINITIONS

 // CREATORS

 inline
 MyOutStream::MyOutStream()
 : d_length(0)
 {
 }

 inline
 MyOutStream::~MyOutStream()
 {
 }

 // MANIPULATORS

 inline
 MyOutStream& MyOutStream::putVersion(int value)
 {
     unsigned char temp = value;
     memcpy(d_buffer + d_length, &temp, 1);
     ++d_length;
     return *this;
  }

 inline
 MyOutStream& MyOutStream::putInt32(int value)
 {
     memcpy(d_buffer + d_length, (const char *) &value, 4);
     d_length += 4;
     return *this;
 }

 inline
 MyOutStream& MyOutStream::putInt16(int value)
 {
     unsigned short temp = value;
     memcpy(d_buffer + d_length, (const char *) &temp, 2);
     d_length += 2;
     return *this;
 }

 inline
 MyOutStream& MyOutStream::putInt8(int value)
 {
     unsigned char temp = value;
     memcpy(d_buffer + d_length, &temp, 1);
     d_length += 1;  // Imp Note: using syntax parallel to other 'putInt'
     return *this;
 }

 inline
 MyOutStream& MyOutStream::putFloat64(double value)
 {
     const char *tempPtr = (const char *) &value;
     memcpy(d_buffer + d_length, &tempPtr, 8);
     d_length += 8;
     return *this;
 }

 inline
 void MyOutStream::removeAll()
 {
     d_length = 0;
 }

 // ACCESSORS
 inline
 const char *MyOutStream::data() const
 {
     return (const char *) d_buffer;
 }

 inline
 int MyOutStream::length() const
 {
     return d_length;
 }

//=============================================================================
//                      GLOBAL TEST CLASSES
//-----------------------------------------------------------------------------

enum MyTestEnum {
    EnumA = INT_MIN,
    EnumB = -1,
    EnumC = 0,
    EnumD = 1,
    EnumE = INT_MAX
};

class MyTestClass {
    // This test class is used for testing the streaming functionality.

  public:

    enum {
        VERSION = 999
    };

    // CREATORS
    MyTestClass() { }
    ~MyTestClass() { }

    // ACCESSORS
    template <class STREAM>
    STREAM& bslxStreamOut(STREAM& stream, int version) const;

    bool isBslxVersionSupported(int version) const;

};

template <class STREAM>
STREAM& MyTestClass::bslxStreamOut(STREAM& stream, int /*version*/) const
{
    stream.removeAll();
    return stream;
}

bool MyTestClass::isBslxVersionSupported(int version) const
{
    return VERSION == version;
}

class MyTestOutStream {
    // Test class used to test streaming.

    mutable int d_fun;  // holds code describing function:
                //   - Negative implies signed, positive unsigned (e.g., -32
                //     is a signed 32-bit integer, and +16 is an unsigned 16
                //     bit integer).
                //   - The 3rd digit is flag for floating point (e.g., 164 is a
                //     double, and 132 is a float).
                //   - Over 1000 means an arrayed function (e.g., 1164 is an
                //     array of doubles and -1008 is an array of signed char.
                //     Note that -1001 and 1001 correspond to arrays of char
                //     output as signed and unsigned char values, respectively.
                //   - Miscellaneous manipulator functions, when code is a two
                //     digit number with first digit 9.
                //   - Miscellaneous accessor functions, when code is a two
                //     digit number with first digit 8.
    bool        d_versionFlag; // Flag indicating if the 'putVersion' function
                               // was called.
  public:
    // CREATORS
    MyTestOutStream()
      : d_fun(0)
      , d_versionFlag(false)
    {
    }

    ~MyTestOutStream() { }

    MyTestOutStream& putLength(int /*length*/)   { d_fun =  10; return *this; }
    MyTestOutStream& putVersion(int /*version*/) { d_versionFlag = true;
                                                return *this; }

    MyTestOutStream& putInt64(Int64 /*value*/)   { d_fun = -64; return *this; }
    MyTestOutStream& putUint64(Int64 /*value*/)  { d_fun = +64; return *this; }
    MyTestOutStream& putInt56(Int64 /*value*/)   { d_fun = -56; return *this; }
    MyTestOutStream& putUint56(Int64 /*value*/)  { d_fun = +56; return *this; }
    MyTestOutStream& putInt48(Int64 /*value*/)   { d_fun = -48; return *this; }
    MyTestOutStream& putUint48(Int64 /*value*/)  { d_fun = +48; return *this; }
    MyTestOutStream& putInt40(Int64 /*value*/)   { d_fun = -40; return *this; }
    MyTestOutStream& putUint40(Int64 /*value*/)  { d_fun = +40; return *this; }
    MyTestOutStream& putInt32(int /*value*/)     { d_fun = -32; return *this; }
    MyTestOutStream& putUint32(unsigned int /*value*/)
                                              { d_fun = +32; return *this; }
    MyTestOutStream& putInt24(int /*value*/)     { d_fun = -24; return *this; }
    MyTestOutStream& putUint24(int /*value*/)    { d_fun = +24; return *this; }
    MyTestOutStream& putInt16(int /*value*/)     { d_fun = -16; return *this; }
    MyTestOutStream& putUint16(int /*value*/)    { d_fun = +16; return *this; }
    MyTestOutStream& putInt8(int /*value*/)      { d_fun =  -8; return *this; }
    MyTestOutStream& putUint8(int /*value*/)     { d_fun =  +8; return *this; }
    MyTestOutStream& putFloat64(double /*value*/){ d_fun = 164; return *this; }
    MyTestOutStream& putFloat32(float /*value*/) { d_fun = 132; return *this; }

    typedef bsls::Types::Uint64 Uint64;

    MyTestOutStream& putArrayInt64(const Int64* /*array*/, int /*count*/)
                                            { d_fun = -1064; return *this; }
    MyTestOutStream& putArrayUint64(const Uint64* /*array*/, int /*count*/)
                                            { d_fun = +1064; return *this; }
    MyTestOutStream& putArrayInt56(const Int64* /*array*/, int /*count*/)
                                            { d_fun = -1056; return *this; }
    MyTestOutStream& putArrayUint56(const Uint64* /*array*/, int /*count*/)
                                            { d_fun = +1056; return *this; }
    MyTestOutStream& putArrayInt48(const Int64* /*array*/, int /*count*/)
                                            { d_fun = -1048; return *this; }
    MyTestOutStream& putArrayUint48(const Uint64* /*array*/, int /*count*/)
                                            { d_fun = +1048; return *this; }
    MyTestOutStream& putArrayInt40(const Int64* /*array*/, int /*count*/)
                                            { d_fun = -1040; return *this; }
    MyTestOutStream& putArrayUint40(const Uint64* /*array*/, int /*count*/)
                                            { d_fun = +1040; return *this; }
    MyTestOutStream& putArrayInt32(const int* /*array*/, int /*count*/)
                                            { d_fun = -1032; return *this; }
    MyTestOutStream& putArrayUint32(const unsigned int* /*array*/,
                                    int                 /*count*/)
                                            { d_fun = +1032; return *this; }
    MyTestOutStream& putArrayInt24(const int* /*array*/, int /*count*/)
                                            { d_fun = -1024; return *this; }
    MyTestOutStream& putArrayUint24(const unsigned int* /*array*/,
                                    int                 /*count*/)
                                            { d_fun = +1024; return *this; }
    MyTestOutStream& putArrayInt16(const short* /*array*/, int /*count*/)
                                            { d_fun = -1016; return *this; }
    MyTestOutStream& putArrayUint16(const unsigned int* /*array*/,
                                    int                 /*count*/)
                                            { d_fun = +1016; return *this; }
    MyTestOutStream& putArrayInt8(const signed char* /*array*/, int /*count*/)
                                            { d_fun = -1008; return *this; }
    MyTestOutStream& putArrayUint8(const unsigned int* /*array*/,
                                   int                 /*count*/)
                                            { d_fun = +1008; return *this; }
    MyTestOutStream& putArrayInt8(const char* /*array*/, int /*count*/)
                                            { d_fun = -1001; return *this; }
    MyTestOutStream& putArrayUint8(const char* /*array*/, int /*count*/)
                                            { d_fun = +1001; return *this; }
    MyTestOutStream& putArrayFloat64(const double* /*array*/, int /*count*/)
                                            { d_fun = +1164; return *this; }
    MyTestOutStream& putArrayFloat32(const float* /*array*/, int /*count*/)
                                            { d_fun = +1132; return *this; }

    MyTestOutStream& putString(const bsl::string& /*value*/)
                                            { d_fun = +10000; return *this; }

    // MANIPULATORS
    void            invalidate()            { d_fun =  99; }
    void            removeAll()             { d_fun =  98; }
    void            reserveCapacity(int /*newCapacity*/)
                                            { d_fun =  97; }
    void            resetVersionFlag()      { d_versionFlag = false; }

    // ACCESSORS
    operator const void *() const           { d_fun =  89; return this; }
    const char     *data() const            { d_fun =  88; return 0; }
    int             length() const          { d_fun =  87; return 0; }

    int  fun() const { return d_fun; }
    bool versionFlag() const { return d_versionFlag; }
        // Return descriptive code for the function called.
};

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

//..
// We can now write a small 'main' test program that will use the above 'enum',
// point class, and output stream to illustrate 'OutStreamFunctions'
// functionality.  This test program does not attempt to do anything more
// useful than writing known values to a stream and confirming that the
// expected byte pattern was in fact written.
//..
// my_testapp.m.cpp

// using namespace bsl;

// int main(int argc, char **argv)
// {
    int             i     = 168496141;       // byte pattern 0a 0b 0c 0d
    Color           color = BLUE;            // byte pattern 02
    MyPoint         p(0, -1, color);         // byte pattern 00 00 ff ff 02

    MyOutStream     out;                ASSERT(0 == out.length());

    bslx::OutStreamFunctions::streamOut(out, i, 1);
                                        ASSERT(4 == out.length());
    out.removeAll();                    ASSERT(0 == out.length());
    bslx::OutStreamFunctions::streamOut(out, i, 0);
                                        ASSERT(4 == out.length());

    out.removeAll();                    ASSERT(0 == out.length());
    bslx::OutStreamFunctions::streamOut(out, color, 1);
                                        ASSERT(4 == out.length());
    out.removeAll();                    ASSERT(0 == out.length());
    bslx::OutStreamFunctions::streamOut(out, color, 0);
                                        ASSERT(4 == out.length());

    out.removeAll();                    ASSERT(0 == out.length());
    bslx::OutStreamFunctions::streamOut(out, p, 1);
                                        ASSERT(5 == out.length());
    out.removeAll();                    ASSERT(0 == out.length());
    bslx::OutStreamFunctions::streamOut(out, p, 1);
                                        ASSERT(5 == out.length());
// }
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Plan:
        //  Test the 'streamOut' functions for various types.  The
        //  plan for this test case is to test that the correct
        //  externalization functions are called for each data type.
        //  Verification of whether the right data is written to the
        //  stream is left for more thorough tests.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const bool                        X0 = true;

        const char                        XA = 'x';
        const signed char                 XB = 127;
        const unsigned char               XC = 255;

        const short                       XD = 12;
        const unsigned short              XE = 0xFFFF;

        const int                         XF = 103;
        const unsigned int                XG = 0xFFFFFFFF;

        const long                        XFA = 1;
        const unsigned long               XFB = 0xFFFFFFFF;

        const Int64                       XH = 10004;
        const Uint64                      XI = 0xFFFFFFFFFFFFFFFFLL;

        const float                       XJ = 105.5;
        const double                      XK = 106.006;

        const bsl::string                 XL = "one-seven---";
        const MyTestClass                XM;

        bsl::vector<char>                 XN_;
        const bsl::vector<char>&          XN = XN_;

        bsl::vector<signed char>          XNS_;
        const bsl::vector<signed char>&   XNS = XNS_;

        bsl::vector<unsigned char>        XNU_;
        const bsl::vector<unsigned char>& XNU = XNU_;

        bsl::vector<MyTestClass>         XO_;
        const bsl::vector<MyTestClass>&  XO = XO_;

        bsl::vector<char>                 XP_;
        const bsl::vector<char>&          XP = XP_;
        XP_.push_back(XA);
        XP_.push_back(XA);

        bsl::vector<signed char>          XPS_;
        const bsl::vector<signed char>&   XPS = XPS_;
        XPS_.push_back(XB);
        XPS_.push_back(XB);

        bsl::vector<unsigned char>        XPU_;
        const bsl::vector<unsigned char>& XPU = XPU_;
        XPU_.push_back(XC);
        XPU_.push_back(XC);

        bsl::vector<short>                XQ_;
        const bsl::vector<short>&         XQ = XQ_;
        XQ_.push_back(XD);
        XQ_.push_back(XD);

        bsl::vector<unsigned short>        XQU_;
        const bsl::vector<unsigned short>& XQU = XQU_;
        XQU_.push_back(XE);
        XQU_.push_back(XE);

        bsl::vector<int>                  XR_;
        const bsl::vector<int>&           XR = XR_;
        XR_.push_back(XF);
        XR_.push_back(XF);

        bsl::vector<unsigned int>         XRU_;
        const bsl::vector<unsigned int>&  XRU = XRU_;
        XRU_.push_back(XG);
        XRU_.push_back(XG);

        bsl::vector<Int64>                XS_;
        const bsl::vector<Int64>&         XS = XS_;
        XS_.push_back(XH);
        XS_.push_back(XH);

        bsl::vector<Uint64>                XSU_;
        const bsl::vector<Uint64>&         XSU = XSU_;
        XSU_.push_back(XI);
        XSU_.push_back(XI);

        bsl::vector<float>                XT_;
        const bsl::vector<float>&         XT = XT_;
        XT_.push_back(XJ);
        XT_.push_back(XJ);

        bsl::vector<double>               XU_;
        const bsl::vector<double>&        XU = XU_;
        XU_.push_back(XK);
        XU_.push_back(XK);

        bsl::vector<bsl::string>          XV_;
        const bsl::vector<bsl::string>&   XV = XV_;
        XV_.push_back(XL);
        XV_.push_back(XL);

        bsl::vector<MyTestClass>        XW_;
        const bsl::vector<MyTestClass>& XW = XW_;
        XW_.push_back(XM);
        XW_.push_back(XM);

        MyTestEnum                      XX = (MyTestEnum) INT_MIN;
        MyTestEnum                      XY = (MyTestEnum) 0;
        MyTestEnum                      XZ = (MyTestEnum) INT_MAX;

        bsl::vector<MyTestEnum>         XXA;
        bsl::vector<MyTestEnum>         XXB;
        XXB.push_back(XX);
        XXB.push_back(XY);

        bsl::vector<bsl::vector<char> >        XNA_;
        const bsl::vector<bsl::vector<char> >& XNA = XNA_;
        XNA_.push_back(XN);
        XNA_.push_back(XP);

        bsl::vector<bsl::vector<unsigned char> >        XNAU_;
        const bsl::vector<bsl::vector<unsigned char> >& XNAU = XNAU_;
        XNAU_.push_back(XNU);
        XNAU_.push_back(XPU);

        bsl::vector<bsl::vector<signed char> >        XNAS_;
        const bsl::vector<bsl::vector<signed char> >& XNAS = XNAS_;
        XNAS_.push_back(XPS);
        XNAS_.push_back(XPS);

        bsl::vector<bsl::vector<short> >        XQA_;
        const bsl::vector<bsl::vector<short> >& XQA = XQA_;
        XQA_.push_back(XQ);

        bsl::vector<bsl::vector<unsigned short> >        XQAU_;
        const bsl::vector<bsl::vector<unsigned short> >& XQAU = XQAU_;
        XQAU_.push_back(XQU);

        bsl::vector<bsl::vector<int> >        XRA_;
        const bsl::vector<bsl::vector<int> >& XRA = XRA_;
        XRA_.push_back(XR);

        bsl::vector<bsl::vector<unsigned int> >        XRAU_;
        const bsl::vector<bsl::vector<unsigned int> >& XRAU = XRAU_;
        XRAU_.push_back(XRU);

        if (verbose) cout << "\nTesting 'streamOut'" << endl;
        {

            MyTestOutStream stream;

            bslx::OutStreamFunctions::streamOut(stream, X0, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -8 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XA, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -8 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XB, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -8 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XC, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +8 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XD, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -16 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XE, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +16 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XF, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -32 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XG, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +32 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XFA, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -32 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XFB, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +32 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XH, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -64 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XI, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +64 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XJ, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 132 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XK, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 164 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XL, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 10000 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XM, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 98 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XN, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 10 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XNS, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 10 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XNU, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 10 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XO, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 10 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XP, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1001 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XPS, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -8 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XPU, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 8 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XQ, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1016 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XQU, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 16 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XR, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1032 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XRU, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 32 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XS, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1064 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XSU, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 64 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XT, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +1132 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XU, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +1164 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XV, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 10000 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XW, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 98 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XX, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -32 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XY, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -32 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XZ, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -32 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XXA, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 10 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XXB, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -32 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XNA, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1001 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XNAU, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 8 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XNAS, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -8 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XQA, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1016 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XQAU, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 16 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XRA, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1032 == stream.fun());

            bslx::OutStreamFunctions::streamOut(stream, XRAU, 1);
            ASSERT(false  == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 32 == stream.fun());
        }
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
