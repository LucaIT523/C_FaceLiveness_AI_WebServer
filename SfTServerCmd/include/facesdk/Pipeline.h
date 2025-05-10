/* Copyright 2023 ID R&D Inc. All Rights Reserved. */

/**
 * @file Pipeline.h
 * FaceSDK liveness pipeline header file
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
#include <facesdk/Image.h>
#include <facesdk/ImageBatch.h>
#include <facesdk/QualityEngine.h>

namespace facesdk {

/**
 * @brief Meta information provided for calibration
 */
struct Meta {

    /**
     * @brief OS type for calibration
     */
    enum class OS { ANDROID, IOS, DESKTOP, UNKNOWN };

    /**
     * @brief Device manufacture
     */
    enum class MANUFACTURE { UNKNOWN };

    /**
     * @brief Device model
     */
    enum class MODEL { UNKNOWN };

    /**
     * @brief Liveness checking threshold calibration type
     */
    enum class CALIBRATION {
        REGULAR, /**< Regular calibration, targets low APCER */
        SOFT,    /**< Soft calibration, achieves lower BPCER while still having acceptable APCER */
        HARDENED /**< Hardened calibration, targets extra low APCER with higher BPCER */
    };

    Meta() = default;

    Meta(std::string os_vesion, OS os, MANUFACTURE manufacture, MODEL model,
         CALIBRATION calibration = CALIBRATION::REGULAR)
        : os_version(std::move(os_vesion)),
          os(os),
          manufacture(manufacture),
          model(model),
          calibration(calibration) {}

    std::string os_version;                         /**< OS version */
    OS os = OS::UNKNOWN;                            /**< Device OS type */
    MANUFACTURE manufacture = MANUFACTURE::UNKNOWN; /**< Device manufacture type */
    MODEL model = MODEL::UNKNOWN;                   /**< Device model type */
    CALIBRATION calibration = CALIBRATION::REGULAR; /**< Liveness checking threshold calibration type to use */
};

/**
 * @brief Structure for liveness detection result
 */
struct LivenessResult {
    float score;       /**< Classifiers RAW output */
    float probability; /**< Probability of liveness in range=[0, 1] */

    LivenessResult() = default;

    /**
     * @brief Create LivenessResult object
     *
     * @param score Score value
     * @param probability Probability value
     */
    LivenessResult(float score, float probability) : score(score), probability(probability) {}

    /**
     * @brief Override operator<<
     *
     * @param os Stream object reference
     * @param result LivenessResult instance
     * @return stream reference
     */
    friend FACE_SDK_API std::ostream& operator<<(std::ostream& os, const LivenessResult& result);
};

/**
 * @brief Structure for liveness detection result
 */
struct PipelineResult {
    LivenessResult liveness_result; /**< Liveness result */
    QualityResult quality_result;   /**< Quality result */

    /**
     * @brief Override operator<<
     *
     * @param os Stream object reference
     * @param result PipelineResult instance
     * @return stream reference
     */
    friend FACE_SDK_API std::ostream& operator<<(std::ostream& os, const PipelineResult& result);
};

/**
 * @brief Alias for optional pipeline result
 */
using OptionalPipelineResult = OptionalResult<PipelineResult>;

/**
 * @brief Smart pointer of Pipeline
 */
using PipelinePtr = std::shared_ptr<class Pipeline>;

/**
 * @brief Interface of liveness detection pipeline
 */
class FACE_SDK_API Pipeline {
public:
    /**
     * @brief Check liveness of face from input image
     *
     * @param image Input image
     * @param meta Meta information about device
     * @return Result of liveness detection
     * @throw FaceException
     */
    virtual PipelineResult checkLiveness(ImagePtr image, Meta meta = {}) = 0;

    /**
     * @brief Perform liveness check on a sequence of photos (image batch).
     * A single results corresponding to the whole image batch will be produced.
     *
     * @param image_batch Input batch of images (a sequence of photos)
     * @param meta Meta information about device
     * @return A single liveness detection result for a given sequence of photos
     * @throw FaceException
     */
    virtual PipelineResult checkLiveness(const ImageBatch& image_batch, Meta meta = {}) = 0;

    /**
     * @brief This method implements batched liveness check.
     *
     * @param images Input images
     * @param meta Meta information about device for all or none images
     * @return Optional liveness check results.
     * The size of the result vector is guaranteed equal to `images` vector size.
     * @throw FaceException
     * @deprecated Using this method has no benefits compared to the usual `checkLiveness` method.
     */
    virtual std::vector<OptionalPipelineResult> checkLivenessBatch(const std::vector<facesdk::ImagePtr>& images,
                                                                   const std::vector<Meta>& meta = {}) = 0;

    virtual ~Pipeline() = default;
};

}  // namespace facesdk
