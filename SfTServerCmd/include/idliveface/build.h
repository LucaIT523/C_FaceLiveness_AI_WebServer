// Copyright 2023 ID R&D Inc. All Rights Reserved.

#pragma once

#define IDLF_API __declspec(dllimport)

// clang-format off



//#if defined _WIN32
//#if defined IDLF_API_IMPLEMENTATION
//#define IDLF_API __declspec(dllexport)
//#else
//#define IDLF_API __declspec(dllimport)
//#define IDLF_INTERNAL_API __declspec(dllimport)
//#endif
//#elif __GNUC__ >= 4 || defined __clang__
//#define IDLF_API __attribute__((visibility("default")))
//#else
//#define IDLF_API
//#endif
//
//#if !defined(IDLF_INTERNAL_API)
//#if defined IDLF_INTERNAL_API_IMPLEMENTATION
//#define IDLF_INTERNAL_API IDLF_API
//#else
//#define IDLF_INTERNAL_API
//#endif
//#endif





// 
// clang-format on
