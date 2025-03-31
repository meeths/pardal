#pragma once
#include <Math/Vector2.h>
#include <Math/Vector2i.h>

namespace pdl
{
namespace Math
{
    class Rectangle
    {

    public:
        Vector2 GetOrigin() const { return mMin; }
        Vector2 GetDimensions() const { return mMax - mMin; }

        Vector2 mMin = {};
        Vector2 mMax = {};

        friend bool operator ==(const Rectangle& a, const Rectangle& b)
        {
            return a.mMax == b.mMax && a.mMin == b.mMin;
        }

        friend bool operator !=(const Rectangle& a, const Rectangle& b)
        {
            return !(a == b);
        }

        bool IsPointInsideRect(const Vector2& _point) const
        {
            return
                mMin.x <= _point.x &&
                mMin.y <= _point.y &&
                mMax.x >= _point.x &&
                mMax.y >= _point.y;
    }

    };

    class Rectanglei
    {

    public:
        Vector2i GetOrigin() const { return mMin; }
        Vector2i GetDimensions() const { return mMax - mMin; }

        Vector2i mMin = {};
        Vector2i mMax = {};

        friend bool operator ==(const Rectanglei& a, const Rectanglei& b)
        {
            return a.mMax == b.mMax && a.mMin == b.mMin;
        }

        friend bool operator !=(const Rectanglei& a, const Rectanglei& b)
        {
            return !(a == b);
        }
    };
}
}