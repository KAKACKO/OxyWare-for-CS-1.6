/*
*	OXWARE developed by oxiKKK
*	Copyright (c) 2023
* 
*	This program is licensed under the MIT license. By downloading, copying, 
*	installing or using this software you agree to this license.
*
*	License Agreement
*
*	Permission is hereby granted, free of charge, to any person obtaining a 
*	copy of this software and associated documentation files (the "Software"), 
*	to deal in the Software without restriction, including without limitation 
*	the rights to use, copy, modify, merge, publish, distribute, sublicense, 
*	and/or sell copies of the Software, and to permit persons to whom the 
*	Software is furnished to do so, subject to the following conditions:
*
*	The above copyright notice and this permission notice shall be included 
*	in all copies or substantial portions of the Software. 
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
*	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
*	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
*	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
*	IN THE SOFTWARE.
*/

#ifndef VECTOR_H
#define VECTOR_H

#pragma once

#include <assert.h>
#include <cmath>


template <typename T>
class vector_2d
{
public:


	constexpr vector_2d() noexcept :
		x(0.0),
		y(0.0)
	{
	}


	constexpr ~vector_2d() noexcept
	{
	}


	constexpr vector_2d(T X, T Y) noexcept :
		x(X),
		y(Y)
	{
	}


	constexpr vector_2d(T p[2]) noexcept
	{
		if (p)
		{
			x = p[0];
			y = p[1];
		}
		else
		{
			assert(0);

			x = 0.0;
			y = 0.0;
		}
	};


	constexpr vector_2d(const vector_2d& in) noexcept :
		x(in.x),
		y(in.y)
	{
	}


	constexpr vector_2d(vector_2d& in) noexcept :
		x(in.x),
		y(in.y)
	{
	}

public:


	constexpr inline operator T*() noexcept
	{
		return &x;
	}


	constexpr inline operator const T*() const noexcept
	{
		return &x;
	}


	constexpr inline auto& operator=(const vector_2d& other) noexcept
	{
		x = other.x;
		y = other.y;

		return *this;
	}


	constexpr inline auto& operator=(T p[2]) noexcept
	{
		if (p)
		{
			x = p[0];
			y = p[1];
		}
		else
		{
			assert(0);

			x = y = 0.0;
		}

		return *this;
	}


	constexpr inline auto& operator=(T f) noexcept
	{
		x = y = f;

		return *this;
	}


	constexpr inline auto& operator+=(const vector_2d& other) noexcept
	{
		x += other.x;
		y += other.y;

		return *this;
	}


	constexpr inline auto& operator+=(T p[2]) noexcept
	{
		if (p)
		{
			x += p[0];
			y += p[1];
		}
		else
			assert(0);

		return *this;
	}


	constexpr inline auto& operator+=(T f) noexcept
	{
		x += f;
		y += f;

		return *this;
	}


	constexpr inline auto& operator-=(const vector_2d& other) noexcept
	{
		x -= other.x;
		y -= other.y;

		return *this;
	}


	constexpr inline auto& operator-=(T p[2]) noexcept
	{
		if (p)
		{
			x -= p[0];
			y -= p[1];
		}
		else
			assert(0);

		return *this;
	}


	constexpr inline auto& operator-=(T f) noexcept
	{
		x -= f;
		y -= f;

		return *this;
	}


	constexpr inline auto& operator*=(const vector_2d& other) noexcept
	{
		x *= other.x;
		y *= other.y;

		return *this;
	}


	constexpr inline auto& operator*=(T p[2]) noexcept
	{
		if (p)
		{
			x *= p[0];
			y *= p[1];
		}
		else
			assert(0);

		return *this;
	}


	constexpr inline auto& operator*=(T f) noexcept
	{
		x *= f;
		y *= f;

		return *this;
	}


	constexpr inline auto& operator/=(const vector_2d& other) noexcept
	{
		if (other.x != 0.0 && other.y != 0.0)
		{
			x /= other.x;
			y /= other.y;
		}

		return *this;
	}


	constexpr inline auto& operator/=(T p[2]) noexcept
	{
		if (p)
		{
			if (p[0] != 0.0 && p[1] != 0.0)
			{
				x /= p[0];
				y /= p[1];
			}
		}
		else
			assert(0);

		return *this;
	}


