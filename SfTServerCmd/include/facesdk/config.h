/* Copyright 2023 ID R&D Inc. All Rights Reserved. */

#pragma once

#if defined _WIN32

#if defined IDLF_API_IMPLEMENTATION
#define FACE_SDK_API __declspec(dllexport)
#else
#define FACE_SDK_API __declspec(dllimport)
#endif

#elif __GNUC__ >= 4 || defined __clang__
#define FACE_SDK_API __attribute__((visibility("default")))

#else
#define FACE_SDK_API

#endif
