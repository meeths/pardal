
#pragma once

// Created on 2019-03-15 by fmacias

#include <Base/DebugHelpers.h>
#include <String/String.h>

#include "Math/Quaternion.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"


namespace pdl
{

namespace StringCast
{
    template <typename T>
    String ToString(const T& value);

    // template specializations

    String ToString(const bool& value);
    String ToString(const int& value);
    String ToString(const unsigned int& value);
    String ToString(const float& value);
    String ToString(const String& value);

    String ToString(const Vector3& value);
    String ToString(const Vector4& value);
    String ToString(const Quaternion& value);


    template <typename T>
    T FromString(const String& value);

    template <>
    bool FromString(const String& valueStr);

    template <>
    int FromString(const String& valueStr);

    template <>
    unsigned int FromString(const String& valueStr);

    template <>
    float FromString(const String& valueStr);

    template <>
    Vector3 FromString(const String& valueStr);
    template <>
    Vector4 FromString(const String& valueStr);
    template <>
    Quaternion FromString(const String& valueStr);

    template <>
    String FromString(const String& valueStr);


    // return empty string if no conversion possible
    template <typename T>
    String ToString(const T& /* value */)
    {
        // Not convertible to string (no suitable ToString override) OR
        // Basic type not serializable (no jsonserializer BasicSerializer override) OR
        // Or compound type not meta-described via meta::registerMembers
        // Maybe forgot to run Component code generation script?
        pdlAssert(0);
        return String("##TOSTRING_NOT_IMPLEMENTED##");
    }

    template <typename T>
    T FromString(const String& /* value */)
    {
        return T();
    }
};

}

