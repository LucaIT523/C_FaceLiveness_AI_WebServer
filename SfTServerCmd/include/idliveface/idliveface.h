// Copyright 2023 ID R&D Inc. All Rights Reserved.

//! \file

#pragma once

//! \cond
#include <memory>
#include <string>
#include <utility>
#include <vector>

//! \endcond

//#include <idliveface/build.h>

#include <idliveface/base_types.h>
//#include <idliveface/exception.h>

namespace idliveface {

//! \brief Information about the IDLive Face release.
struct ReleaseInfo {
    //! \brief The IDLive Face version.
    std::string version;

    //! \brief The license expiration date.
    //!
    //! The format is the ISO-8601 instant, such as `2023-12-03T23:59:59Z`.
    std::string expiration_date;
};

typedef ReleaseInfo(*GetReleaseInfoFunc)();

//! @private
IDLF_API std::ostream& operator<<(std::ostream& os, const ReleaseInfo& value);

//! \brief Returns information about this IDLive Face release.
IDLF_API ReleaseInfo GetReleaseInfo();

//! @private
class ImageInternal;

//! \brief Contains the content of the decoded image.
//!
//! Copying the `Image` object only copies the pointer to the shared state, it
//! does not create a copy of the image.
class IDLF_API Image {
public:
    //! \brief Constructs the image from the raw pixels.
    //!
    //! Depending on the `pixel_format` will read either `width*height*3` or `width*height` bytes
    //! from the `pixels`.
    Image(const uint8_t* pixels, int32_t width, int32_t height, PixelFormat pixel_format);

    //! @private
    explicit Image(std::shared_ptr<ImageInternal> impl) : impl_(std::move(impl)) {}

    //! @private
    ~Image();

    //! Returns image attributes.
    const ImageAttributes& GetAttributes() const;

    //! @private
    const ImageInternal& impl() const {
        return *impl_;
    }

private:
    std::shared_ptr<ImageInternal> impl_;
};

//! @private
class ImageDecoderInternal;

//! \brief Decodes compressed images.
class IDLF_API ImageDecoder {
public:
    //! @private
    explicit ImageDecoder(std::shared_ptr<ImageDecoderInternal> impl) : impl_(std::move(impl)) {}
    //! @private
    ~ImageDecoder();

    //! \brief Loads and decodes the image from the file.
    //! \param file_path Path to the image file.
    //! \throw ImageDecodingException if the image can't be decoded or the invalid path is provided.
    Image DecodeFile(const std::string& file_path);

    //! \brief Decodes the image from the memory.
    //! \param bytes %Image content.
    //! \throw ImageDecodingException if the image can't be decoded.
    Image Decode(const std::vector<uint8_t>& bytes);

    //! \brief Decodes the image from the memory.
    //! \param bytes %Image content.
    //! \param size Size of the `bytes` array.
    //! \throw ImageDecodingException if the image can't be decoded.
    Image Decode(const uint8_t* bytes, size_t size);

private:
    std::shared_ptr<ImageDecoderInternal> impl_;
};

//! The information about the detected face.
struct DetectedFace {
    //! \brief Attributes of the detected face.
    FaceAttributes attributes;

    //! \brief Validations that are failed to pass for this particular face.
    std::vector<Validation> failed_validations;
};

//! @private
IDLF_API std::ostream& operator<<(std::ostream& os, const DetectedFace& value);

//! \brief The result of the face detection.
struct FaceDetectionResult {
    //! \brief The detected faces.
    //!
    //! When no faces are detected, the list will be empty and the `failed_validations`
    //! will contain `Validation::kFaceNotFound`.
    std::vector<DetectedFace> faces;

    //! \brief Quality attributes of the image.
    ImageQualityAttributes image_quality_attributes;

    //! \brief All failed validations.
    //!
    //! Contains global failed validations and ones from all the faces.
    std::vector<Validation> failed_validations;
};

//! @private
IDLF_API std::ostream& operator<<(std::ostream& os, const FaceDetectionResult& value);

//! @private
class FaceDetectorInternal;

//! \brief Detects and validates faces and their attributes.
class IDLF_API FaceDetector {
public:
    //! @private
    explicit FaceDetector(std::shared_ptr<FaceDetectorInternal> impl) : impl_(std::move(impl)) {}
    //! @private
    ~FaceDetector();

    //! \brief Runs the face detection.
    //!
    //! \param image Source image.
    //! \throw LicenseExpiredException if the license has expired.
    FaceDetectionResult DetectFaces(const Image& image) const;

private:
    std::shared_ptr<FaceDetectorInternal> impl_;
};

//! \brief The status of the analyzed face.
enum class FaceStatus {
    //! The face on the image is genuine and belongs to a real person.
    kGenuine,

    //! The face on the image is not genuine, which can be a result of
    //! the presentation attack.
    kNotGenuine,

    //! The face on the image is not suitable for the analysis or not present at all.
    kInvalid,
};

//! @private
IDLF_API std::ostream& operator<<(std::ostream& os, FaceStatus value);

//! \brief The result of the face analysis.
struct FaceAnalysisResult {
    //! \brief The status of the analyzed face.
    //!
    //! Set to `kGenuine` when `genuine_probability` is higher or equal to 0.5, and
    //! to `kNotGenuine` otherwise. The status `kInvalid` means some validations did
    //! non pass. They can be found in the `failed_validations`.
    FaceStatus status;

