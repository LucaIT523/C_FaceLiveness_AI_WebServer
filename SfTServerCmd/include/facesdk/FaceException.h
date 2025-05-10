/* Copyright 2023 ID R&D Inc. All Rights Reserved. */

/**
 * @file FaceException.h
 * FaceSDK exception header file
 */

#pragma once

//! \cond
#include <string>
#include <utility>

#include <facesdk/config.h>
//! \endcond

namespace facesdk {

/**
 * @brief Custom runtime exception for FaceSDK methods
 */
class FaceException : public std::exception {
public:
    /**
     * @brief Error code
     */
    enum STATUS {
        FACE_TOO_CLOSE,                            //!< A distance between face and image border is too small for
                                                   //!< preprocessing issues.
        FACE_CLOSE_TO_BORDER,                      //!< Face is too close to one or more borders. May reduce the
                                                   //!< accuracy of spoofing detection because edges of face may
                                                   //!< not be seen.
        FACE_CROPPED,                              //!< Face is cropped. May reduce the accuracy of spoofing detection
                                                   //!< because edges of face may not be seen.
        FACE_NOT_FOUND,                            //!< Face detector can't find face on image.
        TOO_MANY_FACES,                            //!< Face detector found more than one face on image.
        FACE_TOO_SMALL,                            //!< Facial area is not big enough for analysis.
        FACE_ANGLE_TOO_LARGE,                      //!< Facial out-of-plane rotation angle is extremely large.
        FAILED_TO_READ_IMAGE,                      //!< File decoding error.
        FAILED_TO_WRITE_IMAGE,                     //!< File encoding error.
        FAILED_TO_READ_MODEL,                      //!< Model deserializing error.
        FAILED_TO_BUILD_INTERPRETER,               //!< tflite::Interpreter building error.
        FAILED_TO_INVOKE_INTERPRETER,              //!< tflite::Interpreter invoking error.
        FAILED_TO_ALLOCATE,                        //!< Memory allocation error.
        INVALID_CONFIG,                            //!< Config deserializing error.
        NO_SUCH_OBJECT_IN_BUILD,                   //!< Engine or backend is not supported by the build.
        FAILED_TO_PREPROCESS_IMAGE_WHILE_PREDICT,  //!< Liveness prediction error.
        FAILED_TO_PREPROCESS_IMAGE_WHILE_DETECT,   //!< Face detection error.
        FAILED_TO_PREDICT_LANDMARKS,               //!< Landmarks prediction error.
        INVALID_FUSE_MODE,                         //!< Invalid fuse mode provided.
        NULLPTR,                                   //!< Nullptr provided.
        LICENSE_ERROR,                             //!< Some error occurred during license checking.
        INVALID_META,                              //!< Invalid facesdk::Meta value provided.
        UNKNOWN,                                   //!< Unhandled exception in the code.
        OK,                                        //!< No errors.
        FACE_IS_OCCLUDED,                          //!< The biggest face on the input image is occluded, so liveness
                                                   //!< check is not possible.
        FAILED_TO_FETCH_COREML_DECRYPTION_KEY,     //!< Failed to fetch CoreML model decryption key from Apple serve.rs
        EYES_CLOSED                                //!< Eyes are closed.
    };

    /**
     * @brief FaceException constructor
     *
     * @param message_ Error message
     * @param code_ Error code
     */
    FaceException(std::string message_, STATUS code_) : message(std::move(message_)), code(code_) {}

    /**
     * @brief Get error message as C-string
     *
     * @return Error message in C-string format
     */
    const char* what() const noexcept override {
        return message.c_str();
    }

    /**
     * @brief Get error code
     *
     * @return Error code from STATUS enum
     */
    STATUS status() const {
        return code;
    }

private:
    friend FACE_SDK_API std::ostream& operator<<(std::ostream& os, const STATUS& status);
    friend FACE_SDK_API std::ostream& operator<<(std::ostream& os, const FaceException& exception);

    std::string message;
    STATUS code;
};

}  // namespace facesdk