	constexpr inline auto& operator/=(T f) noexcept
	{
		if (f != 0.0)
		{
			x /= f;
			y /= f;
		}

		return *this;
	}


	constexpr inline auto operator+(const vector_2d& other) const noexcept
	{
		return vector_2d(x + other.x, y + other.y);
	}


	constexpr inline auto operator+(T p[2]) const noexcept
	{
		if (p)
			return vector_2d(x + p[0], y + p[1]);

		assert(0);
		return *this;
	}


	constexpr inline auto operator+(T f) const noexcept
	{
		return vector_2d(x + f, y + f);
	}


	constexpr inline auto operator-(const vector_2d& other) const noexcept
	{
		return vector_2d(x - other.x, y - other.y);
	}


	constexpr inline auto operator-(T p[2]) const noexcept
	{
		if (p)
			return vector_2d(x - p[0], y - p[1]);

		assert(0);
		return *this;
	}


	constexpr inline auto operator-(T f) const noexcept
	{
		return vector_2d(x - f, y - f);
	}


	constexpr inline auto operator-() const noexcept
	{
		return vector_2d(-x, -y);
	}


	constexpr inline auto operator*(const vector_2d& other) const noexcept
	{
		return vector_2d(x * other.x, y * other.y);
	}


	constexpr inline auto operator*(T p[2]) const noexcept
	{
		if (p)
			return vector_2d(x * p[0], y * p[1]);

		assert(0);
		return *this;
	}


	constexpr inline auto operator*(T f) const noexcept
	{
		return vector_2d(x * f, y * f);
	}


	constexpr inline auto operator/(const vector_2d& other) const noexcept
	{
		if (other.x != 0.0 && other.y != 0.0)
			return vector_2d(x / other.x, y / other.y);

		return *this;
	}


	constexpr inline auto operator/(T p[2]) const noexcept
	{
		if (p)
		{
			if (p[0] != 0.0 && p[1] != 0.0)
				return vector_2d(x / p[0], y / p[1]);
		}
		else
			assert(0);

		return *this;
	}


	constexpr inline auto operator/(T f) const noexcept
	{
		if (f != 0.0)
			return vector_2d(x / f, y / f);

		return *this;
	}


	constexpr inline auto& operator[](int i) const noexcept
	{
		if (i >= 0 && i < 3)
			return ((T*)this)[i];

		assert("Bad Vector2D index");

		return ((T*)this)[0];
	}


	constexpr inline bool operator!() const noexcept
	{
		return x == 0.0 && y == 0.0;
	}


	constexpr inline bool operator==(const vector_2d& other) const noexcept
	{
		return x == other.x && y == other.y;
	}


	constexpr inline bool operator!=(const vector_2d& other) const noexcept
	{
		return x != other.x || y != other.y;
	}


	constexpr inline bool operator<(const vector_2d& other) const noexcept
	{
		return x < other.x && y < other.y;
	}


	constexpr inline bool operator>(const vector_2d& other) const noexcept
	{
		return x > other.x && y > other.y;
	}

public:


	constexpr inline bool IsZero() const noexcept
	{
		return x == 0.0 && y == 0.0;
	}


	constexpr inline auto Base() noexcept
	{
		return &x;
	}


	constexpr inline auto Base() const noexcept
	{
		return &x;
	}


	constexpr inline auto& Clear() noexcept
	{
		x = y = 0.0;

		return *this;
	}


	constexpr inline auto& Negate() noexcept
	{
		x = -x;
		y = -y;

		return *this;
	}


	constexpr inline auto Dot(const vector_2d& other) const noexcept
	{
		return x * other.x + y * other.y;
	}


	constexpr inline auto LengthSqr() const noexcept
	{
		return x * x + y * y;
	}


	constexpr inline auto& CrossProduct(const vector_2d& a, const vector_2d& b) noexcept
	{
		x = (a.y * b.z) - (a.z * b.y);
		y = (a.z * b.x) - (a.x * b.z);

		return *this;
	}


	constexpr inline void Lerp(const vector_2d& a, const vector_2d& b, float t)
	{
		x = a.x + (b.x - a.x) * t;
		y = a.y + (b.y - a.y) * t;
	}


