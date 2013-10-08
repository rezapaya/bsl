// bslx_byteinstream.h                                                -*-C++-*-
#ifndef INCLUDED_BSLX_BYTEINSTREAM
#define INCLUDED_BSLX_BYTEINSTREAM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Enable (un)externalization of fundamental types.
//
//@CLASSES:
//  bslx::ByteInStream: byte-array-based input stream for fundamental types
//
//@SEE_ALSO: bslx_byteoutstream
//
//@DESCRIPTION: This component implements a byte-array-based data
// stream object that provides platform-independent input methods
// ("unexternalization") on values, and arrays of values, of
// fundamental types, and on 'bsl::string'.
//
// This component is intended to be used in conjunction with the
// 'bslx_byteoutstream' externalization component.  Each input method
// of 'bslx::ByteInStream' reads either a value or a homogeneous array
// of values of a fundamental type, in a format that was written by
// the corresponding 'bslx::ByteOutStream' method. In general, the
// user of this component cannot rely on being able to read data that
// was written by any mechanism other than 'bslx::ByteOutStream'.
//
// The supported types and required content are listed in the table
// below.  All of the fundamental types in the table may be input as
// scalar values or as homogeneous arrays.  'bsl::string' is input as
// a logical scalar string.  Note that 'Int64' and 'Uint64' denote
// 'bsls::Types::Int64' and 'bsls::Types::Uint64', which in turn are
// 'typedef' names for the signed and unsigned 64-bit integer types,
// respectively, on the host platform:
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
//
//      bsl::string       BDE STLport implementation of the STL string class
//..
// This component also supports compact streaming in of integer types.  In
// particular, 64-bit values can be streamed in from 40, 48, 56, or 64-bit
// values, and 32-bit values can be streamed in from 24 and 32-bit values
// (consistent with what has been written to the stream, of course).  Note
// that, for signed types, the sign is preserved for all streamed-in values.
//
// Note that input streams can be *invalidated* explicitly and queried for
// *validity* and *emptiness*.  Reading from an initially invalid stream has no
// effect.  Attempting to read beyond the end of a stream will automatically
// invalidate the stream.  Whenever an inconsistent value is detected, the
// stream should be invalidated explicitly.
//
///EXTERNALIZATION AND VALUE SEMANTICS
///-----------------------------------
// The concept of "externalization" in this component is intentionally
// specialized to support streaming the *values* of entities atomically.  In
// particular, streaming an array of a specific length is considered to be an
// atomic operation.  It is therefore an error, for example, to stream out an
// array of 'int' of length three and then stream in those three 'int' values
// as three scalar 'int' variables.  Similarly, it is an error to stream out an
// array of length L and stream back in an array of length other than L.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:
///- - - - -
// Suppose we wish to implement a (deliberately simple) 'MyPerson' class (in
// the currently installed 'my_person' component) as a value-semantic object
// that supports 'bslx' externalization.  In addition to whatever data and
// methods that we choose to put into our design, we must supply three methods
// having specific names and signatures in order to comply with the 'bslx'
// "protocol": a class method 'isBslxVersionSupported' and two object
// methods, an accessor (i.e., a 'const' method) 'bslxStreamOut', and a
// manipulator (i.e., a non-'const' method) 'bslxStreamIn'.  This example
// shows how to implement those three methods for the simple "person"
// component.
//
// In this example we will not worry overly about "good design" of the person
// component, and we will declare but not implement illustrative methods and
// free operators, except for the three required 'bslx' methods, which are
// implemented in full.  In particular, we will not make explicit use of
// 'bslma' allocators; a more complete design would do so:
//..
//  // myperson.h
//
//  class MyPerson {
//      bsl::string d_firstName;
//      bsl::string d_lastName;
//      int         d_age;
//
//      friend bool operator==(const MyPerson&, const MyPerson&);
//
//    public:
//      // CLASS METHODS
//      static bool isBslxVersionSupported(int version) { return 1 == version;}
//          // Return 'true' if this class supports the specified 'version',
//          // and 'false' otherwise.  (See the 'bslx' package-level
//          // documentation for more information on 'bslx' streaming of
//          // container types.)
//
//      // CREATORS
//      MyPerson();
//          // Create a default person.
//
//      MyPerson(const char *firstName, const char *lastName, int age);
//          // Create a person having the specified 'firstName', 'lastName',
//          // and 'age'.
//
//      MyPerson(const MyPerson& original);
//          // Create a person having value of the specified 'original' person.
//
//      ~MyPerson();
//          // Destroy this object.
//
//      // MANIPULATORS
//      MyPerson& operator=(const MyPerson& rhs);
//          // Assign to this person the value of the specified 'rhs' person.
//
//      template <class STREAM>
//      STREAM& bslxStreamIn(STREAM& stream, int version);
//          // Assign to this object the value read from the specified
//          // input 'stream' using the specified 'version' format and
//          // return a reference to the modifiable 'stream'.  If
//          // 'stream' is initially invalid, this operation has no
//          // effect.  If 'stream' becomes invalid during this
//          // operation, this object is valid, but its value is
//          // undefined.  If the specified 'version' is not supported,
//          // 'stream' is marked invalid, but this object is unaltered.
//          // Note that no version is read from 'stream'.  (See the
//          // 'bslx' package-level documentation for more information on
//          // 'bslx' streaming of container types.)
//
//      // Other manipulators omitted.
//
//      // ACCESSORS
//      const bsl::string& firstName() const;
//          // Return the first name of this person.
//
//      const bsl::string& lastName() const;
//          // Return the last name of this person.
//
//     int age() const;
//         // Return the age of this person.
//
//      template <class STREAM>
//      STREAM& bslxStreamOut(STREAM& stream, int version) const;
//          // Write this value to the specified output 'stream' and
//          // return a reference to the modifiable 'stream'.  Optionally
//          // specify an explicit 'version' format; by default, the
//          // maximum supported version is written to 'stream' and used
//          // as the format.  If 'version' is specified, that format is
//          // used but *not* written to 'stream'.  If 'version' is not
//          // supported, 'stream' is left unmodified.  (See the 'bslx'
//          // package-level documentation for more information on 'bslx'
//          // streaming of container types).
//
//      // Other accessors omitted.
//
//  };
//
//  // FREE OPERATORS
//  bool operator==(const MyPerson& lhs, const MyPerson& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' person objects have
//      // the same value and 'false' otherwise.  Two person objects have the
//      // same value if they have the same first name, last name, and age.
//
//  bool operator!=(const MyPerson& lhs, const MyPerson& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' person objects do not
//      // have the same value and 'false' otherwise.  Two person objects
//      // differ in value if they differ in first name, last name, or age.
//
//  bsl::ostream& operator<<(bsl::ostream& stream, const MyPerson& person);
//      // Write the specified 'date' value to the specified output 'stream' in
//      // some reasonable format.
//
//                          // INLINE FUNCTION DEFINITIONS
//  template <class STREAM>
//  inline
//  STREAM& MyPerson::bslxSstreamIn(STREAM& stream, int version)
//  {
//      if (stream) {
//          switch (version) {  // switch on the 'bslx' version
//            case 1: {
//                stream.getString(d_firstName);
//                if (!stream) {
//                     d_firstName = "stream error";  // *might* be corrupted;
//                                                    //  value for testing
//                    return stream;
//                }
//                stream.getString(d_lastName);
//                if (!stream) {
//                    d_lastName = "stream error";  // *might* be corrupted;
//                                                  //  value for testing
//                    return stream;
//                }
//                stream.getInt32(d_age);
//                if (!stream) {
//                    d_age = 1;     // *might* be corrupted; value for testing
//                    return stream;
//                }
//            } break;
//            default: {
//              stream.invalidate();
//            }
//          }
//      }
//      return stream;
//  }
//
//  // ACCESSORS
//  template <class STREAM>
//  inline
//  STREAM& MyPerson::bslxStreamOut(STREAM& stream, int version) const
//  {
//      switch (version) {
//        case 1: {
//            stream.putString(d_firstName);
//            stream.putString(d_lastName);
//            stream.putInt32(d_age);
//        } break;
//      }
//      return stream;
//  }
//..
// We can now exercise the new 'myperson' value-semantic component by
// externalizing and reconstituting an object.  First create a 'MyPerson'
// 'JaneSmith' and a 'bslx::ByteOutStream' 'outStream':
//..
//  // mytestapp.m.cpp
//
//  using namespace std;
//
//  int main(int argc, char *argv[])
//  {
//      MyPerson JaneSmith("Jane", "Smith", 42);
//      bslx::ByteOutStream outStream;
//      JaneSmith.bslxStreamOut(outStream, VERSION);
//..
// Next create a 'MyPerson' 'janeCopy' initialized to the default value, and
// assert that 'janeCopy' is different from 'janeSmith':
//..
//      MyPerson janeCopy;                       assert(janeCopy != JaneSmith);
//..
// Now create a 'bslx::ByteInStream' 'inStream' initialized with the
// buffer from the 'bslx::ByteOutStream' object 'outStream':
//..
//      bslx::ByteInStream inStream(outStream.data(), outStream.length());
//      int version;
//      inStream.getVersion(version);
//      assert(version == VERSION);
//      janeCopy.bslxStreamIn(inStream, version);
//      assert(janeCopy == JaneSmith);
//
//      // Verify the results on 'stdout'.
//      if (janeCopy == JaneSmith) {
//          cout << "Successfully serialized and de-serialized Jane Smith:"
//               << "\n\tFirstName: " << janeCopy.firstName()
//               << "\n\tLastName : " << janeCopy.lastName()
//               << "\n\tAge      : " << janeCopy.age() << endl;
//      }
//      else {
//          cout << "Serialization unsuccessful.  janeCopy holds:"
//               << "\n\tFirstName: " << janeCopy.firstName()
//               << "\n\tLastName : " << janeCopy.lastName()
//               << "\n\tAge      : " << janeCopy.age() << endl;
//      }
//      return 0;
//  }
//..

