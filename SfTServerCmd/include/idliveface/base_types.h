// Copyright 2023 ID R&D Inc. All Rights Reserved.

//! \file

#pragma once

//! \cond
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

//#include <idliveface/build.h>
//! \endcond

#include <idliveface/utility.h>

namespace idliveface {

//! \brief Defines a subset of characteristics that the face analysis should be sensitive to.
//!
//! Correctly specified domain leads to more precise analysis results.
enum class Domain {
    //! General case. Used by default.
    kGeneral,

    //! Targets images taken via the desktop web-camera.
    kDesktop
};

//! \brief Defines how strict the face analysis should be, which in return affects APCER / BPCER balance.
enum class Tolerance {
    //! Targets low APCER. Used by default.
    kRegular,

    //! Achieves lower BPCER while still having acceptable APCER.
    kSoft,

    //! Targets extra low APCER with higher BPCER.
    kHardened
};

//! @private
IDLF_API std::ostream& operator<<(std::ostream& os, Domain value);
//! @private
IDLF_API std::ostream& operator<<(std::ostream& os, Tolerance value);

//! Two-dimensional point.
struct Point {
    //! Creates [0, 0] point.
    Point() = default;

    //! Creates point with the provided coordinates.
    Point(int32_t x, int32_t y) : x(x), y(y) {}

    //! The x coordinate of the point.
    int32_t x = 0;

    //! The y coordinate of the point.
    int32_t y = 0;
};

//! @private
IDLF_API std::ostream& operator<<(std::ostream& os, Point value);

//! The bounding box around the face.
struct BoundingBox {
    //! Creates empty bounding box in the [0, 0] coordinates.
    BoundingBox() = default;

    //! Creates bounding box with the provided dimensions and coordinates.
    BoundingBox(int32_t x, int32_t y, int32_t width, int32_t height) : x(x), y(y), width(width), height(height) {}

    //! The x coordinate of the top-left point.
    int32_t x = 0;

    //! The y coordinate of the top-left point.
    int32_t y = 0;

    //! The width of the box.
    int32_t width = 0;

    //! The height of the box.
    int32_t height = 0;
};

//! @private
IDLF_API std::ostream& operator<<(std::ostream& os, const BoundingBox& value);

//! The roll, pitch and yaw angles of the head, in degrees.
struct HeadPose {
    //! \brief The roll angle, ranges from -180 to 180.
    float roll = 0;

    //! \brief The pitch angle, ranges from -180 to 180.
    float pitch = 0;

    //! \brief The yaw angle, ranges from -180 to 180.
    float yaw = 0;
};

//! @private
IDLF_API std::ostream& operator<<(std::ostream& os, const HeadPose& value);

//! The 68 facial landmark points.
typedef std::vector<Point> Landmarks68;

//! @private
IDLF_API std::ostream& operator<<(std::ostream& os, const Landmarks68& value);

//! Attributes of the detected face.
struct FaceAttributes {
    //! \brief The bounding box around the face.
    BoundingBox box;

    //! \brief The 68 facial landmark points.
    Landmarks68 landmarks;

    //! \brief The roll, pitch and yaw rotation angles of the head.
    HeadPose head_pose;

    //! \brief The orientation of the face, in degrees.
    //!
    //! It's a roll angle rounded to a nearest multiply of 90. Can be -90, 0, 90 or 180.
    int32_t orientation = 0;

    //! \brief The distance between the centers of pupils, in pixels.
    int32_t pupillary_distance = 0;

    //! \brief The probability that the face is occluded (for example with medical mask),
    //! from 0 (not occluded) to 1 (occluded).
    float occlusion = 0;

    //! \brief The probability that the eyes are closed, from 0 (open) to 1 (closed).
    float eyes_closed = 0;
};

//! @private
IDLF_API std::ostream& operator<<(std::ostream& os, const FaceAttributes& value);

//! \brief All possible criteria the image is being validated on.
enum class Validation : uint8_t {
    //! No faces are found on the image.
    kFaceNotFound,

    //! There is more than one face on the image.
    kTooManyFaces,

    //! The face box is too small.
    kSmallFaceSize,

    //! The face relative size is too small.
    kSmallRelativeFaceSize,