	constexpr inline void MulAdd(const vector_2d& a, const vector_2d& b, float scalar)
	{
		x = a.x + b.x * scalar;
		y = a.y + b.y * scalar;
	}


	constexpr inline void CopyToArray(float* rgfl) const
	{
		rgfl[0] = x;
		rgfl[1] = y;
	}


	inline bool IsValid() const noexcept
	{
		return isfinite(x) && isfinite(y);
	}


	inline auto Length() const noexcept
	{
		return static_cast<T>(sqrt(LengthSqr()));
	}


	inline auto Distance(const vector_2d& ToVector) const noexcept
	{
		return (ToVector - *this).Length();
	}


	inline auto Normalize() const noexcept
	{
		T flLen = Length();

		if (flLen == 0.0)
			return vector_2d(0.0, 0.0);

		flLen = 1.0 / flLen;
		return vector_2d(x * flLen, y * flLen);
	}


	inline auto AngleBetween(vector_2d& other) const noexcept
	{
		other.Normalize();
		Normalize();

		return static_cast<T>(std::acos(Dot(other)) * static_cast<T>(180.0 / 3.14159265358979323846));
	}

public:
	T x, y;
};


template <typename T>
constexpr inline vector_2d<T> operator*(float p, const vector_2d<T>& v)
{
	return v * p;
};

using Vector2D = vector_2d<float>;


template <typename T>
class vector_3d
{
public:


	constexpr vector_3d() noexcept :
		x(0.0),
		y(0.0),
		z(0.0)
	{
	}


	constexpr ~vector_3d() noexcept
	{
	}


	constexpr vector_3d(T X, T Y, T Z) noexcept :
		x(X),
		y(Y),
		z(Z)
	{}


	constexpr vector_3d(T p[3]) noexcept
	{
		if (p)
		{
			x = p[0];
			y = p[1];
			z = p[2];
		}
		else
		{
			x = 0.0;
			y = 0.0;
			z = 0.0;
		}
	};


	constexpr vector_3d(const vector_3d& in) noexcept :
		x(in.x),
		y(in.y),
		z(in.z)
	{
	}


	constexpr vector_3d(vector_3d& in) noexcept :
		x(in.x),
		y(in.y),
		z(in.z)
	{
	}


	constexpr vector_3d(vector_3d&& in) noexcept :
		x(in.x),
		y(in.y),
		z(in.z)
	{}

public:


	constexpr inline operator T*() noexcept
	{
		return &x;
	}


	constexpr inline operator const T*() const noexcept
	{
		return &x;
	}


	constexpr inline auto& operator=(const vector_3d& other) noexcept
	{
		x = other.x;
		y = other.y;
		z = other.z;

		return *this;
	}


	constexpr inline auto& operator=(T p[3]) noexcept
	{
		if (p)
		{
			x = p[0];
			y = p[1];
			z = p[2];
		}
		else
		{
			assert(0);

			x = y = z = 0.0;
		}

		return *this;
	}


	constexpr inline auto& operator=(T f) noexcept
	{
		x = y = z = f;

		return *this;
	}


	constexpr inline auto& operator+=(const vector_3d& other) noexcept
	{
		x += other.x;
		y += other.y;
		z += other.z;

		return *this;
	}


	constexpr inline auto& operator+=(T p[3]) noexcept
	{
		if (p)
		{
			x += p[0];
			y += p[1];
			z += p[2];
		}
		else
			assert(0);

		return *this;
	}


	constexpr inline auto& operator+=(T f) noexcept
	{
		x += f;
		y += f;
		z += f;

		return *this;
	}


	constexpr inline auto& operator-=(const vector_3d& other) noexcept
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;

