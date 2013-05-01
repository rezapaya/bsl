// bslx_outstreamfunctions.h                                          -*-C++-*-
#ifndef INCLUDED_BSLX_OUTSTREAMFUNCTIONS
#define INCLUDED_BSLX_OUTSTREAMFUNCTIONS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enable uniform externalization of user and fundamental types.
//
//@CLASSES:
//  bslx::OutStreamFunctions: namespace for uniform 'bslx' type externalization
//
//@SEE_ALSO: bslx_instreamfunctions, bslx_versionfunctions
//
//@DESCRIPTION: This component provides a namespace,
// 'bslx::OutStreamFunctions', that enables uniform support for 'bslx'
// externalization across all 'bslx' compliant user-defined types,
// including template types and containers, as well as those
// fundamental types (and 'bsl::string' and 'bsl::vector') for which
// the 'bslx' protocol provides direct support.
//
// The namespace 'bslx::OutStreamFunctions' allows clients to externalize
// objects in a uniform, type independent manner.  It contains the 'streamOut'
// function that externalizes objects of all 'bslx' compliant types.  This
// function externalizes the specified 'object' in the specified 'version'.
// The 'streamOut' function is specialized for fundamental types, 'enum' types,
// 'bsl::string' and 'bsl::vector'.  Note that version numbers are not output
// while externalizing these types.
//
// Enum types are streamed out as 32 bit 'int's.  Users can override this
// default behavior by providing specializations of the 'bslx::streamOut'
// function for their 'enum' type.  The general form of this specialization
// will be:
//..
//  namespace bslx {
//  namespace OutStreamFunctions {
//
//    template <class STREAM>
//    STREAM& streamOut(STREAM& stream, const MyEnum& value, int version)
//    {
//        // Code to stream out objects of MyEnum type
//
//        return stream;
//    }
//..
// For value-semantic types following the 'bslx' protocol, 'streamOut' calls
// the 'bslxStreamOut' function for that type.
//
///Component Design, Anticipated Usage, and the 'bslx' Contract
///------------------------------------------------------------
// 'bslx_outstreamfunctions' is an integral part of the 'bslx' externalization
// contract.  The 'bslx' contract is at least in part "collaborative", which is
// to say that each developer of a given *kind* of component (e.g., a stream or
// a value-semantic container) must comply with the relevant portions of the
// contract to ensure that the "system as a whole" works for everybody.
// 'bslx_outstreamfunctions' plays several related but different roles in
// helping various developers to produce 'bslx'-compliant components.  In this
// section we briefly highlight how and why 'bslx_outstreamfunctions' is
// helpful (or required) for these different developers.  By discussing
// different aspects of usage, we hope also to convey the general design goals
// of this component, and, to a certain extent, the overall 'bslx' contract.
// See the 'bslx' package-level documentation for a full specification of the
// 'bslx' contract.
//
///Implementing 'bslx' Streaming in Value-Semantic Template Classes
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The author of a non-template value-semantic type has full knowledge of the
// details of the "value" of that type, and may choose to use the appropriate
// output stream 'put' methods directly when implementing the required
// 'bslxStreamOut' method for that type. However, if one or more aspects of
// the value are of template parameterized type, then the author cannot in
// general know how to stream the value using the 'put' methods.  For example,
// if a type has as its value one 'int' data member:
//..
//    int d_value;
//..
// then the implementation of the 'bslxStreamOut' method can contain:
//..
//    stream.putInt32(d_value);
//..
// However, if the data member is of parameterized 'TYPE':
//..
//    TYPE d_value;
//..
// then the implementation of the 'bslxStreamOut' method must rely on the
// 'bslx::OutStreamFunctions' implementation to write the value:
//..
//    bslx::OutStreamFunctions::streamOut(stream, d_value, 1);
//..
// This call will resolve to the correct sequence of 'put' calls no matter
// whether 'TYPE' is a fundamental type, a 'bslx'-compliant 'enum', or a proper
// 'bslx'-compliant class. In the latter two cases, the explicit specification
// of the version format (in this case, 1) guarantees the stable operation of
// this method whether or not 'TYPE' chooses to change its version format.
//
///Factored "Output" Methods
///- - - - - - - - - - - - -
// One obvious design choice for 'bslx_outstreamfunctions' is that it supports
// the *output* portion of the 'bslx' "contract" only.  This factoring is by no
// means central to the 'bslx' contract as a whole, but allows authors of
// special-purpose (i.e., non-value-semantic) components to implement a subset
// of the 'bslx' facilities without causing a compile-time error when their
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
// invocations just to show the syntax clearly. To accomplish this,
// we exhibit two separate example "components": a value-semantic
// point object, and an 'enum'. In all cases, the component designs
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
// problem, the 'mypoint' component would be implemented differently.
//
// Note that the 'MyPoint' class in this example represents its coordinates as
// 'short' integer values; this is done to make the 'bslx' stream output byte
// pattern somewhat easier for the reader of this example to recognize when the
// output buffer is printed:
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
//    STREAM& bslxStreamOut(STREAM& stream, int version) const;
//        // Write this value to the specified output 'stream' using the
//        // specified 'version' format and return a reference to the
//        // modifiable 'stream'.  ...
//  };
//
//  // FREE OPERATORS
//  inline
//  int operator==(const MyPoint& lhs, const MyPoint& rhs);
//..
// Representative (inline) implementations of these methods are shown below:
//..
//  //========================================================================
//  //             INLINE DEFINITIONS
//  //========================================================================
//
//  // CLASS METHODS
//  inline
//  bool MyPoint::isBslxVersionSupported(int version)
//  {
//      return 1 == version;
//  }
//
//  // CREATORS
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
//  int MyPoint::x() const
//  {
//      return d_x;
//  }
//
//  // ...
//
//  template <class STREAM>
//  inline
//  STREAM& MyPoint::bslxStreamOut(STREAM& stream, int version) const
//  {
//      switch (version) {
//        case 1: {
//          stream.putInt16(d_x);           // write the x coordinate
//          stream.putInt16(d_y);           // write the y coordinate
//          stream.putInt8((char)d_color);  // write the color enum as one byte
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
// We can now write a small 'main' test program that will use the above 'enum',
// point class, and output stream to illustrate 'bslx::OutStreamFunctions'
// functionality.  This test program does not attempt to do anything more
// useful than writing known values to a stream and confirming that the
// expected byte pattern was in fact written:
//..
//  // mytestapp.m.cpp
//
//  #include <bslx_byteoutstream.h>
//
//  int main(int argc, char **argv)
//  {
//      int             i     = 168496141;  // byte pattern 0a 0b 0c 0d
//      Color           color = BLUE;       // byte pattern 02
//      MyPoint         p(0, -1, color);    // byte pattern 00 00 ff ff 02
//
//      bslx::ByteOutStream out;            assert(0 == out.length());
//
//      bslx::OutStreamFunctions::streamOut(out, i, 1);
//                                          assert(4 == out.length());
//      out.removeAll();                    assert(0 == out.length());
//      bslx::OutStreamFunctions::streamOut(out, i, 0);
//                                          assert(4 == out.length());
//
//      out.removeAll();                    assert(0 == out.length());
//      bslx::OutStreamFunctions::streamOut(out, color, 1);
//                                          assert(4 == out.length());
//      out.removeAll();                    assert(0 == out.length());
//      bslx::OutStreamFunctions::streamOut(out, color, 0);
//                                          assert(4 == out.length());
//
//      out.removeAll();                    assert(0 == out.length());
//      bslx::OutStreamFunctions::streamOut(out, p, 1);
//                                          assert(5 == out.length());
//      out.removeAll();                    assert(0 == out.length());
//      bslx::OutStreamFunctions::streamOut(out, p, 1);
//                                          assert(5 == out.length());
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

                         // ================
                         // class IsEnumType
                         // ================