    //! \brief The probability that the face is genuine and belongs to a real person.
    //!
    //! Ranges from 0 (not genuine) to 1 (genuine). The face can be considered genuine
    //! when the probability is higher or equal to 0.5.
    //!
    //! Empty when the `status` is `kInvalid`.
    Optional<float> genuine_probability;

    //! \brief The bounding box around the detected face.
    //!
    //! Filled when the face was detected, even when the resulting status is `kInvalid`.
    //! Empty if no faces were detected.
    Optional<BoundingBox> box;

    //! \brief List of validations that have failed to pass.
    //!
    //! Only filled when the `status` is set to `kInvalid`.
    std::vector<Validation> failed_validations;

    //! @private
    std::shared_ptr<void> internal;
};

//! @private
IDLF_API std::ostream& operator<<(std::ostream& os, const FaceAnalysisResult& value);

//! \brief Optional parameters for the face analysis.
struct FaceAnalysisParameters {
    //! \brief The analysis domain.
    Optional<Domain> domain;

    //! \brief The analysis tolerance.
    Optional<Tolerance> tolerance;
};

//! @private
class FaceAnalyzerInternal;

//! \brief Detects face on the image and analyzes if it belongs to a real person or not.
class IDLF_API FaceAnalyzer {
public:
    //! @private
    explicit FaceAnalyzer(std::shared_ptr<FaceAnalyzerInternal> impl) : impl_(std::move(impl)) {}
    //! @private
    ~FaceAnalyzer();

    //! \brief Runs the face analysis.
    //!
    //! \param image The source image.
    //! \param parameters Optional face analysis parameters.
    //! \throw LicenseExpiredException if the license has expired.
    FaceAnalysisResult Analyze(const Image& image, const FaceAnalysisParameters& parameters = {}) const;

    //! \brief Returns the pipeline used.
    std::string GetPipeline();

private:
    std::shared_ptr<FaceAnalyzerInternal> impl_;
};

//! \brief Creates `RuntimeConfiguration` with the fields set to optimized defaults.
//!
//! By default assumes all CPU cores should be used. If there is a need to use CPU
//! resources, the number of cores can be limited with the `effective_cpu_cores` parameter.
//!
//! \param effective_cpu_cores Number of CPU cores to use. 0 means "use all".
IDLF_API RuntimeConfiguration CreateRuntimeConfiguration(int32_t effective_cpu_cores = 0);

//! @private
class BlueprintInternal;

//! \brief Factory for the IDLive Face's objects.
class IDLF_API Blueprint {
public:
    //! \brief Creates a new blueprint using the provided init data.
    //!
    //! The init data is located in the `data` directory of the IDLive Face bundle.
    //!
    //! \param init_data_path Path to the init data.
    //! \throw Exception if the provided path is invalid.
    explicit Blueprint(const std::string& init_data_path);

    //! \brief Creates a new blueprint using the provided init data and a custom
    //! runtime configuration.
    //!
    //! The init data is located in the `data` directory of the IDLive Face bundle.
    //!
    //! \param init_data_path Path to the init data.
    //! \param runtime_config Custom runtime configuration.
    //! \throw Exception if the provided path is invalid or the runtime configuration
    //!        contains invalid values.
    Blueprint(const std::string& init_data_path, const RuntimeConfiguration& runtime_config);

    //! \cond
    ~Blueprint();

    Blueprint(const Blueprint&) = delete;
    Blueprint& operator=(const Blueprint&) = delete;
    Blueprint(Blueprint&&) noexcept;
    Blueprint& operator=(Blueprint&&) noexcept;
    //! \endcond

    //! \brief Creates new `ImageDecoder`.
    ImageDecoder CreateImageDecoder() const;

    //! \brief Creates new `FaceAnalyzer`.
    FaceAnalyzer CreateFaceAnalyzer() const;

    //! \brief Creates new `FaceAnalyzer` that uses the specified pipeline.
    //!
    //! \param pipeline The name of the pipeline
    FaceAnalyzer CreateFaceAnalyzer(const std::string& pipeline) const;

    //! \brief Creates new `FaceDetector`.
    FaceDetector CreateFaceDetector() const;

    //! \brief Returns all available pipelines.
    //!
    //! The first one in the list is the default pipeline, which will be used by
    //! `CreateFaceAnalyzer()` if no other pipeline is specified.
    std::vector<std::string> GetAvailablePipelines() const;

    //! \brief Sets the analysis domain.
    void SetDomain(Domain domain);

    //! \brief Sets the analysis tolerance.
    void SetTolerance(Tolerance tolerance);

    //! \brief Suppresses (or unsuppresses) the validation.
    //!
    //! For all suppressed validations the `FaceAnalyzer` won't mark the face as
    //! invalid if the specific validation has failed to pass. `FaceDetector`
    //! won't return this validation either.
    //!
    //! You can suppress these validations:
    //!
    //! - kFaceOccluded
    //! - kEyesClosed
    //! - kDarkImage
    //!
    //! \param validation The validation to suppress.
    //! \param suppress `true` will suppress the validation, `false` will clear the suppression status.
    //! \throw Exception if the validation can't be suppressed.
    void SuppressValidation(Validation validation, bool suppress = true);

    //! Overrides validation parameters for all analysis domains.
    void OverrideValidationParameters(const CustomValidationParameters& custom_parameters);

    //! Overrides validation parameters for a specific analysis domain.
    void OverrideValidationParametersForDomain(Domain domain, const CustomValidationParameters& custom_parameters);

private:
    std::unique_ptr<BlueprintInternal> impl_;
};

}  // namespace idliveface