		return *this;
	}


	constexpr inline auto& operator-=(T p[3]) noexcept
	{
		if (p)
		{
			x -= p[0];
			y -= p[1];
			z -= p[2];
		}
		else
			assert(0);

		return *this;
	}


	constexpr inline auto& operator-=(T f) noexcept
	{
		x -= f;
		y -= f;
		z -= f;

		return *this;
	}


	constexpr inline auto& operator*=(const vector_3d& other) noexcept
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;

		return *this;
	}


	constexpr inline auto& operator*=(T p[3]) noexcept
	{
		if (p)
		{
			x *= p[0];
			y *= p[1];
			z *= p[2];
		}
		else
			assert(0);

		return *this;
	}


	constexpr inline auto& operator*=(T f) noexcept
	{
		x *= f;
		y *= f;
		z *= f;

		return *this;
	}


	constexpr inline auto& operator/=(const vector_3d& other) noexcept
	{
		if (other.x != 0.0 && other.y != 0.0 && other.z != 0.0)
		{
			x /= other.x;
			y /= other.y;
			z /= other.z;
		}

		return *this;
	}


	constexpr inline auto& operator/=(T p[3]) noexcept
	{
		if (p)
		{
			if (p[0] != 0.0 && p[1] != 0.0 && p[2] != 0.0)
			{
				x /= p[0];
				y /= p[1];
				z /= p[2];
			}
		}
		else
			assert(0);

		return *this;
	}


	constexpr inline auto& operator/=(T f) noexcept
	{
		if (f != 0.0)
		{
			x /= f;
			y /= f;
			z /= f;
		}

		return *this;
	}


	constexpr inline auto operator+(const vector_3d& other) const noexcept
	{
		return vector_3d(x + other.x, y + other.y, z + other.z);
	}


	constexpr inline auto operator+(T p[3]) const noexcept
	{
		if (p)
			return vector_3d(x + p[0], y + p[1], z + p[2]);

		assert(0);
		return *this;
	}


	constexpr inline auto operator+(T f) const noexcept
	{
		return vector_3d(x + f, y + f, z + f);
	}


	constexpr inline auto operator-(const vector_3d& other) const noexcept
	{
		return vector_3d(x - other.x, y - other.y, z - other.z);
	}


	constexpr inline auto operator-(T p[3]) const noexcept
	{
		if (p)
			return vector_3d(x - p[0], y - p[1], z - p[2]);

		assert(0);
		return *this;
	}


	constexpr inline auto operator-(T f) const noexcept
	{
		return vector_3d(x - f, y - f, z - f);
	}


	constexpr inline auto operator-() const noexcept
	{
		return vector_3d(-x, -y, -z);
	}


	constexpr inline auto operator*(const vector_3d& other) const noexcept
	{
		return vector_3d(x * other.x, y * other.y, z * other.z);
	}


	constexpr inline auto operator*(T p[3]) const noexcept
	{
		if (p)
			return vector_3d(x * p[0], y * p[1], z * p[2]);

		assert(0);
		return *this;
	}


	constexpr inline auto operator*(T f) const noexcept
	{
		return vector_3d(x * f, y * f, z * f);
	}


	constexpr inline auto operator/(const vector_3d& other) const noexcept
	{
		if (other.x != 0.0 && other.y != 0.0 && other.z != 0.0)
			return vector_3d(x / other.x, y / other.y, z / other.z);

		return *this;
	}


	constexpr inline auto operator/(T p[3]) const noexcept
	{
		if (p)
		{
			if (p[0] != 0.0 && p[1] != 0.0 && p[2] != 0.0)
				return vector_3d(x / p[0], y / p[1], z / p[2]);
		}
		else
			assert(0);

		return *this;
	}


	constexpr inline auto operator/(T f) const noexcept
	{
		if (f != 0.0)
			return vector_3d(x / f, y / f, z / f);

		return *this;
	}


	constexpr inline auto& operator[](int i) const noexcept
	{
		if (i >= 0 && i < 3)
			return ((T*)this)[i];

		assert(0);

		return ((T*)this)[0];
	}


	constexpr inline bool operator!() const noexcept
	{
		return x == 0.0 && y == 0.0 && z == 0.0;
	}


	constexpr inline bool operator==(const vector_3d& other) const noexcept
	{
		return x == other.x && y == other.y && z == other.z;
	}


	constexpr inline bool operator!=(const vector_3d& other) const noexcept
	{
		return x != other.x || y != other.y || z != other.z;
	}


	constexpr inline bool operator<(const vector_3d& other) const noexcept
	{
		return x < other.x && y < other.y && z < other.z;
	}


	constexpr inline bool operator>(const vector_3d& other) const noexcept
	{
		return x > other.x && y > other.y && z > other.z;
	}

