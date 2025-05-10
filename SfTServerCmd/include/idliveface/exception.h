// Copyright 2023 ID R&D Inc. All Rights Reserved.

//! \file

#pragma once

//! \cond
#include <stdexcept>
#include <string>

//! \endcond

//#include <idliveface/build.h>

#define IDLF_API __declspec(dllimport)

namespace idliveface {

    //! \brief General IDLive Face exception.
    //!
    //! Can be thrown when the invalid parameter has been provided, or when
    //! the unrecoverable internal error has occurred.
    class IDLF_API Exception : public std::runtime_error {
    public:
        explicit Exception(const std::string& message) : std::runtime_error(message) {}
        explicit Exception(const char* message) : std::runtime_error(message) {}
    };

    //! Thrown by the `ImageDecoder` when the image can't be decoded.
    class IDLF_API ImageDecodingException : public Exception {
    public:
        explicit ImageDecodingException(const std::string& message) : Exception(message) {}
        explicit ImageDecodingException(const char* message) : Exception(message) {}
    };

    //! Thrown by the `FaceAnalyser` and the `FaceDetector` when the license has expired.
    class IDLF_API LicenseExpiredException : public Exception {
    public:
        explicit LicenseExpiredException(const std::string& message) : Exception(message) {}
        explicit LicenseExpiredException(const char* message) : Exception(message) {}
    };

}  // namespace idliveface
