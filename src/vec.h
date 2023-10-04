#ifndef VEC_H
#define VEC_H

class Vec2
{
public:
	float x, y;

public:
	Vec2() : x(0.0f), y(0.0f) {}
	Vec2(float _x, float _y) : x(_x), y(_y) {}
	Vec2(const Vec2 & v) : x(v.x), y(v.y) {}
};

class Vec3
{
public:
	float x, y, z;

public:
	Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
	Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	Vec3(const Vec3 & v) : x(v.x), y(v.y), z(v.z) {}

public:
	Vec3 normalize()
	{
		float l = sqrtf(x * x + y * y + z * z);
		if (l != 0.0f)
		{
			x /= l;
			y /= l;
			z /= l;
		}
		return *this;
	}

	Vec3 cross(const Vec3 & vec) const
	{
		return Vec3(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);
	}

	float dot(const Vec3 & vec) const
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}

	Vec3 interp(const Vec3 & vec, float coeff) const
	{
		float coeff_1 = 1.0f - coeff;
		return Vec3(x * coeff_1 + vec.x * coeff, y * coeff_1 + vec.y * coeff, z * coeff_1 + vec.z * coeff);
	}

	float squaredDist() const
	{
		return x * x + y * y + z * z;
	}

	friend Vec3 operator + (const Vec3 & u, const Vec3 & v)
	{
		return Vec3(u.x + v.x, u.y + v.y, u.z + v.z);
	}

	friend Vec3 operator - (const Vec3 & u, const Vec3 & v)
	{
		return Vec3(u.x - v.x, u.y - v.y, u.z - v.z);
	}

	friend Vec3 operator * (const Vec3 & u, const Vec3 & v)
	{
		return Vec3(u.x * v.x, u.y * v.y, u.z * v.z);
	}

	friend Vec3 operator / (const Vec3 & u, const Vec3 & v)
	{
		return Vec3(u.x / v.x, u.y / v.y, u.z / v.z);
	}

	friend bool operator == (const Vec3 & u, const Vec3 & v)
	{
		return u.x == v.x && u.y == v.y && u.z == v.z;
	}

	friend bool operator != (const Vec3 & u, const Vec3 & v)
	{
		return u.x != v.x || u.y != v.y || u.z != v.z;
	}

public:
	static const Vec3 Zero()
	{
		return Vec3();
	}
};

class Vec4
{
public:
	float x, y, z, w;

public:
	Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	Vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	Vec4(const Vec4 & v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

public:
	float dot(const Vec4 & vec) const
	{
		return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
	}

	Vec4 interp(const Vec4 & vec, float coeff) const
	{
		float coeff_1 = 1.0f - coeff;
		return Vec4(x * coeff_1 + vec.x * coeff, y * coeff_1 + vec.y * coeff, z * coeff_1 + vec.z * coeff, w * coeff_1 + vec.w * coeff);
	}

	friend Vec4 operator + (const Vec4 & u, const Vec4 & v)
	{
		return Vec4(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w);
	}

	friend Vec4 operator - (const Vec4 & u, const Vec4 & v)
	{
		return Vec4(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w);
	}

	friend Vec4 operator * (const Vec4 & u, const Vec4 & v)
	{
		return Vec4(u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w);
	}

	friend Vec4 operator / (const Vec4 & u, const Vec4 & v)
	{
		return Vec4(u.x / v.x, u.y / v.y, u.z / v.z, u.w / v.w);
	}
};

#endif // of VEC_H
