#ifndef GIN_GEOMETRY_H
#define GIN_GEOMETRY_H

#include <cmath>

namespace gin
{
    template <typename T> struct Vec2
    {
        T x;
        T y;

        Vec2() : x(0), y(0) {}
        Vec2(T x, T y) : x(x), y(y) {}
        Vec2(const Vec2& p) : x(p.x), y(p.y) {}

        ~Vec2(){}

        static Vec2 zero() { return Vec2(0, 0); }
        static Vec2 north() { return Vec2(0, -1); }
        static Vec2 north_east() { return Vec2(1, -1); }
        static Vec2 north_west() { return Vec2(-1, -1); }
        static Vec2 south() { return Vec2(0, 1); }
        static Vec2 south_east() { return Vec2(1, 1); }
        static Vec2 south_west() { return Vec2(-1, 1); }
        static Vec2 east() { return Vec2(1, 0); }
        static Vec2 west() { return Vec2(-1, 0); }

        static bool is_zero(const Vec2& v) { return v.x == 0 && v.y == 0; }
        static bool is_north(const Vec2& v) { return v.x == 0 && v.y == -1; }
        static bool is_east(const Vec2& v) { return v.x == 1 && v.y == 0; }
        static bool is_south(const Vec2& v) { return v.x == 0 && v.y == 1; }
        static bool is_west(const Vec2& v) { return v.x == -1 && v.y == 0; }

        void operator=(const Vec2& rhs)
        {
            x = rhs.x;
            y = rhs.y;
        }

        bool operator==(const Vec2& rhs) const { return (x == rhs.x && y == rhs.y); }
        bool operator!=(const Vec2& rhs) { return (x != rhs.x || y != rhs.y); }

        Vec2 operator+(const Vec2& rhs) { return Vec2(x + rhs.x, y + rhs.y); }

        Vec2& operator+=(const Vec2& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        Vec2 operator-(const Vec2& rhs) { return Vec2(x - rhs.x, y - rhs.y); }

        Vec2 operator*(const T& rhs) { return Vec2(x * rhs, y * rhs); }

        bool operator<(const Vec2& rhs) const { return (x < rhs.x) || (x == rhs.x && y < rhs.y); }
    };

    using Vec2f = Vec2<float>;
    using Vec2i = Vec2<int>;

    template <typename T> struct Rect
    {
        T x;
        T y;
        T w;
        T h;

        Rect()
        {
            x = 0;
            y = 0;
            w = 0;
            h = 0;
        }

        Rect(T x, T y, T w, T h)
        {
            this->x = x;
            this->y = y;
            this->w = w;
            this->h = h;
        }

        Rect(const Rect& r)
        {
            this->x = r.x;
            this->y = r.y;
            this->w = r.w;
            this->h = r.h;
        }

        void operator=(const Rect& rhs)
        {
            x = rhs.x;
            y = rhs.y;
            w = rhs.w;
            h = rhs.h;
        }

        bool operator==(const Rect& rhs) { return (x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h); }

        bool operator!=(const Rect& rhs) { return (x != rhs.x || y != rhs.y || w != rhs.w || h != rhs.h); }

        T top() { return y; }
        T left() { return x; }
        T bottom() { return y + h; }
        T right() { return x + w; }
    };

    using Rectf = Rect<float>;
    using Recti = Rect<int>;
} // namespace gin

#endif
