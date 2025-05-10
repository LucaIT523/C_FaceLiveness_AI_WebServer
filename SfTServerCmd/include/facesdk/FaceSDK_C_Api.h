/* Copyright 2023 ID R&D Inc. All Rights Reserved. */

/**
 * @file FaceSDK_C_Api.h
 * FaceSDK C API header file
 */

#pragma once

//! \cond
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <facesdk/config.h>
//! \endcond

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The default size for the buffer used for error messages (passed via `msg` argument).
 *
 * You can change it with `set_message_buffer_size` if needed.
 */
#define MESSAGE_BUFFER_SIZE 256

/**
 * @brief Configuration file parser
 */
struct CInitConfig;

/**
 * @brief Structure for storing image in Face SDK format
 */
struct CImage;

/**
 * @brief Structure for storing Image batch (a sequence of photos)
 */
struct CImageBatch;

/**
 * @brief Liveness detection pipeline
 */
struct CPipeline;

/**
 * @brief Face detector (bounding box and keypoints)
 */
struct CDetectEngine;

/**
 * @brief Quality estimation engine
 */
struct CQualityEngine;

typedef struct CInitConfig CInitConfig_t;
typedef struct CImage CImage_t;
typedef struct CImageBatch CImageBatch_t;
typedef struct CPipeline CPipeline_t;
typedef struct CDetectEngine CDetectEngine_t;
typedef struct CQualityEngine CQualityEngine_t;

/**
 * @brief Error codes enumeration
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
 * @brief Meta information: OS type for calibration
 */
typedef enum OS { ANDROID, IOS, DESKTOP, UNKNOWN_OS } OS_t;

/**
 * @brief Meta information: device manufacture
 */
typedef enum MANUFACTURE { UNKNOWN_MANUFACTURE } MANUFACTURE_t;

/**
 * @brief Meta information: device model
 */
typedef enum MODEL { UNKNOWN_MODEL } MODEL_t;

/**
 * @brief Liveness checking threshold calibration type
 */
typedef enum CALIBRATION {
    REGULAR = 0,  //!<Regular calibration, targets low APCER
    SOFT = 1,     //!<Soft calibration, achieves lower BPCER while still having acceptable APCER
    HARDENED = 2  //!<Hardened calibration, targets extra low APCER with higher BPCER
} CALIBRATION_t;

/**
 * @brief FaceSDK threading level, used to set different number of threads for different levels
 */
typedef enum ThreadingLevel {
    PIPELINE = 0,  //!< Pipeline threading level,
                   //!< corresponding environment variable: FACESDK_NUM_THREADS_PIPELINE.
    ENGINE = 1,    //!< Single engine (e.g. Pipeline, QualityEngine) threading level,
                   //!< corresponding environment variable: FACESDK_NUM_THREADS_ENGINE.
    OPERATOR = 2   //!< Single internal engine operator threading level,
                   //!< corresponding environment variable: FACESDK_NUM_THREADS_OPERATOR.
} ThreadingLevel_t;

/**
 * @brief Meta information provided for calibration
 */
struct CMeta {
    char* os_version;           //!< OS version
    OS_t os;                    //!< Device OS type
    MANUFACTURE_t manufacture;  //!< Device manufacture type
    MODEL_t model;              //!< Device model type
    CALIBRATION_t calibration;  //!< Liveness checking threshold calibration type to use
};
typedef struct CMeta CMeta_t;

/**
 * @brief Color encoding enum
 */
typedef enum COLOR_ENCODING {
    RGB888,  //!< Bytes in RGB format
    BGR888   //!< Bytes in BGR format
} COLOR_ENCODING_t;

/**
 * @brief Structure for quality estimation result
 */
struct CQualityResult {
    float score;  //!< Quality RAW output
    bool class_;  //!< Class, 0 - bad image, 1 - good image

    bool ok;  //!< Whether the result is valid (no errors occurred during processing)
};
typedef struct CQualityResult CQualityResult_t;

/**
 * @brief Structure for liveness detection result
 */
struct CLivenessResult {
    float score;        //!< Classifiers RAW output
    float probability;  //!< Probability of liveness in range [0, 1]

