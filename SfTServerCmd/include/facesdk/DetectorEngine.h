/* Copyright 2023 ID R&D Inc. All Rights Reserved. */

/**
 * @file DetectorEngine.h
 * FaceSDK detection engine header file
 */

#pragma once

//! \cond
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <facesdk/config.h>
//! \endcond

#include <facesdk/Common.h>
#include <facesdk/FaceException.h>
#include <facesdk/Image.h>

namespace facesdk {

/**
  * @brief  Smart pointer of DetectEngine
  */
using DetectEnginePtr = std::shared_ptr<class DetectEngine>;

/**
 * @brief Face rectangle bounding box
 */
struct BoundingBox {
    int left_top_x = 0; /**< X position of left top corner */
    int left_top_y = 0; /**< Y position of left top corner */

    int bottom_right_x = 0; /**< X position of right bottom corner */
    int bottom_right_y = 0; /**< Y position of right bottom corner */

    BoundingBox() = default;

    BoundingBox(int left_top_x, int left_top_y, int bottom_right_x, int bottom_right_y)
        : left_top_x(left_top_x),
          left_top_y(left_top_y),
          bottom_right_x(bottom_right_x),
          bottom_right_y(bottom_right_y) {}

    /**
     * @brief Override operator<<
     *
     * @param os Stream object reference
     * @param box Bounding box instance
     * @return stream reference
     */
    friend FACE_SDK_API std::ostream& operator<<(std::ostream& os, const BoundingBox& box);
};

/**
  * @brief Keypoints structure which is returned by DetectEngine
  */
struct Keypoints {
    int landmarks68[68][2] = {{0}}; /**< Coordinates of 68 facial landmarks */

    /**
     * @brief override operator<<
     *
     * @param os stream object reference
     * @param keypoints Keypoints instance
     * @return stream reference
     */
    friend FACE_SDK_API std::ostream& operator<<(std::ostream& os, const Keypoints& keypoints);
};

/**
 * @brief Set of yaw, pith, and roll angles (in degrees)
 */
struct HeadPose {
    float pitch = 0; /**< Pitch angle in degrees */
    float yaw = 0;   /**< Yaw angle in degrees */
    float roll = 0;  /**< Roll angle in degrees */

    HeadPose() = default;

    HeadPose(float pitch, float yaw, float roll) : pitch(pitch), yaw(yaw), roll(roll) {}

    friend FACE_SDK_API std::ostream& operator<<(std::ostream& os, const HeadPose& headpose);
};

/**
* @brief Structure for storing facial parameters
*/
struct FaceParameters {
    Keypoints keypoints;               /**< Facial keypoints */
    BoundingBox bounding_box;          /**< Facial bounding box */
    HeadPose head_pose;                /**< Facial head pose (yaw, pitch, roll) */
    float interpupillary_distance = 0; /**< Distance between pupils */
    float occlusion_probability = 0;   /**< Face occlusion probability from 0 to 1 */
    float closed_eyes_probability = 0; /**< Closed eyes probability from 0 to 1 */

    FaceParameters() = default;

    FaceParameters(const Keypoints& keypoints, const BoundingBox& bounding_box, const HeadPose& head_pose,
                   float interpupillary_distance, float occlusion_probability, float closed_eyes_probability)
        : keypoints(keypoints),
          bounding_box(bounding_box),
          head_pose(head_pose),
          interpupillary_distance(interpupillary_distance),
          occlusion_probability(occlusion_probability),
          closed_eyes_probability(closed_eyes_probability) {}

    /**
      * @brief override operator<<
      *
      * @param os stream object reference
      * @param params FaceParameters instance
      * @return stream reference
      */
    friend FACE_SDK_API std::ostream& operator<<(std::ostream& os, const FaceParameters& params);
};

/**
 * @brief Result of DetectEngine
 */
struct DetectionResult {
    std::vector<FaceParameters> faces; /**< FaceParameters array corresponding to detected faces */
    int orientation = 0;               /**< Approximated image or face orientation in degrees */

    DetectionResult() = default;

    DetectionResult(std::vector<FaceParameters> faces, int orientation)
        : faces(std::move(faces)),
          orientation(orientation) {}

    /**
     * @brief override operator<<
     *
     * @param os stream object reference
     * @param result DetectionResult instance
     * @return stream reference
     */
    friend FACE_SDK_API std::ostream& operator<<(std::ostream& os, const DetectionResult& result);
};

/**
 * @brief Alias for optional detection result
 */
using OptionalDetectionResult = OptionalResult<DetectionResult>;

/**
  * @brief Interface of facial keypoint detector
  */
class FACE_SDK_API DetectEngine {
public:
    /**
     * @brief Facial keypoints and bbox detecting method
     *
     * @param image Reference to Image object
     * @return Detection result
     * @throw FaceException
     */
    virtual DetectionResult detect(ImagePtr image) = 0;

    /**
     * @brief Facial keypoints and bbox batch detecting method
     *
     * @param images Input image batch
     * @return Optional detection results. The size of the result vector is guaranteed equal to `images` vector size.
     * @throw FaceException
     * @deprecated Using this method has no benefits compared to the usual `detect` method.
     */
    virtual std::vector<OptionalDetectionResult> detectBatch(const std::vector<ImagePtr>& images) = 0;

    /**
     * @brief Facial bbox detecting method
     *
     * @param image Reference to Image object
     * @return Facial bbox
     * @throw FaceException
     */
    virtual std::vector<BoundingBox> detectOnlyBoundingBox(ImagePtr image) = 0;

    /**
     * @brief Facial bbox batch detecting method
     *
     * @param images Input image batch
     * @return Optional facial bboxes. The size of the result vector is guaranteed equal to `images` vector size.
     * @throw FaceException
     * @deprecated Using this method has no benefits compared to the usual `detectOnlyBoundingBox` method.
     */
    virtual std::vector<facesdk::OptionalResult<std::vector<BoundingBox>>> detectOnlyBoundingBoxBatch(
            const std::vector<ImagePtr>& images) = 0;

    virtual ~DetectEngine() = default;
};

}  // namespace facesdk
