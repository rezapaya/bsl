// bslx_instreamfunctions.h                                           -*-C++-*-
#ifndef INCLUDED_BSLX_INSTREAMFUNCTIONS
#define INCLUDED_BSLX_INSTREAMFUNCTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enable uniform (un)externalization of user and fundamental types.
//
//@CLASSES:
//  bslx::InStreamFunctions: namespace for 'bslx' type unexternalization
//
//@SEE_ALSO: bslx_outstreamfunctions, bslx_versionfunctions
//
//@DESCRIPTION: This component provides a namespace, 'bslx::InStreamFunctions',
// that enables uniform support for 'bslx' unexternalization across all 'bslx'-
// compliant user-defined types, including template types and containers, as
// well as those fundamental types (and 'bsl::string' and 'bsl::vector') for
// which the 'bslx' protocol provides direct support.
//
// The namespace 'bslx::InStreamFunctions' allows clients to unexternalize
// objects in a uniform, type independent manner.  It contains the 'streamIn'
// function that unexternalizes objects of all 'bslx'-compliant types.  This
// function unexternalizes the specified 'object' in the 'version' read from
// the input stream.  The 'streamOut' function is specialized for fundamental
// types, 'enum' types, 'bsl::string' and 'bsl::vector' does not input the
// version while unexternalizing these types.
//
// Enum types are streamed in as 32 bit 'int's.  Users can override this
// default behavior by providing specializations of the 'bslx::streamIn'
// function for their 'enum' type.  The general form of this specialization
// will be:
//..
//  namespace bslx {
//  namespace InStreamFunctions {
//
//    template <class STREAM>
//    STREAM& streamIn(STREAM& stream, MyEnum& value, int version)
//    {
//        // Code to stream in objects of MyEnum type
//
//        return stream;
//    }
//  }
//  }
//..
// For value-semantic types following the 'bslx' protocol, 'streamIn' calls the
// 'bslxStreamIn' function for that type.
//
///Component Design, Anticipated Usage, and the 'bslx' Contract
///------------------------------------------------------------
// 'bslx_instreamfunctions' is an integral part of the 'bslx' unexternalization
// contract.  The 'bslx' contract is at least in part "collaborative", which is
// to say that each developer of a given *kind* of component (e.g., a stream or
// a value-semantic container) must comply with the relevant portions of the
// contract to ensure that the "system as a whole" works for everybody.
// 'bslx_instreamfunctions' plays several related but different roles in
// helping various developers to produce 'bslx'-compliant components.  In this
// section we briefly highlight how and why 'bslx_instreamfunctions' is helpful
// (or required) for these different developers.  By discussing different
// aspects of usage, we hope also to convey the general design goals of this
// component, and, to a certain extent, the overall 'bslx' contract.  See the
// 'bslx' package-level documentation for a full specification of the 'bslx'
// contract.
//
///Implementing 'bslx' Streaming in Value-Semantic Template Classes
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The author of a non-template value-semantic type has full knowledge of the
// details of the "value" of that type, and may choose to use the appropriate
// input stream 'put' methods directly when implementing the required
// 'bslxStreamIn' method for that type.  However, if one or more aspects of the
// value are of template parameterized type, then the author cannot in general
// know how to stream the value using the 'put' methods.  For example, if a
// type has as its value one 'int' data member:
//..
//    int d_value;
//..
// then the implementation of the 'bslxStreamIn' method can contain:
//..
//    stream.putInt32(d_value);
//..
// However, if the data member is of parameterized 'TYPE':
//..
//    TYPE d_value;
//..
// then the implementation of the 'bslxStreamIn' method must rely on the
// 'bslx::InStreamFunctions' implementation to write the value:
//..
//    bslx::InStreamFunctions::streamIn(stream, d_value, 1);
//..
// This call will resolve to the correct sequence of 'put' calls no matter
// whether 'TYPE' is a fundamental type, a 'bslx'-compliant 'enum', or a proper
// 'bslx'-compliant class.  In the latter two cases, the explicit specification
// of the version format (in this case, 1) guarantees the stable operation of
// this method whether or not 'TYPE' chooses to change its version format.
//
///Factored "Input" Methods
///- - - - - - - - - - - - -
// One obvious design choice for 'bslx_instreamfunctions' is that it supports
// the *input* portion of the 'bslx' "contract" only.  This factoring is by no
// means central to the 'bslx' contract as a whole, but allows authors of
// special-purpose (i.e., non-value-semantic) components to implement a subset
// of the 'bslx' facilities within causing a compile-time error when their
// clients attempt to use that 'bslx' functionality through the template
// mechanism of this component.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:
///- - - - -
// In this example we illustrate the primary intended use of the
// templatized methods of this component, as well as a few trivial
// invocations just to show the syntax clearly.  To accomplish this,
// we exhibit two separate example "components": a value-semantic
// point object, and an 'enum'.  In all cases, the component designs
// are very simple, with much of the implied functionality omitted, in
// order to focus attention on the key aspects of the functionality of
// *this* component.
//
// First, consider an 'enum' 'Color' that enumerates a set of colors:
//..
//  enum Color {
//      RED            =  0,
//      GREEN          =  1,
//      BLUE           =  2
//  };
//..
// Next, we consider a very special-purpose point that has as a data member its
// color.  Such a point provides an excellent opportunity for factoring, but
// since we are interested in highlighting 'bslx' streaming of various types,
// we will present a simple and unfactored design here.  In a real-world
// problem, the 'my_point' component would be implemented differently.
//
// Note that the 'MyPoint' class in this example represents its coordinates as
// 'short' integer values; this is done to make the 'bslx' stream input byte
// pattern somewhat easier for the reader of this example to recognize when the
// input buffer is printed:
//..
//  // mypoint.h
//
//  class MyPoint {
//      // This class provides a geometric point having integer coordinates and
//      // an enumerated color property.
//
//      short d_x;      // x coordinate
//      short d_y;      // y coordinate
//      Color d_color;  // enumerated color property
//
//    public:
//    // CLASS METHODS
//    static bool isBslxVersionSupported(int version);
//        // Return 'true' if this class supports the specified 'version',
//        // and 'false' otherwise.
//
//    // CREATORS
//    MyPoint();
//        // Create a valid default point.
//
//    MyPoint(int x, int y, Color color);
//        // Create a point having the specified 'x' and 'y' coordinates
//        // and the specified 'color'.
//
//    ~MyPoint();
//        // Destroy this point.
//
//    // MANIPULATORS
//    // ...
//
//    // ACCESSORS
//    int x() const;
//        // Return the x coordinate of this point.
//
//    int y() const;
//        // Return the y coordinate of this point.
//
//    Color color() const;
//        // Return the enumerated color of this point.
//
//    template <class STREAM>
//    STREAM& bslxStreamIn(STREAM& stream, int version);
//        // Write this value to the specified input 'stream' using the
//        // specified 'version' format and return a reference to the
//        // modifiable 'stream'.  ...
//  };
//
//  // FREE OPERATORS
//  inline
//  int operator==(const MyPoint& lhs, const MyPoint& rhs);
//      // Return 1 if the specified 'lhs' and 'rhs' points have the same value
//      // and 0 otherwise.  Two points have the same value if they have the
//      // same x and y coordinates and the same color.
//
//..
// Representative (inline) implementations of these methods are shown below:
//..
//  //                  INLINE FUNCTION DEFINITIONS
//  //
//  // CLASS METHODS
//  inline
//  bool MyPoint::isBslxVersionSupported(int version)
//  {
//      return 1 == version;
//  }
//
//  // CREATORS
//  inline
//  MyPoint::MyPoint()
//  {
//  }
//
//  inline
//  MyPoint::MyPoint(int x, int y, Color color)
//  : d_x(x)
//  , d_y(y)
//  , d_color(color)
//  {
//  }
//
//  inline
//  MyPoint::~MyPoint()
//  {
//  }
//
//  // ...
//
//  // MANIPULATORS
//  // ...
//
//  // ACCESSORS
//  inline
//  Color MyPoint::color() const
//  {
//      return d_color;
//  }
//  inline
//  int MyPoint::x() const
//  {
//      return d_x;
//  }
//  inline
//  int MyPoint::y() const
//  {
//      return d_y;
//  }
//  // ...
//
//  template <class STREAM>
//  inline
//  STREAM& MyPoint::bslxSreamIn(STREAM& stream, int version)
//  {
//      switch (version) {
//        case 1: {
//          stream.getInt16(d_x);           // write the x coordinate
//          stream.getInt16(d_y);           // write the y coordinate
//          char color;
//          stream.getInt8(color);          // write the color enum as one byte
//          d_color = (Color) color;
//        } break;
//      }
//      return stream;
//  }
//
//  // FREE OPERATORS
//  inline
//  int operator==(const MyPoint& lhs, const MyPoint& rhs)
//  {
//      return lhs.x()     == rhs.x()
//          && lhs.y()     == rhs.y()
//          && lhs.color() == rhs.color();
//  }
//..
// We can now write a small 'main' test program that will use the
// above 'enum', point class, and 'bslx::ByteInStream' to illustrate
// 'bslx::InStreamFunctions' functionality.  This test program does
// not attempt to do anything more useful than reading values from a
// stream whose buffer was written "by hand" and confirming that the
// expected values were read correctly from the known byte pattern in
// the buffer.
//
// For each of a sequence of types ('int', 'Color', and 'MyPoint'), we
// will define a reference value 'EXP', and one or more input buffers
// (without and possibly with an initial "version" byte) containing
// the byte pattern for 'EXP'.  We will then declare one or more
// variables of that type initialized to a value distinct from 'EXP',
// create two 'bslx::ByteInStream' objects from the appropriate
// buffers, and stream in a value from each stream using either the
// 'bslx::InStreamFunctions' method explicitly (which does not consume
// a version byte).  We verify success with 'assert' statements.  The
// astute reader may observe that this "application" is structured
// rather like a test driver.  As mentioned above, our intent is to
// focus on the use of *this* component, and not on the *use* of
// components that are clients of this component.  Typical application
// programmers will rarely, if ever, need to use this component
// directly:
//..
//  // mytestapp.m.cpp
//
//  int main(int argc, char *argv[])
//  {
//      {
//          const int  EXP       = 0x0A0B0C0D;
//          const char buffer[4] = { 0xA, 0xB, 0xC, 0xD };  // int (no version)
//          int        i         = 0;
//
//          bslx::ByteInStream in1(buffer, 4);  // use the one buffer
//          bslx::InStreamFunctions::streamIn(in1, i, 1);
//                                           assert(in1);  assert(EXP == i);
//
//          i = 0;                       // reset 'i'
//          bslx::ByteInStream in2(buffer, 4);  // re-use 'buffer (no version)
//          bslx::InStreamFunctions::streamIn(in2, i, 0);
//                                           assert(in2);  assert(EXP == i);
//      }
//
//      {
//          const MyPoint EXP(0, -1, BLUE);
//          const char buffer1[5] = { 0, 0, -1, -1, 2 };    //MyPoint (no ver)
//          const char buffer2[6] = { 1, 0, 0, -1, -1, 2 }; //version, MyPoint
//          MyPoint p1, p2;  // two default points
//
//          bslx::InStream in1(buffer1, 5);  // 'buffer1' has no version byte
//          bslx::InStreamFunctions::streamIn(in1, p1, 1);
//                                           assert(in1);  assert(EXP == p1);
//
//          bslx::ByteInStream in2(buffer2, 6);  // 'buffer2' *has* a version
//          int version;
//          in2.getVersion(version);         assert(1 == version);
//          bslx::InStreamFunctions::streamIn(in2, p2, version);
//                                           assert(in2);  assert(EXP == p2);
//      }
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

