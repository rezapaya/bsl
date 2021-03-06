// bslscm_version.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLSCM_VERSION
#define INCLUDED_BSLSCM_VERSION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide source control management (versioning) information.
//
//@CLASSES:
// bslscm::Version: namespace for RCS and SCCS versioning information for 'bsl'
//
//@DESCRIPTION: This component provides source control management (versioning)
// information for the 'bsl' package group.  In particular, this component
// embeds RCS-style and SCCS-style version strings in binary executable files
// that use one or more components from the 'bsl' package group.  This version
// information may be extracted from binary files using common UNIX utilities
// (e.g., 'ident' and 'what').  In addition, the 'version' 'static' member
// function in the 'bslscm::Version' 'struct' can be used to query version
// information for the 'bsl' package group at runtime.  The following usage
// examples illustrate these two basic capabilities.
//
// Note that unless the 'version' method will be called, it is not necessary to
// '#include' this component header file to get 'bsl' version information
// embedded in an executable.  It is only necessary to use one or more 'bsl'
// components (and, hence, link in the 'bsl' library).
//
///Usage
///-----
// A program can display the version of BSL that was used to build it by
// printing the version string returned by 'bslscm::Version::version()' to
// 'stdout' as follows:
//..
//  std::printf("BSL version: %s\n", bslscm::Version::version());
//..

#ifndef INCLUDED_BSLS_BUILDTARGET
#include <bsls_buildtarget.h>      // need to ensure consistent build options
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLSCM_VERSIONTAG
#include <bslscm_versiontag.h> // BSL_VERSION_MAJOR, BSL_VERSION_MINOR
#endif

#define BDE_DONT_ALLOW_TRANSITIVE_INCLUDES 1
    // When we don't want to rely on legacy features, we also want to make sure
    // we are not picking up macros or type aliases via (direct or transitive)
    // includes of headers that have migrated from 'bde' to 'bsl' libraries.

namespace BloombergLP {

namespace bslscm {

struct Version {
    static const char *d_ident;
    static const char *d_what;

#define BSLSCM_CONCAT2(a,b,c,d,e,f) a ## b ## c ## d ## e ## f
#define BSLSCM_CONCAT(a,b,c,d,e,f)  BSLSCM_CONCAT2(a,b,c,d,e,f)

// 'BSLSCM_D_VERSION' is a symbol whose name warns users of version mismatch
// linking errors.  Note that the exact string "compiled_this_object" must be
// present in this version coercion symbol.  Tools may look for this pattern to
// warn users of mismatches.

#define BSLSCM_D_VERSION BSLSCM_CONCAT(d_version_BSL_,       \
                                       BSL_VERSION_MAJOR, _, \
                                       BSL_VERSION_MINOR, _, \
                                       compiled_this_object)

    static const char *BSLSCM_D_VERSION;

    static const char *d_dependencies;
    static const char *d_buildInfo;
    static const char *d_timestamp;
    static const char *d_sourceControlInfo;

    static const char *version();
};

inline
const char *Version::version()
{
    return BSLSCM_D_VERSION;
}

}  // close package namespace

// Force linker to pull in this component's object file.

#if defined(BSLS_PLATFORM_CMP_IBM)
static const char **bslscm_version_assertion =
                                            &bslscm::Version::BSLSCM_D_VERSION;
#else
namespace {
    extern const char **const bslscm_version_assertion =
                                            &bslscm::Version::BSLSCM_D_VERSION;
}
#endif


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