    bool ok;  //!< Whether the result is valid (no errors occurred during processing)
};
typedef struct CLivenessResult CLivenessResult_t;

/**
 * @brief Structure for liveness detection result
 */
struct CPipelineResult {
    CQualityResult_t quality_result;    //!< Liveness result
    CLivenessResult_t liveness_result;  //!< Quality result
};
typedef struct CPipelineResult CPipelineResult_t;

/**
 * @brief Face rectangle bounding box
 */
struct CBoundingBox {
    int left_top_x;  //!< X position of left top corner
    int left_top_y;  //!< Y position of left top corner

    int bottom_right_x;  //!< X position of right bottom corner
    int bottom_right_y;  //!< Y position of right bottom corner
};
typedef struct CBoundingBox CBoundingBox_t;

/**
 * @brief Face rectangle bounding boxes array
 */
struct CBoundingBoxes {
    CBoundingBox_t* boxes;
    unsigned int num_boxes;
};
typedef struct CBoundingBoxes CBoundingBoxes_t;

/**
 * @brief Face keypoints structure
 */
struct CKeypoints {
    int landmarks68[68][2];  //!< Coordinates of 68 facial landmarks
};
typedef struct CKeypoints CKeypoints_t;

/**
 * @brief Set of yaw, pith, and roll angles (in degrees)
 */
struct CHeadPose {
    float yaw;    //!< Yaw angle in degrees
    float pitch;  //!< Pitch angle in degrees
    float roll;   //!< Roll angle in degrees
};
typedef struct CHeadPose CHeadPose_t;

/**
 * @brief Structure for storing facial parameters
 */
struct CFaceParameters {
    CKeypoints_t keypoints;         //!< Facial keypoints
    CBoundingBox_t bounding_box;    //!< Facial bounding box
    CHeadPose_t head_pose;          //!< Facial head pose (yaw, pitch, roll)
    float interpupillary_distance;  //!< Distance between pupils
    float occlusion_probability;    //!< Face occlusion probability from 0 to 1
    float closed_eyes_probability;  //!< Closed eyes probability from 0 to 1
};
typedef struct CFaceParameters CFaceParameters_t;

/**
 * @brief Face detection result structure
 */
struct CDetectionResult {
    CFaceParameters_t* faces;  //!< FaceParameters array corresponding to detected faces
    unsigned int num_faces;    //!< Number of detected faces (length of `faces` array)
    int orientation;           //!< Approximated image or face orientation in degrees
};
typedef struct CDetectionResult CDetectionResult_t;

/**
 * @brief Returns `CMeta_t` filled with default values.
 */
FACE_SDK_API CMeta_t get_default_meta();

/**
 * @brief CInitConfig_t instance creator
 *
 * @param [in] folder Path to directory with configuration files
 * @param [in] config_name Configuration file name
 * @param [out] err Output error code optional parameter
 * @param [out] msg Output error message optional parameter
 * @return Pointer to CInitConfig_t, NULL if error occurred
 */
FACE_SDK_API CInitConfig_t* config_create(const char* folder, const char* config_name, int* err, char* msg);

/**
 * @brief CInitConfig_t instance creator
 *
 * @param [in] file_path Full path to configuration file
 * @param [out] err Output error code optional parameter
 * @param [out] msg Output error message optional parameter
 * @return Pointer to CInitConfig_t, NULL if error occurred
 */
FACE_SDK_API CInitConfig_t* config_create2(const char* file_path, int* err, char* msg);

/**
 * @brief CInitConfig_t instance destroyer
 * @param [in] config CInitConfig_t instance
 */
FACE_SDK_API void config_destroy(CInitConfig_t* config);

/**
 * @brief Create CImage_t instance from binary file in memory buffer
 *
 * @param [in] bytes Pointer to memory buffer with binary image file
 * @param [in] size Number of bytes in image file
 * @param [out] err Output error code optional parameter
 * @param [out] msg Output error message optional parameter
 * @return Pointer to CImage_t, NULL if error occurred
 */
FACE_SDK_API CImage_t* image_create_bytes(const uint8_t* bytes, size_t size, int* err, char* msg);

