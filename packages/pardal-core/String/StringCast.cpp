
#include <String/StringCast.h>
#include <String/StringUtils.h>
#include <string>

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
        return eastl::to_string(value);
    }

    String ToString(const unsigned int& value)
    {
        return eastl::to_string(value);
    }

    String ToString(const float& value)
    {
        return eastl::to_string(value);
    }

    String ToString(const String& value)
    {
        return value;
    }

    String ToString(const char* const& value)
    {
        return value;
    }

    String ToString(const Math::Vector3& value)
    {
        return StringUtils::StringFormat("(%f, %f, %f)", value.x, value.y, value.z);
    }

    String ToString(const Math::Vector4& value)
    {
        return StringUtils::StringFormat("(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
    }

    String ToString(const Math::Quaternion& value)
    {
        return StringUtils::StringFormat("(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
    }

    // From string

    template <>
    bool FromString(const StringView& valueStr)
    {
        return valueStr == "true";
    }

    template <>
    int FromString(const StringView& valueStr)
    {
        return std::stoi(valueStr.data());
    }

    template <>
    unsigned int FromString(const StringView& valueStr)
    {
        return std::stoul(valueStr.data());
    }

    template <>
    float FromString(const StringView& valueStr)
    {
        return std::stof(valueStr.data());
    }

    template <>
    Math::Vector3 FromString<Math::Vector3>(const StringView& valueStr)
    {
        Math::Vector3 ret;
        sscanf_s(valueStr.data(), "(%f, %f, %f)", &ret.x, &ret.y, &ret.z);
        return ret;
    }

    template <>
    Math::Vector4 FromString<Math::Vector4>(const StringView& valueStr)
    {
        Math::Vector4 ret;
        sscanf_s(valueStr.data(), "(%f, %f, %f, %f)", &ret.x, &ret.y, &ret.z, &ret.w);
        return ret;
    }

    template <>
    Math::Quaternion FromString<Math::Quaternion>(const StringView& valueStr)
    {
        Math::Quaternion ret;
        sscanf_s(valueStr.data(), "(%f, %f, %f, %f)", &ret.x, &ret.y, &ret.z, &ret.w);
        return ret;
    }

    template <>
    String FromString(const StringView& valueStr)
    {
        return String(valueStr);
    }
}
}

