/* Copyright 2023 ID R&D Inc. All Rights Reserved. */

/**
 * @file FaceSDK.h
 * Base FaceSDK header file
 */

#pragma once

//! \cond
#include <memory>
#include <string>
#include <vector>

#include <facesdk/config.h>
//! \endcond

#include <facesdk/DetectorEngine.h>
#include <facesdk/Pipeline.h>
#include <facesdk/QualityEngine.h>

namespace facesdk {

/**
 * @brief Smart pointer of InitConfig
 */
using InitConfigPtr = std::shared_ptr<struct InitConfig>;

/**
 * @brief Configuration file parser
 */
struct FACE_SDK_API InitConfig {

    /**
     * @brief Configuration object creator
     *
     * @param dir Path to directory with configuration files
     * @param config_file_name Configuration file name
     * @return Smart pointer to configuration object
     * @throw std::runtime_error
     */
    static InitConfigPtr createConfig(const std::string& dir, const std::string& config_file_name);

    /**
     * @brief Configuration object creator
     *
     * @param file_path Full path to configuration file
     * @return Smart pointer to configuration object
     * @throw std::runtime_error
     */
    static InitConfigPtr createConfig(const std::string& file_path);
};

/**
 * @brief FaceSDK threading level, used to set different number of threads for different levels
 */
enum class ThreadingLevel {
    PIPELINE = 0,  //!< Pipeline threading level,
                   //!< corresponding environment variable: FACESDK_NUM_THREADS_PIPELINE.
    ENGINE = 1,    //!< Single engine (e.g. Pipeline, QualityEngine) threading level,
                   //!< corresponding environment variable: FACESDK_NUM_THREADS_ENGINE.
    OPERATOR = 2   //!< Single internal engine operator threading level,
                   //!< corresponding environment variable: FACESDK_NUM_THREADS_OPERATOR.
};

/**
 * @brief Face engines and pipelines aggregator
 */
class FACE_SDK_API FaceSDK {
public:
    /**
     * @brief Return pipeline engine by name
     *
     * @param name Pipeline name
     * @param config Configuration object
     * @return Smart pointer to pipeline object
     * @throw FaceException
     */
    static PipelinePtr getPipeline(const std::string& name, InitConfigPtr config);

    /**
     * @brief Return face detection engine by name
     *
     * @param name Engine name
     * @param config Configuration object
     * @return Smart pointer to face detection engine
     * @throw FaceException
     */
    static DetectEnginePtr getDetectEngine(const std::string& name, InitConfigPtr config);

    /**
     * @brief Return face quality engine by name
     *
     * @param name Engine name
     * @param config Configuration object
     * @return Smart pointer to face detection engine
     * @throw FaceException
     */
    static QualityEnginePtr getQualityEngine(const std::string& name, InitConfigPtr config);

    /**
     * @brief Sets the maximum number of threads available for FaceSDK at the specified level.
     * If 0 is passed, then the optimal number of threads is detected automatically
     * (the same effect is achieved if setNumThreads is not called).
     *
     * @param num_threads Maximum number of threads available for FaceSDK at the specified level
     * @param threading_level Threading level to apply setting
     * @note Function call is equivalent to setting FACESDK_NUM_THREADS_PIPELINE, FACESDK_NUM_THREADS_ENGINE
     * or FACESDK_NUM_THREADS_OPERATOR environment variable before loading FaceSDK library.
     * @note Function call takes precedence over environment variables.
     */
    static void setNumThreads(unsigned int num_threads, ThreadingLevel threading_level) noexcept;

    /**
     * @brief Sets the number of CPU execution streams for OpenVINO backend.
     *
     * @param ov_num_throughput_streams Number of CPU execution streams, valid values are -1
     * (auto-tune number of streams), 0 (use the default setting) and any positive value, other values are dropped
     * without an error
     * @note Function call is equivalent to setting FACESDK_OV_NUM_THROUGHPUT_STREAMS environment variable
     * before loading FaceSDK library.
     * @note Function call takes precedence over environment variables.
     * @note Function does not take effect if FaceSDK build does not use OpenVINO.
     */
    static void setOvNumThroughputStreams(int ov_num_throughput_streams) noexcept;

    /**
     * @brief Sets the CPU bind setting for OpenVINO backend.
     *
     * @param ov_bind_threads Whether to bind execution threads to CPU physical cores or not
     * @note Function call is equivalent to setting FACESDK_OV_BIND_THREADS environment variable
     * before loading FaceSDK library.
     * @note Function call takes precedence over environment variables.
     * @note Function does not take effect if FaceSDK build does not use OpenVINO.
     */
    static void setOvBindThreads(bool ov_bind_threads) noexcept;

    /**
     * @brief Sets the maximum batch size for OpenVINO backend. This is not a hard limit (i.e.
     * passing batches bigger than the limit will not cause an error), however setting this
     * value to the target batch size will increase performance.
     *
     * @param ov_max_batch_size Maximum batch size
     * @note Function call is equivalent to setting FACESDK_OV_MAX_BATCH_SIZE environment variable
     * before loading FaceSDK library.
     * @note Function call takes precedence over environment variables.
     * @note Function does not take effect if FaceSDK build does not use OpenVINO.
     */
    static void setOvMaxBatchSize(unsigned int ov_max_batch_size) noexcept;

    /**
     * @brief Sets whether to enable FaceSDK logging. Logging is enabled by default.
     *
     * @param enable_logging Whether to enable logging or not
     * @note Function call is equivalent to setting FACESDK_ENABLE_LOGGING environment variable
     * before loading FaceSDK library.
     * @note Function call takes precedence over environment variables.
     */
    static void setEnableLogging(bool enable_logging) noexcept;

    /**
     * @brief Sets the maximum number of execution streams (maximum number of requests executed
     * in concurrent) for Pipeline. Default value is number of logical CPU cores.
     *
     * @param num_pipeline_execution_streams Number of execution streams
     * @note Function call does not take any effect, reserved for future usage.
     * @note Function call is equivalent to setting FACESDK_NUM_PIPELINE_EXECUTION_STREAMS environment variable
     * before loading FaceSDK library.
     * @note Function call takes precedence over environment variables.
     */
    static void setNumPipelineExecutionStreams(unsigned int num_pipeline_execution_streams) noexcept;

    /**
     * @brief Toggles face occlusion detection. When enabled the occluded faces will be rejected.
     *
     * @param enabled Whether to enable the detection or not
     * @note Function call is equivalent to setting FACESDK_ENABLE_FACE_OCCLUSION_DETECTION environment variable
     * before loading FaceSDK library.
     * @note Function call takes precedence over environment variables.
     */
    static void setEnableFaceOcclusionDetection(bool enabled) noexcept;

    /**
     * @brief Toggles closed eyes detection. When enabled the faces with closed eyes will be rejected.
     *
     * @param enabled Whether to enable the detection or not
     * @note Function call is equivalent to setting FACESDK_ENABLE_CLOSED_EYES_DETECTION environment variable
     * before loading FaceSDK library.
     * @note Function call takes precedence over environment variables.
     */
    static void setEnableClosedEyesDetection(bool enabled) noexcept;

    /**
     * @brief Returns the release version.
     *
     * @return String containing the release version
     */
    static std::string getVersion();

    /**
     * @brief Returns information about a license if available.
     *
     * @return String containing license information
     * @throw FaceException
     */
    static std::string getLicenseInfo();

    FaceSDK() = delete;
};

}  // namespace facesdk