/**
 * @brief Create CImage_t instance from a file on disk
 *
 * @param [in] path Path to image file
 * @param [out] err Output error code optional parameter
 * @param [out] msg Output error message optional parameter
 * @return Pointer to CImage_t, NULL if error occurred
 */
FACE_SDK_API CImage_t* image_create_path(const char* path, int* err, char* msg);

/**
 * @brief Create CImage_t instance from decoded memory buffer
 *
 * @param [in] data Pointer to memory buffer (bytes)
 * @param [in] rows Number of image rows
 * @param [in] cols Number of image cols
 * @param [in] format Color encoding format of input buffer
 * @param [out] err Output error code optional parameter
 * @param [out] msg Output error message optional parameter
 * @return Pointer to CImage_t, NULL if error occurred
 */
FACE_SDK_API CImage_t* image_create_pixels(const uint8_t* data, size_t rows, size_t cols, COLOR_ENCODING_t format,
                                           int* err, char* msg);

/**
 * @brief CImage_t instance destroyer
 * @param [in] image CImage_t instance
 */
FACE_SDK_API void image_destroy(CImage_t* image);

/**
 * @brief Create CImageBatch_t instance from CImage_t array and timestamps
 *
 * @param [in] images Pointer to CImage_t* array
 * @param [in] num_images Length of images array
 * @param [in] timestamps Optional timestamps array, should have `num_images` length if provided
 * @param [out] err Output error code optional parameter
 * @param [out] msg Output error message optional parameter
 * @return Pointer to CImageBatch_t, NULL if error occurred
 */
FACE_SDK_API CImageBatch_t* image_batch_create(CImage_t** images, size_t num_images, const uint64_t* timestamps,
                                               int* err, char* msg);

/**
 * @brief CImageBatch_t instance destroyer
 * @param [in] image_batch CImageBatch_t instance
 */
FACE_SDK_API void image_batch_destroy(CImageBatch_t* image_batch);

/**
 * @brief Create CDetectEngine_t instance
 *
 * @param [in] name Engine name (e.g. "BaseNnetDetector")
 * @param [in] config Configuration object
 * @param [out] err Output error code optional parameter
 * @param [out] msg Output error message optional parameter
 * @return Pointer to CDetectEngine_t, NULL if error occurred
 */
FACE_SDK_API CDetectEngine_t* detection_create(const char* name, const CInitConfig_t* config, int* err, char* msg);

/**
 * @brief CDetectEngine_t instance destroyer
 * @param [in] engine CDetectEngine_t instance
 */
FACE_SDK_API void detection_destroy(CDetectEngine_t* engine);

/**
 * @brief Facial keypoints and bbox detecting method
 *
 * @param [in] engine CDetectEngine_t instance
 * @param [in] image CImage_t instance
 * @param [out] err Output error code optional parameter
 * @param [out] msg Output error message optional parameter
 * @return Pointer to CDetectionResult_t, NULL if error occurred
 */
FACE_SDK_API CDetectionResult_t* detect(const CDetectEngine_t* engine, const CImage_t* image, int* err, char* msg);

/**
 * @brief Facial keypoints and bbox batch detecting method
 *
 * @param [in] engine CDetectEngine_t instance
 * @param [in] images Pointer to CImage_t* array
 * @param [in] num_images Length of images array
 * @param [out] errors Output error codes array optional parameter, should have `num_images` length if provided
 * @param [out] msg Output error messages array optional parameter, should have `num_images` length if provided
 * @return Pointer to CDetectionResult_t array of `num_images` length, NULL if error occurred
 * @deprecated Using this function has no benefits compared to the usual `detect` function.
 */
FACE_SDK_API CDetectionResult_t* detect_batch(const CDetectEngine_t* engine, const CImage_t** images, size_t num_images,
                                              int* errors, char** msg);

/**
 * @brief Facial bbox detecting method
 *
 * @param [in] engine CDetectEngine_t instance
 * @param [in] image CImage_t instance
 * @param [out] err Output error code optional parameter
 * @param [out] msg Output error message optional parameter
 * @return Pointer to CBoundingBoxes_t, NULL if error occurred
 */