public:


	constexpr inline bool IsZero() const noexcept
	{
		return x == 0.0 && y == 0.0 && z == 0.0;
	}


	constexpr inline bool IsZero2D() const noexcept
	{
		return x == 0.0 && y == 0.0;
	}


	constexpr inline auto Base() noexcept
	{
		return &x;
	}


	constexpr inline auto Base() const noexcept
	{
		return &x;
	}


	constexpr inline auto& Clear() noexcept
	{
		x = y = z = 0.0;

		return *this;
	}


	constexpr inline auto& Negate() noexcept
	{
		x = -x;
		y = -y;
		z = -z;

		return *this;
	}


	constexpr inline auto Dot(const vector_3d& other) const noexcept
	{
		return x * other.x + y * other.y + z * other.z;
	}


	constexpr inline auto Dot2D(const vector_3d& other) const noexcept
	{
		return x * other.x + y * other.y;
	}


	constexpr inline auto LengthSqr() const noexcept
	{
		return x * x + y * y + z * z;
	}


	constexpr inline auto LengthSqr2D() const noexcept
	{
		return x * x + y * y;
	}


	constexpr inline auto& CrossProduct(const vector_3d& a, const vector_3d& b) noexcept
	{
		x = (a.y * b.z) - (a.z * b.y);
		y = (a.z * b.x) - (a.x * b.z);
		z = (a.x * b.y) - (a.y * b.x);

		return *this;
	}


	constexpr inline void Lerp(const vector_3d& a, const vector_3d& b, float t)
	{
		x = a.x + (b.x - a.x) * t;
		y = a.y + (b.y - a.y) * t;
		z = a.z + (b.z - a.z) * t;
	}


	constexpr inline void MulAdd(const vector_3d& a, const vector_3d& b, float scalar)
	{
		x = a.x + b.x * scalar;
		y = a.y + b.y * scalar;
		z = a.z + b.z * scalar;
	}


	constexpr inline Vector2D AsVector2D() const
	{
		return { x, y };
	}


	constexpr inline void CopyToArray(float* rgfl) const
	{
		rgfl[0] = x;
		rgfl[1] = y;
		rgfl[2] = z;
	}


	inline bool IsValid() const noexcept
	{
		return isfinite(x) && isfinite(y) && isfinite(z);
	}


	inline auto Length() const noexcept
	{
		return static_cast<T>(sqrt(LengthSqr()));
	}


	inline auto Length2D() const noexcept
	{
		return static_cast<T>(sqrt(LengthSqr2D()));
	}


	inline auto Distance(const vector_3d& ToVector) const noexcept
	{
		return (ToVector - *this).Length();
	}


	inline auto Distance2D(vector_3d& ToVector) const noexcept
	{
		return (ToVector - *this).Length2D();
	}


	inline auto Normalize() const noexcept
	{
		T flLen = Length();

		if (flLen == 0.0)
			return vector_3d(0.0, 0.0, 1.0);

		flLen = 1.0 / flLen;
		return vector_3d(x * flLen, y * flLen, z * flLen);
	}


	inline auto NormalizeInPlace() noexcept
	{
		T flLen = Length();

		if (flLen == 0)
		{
			x = y = 0;
			z = 1;
			return flLen;
		}

		const T flInvertedLen = 1 / flLen;

		x *= flInvertedLen;
		y *= flInvertedLen;
		z *= flInvertedLen;

		return flLen;
	}


	inline auto AngleBetween(vector_3d& other) const noexcept
	{
		other.Normalize();
		Normalize();

		return static_cast<T>(std::acos(Dot(other)) * static_cast<T>(180.0 / 3.14159265358979323846));
	}

public:
	T x, y, z;
};


template <typename T>
constexpr inline vector_3d<T> operator*(float fl, const vector_3d<T>& v)
{
	return v * fl;
};


template <typename T>
inline vector_3d<T> RotateVectorZ(const vector_3d<T>& a, int angle)
{
	vector_3d<T> new_vector;
	if (angle != 0.f)
	{
		T x_old = a.x;
		T y_old = a.y;
		T R_angle = (T)angle;

		new_vector.x = cos(R_angle) * x_old - sin(R_angle) * y_old;
		new_vector.y = sin(R_angle) * x_old + cos(R_angle) * y_old;
		new_vector.z = a.z;
	}
	return new_vector;
}

using Vector = vector_3d<float>;

#endif