/* Copyright 2023 ID R&D Inc. All Rights Reserved. */

/**
 * @file ImageBatch.h
 * FaceSDK image batch header file
 */

#pragma once

//! \cond
#include <facesdk/config.h>
//! \endcond

#include <facesdk/Image.h>

namespace facesdk {

/** 
  * @brief Structure for storing Image batch (a sequence of photos)
  */
struct FACE_SDK_API ImageBatch {

    ImageBatch() = default;

    /**
     * @brief Create Image batch from Image array with timestamps
     *
     * @param images_ Pointer to Image array
     * @param n_images_ Number of Image objects in array
     * @param timestamps_ Timestamps, corresponding to Image objects in array
     */
    ImageBatch(ImagePtr* images_, size_t n_images_, const uint64_t* timestamps_ = nullptr);

    void assign(ImagePtr* images_, size_t n_images_, const uint64_t* timestamps_ = nullptr);

    /**
     * @brief Create Image batch from another ImageBatch object
     *
     * @param other Reference to ImageBatch to copy
     */
    ImageBatch(ImageBatch&& other) noexcept;

    ImageBatch(const ImageBatch& other);

    ImageBatch& operator=(const ImageBatch& other);

    ~ImageBatch();

    /**
     * @brief Number of images in ImageBatch (use at your own risk)
     */
    size_t n_images = 0;

    /**
     * @brief Smart pointer to stored array of Images (direct use at your own risk)
     */
    ImagePtr* images = nullptr;

    /**
     * @brief Smart pointer to array of timestamps (direct use at your own risk)
     */
    uint64_t* timestamps = nullptr;
};

}  // namespace facesdk