FACE_SDK_API CBoundingBoxes_t* detect_only_bounding_box(const CDetectEngine_t* engine, const CImage_t* image, int* err,
                                                        char* msg);

/**
 * @brief Facial bbox batch detecting method
 *
 * @param [in] engine CDetectEngine_t instance
 * @param [in] images Pointer to CImage_t* array
 * @param [in] num_images Length of images array
 * @param [out] errors Output error codes array optional parameter, should have `num_images` length if provided
 * @param [out] msg Output error messages array optional parameter, should have `num_images` length if provided
 * @return Pointer to CBoundingBoxes_t array of `num_images` length, NULL if error occurred
 * @deprecated Using this function has no benefits compared to the usual `detect_only_bounding_box` function.
 */
FACE_SDK_API CBoundingBoxes_t* detect_only_bounding_box_batch(const CDetectEngine_t* engine, const CImage_t** images,
                                                              size_t num_images, int* errors, char** msg);

/**
 * @brief CDetectionResult_t instance destroyer
 * @param [in] result CDetectionResult_t instance
 */
FACE_SDK_API void CDetectionResult_destroy(CDetectionResult_t* result);

/**
 * @brief CDetectionResult_t instances array destroyer
 * @param [in] result Pointer to CDetectionResult_t array
 * @param [in] size Length of CDetectionResult_t array
 */
FACE_SDK_API void CDetectionResult_destroy_array(CDetectionResult_t* result, size_t size);

/**
 * @brief CBoundingBoxes_t instance destroyer
 * @param [in] result CBoundingBoxes_t instance
 */
FACE_SDK_API void CBoundingBoxes_destroy(CBoundingBoxes_t* result);

/**
 * @brief CBoundingBoxes_t instances array destroyer
 * @param [in] result Pointer to CBoundingBoxes_t array
 * @param [in] size Size of CBoundingBoxes_t array
 */
FACE_SDK_API void CBoundingBoxes_destroy_array(CBoundingBoxes_t* result, size_t size);

/**
 * @brief Create CQualityEngine_t instance
 *
 * @param [in] name Engine name (e.g. "ExpositionQualityEngine")
 * @param [in] config Configuration object
 * @param [out] err Output error code optional parameter
 * @param [out] msg Output error message optional parameter
 * @return Pointer to CQualityEngine_t, NULL if error occurred
 */
FACE_SDK_API CQualityEngine_t* quality_create(const char* name, const CInitConfig_t* config, int* err, char* msg);

/**
 * @brief CQualityEngine_t instance destroyer
 * @param [in] engine CQualityEngine_t instance
 */
FACE_SDK_API void quality_destroy(CQualityEngine_t* engine);

/**
 * @brief Check quality of face from input image
 *
 * @param [in] engine CQualityEngine_t instance
 * @param [in] image CImage_t instance
 * @param [out] err Output error code optional parameter
 * @param [out] msg Output error message optional parameter
 * @return Quality checking result
 */
FACE_SDK_API CQualityResult_t check_quality(const CQualityEngine_t* engine, const CImage_t* image, int* err, char* msg);

/**
 * @brief Check quality of face from input images batch
 *
 * @param [in] engine CQualityEngine_t instance
 * @param [in] images Pointer to CImage_t* array
 * @param [in] num_images Length of images array
 * @param [out] errors Output error codes array optional parameter, should have `num_images` length if provided
 * @param [out] msg Output error messages array optional parameter, should have `num_images` length if provided
 * @return Pointer to CQualityResult_t array of `num_images` length, NULL if error occurred
 * @deprecated Using this function has no benefits compared to the usual `check_quality` function.
 */
FACE_SDK_API CQualityResult_t* check_quality_batch(const CQualityEngine_t* engine, const CImage_t** images,
                                                   size_t num_images, int* errors, char** msg);

/**
 * @brief CQualityResult_t instances array destroyer
 * @param [in] result Pointer to CQualityResult_t array
 */
FACE_SDK_API void CQualityResult_destroy_array(CQualityResult_t* result);

