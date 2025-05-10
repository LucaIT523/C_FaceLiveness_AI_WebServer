/* Copyright 2023 ID R&D Inc. All Rights Reserved. */

/**
 * @file Common.h
 * Common classes and structures
 */

#pragma once

//! \cond
#include <string>
#include <utility>

#include <facesdk/config.h>
//! \endcond

#include <facesdk/FaceException.h>

namespace facesdk {

/**
  * @brief Result status info. Contains the same info as @ref FaceException.
  */
struct StatusInfo {
    StatusInfo() = default;
    explicit StatusInfo(const facesdk::FaceException& e) : message(e.what()), code(e.status()) {}
    StatusInfo(std::string message, facesdk::FaceException::STATUS code) : message(std::move(message)), code(code) {}

    /**
     * @brief Checks if status is OK
     *
     * @return True if status is OK, false otherwise
     */
    bool ok() const {
        return code == facesdk::FaceException::STATUS::OK;
    }

    friend FACE_SDK_API std::ostream& operator<<(std::ostream& os, const StatusInfo& status);

public:
    /**
     * @brief Status message
     */
    std::string message;

    /**
     * @brief Status code
     */
    facesdk::FaceException::STATUS code = facesdk::FaceException::STATUS::OK;
};

//! @private
FACE_SDK_API void WriteOptionalResultHelper(std::ostream& os, const StatusInfo& status);

/**
  * @brief Optional result wrapper.
  * Wraps an instance of a result class and only allows to access it when
  * status is OK.
  */
template<typename T>
struct OptionalResult {

    OptionalResult() = default;

    OptionalResult(const facesdk::FaceException& e) : status_(e) {}  // NOLINT(google-explicit-constructor)

    OptionalResult(StatusInfo status) : status_(std::move(status)) {}  // NOLINT(google-explicit-constructor)

    OptionalResult(T value, StatusInfo status) : value_(std::move(value)), status_(std::move(status)) {}

    OptionalResult(const OptionalResult& other) : value_(other.value_), status_(other.status_) {}

    OptionalResult& operator=(const OptionalResult& other) {
        if (this != &other) {
            value_ = other.value_;
            status_ = other.status_;
        }
        return *this;
    }

    /**
      * @brief Get result value.
      *
      * @return Result value.
      * @throws facesdk::FaceException if status is not OK.
      * @note Use ok() method before value() call to check if result is valid and available.
      */
    const T& value() const {
        if (ok()) {
            return value_;
        } else {
            throw facesdk::FaceException(status_.message, status_.code);
        }
    }

    const StatusInfo& status() const {
        return status_;
    }

    bool ok() const {
        return status_.ok();
    }

    //! @private
    friend std::ostream& operator<<(std::ostream& os, const OptionalResult& optional_result) {
        WriteOptionalResultHelper(os, optional_result.status_);
        return os;
    }

private:
    T value_{};
    StatusInfo status_;
};

}  // namespace facesdk