#ifndef INCLUDED_BSLX_BYTESTREAMIMPUTIL
#include <bslx_bytestreamimputil.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace bslx {

                         // ==================
                         // class ByteInStream
                         // ==================

class ByteInStream {
    // This class provides input methods to unexternalize values, and
    // C-style arrays of values, of the fundamental integral and
    // floating-point types, as well as 'bsl::string' values. Each
    // 'get' method of this class is guaranteed to read stream data
    // written by the corresponding 'put' method of
    // 'bslx::ByteOutStream'. Note that attempting to read beyond the
    // end of a stream will automatically invalidate the stream. The
    // class supports validity testing by providing the method
    // 'bool isValid() const'.

    // DATA
    bsl::vector<char> d_buffer;     // bytes to be unexternalized
    int               d_validFlag;  // stream validity flag, 'true'
                                    // if stream is in valid state
                                    // 'false' otherwise
    int               d_cursor;     // index of the next byte to be
                                    // extracted from this stream

    // FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream&       stream,
                                    const ByteInStream& object);

    // MANIPULATORS
    void validate();
        // Put this input stream in an valid state. This function
        // has no effect if this stream is already valid.

    // NOT IMPLEMENTED
    ByteInStream(const ByteInStream&);
    ByteInStream& operator=(const ByteInStream&);
    bool operator==(const ByteInStream&) const;
    bool operator!=(const ByteInStream&) const;

  public:
    // CREATORS
    explicit ByteInStream(bslma::Allocator *basicAllocator = 0);
        // Create an empty input byte stream.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ByteInStream(const char      *buffer,
                 int              numBytes,
                 bslma::Allocator *basicAllocator = 0);
        // Create an input byte stream containing the specified
        // initial 'numBytes' from the specified 'buffer'.  Optionally
        // specify the 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // '0 <= numBytes', 'numBytes' is less than or equal to the
        // length of 'buffer', and 'buffer' contains data written in a
        // format consistent with the 'bslx_byteoutstream' component.

    ~ByteInStream();
        // Destroy this object.

    // MANIPULATORS
    ByteInStream& getLength(int& length);
        // Consume a length value from this input stream, place that
        // value in the specified 'length', and return a reference
        // to this modifiable stream.  If this stream is initially
        // invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'length' is undefined.
        //
        // Consume one byte.  If the most significant bit (MSB) is set
        // to 0, then set 'numValues' to that one-byte integer value;
        // otherwise, set that MSB to zero, consume three more bytes,
        // and interpret the four bytes in network byte order to
        // provide the value for 'numValues'.

    ByteInStream& getVersion(int& version);
        // Consume a version value from this input stream, place that
        // value in the specified 'version', and return a reference to
        // this modifiable stream.  If this stream is initially
        // invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'version' is undefined.
        //
        // Consume one byte interpreted as an unsigned integer to
        // provide the value for 'version'.

    ByteInStream& getInt64(bsls::Types::Int64& variable);
        // Consume a 64-bit signed integer value from this input
        // stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this
        // stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable'
        // is undefined.
        //
        // Consume eight bytes interpreted as a 64-bit signed integer
        // in network byte order to provide the value for 'variable'.
        // Note that the bit pattern is sign extended on platforms
        // where 'variable' is more than 64 bits.

    ByteInStream& getUint64(bsls::Types::Uint64& variable);
        // Consume a 64-bit unsigned integer value from this input
        // stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this
        // stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable'
        // is undefined.
        //
        // Consume eight bytes interpreted as a 64-bit unsigned
        // integer in network byte order to provide the value for
        // 'variable'.  Note that the bit pattern is zero extended on
        // platforms where 'variable' is more than 64 bits.

    ByteInStream& getInt56(bsls::Types::Int64& variable);
        // Consume a 56-bit signed integer value from this input
        // stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this
        // stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable'
        // is undefined.
        //
        // Consume seven bytes interpreted as a sign extended 64-bit
        // signed integer in network byte order to provide the value
        // for 'variable'.

    ByteInStream& getUint56(bsls::Types::Uint64& variable);
        // Consume a 56-bit unsigned integer value from this input
        // stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this
        // stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable'
        // is undefined.
        //
        // Consume seven bytes interpreted as a zero extended 64-bit
        // unsigned integer in network byte order to provide the value
        // for 'variable'.

    ByteInStream& getInt48(bsls::Types::Int64& variable);
        // Consume a 48-bit signed integer value from this input
        // stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this
        // stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable'
        // is undefined.
        //
        // Consume six bytes interpreted as a sign extended 64-bit
        // signed integer in network byte order to provide the value
        // for 'variable'.

    ByteInStream& getUint48(bsls::Types::Uint64& variable);
        // Consume a 48-bit unsigned integer value from this input
        // stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this
        // stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable'
        // is undefined.
        //
        // Consume six bytes interpreted as a zero extended 64-bit
        // unsigned integer in network byte order to provide the value
        // for 'variable'.

    ByteInStream& getInt40(bsls::Types::Int64& variable);
        // Consume a 40-bit signed integer value from this input
        // stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this
        // stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable'
        // is undefined.
        //
        // Consume five bytes interpreted as a sign extended 64-bit
        // signed integer in network byte order to provide the value
        // for 'variable'.

    ByteInStream& getUint40(bsls::Types::Uint64& variable);
        // Consume a 40-bit unsigned integer value from this input
        // stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this
        // stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable'
        // is undefined.
        //
        // Consume five bytes interpreted as a zero extended 64-bit
        // unsigned integer in network byte order to provide the value
        // for 'variable'.

    ByteInStream& getInt32(int& variable);
        // Consume a 32-bit signed integer value from this input
        // stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this
        // stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable'
        // is undefined.
        //
        // Consume four bytes interpreted as a signed 32-bit integer
        // in network byte order to provide the value for 'variable'.
        // Note that the bit pattern is sign extended on platforms
        // where 'variable' is more than 32 bits.

    ByteInStream& getUint32(unsigned int& variable);
        // Consume a 32-bit unsigned integer value from this input
        // stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this
        // stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable'
        // is undefined.
        //
        // Consume four bytes interpreted as a 32-bit unsigned integer
        // in network byte order to provide the value for 'variable'.
        // Note that the bit pattern is zero extended on platforms
        // where 'variable' is more than 32 bits.

    ByteInStream& getInt24(int& variable);
        // Consume a 24-bit signed integer value from this input
        // stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this
        // stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable'
        // is undefined.
        //
        // Consume three bytes interpreted as a sign extended 32-bit
        // signed integer in network byte order to provide the value
        // for 'variable'.

    ByteInStream& getUint24(unsigned int& variable);
        // Consume a 24-bit unsigned integer value from this input
        // stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this
        // stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable'
        // is undefined.
        //
        // Consume three bytes interpreted as a zero extended 32-bit
        // unsigned integer in network byte order to provide the value
        // for 'variable'.

    ByteInStream& getInt16(short& variable);
        // Consume a 16-bit signed integer value from this input
        // stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this
        // stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable'
        // is undefined.
        //
        // Consume two bytes interpreted as a 16-bit signed integer in
        // network byte order to provide the value for 'variable'.
        // Note that the bit pattern is sign extended on platforms
        // where 'variable' is more than 16 bits.

    ByteInStream& getUint16(unsigned short& variable);
        // Consume a 16-bit unsigned integer value from this input
        // stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this
        // stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable'
        // is undefined.
        //
        // Consume two bytes interpreted as a 16-bit unsigned integer
        // in network byte order to provide the value for 'variable'.
        // Note that the bit pattern is zero extended on platforms
        // where 'variable' is more than 16 bits.

    ByteInStream& getInt8(char&        variable);
    ByteInStream& getInt8(signed char& variable);
        // Consume an 8-bit signed integer value from this input
        // stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this
        // stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value
        //
        // Consume one byte interpreted as an 8-bit signed integer to
        // provide the value for 'variable'.

    ByteInStream& getUint8(char&          variable);
    ByteInStream& getUint8(unsigned char& variable);
        // Consume an 8-bit unsigned integer value from this input
        // stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this
        // stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable'
        // is undefined.
        //
        // Consume one byte interpreted as an 8-bit unsigned integer
        // to provide the value for 'variable'.

    ByteInStream& getFloat64(double& variable);
        // Consume a 64-bit floating-point value from this input
        // stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this
        // stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable'
        // is undefined.  Note that, for non-conforming platforms,
        // this operation may be lossy.
        //
        // Consume eight bytes interpreted as an IEEE double-precision
        // value in network byte order to provide the value for 'variable'.

    ByteInStream& getFloat32(float& variable);
        // Consume a 32-bit floating-point value from this input
        // stream, place that value in the specified 'variable', and
        // return a reference to this modifiable stream.  If this
        // stream is initially invalid, this operation has no effect.
        // If this function otherwise fails to extract a valid value,
        // this stream is marked invalid and the value of 'variable'
        // is undefined.  Note that, for non-conforming platforms,
        // this operation may be lossy.
        //
        // Consume four bytes interpreted as an IEEE single-precision
        // value in network byte order to provide the value format 'variable'.

    ByteInStream& getString(bsl::string& stringValue);
        // Consume a string from this input stream, place that value
        // in the specified 'stringValue', and return a reference to
        // this modifiable stream.  If this stream is initially
        // invalid, this operation has no effect.  If this function
        // otherwise fails to extract a valid value, this stream is
        // marked invalid and the value of 'stringValue' is undefined.
        // The string must be prefaced by a non-negative integer
        // indicating the number of characters composing the string.
        // The behavior is undefined if the length indicator is not
        // greater than 0.
        //
        // First extract a length value.  Consume one byte and, if the
        // most significant bit (MSB) is set to 0, then that one-byte
        // integer value is the length value; otherwise, set that MSB
        // to zero, consume three more bytes, and interpret the four
        // bytes in network byte order to provide the length value.
        // Then, for each character of 'stringValue' (of the extracted
        // length) consume one byte providing the value for that
        // character.

    ByteInStream& getArrayInt64(bsls::Types::Int64 *values, int numValues);
        // Consume the 64-bit signed integer array of the specified
        // 'numValues' from this input stream, place that value in the
        // specified 'values', and return a reference to this
        // modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise
        // fails to extract a valid array of the required 'numValues',
        // this stream is marked invalid and the value of 'values' is
        // undefined.  The behavior is undefined unless '0 <= numValues'.
        //
        // For each of the 'numValues' elements of 'values', consume
        // eight bytes interpreted as a 64-bit signed integer in
        // network byte order to provide the value for that element.
        // Note that the bit pattern is sign extended on platforms
        // where 'bsls::Types::Int64' is more than 64 bits.

    ByteInStream& getArrayUint64(bsls::Types::Uint64 *values, int numValues);
        // Consume the 64-bit unsigned integer array of the specified
        // 'numValues' from this input stream, place that value in the
        // specified 'values', and return a reference to this
        // modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise
        // fails to extract a valid array of the required 'numValues',
        // this stream is marked invalid and the value of 'values' is
        // undefined.  The behavior is undefined unless '0 <= numValues'.
        //
        // For each of the 'numValues' elements of 'values', consume
        // eight bytes interpreted as a 64-bit unsigned integer in
        // network byte order to provide the value for that element.
        // Note that the bit pattern is zero extended on platforms
        // where 'bsls::Types::Uint64' is more than 64 bits.

    ByteInStream& getArrayInt56(bsls::Types::Int64 *values, int numValues);
        // Consume the 56-bit signed integer array of the specified
        // 'numValues' from this input stream, place that value in the
        // specified 'values', and return a reference to this
        // modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise
        // fails to extract a valid array of the required 'numValues',
        // this stream is marked invalid and the value of 'values' is
        // undefined.  The behavior is undefined unless '0 <= numValues'.
        //
        // For each of the 'numValues' elements of 'values', consume
        // seven bytes interpreted as a sign extended 64-bit signed
        // integer in network byte order to provide the value for that
        // element.

    ByteInStream& getArrayUint56(bsls::Types::Uint64 *values, int numValues);
        // Consume the 56-bit unsigned integer array of the specified
        // 'numValues' from this input stream, place that value in the
        // specified 'values', and return a reference to this
        // modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise
        // fails to extract a valid array of the required 'numValues',
        // this stream is marked invalid and the value of 'values' is
        // undefined.  The behavior is undefined unless '0 <= numValues'.
        //
        // For each of the 'numValues' elements of 'values', consume
        // seven bytes interpreted as a zero extended 64-bit unsigned
        // integer in network byte order to provide the value for that
        // element.

    ByteInStream& getArrayInt48(bsls::Types::Int64 *values, int numValues);
        // Consume the 48-bit signed integer array of the specified
        // 'numValues' from this input stream, place that value in the
        // specified 'values', and return a reference to this
        // modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise
        // fails to extract a valid array of the required 'numValues',
        // this stream is marked invalid and the value of 'values' is
        // undefined.  The behavior is undefined unless '0 <= numValues'.
        //
        // For each of the 'numValues' elements of 'values', consume
        // six bytes interpreted as a sign extended 64-bit signed
        // integer in network byte order to provide the value for that
        // element.

    ByteInStream& getArrayUint48(bsls::Types::Uint64 *values, int numValues);
        // Consume the 48-bit unsigned integer array of the specified
        // 'numValues' from this input stream, place that value in the
        // specified 'values', and return a reference to this
        // modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise
        // fails to extract a valid array of the required 'numValues',
        // this stream is marked invalid and the value of 'values' is
        // undefined.  The behavior is undefined unless '0 <= numValues'.
        //
        // For each of the 'numValues' elements of 'values', consume
        // six bytes interpreted as a zero extended 64-bit unsigned
        // integer in network byte order to provide the value for that
        // element.

    ByteInStream& getArrayInt40(bsls::Types::Int64 *values, int numValues);
        // Consume the 40-bit signed integer array of the specified
        // 'numValues' from this input stream, place that value in the
        // specified 'values', and return a reference to this
        // modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise
        // fails to extract a valid array of the required 'numValues',
        // this stream is marked invalid and the value of 'values' is
        // undefined.  The behavior is undefined unless '0 <= numValues'.
        //
        // For each of the 'numValues' elements of 'values', consume
        // five bytes interpreted as a sign extended 64-bit signed
        // integer in network byte order to provide the value for that
        // element.

    ByteInStream& getArrayUint40(bsls::Types::Uint64 *values, int numValues);
        // Consume the 40-bit unsigned integer array of the specified
        // 'numValues' from this input stream, place that value in the
        // specified 'values', and return a reference to this
        // modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise
        // fails to extract a valid array of the required 'numValues',
        // this stream is marked invalid and the value of 'values' is
        // undefined.  The behavior is undefined unless '0 <= numValues'.
        //
        // For each of the 'numValues' elements of 'values', consume
        // five bytes interpreted as a zero extended 64-bit unsigned
        // integer in network byte order to provide the value for that
        // element.

    ByteInStream& getArrayInt32(int *values, int numValues);
        // Consume the 32-bit signed integer array of the specified
        // 'numValues' from this input stream, place that value in the
        // specified 'values', and return a reference to this
        // modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise
        // fails to extract a valid array of the required 'numValues',
        // this stream is marked invalid and the value of 'values' is
        // undefined.  The behavior is undefined unless '0 <= numValues'.
        //
        // For each of the 'numValues' elements of 'values', consume
        // four bytes interpreted as a 32-bit signed integer in
        // network byte order to provide the value for that element.
        // Note that the bit pattern is sign extended on platforms
        // where 'int' is more than 32 bits.

    ByteInStream& getArrayUint32(unsigned int *values, int numValues);
        // Consume the 32-bit unsigned integer array of the specified
        // 'numValues' from this input stream, place that value in the
        // specified 'values', and return a reference to this
        // modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise
        // fails to extract a valid array of the required 'numValues',
        // this stream is marked invalid and the value of 'values' is
        // undefined.  The behavior is undefined unless '0 <= numValues'.
        //
        // For each of the 'numValues' elements of 'values', consume
        // four bytes interpreted as a 32-bit unsigned integer in
        // network byte order to provide the value for that element.
        // Note that the bit pattern is zero extended on platforms
        // where 'unsigned' 'int' is more than 32 bits.

    ByteInStream& getArrayInt24(int *values, int numValues);
        // Consume the 24-bit signed integer array of the specified
        // 'numValues' from this input stream, place that value in the
        // specified 'values', and return a reference to this
        // modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise
        // fails to extract a valid array of the required 'numValues',
        // this stream is marked invalid and the value of 'values' is
        // undefined.  The behavior is undefined unless '0 <= numValues'.
        //
        // For each of the 'numValues' elements of 'values', consume
        // three bytes interpreted as a sign extended 32-bit signed
        // integer in network byte order to provide the value for that
        // element.

    ByteInStream& getArrayUint24(unsigned int *values, int numValues);
        // Consume the 24-bit unsigned integer array of the specified
        // 'numValues' from this input stream, place that value in the
        // specified 'values', and return a reference to this
        // modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise
        // fails to extract a valid array of the required 'numValues',
        // this stream is marked invalid and the value of 'values' is
        // undefined.  The behavior is undefined unless '0 <= numValues'.
        //
        // For each of the 'numValues' elements of 'values', consume
        // three bytes interpreted as a zero extended 32-bit unsigned
        // integer in network byte order to provide the value for that
        // element.

    ByteInStream& getArrayInt16(short *values, int numValues);
        // Consume the 16-bit signed integer array of the specified
        // 'numValues' from this input stream, place that value in the
        // specified 'values', and return a reference to this
        // modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise
        // fails to extract a valid array of the required 'numValues',
        // this stream is marked invalid and the value of 'values' is
        // undefined.  The behavior is undefined unless '0 <= numValues'.
        //
        // For each of the 'numValues' elements of 'values', consume
        // two bytes interpreted as a 16-bit signed integer in network
        // byte order to provide the value for that element.  Note
        // that the bit pattern is sign extended on platforms where
        // 'short' is more than 16 bits.

    ByteInStream& getArrayUint16(unsigned short *values, int numValues);
        // Consume the 16-bit unsigned integer array of the specified
        // 'numValues' from this input stream, place that value in the
        // specified 'values', and return a reference to this
        // modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise
        // fails to extract a valid array of the required 'numValues',
        // this stream is marked invalid and the value of 'values' is
        // undefined.  The behavior is undefined unless '0 <= numValues'.
        //
        // For each of the 'numValues' elements of 'values', consume
        // two bytes interpreted as a 16-bit unsigned integer in
        // network byte order to provide the value for that element.
        // Note that the bit pattern is zero extended on platforms
        // where 'unsigned' 'short' is more than 16 bits.

    ByteInStream& getArrayInt8(char *values,        int numValues);
    ByteInStream& getArrayInt8(signed char *values, int numValues);
        // Consume the 8-bit signed integer array of the specified
        // 'numValues' from this input stream, place that value in the
        // specified 'values', and return a reference to this
        // modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise
        // fails to extract a valid array of the required 'numValues',
        // this stream is marked invalid and the value of 'values' is
        // undefined.  The behavior is undefined unless '0 <= numValues'.
        //
        // For each of the 'numValues' elements of 'values', consume
        // one byte interpreted as an 8-bit signed integer to provide
        // the value for that element.

    ByteInStream& getArrayUint8(char *values,          int numValues);
    ByteInStream& getArrayUint8(unsigned char *values, int numValues);
        // Consume the 8-bit unsigned integer array of the specified
        // 'numValues' from this input stream, place that value in the
        // specified 'values', and return a reference to this
        // modifiable stream.  If this stream is initially invalid,
        // this operation has no effect.  If this function otherwise
        // fails to extract a valid array of the required 'numValues',
        // this stream is marked invalid and the value of 'values' is
        // undefined.  The behavior is undefined unless '0 <= numValues'.
        //
        // For each of the 'numValues' elements of 'values', consume
        // one byte interpreted as an 8-bit unsigned integer to
        // provide the value for that element.

    ByteInStream& getArrayFloat64(double *values, int numValues);
        // Consume the IEEE double-precision (8-byte) floating-point
        // array of the specified 'numValues' from this input stream,
        // place that value in the specified 'values', and return a
        // reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid array of the
        // required 'numValues', this stream is marked invalid and the
        // value of 'values' is undefined.  The behavior is undefined
        // unless '0 <= numValues'.  Note that for non-conforming
        // platforms, this operation may be lossy.
        //
        // For each of the 'numValues' elements of 'values', consume
        // eight bytes interpreted as an IEEE double-precision value
        // in network byte order to provide the value for that
        // element.

    ByteInStream& getArrayFloat32(float *values, int numValues);
        // Consume the IEEE single-precision (4-byte) floating-point
        // array of the specified 'numValues' from this input stream,
        // place that value in the specified 'values', and return a
        // reference to this modifiable stream.  If this stream is
        // initially invalid, this operation has no effect.  If this
        // function otherwise fails to extract a valid array of the
        // required 'numValues', this stream is marked invalid and the
        // value of 'values' is undefined.  The behavior is undefined
        // unless '0 <= numValues'.  Note that for non-conforming
        // platforms, this operation may be lossy.
        //
        // For each of the 'numValues' elements of 'values', consume
        // four bytes interpreted as an IEEE single-precision value in
        // network byte order to provide the value for that element.

    void invalidate();
        // Put this input stream in an invalid state.  This function
        // has no effect if this stream is already invalid.  Note that
        // this function should be called whenever a value extracted
        // from this stream is determined to be invalid, inconsistent,
        // or otherwise incorrect.

    // ACCESSORS
    bool isValid() const;
        // Return 'true' if this stream is valid, and 'false'
        // otherwise. An invalid stream is a stream in which
        // insufficient or invalid data was detected during an
        // extraction operation.  Note that an empty stream will be
        // valid unless an extraction attempt or explicit invalidation
        // causes it to be otherwise.

    operator const void *() const;
        // Return a non-zero value if this stream is valid, and 0 otherwise.
        // An invalid stream is a stream for which an output operation was
        // detected to have failed.

    const char *data() const;
        // Return the address of the contiguous, non-modifiable
        // internal memory buffer of this stream.  The address will
        // remain valid as long as this array is not destroyed or
        // modified (i.e., the current capacity is not exceeded).  The
        // behavior of accessing elements outside the range
        // '[ data() - data() + (length() - 1) ]' is undefined.

    bool isEmpty() const;
        // Return 'true' if this stream is empty, and 'false'
        // otherwise.  The behavior is undefined unless this stream is
        // valid Note that this function enables higher-level
        // components to verify that, after successfully reading all
        // expected data, no data remains.

    int length() const;
        // Return the total number of bytes stored in this stream.

    int cursor() const;
        // Return the index of the next byte to be extracted from this stream.
        // The behavior is undefined unless this stream is valid.
};