/**
 * @brief Create CPipeline_t instance
 *
 * @param [in] name Pipeline name (e.g. "ConfigurablePipeline")
 * @param [in] config Configuration object
 * @param [out] err Output error code optional parameter
 * @param [out] msg Output error message optional parameter
 * @return Pointer to CPipeline_t, NULL if error occurred
 */
FACE_SDK_API CPipeline_t* pipeline_create(const char* name, const CInitConfig_t* config, int* err, char* msg);

/**
 * @brief CPipeline_t instance destroyer
 * @param [in] engine CPipeline_t instance
 */
FACE_SDK_API void pipeline_destroy(CPipeline_t* engine);

/**
 * @brief Check liveness of face from input image
 *
 * @param [in] engine CPipeline_t instance
 * @param [in] image CImage_t instance
 * @param [in] meta Optional meta information about device
 * @param [out] err Output error code optional parameter
 * @param [out] msg Output error message optional parameter
 * @return Liveness detection result
 */
FACE_SDK_API CPipelineResult_t pipeline_check_liveness(const CPipeline_t* engine, const CImage_t* image,
                                                       const CMeta_t* meta, int* err, char* msg);

/**
 * @brief Perform liveness check on a sequence of photos (image batch).
 * A single results corresponding to the whole image batch will be produced
 *
 * @param [in] engine CPipeline_t instance
 * @param [in] image_batch Input batch of images (a sequence of photos)
 * @param [in] meta Optional meta information about device
 * @param [out] err Output error code optional parameter
 * @param [out] msg Output error message optional parameter
 * @return A single liveness detection result for a given sequence of photos
 */
FACE_SDK_API CPipelineResult_t pipeline_check_liveness_batch(const CPipeline_t* engine,
                                                             const CImageBatch_t* image_batch, const CMeta_t* meta,
                                                             int* err, char* msg);

/**
 * @brief This method implements batched liveness check
 *
 * @param [in] engine CPipeline_t instance
 * @param [in] images Pointer to CImage_t* array
 * @param [in] num_images Length of images array
 * @param [in] meta Optional meta information about device array, should have `num_images` length if provided
 * @param [out] errors Output error codes array optional parameter, should have `num_images` length if provided
 * @param [out] msg Output error messages array optional parameter, should have `num_images` length if provided
 * @return Pointer to CPipelineResult_t array of `num_images` length, NULL if error occurred
 * @deprecated Using this function has no benefits compared to the usual `pipeline_check_liveness` function.
 */
FACE_SDK_API CPipelineResult_t* pipeline_check_liveness_batch2(const CPipeline_t* engine, const CImage_t** images,
                                                               size_t num_images, const CMeta_t* meta, int* errors,
                                                               char** msg);

/**
 * @brief CPipelineResult_t instances array destroyer
 * @param [in] result Pointer to CPipelineResult_t array
 */
FACE_SDK_API void CPipelineResult_destroy_array(CPipelineResult_t* result);

/**
 * @brief Sets the maximum number of threads available for FaceSDK at the specified level.
 * If 0 is passed, then the optimal number of threads is detected automatically
 * (the same effect is achieved if setNumThreads is not called).
 *
 * @param [in] num_threads Maximum number of threads available for FaceSDK at the specified level
 * @param [in] threading_level Threading level to apply setting
 * @param [out] err Output error code optional parameter
 * @param [out] msg Output error message optional parameter
 * @note Function call is equivalent to setting FACESDK_NUM_THREADS_PIPELINE, FACESDK_NUM_THREADS_ENGINE
 * or FACESDK_NUM_THREADS_OPERATOR environment variable before loading FaceSDK library.
 * @note Function call takes precedence over environment variables.
 */
FACE_SDK_API void set_num_threads(unsigned int num_threads, const ThreadingLevel_t* threading_level, int* err,
                                  char* msg);

/**
 * @brief Sets the number of CPU execution streams for OpenVINO backend.
 *
 * @param [in] ov_num_throughput_streams Number of CPU execution streams, valid values are -1
 * (auto-tune number of streams), 0 (use the default setting) and any positive value, other values are dropped
 * without an error
 * @note Function call is equivalent to setting FACESDK_OV_NUM_THROUGHPUT_STREAMS environment variable
 * before loading FaceSDK library.
 * @note Function call takes precedence over environment variables.
 * @note Function does not take effect if FaceSDK build does not use OpenVINO.
 */