    //! The distance between pupils on the face is too small.
    kSmallPupillaryDistance,

    //! The rotation angle of the head (inc. roll, pitch an yaw) is too big.
    kLargeFaceRotationAngle,

    //! The face is too close to the camera.
    kFaceTooClose,

    //! The face is too close to one or more image borders.
    kFaceCloseToBorder,

    //! The face is cropped.
    kFaceCropped,

    //! The face is occluded, for example with a medical mask.
    kFaceOccluded,

    //! The eyes are closed.
    kEyesClosed,

    //! The image is too dark.
    kDarkImage
};

//! @private
IDLF_API std::ostream& operator<<(std::ostream& os, Validation value);
//! @private
IDLF_API std::ostream& operator<<(std::ostream& os, const std::vector<Validation>& value);

//! General attributes of the image.
struct ImageAttributes {
    //! \brief The width of the image.
    int32_t width = 0;

    //! \brief The height of the image.
    int32_t height = 0;

    //! \brief The compression format of the source image.
    std::string source_format;
};

//! @private
IDLF_API std::ostream& operator<<(std::ostream& os, const ImageAttributes& value);

//! Quality attributes of the image.
struct ImageQualityAttributes {
    //! \brief How dark the image is, from 0 (normal) to 1 (too dark).
    float underexposure = 0;
};

//! @private
IDLF_API std::ostream& operator<<(std::ostream& os, const ImageQualityAttributes& value);

//! The format of the image pixel.
enum class PixelFormat {
    //! Red-Green-Blue, three bytes per pixel.
    kRGB,

    //! Blue-Green-Red, three bytes per pixel (used by OpenCV).
    kBGR,

    //! Grayscale, one byte per pixel.
    kGrayscale
};

//! @private
IDLF_API std::ostream& operator<<(std::ostream& os, PixelFormat value);

//! \brief The resource management and performance configuration.
//!
//! Use `CreateRuntimeConfiguration()` to get optimized defaults.
struct RuntimeConfiguration {

    //! \brief The number of threads used by IDLive Face for internal tasks.
    //!
    //! These tasks are always run on CPU.
    int32_t worker_threads = 0;

    //! \brief The number of threads used by the Neural Network Backend.
    //!
    //! Defines the number of active threads executed simultaneously by the Neural
    //! Network Backend. Depending on the backend it may be CPU threads or something
    //! else entirely.
    int32_t backend_threads = 0;

    //! \brief The maximum number of the concurrent Neural Network Backend's invocations.
    //!
    //! Logically represent the number of Neural Networks that can be run at once.
    int32_t backend_invocations = 0;

    //! \brief Other runtime parameters.
    std::map<std::string, std::string> parameters;

    //! @private
    std::shared_ptr<void> internal;
};

//! @private
IDLF_API std::ostream& operator<<(std::ostream& os, const RuntimeConfiguration& value);

//! \brief Overrides for the validation parameters.
//!
//! Affects the face detection and validation process. More relaxed values can
//! decrease the number of images being marked as invalid, but the accuracy will
//! degrade for such images.
struct CustomValidationParameters {
    //! \brief Minimal width and height of the face box, in pixels.
    Optional<int32_t> min_face_size;

    //! \brief Minimal distance from the image's border to the face box, in pixels.
    Optional<int32_t> min_face_padding;

    //! \brief Minimal distance between the pupils on the face, in pixels.
    Optional<int32_t> min_pupillary_distance;

    //! \brief Minimal relative face's width and height.
    //!
    //! It's a ratio of the face's width and height to the image's width and height.
    //! Represented as a value between 0 and 1. For example, if the value is 0.25,
    //! each side of the face box should be at least a quarter of the image's corresponding side.
    Optional<float> min_face_size_relative;

    //! \brief Minimal relative face's width and height for the face to be detected.
    //!
    //! Acts similar to the `min_face_size_relative`, but if the face box is too small, the face is
    //! just being skipped. It is used to filter the small faces in the background.
    Optional<float> detectable_face_size_relative;

    //! \brief Max yaw angle of the head.
    Optional<float> max_yaw;

    //! \brief Max pitch angle of the head.
    Optional<float> max_pitch;

    //! \brief Max roll angle of the head.
    Optional<float> max_roll;
};

}  // namespace idliveface