namespace BloombergLP {
namespace bslx {

                         // ===========================
                         // namespace InStreamFunctions
                         // ===========================

namespace InStreamFunctions {

                         // ================
                         // class IsEnumType
                         // ================

class IsEnumType {
    // This class is used to perform function overload
    // resolution for types that are 'enum' types.
    // This class contains no interface or implementation by design.
};

                         // ===================
                         // class IsNotEnumType
                         // ===================

class IsNotEnumType {
    // This class is used to perform function overload
    // resolution for types that are *not* 'enum' types.
    // This class contains no interface or implementation by design.
};

// CLASS METHODS
template <typename STREAM, typename TYPE>
STREAM& streamInImp(STREAM&              stream,
                    TYPE&                object,
                    int                  version,
                    const IsEnumType&    dummy);
    // Assign to the specified 'object' the value read from the specified
    // input 'stream', and return a reference to the modifiable 'stream'.
    // If 'stream' is initially invalid, this operation has no effect.  If
    // 'stream' becomes invalid during this operation, 'object' is valid,
    // but its value is undefined.  Note that this function is called for
    // 'enum' types and that no version is read from 'stream'.

template <typename STREAM, typename TYPE>
STREAM& streamInImp(STREAM&              stream,
                    TYPE&                object,
                    int                  version,
                    const IsNotEnumType& dummy);
    // Assign to the specified 'object' the value read from the
    // specified input 'stream' using the specified 'version' format
    // and return a reference to the modifiable 'stream'.  If 'stream'
    // is initially invalid, this operation has no effect.  If
    // 'stream' becomes invalid during this operation, 'object' is
    // valid, but its value is undefined.  If 'version' is not
    // supported, 'stream' is marked invalid and 'object' is
    // unaltered.  The behavior is undefined unless 'TYPE' supports a
    // 'bslx'-compliant 'bslxStreamIn' method and 'STREAM' is a
    // 'bslx'-compliant input stream.  Note that no version is read
    // from 'stream'.

template <typename STREAM, typename TYPE>
STREAM& streamIn(STREAM& stream, TYPE& object, int version)
    // Assign to the specified 'object' the value read from the specified
    // input 'stream' using the specified 'version' format and return a
    // reference to the modifiable 'stream'.  If 'stream' is initially
    // invalid, this operation has no effect.  If 'stream' becomes invalid
    // during this operation, 'object' is valid, but its value is
    // undefined.  If 'version' is not supported, 'stream' is marked
    // invalid and 'object' is unaltered.  Note that no version is read
    // from 'stream'.
{
    // Select the function to call depending on whether the TYPE is an
    // enum type or not.  Note the use of the dummy structs
    // 'IsEnumType' and 'IsNotEnumType' to choose the correct
    // function.

    typedef typename bslmf::If<bslmf::IsEnum<TYPE>::VALUE,
                               IsEnumType,
                               IsNotEnumType>::Type dummyType;

    return streamInImp(stream, object, version, dummyType());
}