class IsEnumType {
    // This class is used to perform function overload resolution for
    // types that are enum types. This class contains no interface
    // or implementation by design.
};

                         // ===================
                         // class IsNotEnumType
                         // ===================

class IsNotEnumType {
    // This class is used to perform function overload resolution for
    // types that are *not* enum types. This class contains no
    // interface or implementation by design.
};

                         // ============================
                         // namespace OutStreamFunctions
                         // ============================

namespace OutStreamFunctions {
    // This namespace allows externalization of various types in a
    // type independent manner all 'bslx'-compliant types. The
    // externalization functions are specialized for fundamental
    // types, enum types, 'bsl::string' and 'bsl::vector'.  A
    // compilation error will occur if 'TYPE' does not support these
    // two methods with appropriate signatures.  Additionally, the
    // functions are specialized for fundamental types, 'enum' types,
    // 'bsl::string' and 'bsl::vector'.

    // CLASS METHODS
    template <class STREAM, class TYPE>
    STREAM& streamOutImp(STREAM&              stream,
                         const TYPE&          value,
                         int                  version,
                         const IsEnumType&    dummy);
        // Write the specified 'value' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.  Note that this
        // function is called only for enum types and that a version is not
        // written to 'stream'.

    template <class STREAM, class TYPE>
    STREAM& streamOutImp(STREAM&              stream,
                         const TYPE&          value,
                         int                  version,
                         const IsNotEnumType& dummy);
        // Write the specified 'value' to the specified output 'stream' using
        // the specified 'version' format and return a reference to the
        // modifiable 'stream'. If 'version' is not supported, 'stream' is
        // unmodified. The behavior is undefined unless 'TYPE' supports a
        // 'bslx'-compliant method having the same name and 'STREAM' is a
        // 'bslx'-compliant output stream. Note that 'version' is not written
        // to 'stream'.

