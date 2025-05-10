/* Copyright 2023 ID R&D Inc. All Rights Reserved. */

/**
 * @file Image.h
 * FaceSDK image header file
 */

#pragma once

//! \cond
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <facesdk/config.h>
//! \endcond

namespace facesdk {

/** 
  * @brief Smart pointer of Image
  */
using ImagePtr = std::shared_ptr<struct Image>;

/** 
  * @brief Structure for storing image in Face SDK format
  */
struct FACE_SDK_API Image {

    /**
     * @brief Color encoding enum
     */
    enum COLOR_ENCODING {
        RGB888, /**< Bytes in RGB format */
        BGR888  /**< Bytes in BGR format */
    };

    /**
     * @brief Create empty Image object
     *
     * @return Smart pointer to empty Image object
     */
    static ImagePtr createImage();

    /**
     * @brief Create Image object from binary file
     *
     * @param bytes Vector of image file bytes
     * @return Smart pointer to Image
     * @throw FaceException
     */
    static ImagePtr createImage(const std::vector<uint8_t>& bytes);

    /**
     * @brief Create Image object from binary file in memory buffer
     *
     * @param bytes Pointer to memory buffer with binary image file
     * @param size Number of bytes in image file
     * @return Smart pointer to Image
     * @throw FaceException
     */
    static ImagePtr createImage(const uint8_t* bytes, size_t size);

    /**
     * @brief Create Image object from disk
     *
     * @param path Path to image file on disk
     * @return Smart pointer to Image
     * @throw FaceException
     */
    static ImagePtr createImage(const std::string& path);

    /**
     * @brief Create Image object from memory buffer
     *
     * @param data Pointer to memory buffer (bytes)
     * @param rows Number of image rows
     * @param cols Number of image cols
     * @param format Color encoding format of input buffer
     * @return Smart pointer to Image
     * @throw FaceException
     */
    static ImagePtr createImage(const uint8_t* data, size_t rows, size_t cols, COLOR_ENCODING format);

    virtual ~Image() = default;

    /**
     * @brief Check if image is empty (no dynamic data allocated)
     *
     * @throw FaceException
     */
    virtual bool empty() const = 0;
};

}  // namespace facesdk