       // ------------------------------
       // template class specializations
       // ------------------------------

template <typename STREAM>
STREAM& streamIn(STREAM& stream, bool& variable, int version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM>
STREAM& streamIn(STREAM& stream, char& variable, int version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM>
STREAM& streamIn(STREAM& stream, signed char& variable, int version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM>
STREAM& streamIn(STREAM& stream, unsigned char& variable, int version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM>
STREAM& streamIn(STREAM& stream, short& variable, int version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM>
STREAM& streamIn(STREAM& stream, unsigned short& variable, int version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM>
STREAM& streamIn(STREAM& stream, int& variable, int version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM>
STREAM& streamIn(STREAM& stream, unsigned int& variable, int version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM>
STREAM& streamIn(STREAM& stream, long& variable, int version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.  Also note
    // that only 4 bytes are read from 'stream', even on platforms on which
    // 'sizeof(long)' is larger.

template <typename STREAM>
STREAM& streamIn(STREAM& stream, unsigned long& variable, int version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.  Also note
    // that only 4 bytes are read from 'stream', even on platforms on which
    // 'sizeof(unsigned long)' is larger.

template <typename STREAM>
STREAM& streamIn(STREAM&             stream,
                 bsls::Types::Int64& variable,
                 int                 version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM>
STREAM& streamIn(STREAM&              stream,
                 bsls::Types::Uint64& variable,
                 int                  version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM>
STREAM& streamIn(STREAM& stream, float& variable, int version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM>
STREAM& streamIn(STREAM& stream, double& variable, int version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM>
STREAM& streamIn(STREAM& stream, bsl::string& variable, int version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM, typename ALLOC>
STREAM& streamIn(STREAM&                   stream,
                 bsl::vector<char, ALLOC>& variable,
                 int                       version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM, typename ALLOC>
STREAM& streamIn(STREAM&                    stream,
                 bsl::vector<short, ALLOC>& variable,
                 int                        version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM, typename ALLOC>
STREAM& streamIn(STREAM&                  stream,
                 bsl::vector<int, ALLOC>& variable,
                 int                      version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM, typename ALLOC>
STREAM& streamIn(STREAM&                                 stream,
                 bsl::vector<bsls::Types::Int64, ALLOC>& variable,
                 int                                     version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM, typename ALLOC>
STREAM& streamIn(STREAM&                    stream,
                 bsl::vector<float, ALLOC>& variable,
                 int                        version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM, typename ALLOC>
STREAM& streamIn(STREAM&                     stream,
                 bsl::vector<double, ALLOC>& variable,
                 int                         version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream', and
    // return a reference to the modifiable 'stream'.  If 'stream' is
    // initially invalid, this operation has no effect.  If 'stream'
    // becomes invalid during this operation, the value of 'variable' is
    // undefined.  Note that no version is read from 'stream'.

template <typename STREAM, typename TYPE, typename ALLOC>
STREAM& streamIn(STREAM&                   stream,
                 bsl::vector<TYPE, ALLOC>& variable,
                 int                       version);
    // Assign to the specified 'variable' the value read from the specified
    // input 'stream' using the appropriate 'get' method of 'stream' that
    // is passed the specified 'version', and return a reference to the
    // modifiable 'stream'.  If 'stream' is initially invalid, this
    // operation has no effect.  If 'stream' becomes invalid during this
    // operation, the value of 'variable' is undefined.  Note that no
    // version is read from 'stream'.
}

// ============================================================================
//                 INLINE DEFINITIONS
// ============================================================================

template <typename STREAM, typename TYPE>
inline
STREAM& InStreamFunctions::streamInImp(STREAM&                 stream,
                                       TYPE&                   object,
                                       int                  /* version */,
                                       const IsEnumType&    /* dummy   */)
{
    if (stream) {
        int enumValue = 0;
        stream.getInt32(enumValue);

        if (stream) {
            object = (TYPE) enumValue;
        }
    }
    return stream;
}

template <typename STREAM, typename TYPE>
inline
STREAM& InStreamFunctions::streamInImp(STREAM&              stream,
                                       TYPE&                object,
                                       int                  version,
                                       const IsNotEnumType& /* dummy   */)
{
    return object.bslxStreamIn(stream, version);
}

template <typename STREAM>
inline
STREAM& InStreamFunctions::streamIn(STREAM& stream,
                                    bool&   variable,
                                    int  /* version */)
{
    char temp = 0;

    stream.getInt8(temp);
    variable = static_cast<bool>(temp);

    return stream;
}

template <typename STREAM>
inline
STREAM& InStreamFunctions::streamIn(STREAM& stream,
                                    char&   variable,
                                    int  /* version */)
{
    return stream.getInt8(variable);
}

template <typename STREAM>
inline
STREAM& InStreamFunctions::streamIn(STREAM&      stream,
                                    signed char& variable,
                                    int       /* version */)
{
    return stream.getInt8(variable);
}

template <typename STREAM>
inline
STREAM& InStreamFunctions::streamIn(STREAM&        stream,
                                    unsigned char& variable,
                                    int         /* version */)
{
    return stream.getUint8(variable);
}

template <typename STREAM>
inline
STREAM& InStreamFunctions::streamIn(STREAM& stream,
                                    short&  variable,
                                    int  /* version */)
{
    return stream.getInt16(variable);
}

template <typename STREAM>
inline
STREAM& InStreamFunctions::streamIn(STREAM&         stream,
                                    unsigned short& variable,
                                    int          /* version */)
{
    return stream.getUint16(variable);
}

template <typename STREAM>
inline
STREAM& InStreamFunctions::streamIn(STREAM& stream,
                                    int&    variable,
                                    int  /* version */)
{
    return stream.getInt32(variable);
}

template <typename STREAM>
inline
STREAM& InStreamFunctions::streamIn(STREAM&       stream,
                                    unsigned int& variable,
                                    int        /* version */)
{
    return stream.getUint32(variable);
}

template <typename STREAM>
inline
STREAM& InStreamFunctions::streamIn(STREAM& stream,
                                    long&   variable,
                                    int  /* version */)
{
    int temp = 0;  // 'long' and 'int' may not be the same size.
    stream.getInt32(temp);
    variable = temp;
    return stream;
}

template <typename STREAM>
inline
STREAM& InStreamFunctions::streamIn(STREAM&        stream,
                                    unsigned long& variable,
                                    int         /* version */)
{
    unsigned int temp = 0;  // 'unsigned long' and 'unsigned int' may not
                            // be the same size.
    stream.getUint32(temp);
    variable = temp;
    return stream;
}

template <typename STREAM>
inline
STREAM& InStreamFunctions::streamIn(STREAM&             stream,
                                    bsls::Types::Int64& variable,
                                    int              /* version */)
{
    return stream.getInt64(variable);
}

template <typename STREAM>
inline
STREAM& InStreamFunctions::streamIn(STREAM&              stream,
                                    bsls::Types::Uint64& variable,
                                    int               /* version */)
{
    return stream.getUint64(variable);
}

template <typename STREAM>
inline
STREAM& InStreamFunctions::streamIn(STREAM& stream,
                                    float&  variable,
                                    int  /* version */)
{
    return stream.getFloat32(variable);
}

template <typename STREAM>
inline
STREAM& InStreamFunctions::streamIn(STREAM& stream,
                                    double& variable,
                                    int  /* version */)
{
    return stream.getFloat64(variable);
}

template <typename STREAM>
inline
STREAM& InStreamFunctions::streamIn(STREAM&      stream,
                                    bsl::string& variable,
                                    int       /* version */)
{
    return stream.getString(variable);
}

template <typename STREAM, typename ALLOC>
inline
STREAM& InStreamFunctions::streamIn(STREAM&                   stream,
                                    bsl::vector<char, ALLOC>& variable,
                                    int                    /* version */)
{
    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;
    }

    variable.resize(length);

    return (0 < length) ? stream.getArrayInt8(&variable[0], length)
                        : stream;
}

template <typename STREAM, typename ALLOC>
inline
STREAM& InStreamFunctions::streamIn(STREAM&                    stream,
                                    bsl::vector<short, ALLOC>& variable,
                                    int                     /* version */)
{
    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;
    }

    variable.resize(length);

    return (0 < length) ? stream.getArrayInt16(&variable[0], length)
                        : stream;
}

template <typename STREAM, typename ALLOC>
inline
STREAM& InStreamFunctions::streamIn(STREAM&                  stream,
                                    bsl::vector<int, ALLOC>& variable,
                                    int                   /* version */)
{
    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;
    }

    variable.resize(length);

    return (0 < length) ? stream.getArrayInt32(&variable[0], length)
                        : stream;
}

template <typename STREAM, typename ALLOC>
inline
STREAM& InStreamFunctions::streamIn(
                            STREAM&                                 stream,
                            bsl::vector<bsls::Types::Int64, ALLOC>& variable,
                            int                                  /* version */)
{
    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;
    }

    variable.resize(length);

    return (0 < length) ? stream.getArrayInt64(&variable[0], length)
                        : stream;
}

template <typename STREAM, typename ALLOC>
inline
STREAM& InStreamFunctions::streamIn(STREAM&                    stream,
                                    bsl::vector<float, ALLOC>& variable,
                                    int                     /* version */)
{
    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;
    }

    variable.resize(length);

    return (0 < length) ? stream.getArrayFloat32(&variable[0], length)
                        : stream;
}

template <typename STREAM, typename ALLOC>
inline
STREAM& InStreamFunctions::streamIn(STREAM&                     stream,
                                    bsl::vector<double, ALLOC>& variable,
                                    int                      /* version */)
{
    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;
    }

    variable.resize(length);

    return (0 < length) ? stream.getArrayFloat64(&variable[0], length)
                        : stream;
}

template <typename STREAM, typename TYPE, typename ALLOC>
inline
STREAM& InStreamFunctions::streamIn(STREAM&                   stream,
                                    bsl::vector<TYPE, ALLOC>& variable,
                                    int                       version)
{
    typedef typename bsl::vector<TYPE, ALLOC>::iterator Iterator;

    int length = 0;
    stream.getLength(length);

    if (!stream) {
        return stream;
    }

    variable.resize(length);

    for (Iterator it = variable.begin(); it != variable.end(); ++it) {

        streamIn(stream, *it, version);

        if (!stream) {
            return stream;
        }
    }

    return stream;
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