// ============================================================================
//                 INLINE DEFINITIONS
// ============================================================================

                         // ------------------
                         // class ByteInStream
                         // ------------------

// MANIPULATORS
inline
void ByteInStream::validate()
{
    d_validFlag = true;
}

// CREATORS
inline
ByteInStream::ByteInStream(bslma::Allocator *basicAllocator)
: d_buffer(basicAllocator)
, d_validFlag(true)
, d_cursor(0)
{
}

inline
ByteInStream::ByteInStream(const char       *buffer,
                           int               numBytes,
                           bslma::Allocator *basicAllocator)
: d_buffer(buffer, buffer + numBytes, basicAllocator)
, d_validFlag(true)
, d_cursor(0)
{
   BSLS_ASSERT_SAFE(0 <= numBytes);
}

inline
ByteInStream::~ByteInStream()
{
}

inline
ByteInStream& ByteInStream::getLength(int& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    if (127 < (unsigned char)d_buffer[cursor()]) {
        // If 'length > 127', 'length' is stored as 4 bytes with top-bit set.
        if (isValid()) {
            getInt32(variable);
            variable ^= (1 << 31);  // Clear top bit.
        }
    }
    else {
        // If 'length <= 127', 'length' is stored as one byte.
        if (isValid()) {
            unsigned char tmp = 0;
            getUint8(tmp);
            if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(isValid())) {
                variable = tmp;
            }
        }
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getVersion(int& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    unsigned char tmp = 0;
    getUint8(tmp);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(isValid())) {
        variable = tmp;
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt64(bsls::Types::Int64& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    if (cursor() + ByteStreamImpUtil::SIZEOF_INT64 <= length()) {
        ByteStreamImpUtil::getInt64(&variable, d_buffer.begin() + cursor());
        d_cursor += ByteStreamImpUtil::SIZEOF_INT64;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getUint64(bsls::Types::Uint64& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    if (cursor() + ByteStreamImpUtil::SIZEOF_INT64 <= length()) {
        ByteStreamImpUtil::getUint64(&variable, d_buffer.begin() + cursor());
        d_cursor += ByteStreamImpUtil::SIZEOF_INT64;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt56(bsls::Types::Int64& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    if (cursor() + ByteStreamImpUtil::SIZEOF_INT56 <= length()) {
        ByteStreamImpUtil::getInt56(&variable, d_buffer.begin() + cursor());
        d_cursor += ByteStreamImpUtil::SIZEOF_INT56;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getUint56(bsls::Types::Uint64& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    if (cursor() + ByteStreamImpUtil::SIZEOF_INT56 <= length()) {
        ByteStreamImpUtil::getUint56(&variable, d_buffer.begin() + cursor());
        d_cursor += ByteStreamImpUtil::SIZEOF_INT56;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt48(bsls::Types::Int64& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    if (cursor() + ByteStreamImpUtil::SIZEOF_INT48 <= length()) {
        ByteStreamImpUtil::getInt48(&variable, d_buffer.begin() + cursor());
        d_cursor += ByteStreamImpUtil::SIZEOF_INT48;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getUint48(bsls::Types::Uint64& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    if (cursor() + ByteStreamImpUtil::SIZEOF_INT48 <= length()) {
        ByteStreamImpUtil::getUint48(&variable, d_buffer.begin() + cursor());
        d_cursor += ByteStreamImpUtil::SIZEOF_INT48;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt40(bsls::Types::Int64& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    if (cursor() + ByteStreamImpUtil::SIZEOF_INT40 <= length()) {
        ByteStreamImpUtil::getInt40(&variable, d_buffer.begin() + cursor());
        d_cursor += ByteStreamImpUtil::SIZEOF_INT40;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getUint40(bsls::Types::Uint64& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    if (cursor() + ByteStreamImpUtil::SIZEOF_INT40 <= length()) {
        ByteStreamImpUtil::getUint40(&variable, d_buffer.begin() + cursor());
        d_cursor += ByteStreamImpUtil::SIZEOF_INT40;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt32(int& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    if (cursor() + ByteStreamImpUtil::SIZEOF_INT32 <= length()) {
        ByteStreamImpUtil::getInt32(&variable, d_buffer.begin() + cursor());
        d_cursor += ByteStreamImpUtil::SIZEOF_INT32;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getUint32(unsigned int& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    if (cursor() + ByteStreamImpUtil::SIZEOF_INT32 <= length()) {
        ByteStreamImpUtil::getUint32(&variable, d_buffer.begin() + cursor());
        d_cursor += ByteStreamImpUtil::SIZEOF_INT32;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt24(int& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    if (cursor() + ByteStreamImpUtil::SIZEOF_INT24 <= length()) {
        ByteStreamImpUtil::getInt24(&variable, d_buffer.begin() + cursor());
        d_cursor += ByteStreamImpUtil::SIZEOF_INT24;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getUint24(unsigned int& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    if (cursor() + ByteStreamImpUtil::SIZEOF_INT24 <= length()) {
        ByteStreamImpUtil::getUint24(&variable, d_buffer.begin() + cursor());
        d_cursor += ByteStreamImpUtil::SIZEOF_INT24;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt16(short& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    if (cursor() + ByteStreamImpUtil::SIZEOF_INT16 <= length()) {
        ByteStreamImpUtil::getInt16(&variable, d_buffer.begin() + cursor());
        d_cursor += ByteStreamImpUtil::SIZEOF_INT16;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getUint16(unsigned short& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    if (cursor() + ByteStreamImpUtil::SIZEOF_INT16 <= length()) {
        ByteStreamImpUtil::getUint16(&variable, d_buffer.begin() + cursor());
        d_cursor += ByteStreamImpUtil::SIZEOF_INT16;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt8(char& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    if (cursor() + ByteStreamImpUtil::SIZEOF_INT8 <= length()) {
        ByteStreamImpUtil::getInt8(&variable, data() + cursor());
        d_cursor += ByteStreamImpUtil::SIZEOF_INT8;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getInt8(signed char& variable)
{
    return getInt8((char &)variable);
}

inline
ByteInStream& ByteInStream::getUint8(char& variable)
{
    return getInt8((char &)variable);
}

inline
ByteInStream& ByteInStream::getUint8(unsigned char& variable)
{
    return getInt8((char &)variable);
}

inline
ByteInStream& ByteInStream::getFloat64(double& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    if (cursor() + ByteStreamImpUtil::SIZEOF_FLOAT64 <= length()) {
        ByteStreamImpUtil::getFloat64(&variable, d_buffer.begin() + cursor());
        d_cursor += ByteStreamImpUtil::SIZEOF_FLOAT64;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getFloat32(float& variable)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    if (cursor() + ByteStreamImpUtil::SIZEOF_FLOAT32 <= length()) {
        ByteStreamImpUtil::getFloat32(&variable, d_buffer.begin() + cursor());
        d_cursor += ByteStreamImpUtil::SIZEOF_FLOAT32;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getString(bsl::string& stringValue)
{
    BSLS_ASSERT(cursor() <= length());

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    int length = 0;
    getLength(length);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    // 'length' could be corrupt or intentionally invalid, so we limit the
    // initial 'resize' to something that can accommodate the preponderance of
    // strings that will arise in practice.  The remaining portion of a string
    // longer than 16M is read in via a second pass.

    enum { BSLX_INITIAL_ALLOCATION_SIZE = 16 * 1024 * 1024 };

    const int initialLength = length < BSLX_INITIAL_ALLOCATION_SIZE
                              ? length
                              : BSLX_INITIAL_ALLOCATION_SIZE;

    stringValue.resize(initialLength);

    if (0 == length) {
        return *this;                                                 // RETURN
    }

    getArrayUint8(&stringValue.front(), initialLength);
    if (isValid() && length > initialLength) {
        stringValue.resize(length);
        getArrayUint8(&stringValue[initialLength], length - initialLength);
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayInt64(bsls::Types::Int64 *values,
                                          int                 numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    const int len = ByteStreamImpUtil::SIZEOF_INT64 * numValues;
    if (cursor() + len  <= length()) {
        ByteStreamImpUtil::getArrayInt64(
            values,
            d_buffer.begin() + cursor(),
            numValues);
        d_cursor += len;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayUint64(bsls::Types::Uint64 *values,
                                           int                  numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    const int len = ByteStreamImpUtil::SIZEOF_INT64 * numValues;
    if (cursor() + len  <= length()) {
        ByteStreamImpUtil::getArrayUint64(
            values,
            d_buffer.begin() + cursor(),
            numValues);
        d_cursor += len;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayInt56(bsls::Types::Int64 *values,
                                          int                 numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    const int len = ByteStreamImpUtil::SIZEOF_INT56 * numValues;
    if (cursor() + len  <= length()) {
        ByteStreamImpUtil::getArrayInt56(
            values,
            d_buffer.begin() + cursor(),
            numValues);
        d_cursor += len;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayUint56(bsls::Types::Uint64 *values,
                                           int                  numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    const int len = ByteStreamImpUtil::SIZEOF_INT56 * numValues;
    if (cursor() + len  <= length()) {
        ByteStreamImpUtil::getArrayUint56(
            values,
            d_buffer.begin() + cursor(),
            numValues);
        d_cursor += len;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayInt48(bsls::Types::Int64 *values,
                                          int                 numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    const int len = ByteStreamImpUtil::SIZEOF_INT48 * numValues;
    if (cursor() + len  <= length()) {
        ByteStreamImpUtil::getArrayInt48(
            values,
            d_buffer.begin() + cursor(),
            numValues);
        d_cursor += len;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayUint48(bsls::Types::Uint64 *values,
                                           int                  numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    const int len = ByteStreamImpUtil::SIZEOF_INT48 * numValues;
    if (cursor() + len  <= length()) {
        ByteStreamImpUtil::getArrayUint48(
            values,
            d_buffer.begin() + cursor(),
            numValues);
        d_cursor += len;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayInt40(bsls::Types::Int64 *values,
                                          int                 numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    const int len = ByteStreamImpUtil::SIZEOF_INT40 * numValues;
    if (cursor() + len  <= length()) {
        ByteStreamImpUtil::getArrayInt40(
            values,
            d_buffer.begin() + cursor(),
            numValues);
        d_cursor += len;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayUint40(bsls::Types::Uint64 *values,
                                           int                  numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    const int len = ByteStreamImpUtil::SIZEOF_INT40 * numValues;
    if (cursor() + len  <= length()) {
        ByteStreamImpUtil::getArrayUint40(
            values,
            d_buffer.begin() + cursor(),
            numValues);
        d_cursor += len;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayInt32(int *values, int numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    const int len = ByteStreamImpUtil::SIZEOF_INT32 * numValues;
    if (cursor() + len  <= length()) {
        ByteStreamImpUtil::getArrayInt32(
            values,
            d_buffer.begin() + cursor(),
            numValues);
        d_cursor += len;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayUint32(unsigned int *values,int numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    const int len = ByteStreamImpUtil::SIZEOF_INT32 * numValues;
    if (cursor() + len  <= length()) {
        ByteStreamImpUtil::getArrayUint32(
            values,
            d_buffer.begin() + cursor(),
            numValues);
        d_cursor += len;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayInt24(int *values, int numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    const int len = ByteStreamImpUtil::SIZEOF_INT24 * numValues;
    if (cursor() + len  <= length()) {
        ByteStreamImpUtil::getArrayInt24(
            values,
            d_buffer.begin() + cursor(),
            numValues);
        d_cursor += len;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayUint24(unsigned int *values, int numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    const int len = ByteStreamImpUtil::SIZEOF_INT24 * numValues;
    if (cursor() + len  <= length()) {
        ByteStreamImpUtil::getArrayUint24(
            values,
            d_buffer.begin() + cursor(),
            numValues);
        d_cursor += len;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayInt16(short *values, int numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    const int len = ByteStreamImpUtil::SIZEOF_INT16 * numValues;
    if (cursor() + len  <= length()) {
        ByteStreamImpUtil::getArrayInt16(
            values,
            d_buffer.begin() + cursor(),
            numValues);
        d_cursor += len;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayUint16(unsigned short *values,
                                           int             numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    const int len = ByteStreamImpUtil::SIZEOF_INT16 * numValues;
    if (cursor() + len  <= length()) {
        ByteStreamImpUtil::getArrayUint16(
            values,
            d_buffer.begin() + cursor(),
            numValues);
        d_cursor += len;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayInt8(char *values, int numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    const int len = ByteStreamImpUtil::SIZEOF_INT8 * numValues;
    if (cursor() + len  <= length()) {
        ByteStreamImpUtil::getArrayInt8(
            values,
            d_buffer.begin() + cursor(),
            numValues);
        d_cursor += len;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayInt8(signed char *values, int numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    return getArrayInt8((char *)values, numValues);
}

inline
ByteInStream& ByteInStream::getArrayUint8(char *values, int numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    return getArrayInt8(values, numValues);
}

inline
ByteInStream& ByteInStream::getArrayUint8(unsigned char *values, int numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    return getArrayInt8((char *)values, numValues);
}

inline
ByteInStream& ByteInStream::getArrayFloat64(double *values, int numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    const int len = ByteStreamImpUtil::SIZEOF_FLOAT64 * numValues;
    if (cursor() + len  <= length()) {
        ByteStreamImpUtil::getArrayFloat64(
            values,
            d_buffer.begin() + cursor(),
            numValues);
        d_cursor += len;
        validate();
    }

    return *this;
}

inline
ByteInStream& ByteInStream::getArrayFloat32(float *values, int numValues)
{
    BSLS_ASSERT(cursor() <= length());
    BSLS_ASSERT_SAFE(values);
    BSLS_ASSERT_SAFE(0 <= numValues);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!isValid())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return *this;
    }

    invalidate();
    const int len = ByteStreamImpUtil::SIZEOF_FLOAT32 * numValues;
    if (cursor() + len  <= length()) {
        ByteStreamImpUtil::getArrayFloat32(
            values,
            d_buffer.begin() + cursor(),
            numValues);
        d_cursor += len;
        validate();
    }

    return *this;
}

inline
void ByteInStream::invalidate()
{
    d_validFlag = false;
}

// ACCESSORS
inline
bool ByteInStream::isValid() const
{
    return d_validFlag;
}

inline
ByteInStream::operator const void *() const
{
    return isValid() ? this : 0;
}

inline
const char *ByteInStream::data() const
{
    return d_buffer.size() ? d_buffer.begin() : 0;
}

inline
bool ByteInStream::isEmpty() const
{
    BSLS_ASSERT(cursor() <= length());
    return cursor() == length();
}

inline
int ByteInStream::length() const
{
    return d_buffer.size();
}

inline
int ByteInStream::cursor() const
{
    return d_cursor;
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
