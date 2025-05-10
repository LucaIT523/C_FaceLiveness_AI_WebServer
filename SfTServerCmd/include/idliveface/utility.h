// Copyright 2023 ID R&D Inc. All Rights Reserved.

//! \file

#pragma once

#include <idliveface/exception.h>

namespace idliveface {

//! \brief An optional value.
template<typename T>
class Optional {
public:
    Optional() : has_value_(false) {}
    Optional(T value) : has_value_(true), value_(value) {}  // NOLINT(google-explicit-constructor)

    bool has_value() const noexcept {
        return has_value_;
    }

    const T& value() const {
        if (has_value_) {
            return value_;
        }
        throw Exception("Optional is empty");
    }

    operator bool() const noexcept {  // NOLINT(google-explicit-constructor)
        return has_value();
    }

    const T& operator*() const {
        return value();
    }

    const T* operator->() const {
        return &value();
    }

private:
    bool has_value_;
    T value_;
};

}  // namespace idliveface
