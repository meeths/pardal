
#include <String/StringCast.h>
#include <String/StringUtils.h>

// Created on 2019-03-15 by fmacias

namespace pdl
{
namespace StringCast
{
    String ToString(const bool& value)
    {
        return value ? "true" : "false";
    }

    String ToString(const int& value)
    {
        return std::to_string(value);
    }

    String ToString(const unsigned int& value)
    {
        return std::to_string(value);
    }

    String ToString(const float& value)
    {
        return std::to_string(value);
    }

    String ToString(const String& value)
    {
        return value;
    }

    String ToString(const Vector3& value)
    {
        return StringUtils::StringFormat("(%f, %f, %f)", value.x, value.y, value.z);
    }

    String ToString(const Vector4& value)
    {
        return StringUtils::StringFormat("(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
    }

    String ToString(const Quaternion& value)
    {
        return StringUtils::StringFormat("(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
    }

    // From string

    template <>
    bool FromString(const String& valueStr)
    {
        return valueStr == "true";
    }

    template <>
    int FromString(const String& valueStr)
    {
        return std::stoi(valueStr);
    }

    template <>
    unsigned int FromString(const String& valueStr)
    {
        return std::stoul(valueStr);
    }

    template <>
    float FromString(const String& valueStr)
    {
        return std::stof(valueStr);
    }

    template <>
    Vector3 FromString<Vector3>(const String& valueStr)
    {
        Vector3 ret;
        sscanf_s(valueStr.c_str(), "(%f, %f, %f)", &ret.x, &ret.y, &ret.z);
        return ret;
    }

    template <>
    Vector4 FromString<Vector4>(const String& valueStr)
    {
        Vector4 ret;
        sscanf_s(valueStr.c_str(), "(%f, %f, %f, %f)", &ret.x, &ret.y, &ret.z, &ret.w);
        return ret;
    }

    template <>
    Quaternion FromString<Quaternion>(const String& valueStr)
    {
        Quaternion ret;
        sscanf_s(valueStr.c_str(), "(%f, %f, %f, %f)", &ret.x, &ret.y, &ret.z, &ret.w);
        return ret;
    }

    template <>
    String FromString(const String& valueStr)
    {
        return valueStr;
    }
}
}