FACE_SDK_API void set_ov_num_throughput_streams(int ov_num_throughput_streams);

/**
 * @brief Sets the CPU bind setting for OpenVINO backend.
 *
 * @param [in] ov_bind_threads Whether to bind execution threads to CPU physical cores or not
 * @note Function call is equivalent to setting FACESDK_OV_BIND_THREADS environment variable
 * before loading FaceSDK library.
 * @note Function call takes precedence over environment variables.
 * @note Function does not take effect if FaceSDK build does not use OpenVINO.
 */
FACE_SDK_API void set_ov_bind_threads(bool ov_bind_threads);

/**
 * @brief Sets the maximum batch size for OpenVINO backend. This is not a hard limit (i.e.
 * passing batches bigger than the limit will not cause an error), however setting this
 * value to the target batch size will increase performance.
 *
 * @param [in] ov_max_batch_size Maximum batch size
 * @note Function call is equivalent to setting FACESDK_OV_MAX_BATCH_SIZE environment variable
 * before loading FaceSDK library.
 * @note Function call takes precedence over environment variables.
 * @note Function does not take effect if FaceSDK build does not use OpenVINO.
 */
FACE_SDK_API void set_ov_max_batch_size(unsigned int ov_max_batch_size);

/**
 * @brief Sets whether to enable FaceSDK logging. Logging is enabled by default.
 *
 * @param [in] enable_logging Whether to enable logging or not
 * @note Function call is equivalent to setting FACESDK_ENABLE_LOGGING environment variable
 * before loading FaceSDK library.
 * @note Function call takes precedence over environment variables.
 */
FACE_SDK_API void set_enable_logging(bool enable_logging);

/**
 * @brief Sets the maximum number of execution streams (maximum number of requests executed
 * in concurrent) for Pipeline. Default value is number of logical CPU cores.
 *
 * @param [in] num_pipeline_execution_streams Number of execution streams
 * @note Function call does not take any effect, reserved for future usage.
 * @note Function call is equivalent to setting FACESDK_NUM_PIPELINE_EXECUTION_STREAMS environment variable
 * before loading FaceSDK library.
 * @note Function call takes precedence over environment variables.
 */
FACE_SDK_API void set_num_pipeline_execution_streams(unsigned int num_pipeline_execution_streams);

/**
 * @brief Toggles face occlusion detection. When enabled the occluded faces will be rejected.
 *
 * @param [in] enabled Whether to enable the detection or not
 * @note Function call is equivalent to setting FACESDK_ENABLE_FACE_OCCLUSION_DETECTION environment variable
 * before loading FaceSDK library.
 * @note Function call takes precedence over environment variables.
 */
FACE_SDK_API void set_enable_face_occlusion_detection(bool enabled);

/**
 * @brief Toggles closed eyes detection. When enabled the faces with closed eyes will be rejected.
 *
 * @param [in] enabled Whether to enable the detection or not
 * @note Function call is equivalent to setting FACESDK_ENABLE_CLOSED_EYES_DETECTION environment variable
 * before loading FaceSDK library.
 * @note Function call takes precedence over environment variables.
 */
FACE_SDK_API void set_enable_closed_eyes_detection(bool enabled);

/**
 * @brief Returns information about a license if available.
 *
 * @param [out] license_info Preallocated char array to store null-terminated string containing
 * license information
 * @param [in] license_info_length Preallocated char array length
 * @param [out] err Output error code optional parameter
 * @param [out] msg Output error message optional parameter
 */
FACE_SDK_API void get_license_info(char* license_info, size_t license_info_length, int* err, char* msg);

/**
 * @brief Sets the size for the buffer used for error messages (passed via `msg` argument). Longer
 * messages will be truncated.
 *
 * @param [in] size Size of the buffer. Passing 0 has no effect.
 */
FACE_SDK_API void set_message_buffer_size(size_t size);

#ifdef __cplusplus
}
#endif
