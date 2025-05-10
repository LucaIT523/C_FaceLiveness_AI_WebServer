/* Copyright 2023 ID R&D Inc. All Rights Reserved. */

/**
 * @file QualityEngine.h
 * FaceSDK quality engine header file
 */

#pragma once

//! \cond
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include <facesdk/config.h>
//! \endcond

#include <facesdk/Common.h>
#include <facesdk/Image.h>

namespace facesdk {

/**
 * @brief Structure for quality estimation result
 */
struct QualityResult {

    float score; /**< Quality RAW output */
    bool class_; /**< Class, 0 - bad image, 1 - good image */

    QualityResult() = default;

    /**
     * @brief Create QualityResult object
     *
     * @param score Score value
     * @param class_value Quality decision
     */
    QualityResult(float score, bool class_value) : score(score), class_(class_value) {}

    /**
     * @brief Override operator<<
     *
     * @param os Stream object reference
     * @param result QualityResult instance
     * @return stream reference
     */
    friend FACE_SDK_API std::ostream& operator<<(std::ostream& os, const QualityResult& result);
};

/**
 * @brief Alias for optional quality result
 */
using OptionalQualityResult = OptionalResult<QualityResult>;

/**
 * @brief Smart pointer of QualityEngine
 */
using QualityEnginePtr = std::shared_ptr<class QualityEngine>;

/**
 * @brief Interface of quality estimation engine (method)
 */
class FACE_SDK_API QualityEngine {
public:
    /**
     * @brief Check quality of face from input image
     *
     * @param image Input image
     * @return Result of quality estimation
     * @throw FaceException
     */
    virtual QualityResult checkQuality(ImagePtr image) = 0;

    /**
     *  @brief Check quality of face from input images batch
     *
     * @param images Input image batch
     * @return Optional results of quality estimation.
     * The size of the result vector is guaranteed equal to `images` vector size.
     * @throw FaceException
     * @deprecated Using this method has no benefits compared to the usual `checkQuality` method.
     */
    virtual std::vector<OptionalQualityResult> checkQualityBatch(const std::vector<ImagePtr>& images) = 0;

    virtual ~QualityEngine() = default;
};

}  // namespace facesdk