    template <class STREAM, class TYPE>
    STREAM& streamOut(STREAM& stream, const TYPE& value, int version);
        // Write the specified 'value' to the specified output 'stream' using
        // the specified 'version' format and return a reference to the
        // modifiable 'stream'. If 'version' is not supported, 'stream' is
        // unmodified. Note that 'version' is not written.

                       // ------------------------------
                       // template class specializations
                       // ------------------------------

    template <class STREAM>
    STREAM& streamOut(STREAM& stream, const bool& value, int version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.

    template <class STREAM>
    STREAM& streamOut(STREAM& stream, const char& value, int version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.

    template <class STREAM>
    STREAM& streamOut(STREAM& stream, const signed char& value, int version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.

    template <class STREAM>
    STREAM& streamOut(STREAM& stream, const unsigned char& value, int version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'.  No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.

    template <class STREAM>
    STREAM& streamOut(STREAM& stream, const short& value, int version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.

    template <class STREAM>
    STREAM& streamOut(STREAM&               stream,
                      const unsigned short& value,
                      int                   version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.

    template <class STREAM>
    STREAM& streamOut(STREAM& stream, const int& value, int version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.


    template <class STREAM>
    STREAM& streamOut(STREAM& stream, const unsigned int& value, int version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.

    template <class STREAM>
    STREAM& streamOut(STREAM& stream, const long& value, int version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream. Note that 'value' is truncated to 4 bytes (if necessary)
        // before being written.

    template <class STREAM>
    STREAM& streamOut(STREAM& stream, const unsigned long& value, int version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream. Note that 'value' is truncated to 4 bytes (if necessary)
        // before being written.

    template <class STREAM>
    STREAM& streamOut(STREAM&                   stream,
                      const bsls::Types::Int64& value,
                      int                       version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.

    template <class STREAM>
    STREAM& streamOut(STREAM&                    stream,
                      const bsls::Types::Uint64& value,
                      int                        version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.

    template <class STREAM>
    STREAM& streamOut(STREAM& stream, const bsl::string& value, int version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.

    template <class STREAM>
    STREAM& streamOut(STREAM& stream, const float& value, int vector);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.

    template <class STREAM>
    STREAM& streamOut(STREAM& stream, const double& value, int version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.

    template <class STREAM, class ALLOC>
    STREAM& streamOut(STREAM&                         stream,
                      const bsl::vector<char, ALLOC>& value,
                      int                             version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used.  The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.

    template <class STREAM, class ALLOC>
    STREAM& streamOut(STREAM&                          stream,
                      const bsl::vector<short, ALLOC>& value,
                      int                              version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.

    template <class STREAM, class ALLOC>
    STREAM& streamOut(STREAM&                        stream,
                      const bsl::vector<int, ALLOC>& value,
                      int                            version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.

    template <class STREAM, class ALLOC>
    STREAM& streamOut(STREAM&                                       stream,
                      const bsl::vector<bsls::Types::Int64, ALLOC>& value,
                      int                                           version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.

    template <class STREAM, class ALLOC>
    STREAM& streamOut(STREAM&                          stream,
                      const bsl::vector<float, ALLOC>& value,
                      int                              version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.

    template <class STREAM, class ALLOC>
    STREAM& streamOut(STREAM&                           stream,
                      const bsl::vector<double, ALLOC>& value,
                      int                               version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream'. No version is used. The
        // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
        // stream.

    template <class STREAM, class TYPE, class ALLOC>
    STREAM& streamOut(STREAM&                         stream,
                      const bsl::vector<TYPE, ALLOC>& value,
                      int                             version);
        // Write the specified 'value' to the specified output 'stream' using
        // the appropriate 'put' method of 'stream' that is passed the
        // specified 'version'. The behavior is undefined unless 'STREAM' is a
        // 'bslx'-compliant output stream.
};

// ============================================================================
//                 INLINE DEFINITIONS
// ============================================================================

template <typename STREAM, typename TYPE>
inline
STREAM& OutStreamFunctions::streamOutImp(STREAM&           stream,
                                         const TYPE&       value,
                                         int,
                                         const IsEnumType&)
    // Write the specified 'value' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.  Note that this
    // function is called only for enum types and that a version is not
    // written to 'stream'.
{
    // Assert that the enum value is between INT_MIN and INT_MAX.
    BSLS_ASSERT_SAFE(
        bsls::Types::Int64(value) >= bsls::Types::Int64(INT_MIN)
        &&
        bsls::Types::Int64(value) <= bsls::Types::Int64(INT_MAX)
        );

    // stream the enum value as a 32-bit 'int'
    int intValue = (int)value;
    return stream.putInt32(intValue);
}

template <typename STREAM, typename TYPE>
inline
STREAM& OutStreamFunctions::streamOutImp(STREAM&              stream,
                                         const TYPE&          value,
                                         int                  version,
                                         const IsNotEnumType&)
    // Write the specified 'value' to the specified output 'stream' using
    // the specified 'version' format and return a reference to the
    // modifiable 'stream'.  If 'version' is not supported, 'stream' is
    // unmodified.  The behavior is undefined unless 'TYPE' supports a
    // 'bslx'-compliant method having the same name and 'STREAM' is a
    // 'bslx'-compliant output stream.  Note that 'version' is not written
    // to 'stream'.
{
    // A compilation error indicating the next line of code implies the
    // class of 'TYPE' does not support the 'bslxStreamOut' method.

    return value.bslxStreamOut(stream, version);
}

template <class STREAM, class TYPE>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&     stream,
                                      const TYPE& value,
                                      int         version)
    // Write the specified 'value' to the specified output 'stream' using
    // the specified 'version' format and return a reference to the
    // modifiable 'stream'. If 'version' is not supported, 'stream' is
    // unmodified. Note that 'version' is not written.
{
    typedef typename bslmf::If<bslmf::IsEnum<TYPE>::VALUE,
                               IsEnumType,
                               IsNotEnumType>::Type dummyType;

    return streamOutImp(stream, value, version, dummyType());
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&     stream,
                                      const bool& value,
                                      int         version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.
{
    return stream.putInt8(static_cast<char>(value));
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&     stream,
                                      const char& value,
                                      int         version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.
{
    return stream.putInt8(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&            stream,
                                      const signed char& value,
                                      int                version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.
{
    return stream.putInt8(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&              stream,
                                      const unsigned char& value,
                                      int                  version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'.  No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.
{
    return stream.putUint8(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&      stream,
                                      const short& value,
                                      int          version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.
{
    return stream.putInt16(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&               stream,
                                      const unsigned short& value,
                                      int                   version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.
{
    return stream.putUint16(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&    stream,
                                      const int& value,
                                      int        version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.

{
    return stream.putInt32(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&             stream,
                                      const unsigned int& value,
                                      int                 version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.
{
    return stream.putUint32(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&     stream,
                                      const long& value,
                                      int         version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream. Note that 'value' is truncated to 4 bytes (if necessary)
    // before being written.
{
    return stream.putInt32(static_cast<int>(value));
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&              stream,
                                      const unsigned long& value,
                                      int                  version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream. Note that 'value' is truncated to 4 bytes (if necessary)
    // before being written.
{
    return stream.putUint32(static_cast<unsigned int>(value));
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&                   stream,
                                      const bsls::Types::Int64& value,
                                      int                       version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.
{
    return stream.putInt64(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&                    stream,
                                      const bsls::Types::Uint64& value,
                                      int                        version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.
{
    return stream.putUint64(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&            stream,
                                      const bsl::string& value,
                                      int                version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.
{
    return stream.putString(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&      stream,
                                      const float& value,
                                      int          vector)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.
{
    return stream.putFloat32(value);
}

template <class STREAM>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&       stream,
                                      const double& value,
                                      int           version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.
{
    return stream.putFloat64(value);
}

template <class STREAM, class ALLOC>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&                         stream,
                                      const bsl::vector<char, ALLOC>& value,
                                      int                             version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used.  The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.
{
    const int length = (int)value.size();
    stream.putLength(length);

    return 0 < length ? stream.putArrayInt8(&value[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&                          stream,
                                      const bsl::vector<short, ALLOC>& value,
                                      int                              version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.
{
    const int length = (int)value.size();
    stream.putLength(length);

    return 0 < length ? stream.putArrayInt16(&value[0], length) : stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&                        stream,
                                      const bsl::vector<int, ALLOC>& value,
                                      int                            version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.
{
    const int length = (int)value.size();
    stream.putLength(length);

    return 0 < length ? stream.putArrayInt32(&value[0], length) : stream;
}


template <class STREAM, class ALLOC>
inline
STREAM& OutStreamFunctions::streamOut(
                         STREAM&                                       stream,
                         const bsl::vector<bsls::Types::Int64, ALLOC>& value,
                         int                                           version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.
{
    const int length = (int)value.size();
    stream.putLength(length);

    return 0 < length ? stream.putArrayInt64(&value[0], length) : stream;
}


template <class STREAM, class ALLOC>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&                          stream,
                                      const bsl::vector<float, ALLOC>& value,
                                      int                              version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.
{
    const int length = (int)value.size();
    stream.putLength(length);

    return 0 < length ? stream.putArrayFloat32(&value[0], length) : stream;
}


template <class STREAM, class ALLOC>
inline
STREAM& OutStreamFunctions::streamOut(
                                     STREAM&                           stream,
                                     const bsl::vector<double, ALLOC>& value,
                                     int                               version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream'. No version is used. The
    // behavior is undefined unless 'STREAM' is a 'bslx'-compliant output
    // stream.
{
    const int length = (int)value.size();
    stream.putLength(length);

    return 0 < length ? stream.putArrayFloat64(&value[0], length) : stream;
}


template <class STREAM, class TYPE, class ALLOC>
inline
STREAM& OutStreamFunctions::streamOut(STREAM&                         stream,
                                      const bsl::vector<TYPE, ALLOC>& value,
                                      int                             version)
    // Write the specified 'value' to the specified output 'stream' using
    // the appropriate 'put' method of 'stream' that is passed the
    // specified 'version'. The behavior is undefined unless 'STREAM' is a
    // 'bslx'-compliant output stream.
{
    typedef typename bsl::vector<TYPE, ALLOC>::const_iterator Iterator;

    const int length = (int)value.size();
    stream.putLength(length);

    for (Iterator it = value.begin(); it != value.end(); ++it) {
        streamOut(stream, *it, version);
    }

    return stream;
}

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
