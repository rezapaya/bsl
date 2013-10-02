// bslx_instreamfunctions.t.cpp                                       -*-C++-*-

#include <bslx_instreamfunctions.h>

#include <bsls_platform.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides a template class that has one static member
// function, which invokes a method of its template parameter.  In
// addition, the component provides a number of specializations of this class
// for fundamental types and 'bsl::string'.  The testing requirements are
// fairly straightforward, and somewhat similar to the testing strategy for an
// abstract protocol.  It is sufficient to provide a test type and a test
// stream, each of which responds in a simple, observable manner when its
// various methods are called.
//-----------------------------------------------------------------------------
// template <typename TYPE> class InStreamFunctions
//      template <typename STREAM>
//      STREAM& streamIn(STREAM& stream, TYPE& object, int version)
//
//  class InStreamFunctions<bsls::Types::Int64>
//      template <typename STREAM>
//      STREAM& streamIn(STREAM& stream, Int64& variable, int version)
//
//  class InStreamFunctions<bsls::Types::Uint64>
//      template <typename STREAM>
//      STREAM& streamIn(STREAM& stream, Uint64& variable, int version)
//
//  class InStreamFunctions<int>
//      template <typename STREAM>
//      STREAM& streamIn(STREAM& stream, int& variable, int version)
//
//  class InStreamFunctions<unsigned int>
//      template <typename STREAM>
//      STREAM& streamIn(STREAM&, unsigned int&, int)
//
//  class InStreamFunctions<short>
//      template <typename STREAM>
//      STREAM& streamIn(STREAM& stream, short& variable, int version)
//
//  class InStreamFunctions<unsigned short>
//      template <typename STREAM>
//      STREAM& streamIn(STREAM&, unsigned short&, int)
//
//  class InStreamFunctions<bsl::string>
//      template <typename STREAM>
//      STREAM& streamIn(STREAM&, bsl::string&, int)
//
//  class InStreamFunctions<char>
//      template <typename STREAM>
//      STREAM& streamIn(STREAM& stream, char& variable, int version)
//
//  class InStreamFunctions<signed char>
//      template <typename STREAM>
//      STREAM& streamIn(STREAM&, signed char&, int)
//
//  class InStreamFunctions<unsigned char>
//      template <typename STREAM>
//
//  class InStreamFunctions<double>
//      template <typename STREAM>
//      STREAM& streamIn(STREAM& stream, double& variable, int version)
//
//  class InStreamFunctions<float>
//      template <typename STREAM>
//      STREAM& streamIn(STREAM& stream, float& variable, int version)
//-----------------------------------------------------------------------------
// [ 1] Usage Example

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
static int verbose = 0;
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
#define T_  cout << "\t" << flush;            // Print a tab (w/o newline)

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
// "components": an in-stream, a value-semantic point object, and an 'enum'.
// In all cases, the component designs are very simple, with much of the
// implied functionality omitted, in order to focus attention on the key
// aspects of the functionality of *this* component.
//
// First, consider an 'enum' 'Color' that enumerates a set of colors.
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
// 'short' integer values; this is done to make the 'bslx' stream input byte
// pattern somewhat easier for the reader of this example to recognize when the
// input buffer is printed.
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
        // Return 'true' if the specified 'version' is supported by
        // this class, and 'flase' otherwise.

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
   STREAM& bslxStreamIn(STREAM& stream, int version);
       // Write this value to the specified input 'stream' using the
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
 MyPoint::MyPoint()
 {
 }

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
 Color MyPoint::color() const
 {
     return d_color;
 }

 inline
 int MyPoint::x() const
 {
     return d_x;
 }

 inline
 int MyPoint::y() const
 {
     return d_y;
 }

 // ...

 template <class STREAM>
 inline
 STREAM& MyPoint::bslxStreamIn(STREAM& stream, int version)
 {
     switch (version) {
       case 1: {
         stream.getInt16(d_x);           // write the x coordinate
         stream.getInt16(d_y);           // write the y coordinate
         char color;
         stream.getInt8(color);          // write the color enum as one byte
         d_color = (Color) color;
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
// Finally, we will implement an extremely simple input stream that supports
// the 'bslx' documentation-only protocol.  For simplicity we will use an
// externally managed buffer, and will only show a few methods needed for this
// example.
//..
 // myinstream.h
 // ...

class MyInStream {
    // This class implements a limited-size fixed-buffer input stream that
    // conforms to the 'bslx' protocol for input streams.  This class is
    // suitable for demonstration purposes only.

    const char *d_buffer;   // input buffer, held but not owned
    int         d_length;   // length of 'd_buffer' (bytes)
    int         d_cursor;   // cursor (index into 'd_buffer'

  public:
    // CREATORS
    MyInStream(const char *buffer, int length);
    // Create an input stream using the specified 'buffer' of the
    // specified 'length'.

    ~MyInStream();
    // Destroy this input byte stream.

    // MANIPULATORS
    MyInStream& getVersion(int& version);
        // Consume a version value from this input stream, place that
        // value in the specified 'version', and return a reference to
        // this modifiable stream.  ...

    MyInStream& getInt32(int& value);
        // Consume a 32-bit signed integer value from this input stream,
        // place that value in the specified 'variable', and return a
        // reference to this modifiable stream.  ...

    MyInStream& getInt16(short& value);
        // Consume a 16-bit signed integer value from this input stream,
        // place that value in the specified 'variable', and return a
        // reference to this modifiable stream.  ...

    MyInStream& getInt8(char& value);
        // Consume an 8-bit signed integer value from this input stream,
        // place that value in the specified 'variable', and return a
        // reference to this modifiable stream.  ...

    void invalidate();
        // Put this input stream in an invalid state.  ...

    // ACCESSORS
    operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0
        // otherwise.  An invalid stream is a stream in which insufficient
        // or invalid data was detected during an extraction operation.
        // Note that an empty stream will be valid unless an extraction
        // attempt or explicit invalidation causes it to be otherwise.

    int cursor() const;
        // Return the index of the next byte to be extracted from this
        // stream.  The behavior is undefined unless this stream is valid.

    bool isEmpty() const;
        // Return 'true' if this stream is empty, and 'false' otherwise.
        // The behavior is undefined unless this stream is valid (i.e.,
        // would convert to a non-zero value via the
        // 'operator const void *()' member).  Note that this function
        // enables higher-level components to verify that, after
        // successfully reading all expected data, no data remains.

    int length() const;
        // Return the total number of bytes stored in this stream.
};

//..
// The relevant (inline) implementations are as follows.
//..
// CREATORS
inline
MyInStream::MyInStream(const char *buffer, int length)
: d_buffer(buffer)
, d_length(length)
, d_cursor(0)
{
}

inline
MyInStream::~MyInStream()
{
}

// MANIPULATORS

inline
MyInStream& MyInStream::getVersion(int& value)
{
    value = (unsigned char) d_buffer[d_cursor++];
    return *this;
}

inline
MyInStream& MyInStream::getInt32(int& value)
{
    const unsigned char *buffer = (const unsigned char *) d_buffer;
    value = static_cast<int>((buffer[d_cursor    ] << 24U) +
                             (buffer[d_cursor + 1] << 16U) +
                             (buffer[d_cursor + 2] <<  8U) +
                             (buffer[d_cursor + 3]       ));
    d_cursor += 4;
    return *this;
}

inline
MyInStream& MyInStream::getInt16(short& value)
{
    const unsigned char *buffer = (const unsigned char *) d_buffer;
    value = static_cast<short>((buffer[d_cursor + 0] <<  8) +
                               (buffer[d_cursor + 1]      ));
    d_cursor += 2;
    return *this;
 }

inline
MyInStream& MyInStream::getInt8(char& value)
{
    value = d_buffer[d_cursor];
    d_cursor += 1;  // Imp Note: using syntax parallel to other 'getInt'
    return *this;
}

inline
void MyInStream::invalidate()
{
    d_buffer = 0;
}

// ACCESSORS
inline
MyInStream::operator const void *() const
{
    return d_cursor <= d_length ? d_buffer : 0;
}

inline
int MyInStream::cursor() const
{
    return d_cursor;
}

inline
bool MyInStream::isEmpty() const
{
    return d_cursor >= d_length;
}

inline
int MyInStream::length() const
{
    return d_length;
}

//..
// We can now write a small 'main' test program that will use the above 'enum',
// point class, and input stream to illustrate 'InStreamFunctions'
// functionality.  This test program does not attempt to do anything more
// useful than reading values from a stream whose buffer was written "by hand"
// and confirming that the expected values were read correctly from the known
// byte pattern in the buffer.
//
// For each of a sequence of types ('int', 'Color', and 'MyPoint'), we will
// define a reference value 'EXP', and one or more input buffers (without and
// possibly with an initial "version" byte) containing the byte pattern for
// 'EXP' (as interpreted by 'MyInStream').  We will then declare one or more
// variables of that type initialized to a value distinct from 'EXP', create
// two 'MyInStream' objects from the appropriate buffers, and stream in a
// value from each stream using either the 'InStreamFunctions' method
// explicitly (which does not consume a version byte).  We verify success with
// 'assert' statements.  The astute reader may observe that this "application"
// is structured rather like a test driver.  As mentioned above, our intent is
// to focus on the use of *this* component, and not on the *use* of components
// that are clients of this component.  Typical application programmers will
// rarely, if ever, need to use this component directly.
//..


//=============================================================================
//                      GLOBAL TEST CLASSES
//-----------------------------------------------------------------------------


class MyTestInStream {
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
                //   - Misc manipulator functions, when code is a two digit
                //     number with first digit 9.
                //   - Misc accessor functions, when code is a two digit
                //     number with first digit 8.
    bool        d_versionFlag; // Flag indicating if the 'putVersion' function
                               // was called.

  public:
    // CREATORS
    MyTestInStream()
      : d_fun(0)
      , d_versionFlag(false)
    {
    }

    ~MyTestInStream() { }

    MyTestInStream& getLength(int& length)
    {
        d_fun =  10;
        length = 1;
        return *this;
    }

    MyTestInStream& getVersion(int /*version*/) { d_versionFlag = true;
                                                  return *this; }


    typedef bsls::Types::Uint64 Uint64;

    MyTestInStream& getInt64(Int64& /*value*/)   { d_fun = -64; return *this; }
    MyTestInStream& getUint64(Uint64& /*value*/) { d_fun = +64; return *this; }
    MyTestInStream& getInt56(Int64& /*value*/)   { d_fun = -56; return *this; }
    MyTestInStream& getUint56(Int64& /*value*/)  { d_fun = +56; return *this; }
    MyTestInStream& getInt48(Int64& /*value*/)   { d_fun = -48; return *this; }
    MyTestInStream& getUint48(Int64& /*value*/)  { d_fun = +48; return *this; }
    MyTestInStream& getInt40(Int64& /*value*/)   { d_fun = -40; return *this; }
    MyTestInStream& getUint40(Int64& /*value*/)  { d_fun = +40; return *this; }
    MyTestInStream& getInt32(int& /*value*/)     { d_fun = -32; return *this; }
    MyTestInStream& getUint32(unsigned int& /*value*/)
                                              { d_fun = +32; return *this; }
    MyTestInStream& getInt24(int& /*value*/)     { d_fun = -24; return *this; }
    MyTestInStream& getUint24(int& /*value*/)    { d_fun = +24; return *this; }
    MyTestInStream& getInt16(short& /*value*/)   { d_fun = -16; return *this; }
    MyTestInStream& getUint16(unsigned short& /*value*/)
                                              { d_fun = +16; return *this; }
    MyTestInStream& getInt8(char& /*value*/)     { d_fun =  -8; return *this; }
    MyTestInStream& getInt8(signed char& /*value*/)
                                              { d_fun =  -8; return *this; }
    MyTestInStream& getUint8(unsigned char& /*value*/)
                                              { d_fun =  +8; return *this; }
    MyTestInStream& getFloat64(double& /*value*/){ d_fun = 164; return *this; }
    MyTestInStream& getFloat32(float& /*value*/) { d_fun = 132; return *this; }

    MyTestInStream& getArrayInt64(Int64* /*array*/, int /*count*/)
                                            { d_fun = -1064; return *this; }
    MyTestInStream& getArrayUint64(Uint64* /*array*/, int /*count*/)
                                            { d_fun = +1064; return *this; }
    MyTestInStream& getArrayInt56(Int64* /*array*/, int /*count*/)
                                            { d_fun = -1056; return *this; }
    MyTestInStream& getArrayUint56(Uint64* /*array*/, int /*count*/)
                                            { d_fun = +1056; return *this; }
    MyTestInStream& getArrayInt48(Int64* /*array*/, int /*count*/)
                                            { d_fun = -1048; return *this; }
    MyTestInStream& getArrayUint48(Uint64* /*array*/, int /*count*/)
                                            { d_fun = +1048; return *this; }
    MyTestInStream& getArrayInt40(Int64* /*array*/, int /*count*/)
                                            { d_fun = -1040; return *this; }
    MyTestInStream& getArrayUint40(Uint64* /*array*/, int /*count*/)
                                            { d_fun = +1040; return *this; }
    MyTestInStream& getArrayInt32(int* /*array*/, int /*count*/)
                                            { d_fun = -1032; return *this; }
    MyTestInStream& getArrayUint32(unsigned int* /*array*/, int /*count*/)
                                            { d_fun = +1032; return *this; }
    MyTestInStream& getArrayInt24(int* /*array*/, int /*count*/)
                                            { d_fun = -1024; return *this; }
    MyTestInStream& getArrayUint24(unsigned int* /*array*/, int /*count*/)
                                            { d_fun = +1024; return *this; }
    MyTestInStream& getArrayInt16(short* /*array*/, int /*count*/)
                                            { d_fun = -1016; return *this; }
    MyTestInStream& getArrayUint16(unsigned short* /*array*/, int /*count*/)
                                            { d_fun = +1016; return *this; }
    MyTestInStream& getArrayInt8(signed char* /*array*/, int /*count*/)
                                            { d_fun = -1008; return *this; }
    MyTestInStream& getArrayUint8(unsigned char* /*array*/, int /*count*/)
                                            { d_fun = +1008; return *this; }
    MyTestInStream& getArrayInt8(char* /*array*/, int /*count*/)
                                            { d_fun = -1001; return *this; }
    MyTestInStream& getArrayUint8(char* /*array*/, int /*count*/)
                                            { d_fun = +1001; return *this; }
    MyTestInStream& getArrayFloat64(double* /*array*/, int /*count*/)
                                            { d_fun = +1164; return *this; }
    MyTestInStream& getArrayFloat32(float* /*array*/, int /*count*/)
                                            { d_fun = +1132; return *this; }

    MyTestInStream& getString(bsl::string& /*value*/)
                                            { d_fun = +10000; return *this; }

    // MANIPULATORS
    void            invalidate()            { d_fun =  99; }
    void            removeAll()             { d_fun =  98; }
    void            reserveCapacity(int /*newCapacity*/)
                                            { d_fun =  97; }
    void            resetVersionFlag()      { d_versionFlag = false; }

    // ACCESSORS
    operator const void *() const           { return this; }
    const char     *data() const            { d_fun =  88; return 0; }
    int             length() const          { d_fun =  87; return 0; }

    int  fun() const { return d_fun; }
    bool versionFlag() const { return d_versionFlag; }
        // Return descriptive code for the function called.
};

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

    // MANIPULATORS
    template <class STREAM>
    STREAM& bslxStreamIn(STREAM& stream, int version);

    // ACCESSORS
    bool isBslxVersionSupported(int version) const;
};

template <class STREAM>
STREAM& MyTestClass::bslxStreamIn(STREAM& stream, int /*version*/)
{
    stream.removeAll();
    return stream;
}

bool MyTestClass::isBslxVersionSupported(int version) const
{
    return VERSION == version;
}

//=============================================================================
//                                  MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    // int veryVerbose = argc > 3;

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

 // Mytestapp.m.cpp

//  int main(int argc, char *argv[])
//  {
     {
         const int  EXP       = 0x0A0B0C0D;
         const char buffer[4] = { 0xA, 0xB, 0xC, 0xD };  // int (no version)
         int        i         = 0;

         MyInStream in1(buffer, 4);  // use the one buffer
         bslx::InStreamFunctions::streamIn(in1, i, 1);
                                          ASSERT(in1);  ASSERT(EXP == i);

         i = 0;                       // reset 'i'
         MyInStream in2(buffer, 4);  // re-use 'buffer (no version)
         bslx::InStreamFunctions::streamIn(in2, i, 0);
                                          ASSERT(in2);  ASSERT(EXP == i);
     }

     {
         const MyPoint EXP(0, -1, BLUE);
         const char buffer1[5] = { 0, 0, -1, -1, 2 };    //MyPoint (no ver)
         const char buffer2[6] = { 1, 0, 0, -1, -1, 2 }; //version, MyPoint
         MyPoint p1, p2;  // two default points

         MyInStream in1(buffer1, 5);  // 'buffer1' has no version byte
         bslx::InStreamFunctions::streamIn(in1, p1, 1);
                                          ASSERT(in1);  ASSERT(EXP == p1);

         MyInStream in2(buffer2, 6);  // 'buffer2' *has* a version byte
         int version;
         in2.getVersion(version);         ASSERT(1 == version);
         bslx::InStreamFunctions::streamIn(in2, p2, version);
                                          ASSERT(in2);  ASSERT(EXP == p2);
     }
//
//      return 0;
//  }
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Create 'bslx::ByteOutStream' objects using default and copy
        //   constructors.  Exercise these objects using some "put" methods,
        //   basic accessors, equality operators, and the assignment operator.
        //   Display object values frequently in verbose mode.
        //
        // Plan:
        //  Test the 'streamOut'functions for all fundamental types,
        //  enums and value-semantic objects.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bool                        X0;

        char                        XA;
        signed char                 XB;
        unsigned char               XC;

        short                       XD;
        unsigned short              XE;

        int                         XF;
        unsigned int                XG;

        long                        XFA;
        unsigned long               XFB;

        Int64                       XH;
        Uint64                      XI;

        float                       XJ;
        double                      XK;

        bsl::string                 XL;
        MyTestClass                 XM;

        bsl::vector<char>           XN;
        bsl::vector<unsigned char>  XNU;
        bsl::vector<signed char>    XNS;

        bsl::vector<MyTestClass>    XO;

        bsl::vector<short>          XQ;
        bsl::vector<unsigned short> XQU;

        bsl::vector<int>            XR;
        bsl::vector<unsigned int>   XRU;

        bsl::vector<Int64>          XS;
        bsl::vector<Uint64>         XSU;

        bsl::vector<float>          XT;
        bsl::vector<double>         XU;
        bsl::vector<bsl::string>    XV;

        MyTestEnum                  XZ;
        bsl::vector<MyTestEnum>     XXA;

        bsl::vector<bsl::vector<char> >           XNA;
        bsl::vector<bsl::vector<unsigned char> >  XNAU;
        bsl::vector<bsl::vector<signed char> >    XNAS;

        bsl::vector<bsl::vector<short> >          XQA;
        bsl::vector<bsl::vector<unsigned short> > XQAU;

        bsl::vector<bsl::vector<int> >            XRA;
        bsl::vector<bsl::vector<unsigned int> >   XRAU;

        if (verbose) cout << "\nTesting 'streamIn'" << endl;
        {

            MyTestInStream stream;

            bslx::InStreamFunctions::streamIn(stream, X0, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -8 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XA, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -8 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XB, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -8 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XC, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +8 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XD, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -16 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XE, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +16 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XF, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -32 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XG, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +32 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XFA, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -32 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XFB, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 32 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XH, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -64 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XI, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +64 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XJ, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 132 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XK, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 164 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XL, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 10000 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XM, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 98 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XN, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1001 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XNU, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 8 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XNS, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -8 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XO, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 98 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XQ, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1016 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XQU, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 16 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XR, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1032 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XRU, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 32 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XS, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1064 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XSU, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 64 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XT, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +1132 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XU, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), +1164 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XV, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 10000 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XZ, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -32 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XXA, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -32 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XNA, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1001 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XNAU, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 8 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XNAS, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -8 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XQA, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1016 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XQAU, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), 16 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XRA, 1);
            ASSERT(false == stream.versionFlag());
            LOOP_ASSERT(stream.fun(), -1032 == stream.fun());

            bslx::InStreamFunctions::streamIn(stream, XRAU, 1);
            ASSERT(false == stream.versionFlag());
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
