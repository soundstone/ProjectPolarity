//core.h
//Basic building blocks of the engine. 
#include "precision.h"

namespace PolarisEngine
{
	/*
		Holds a vector in 3D. Extra data members provides alignment in array.
	*/
	class Vector3
	{
	public:
		//holds the value along X axis
		real x;

		//holds the value along Y axis
		real y;

		//holds the value along the Z axis
		real z;

	private:
		//extra data member for alignment
		real pad;

	public:
		//By default, create a 0 vector3
		Vector3() : x(0), y(0), z(0) {}

		//create a vector3 via input parameters
		Vector3(const real x, const real y, const real z)
			: x(x), y(y), z(z) {}

		//Invert the vector
		void Invert()
		{
			x = -x;
			y = -y;
			z = -z;
		}

		//Get magnitude of vector3
		real Magnitude() const
		{
			return real_sqrt(x*x+y*y+z*z);
		}

		//Get the squared magnitude of vector3
		real SquareMagnitude() const
		{
			return ( (x * x) + (y * y) + (z * z) );
		}

		//Turn non-zero vector into unit length vector
		void Normlize()
		{
			real length = Magnitude();
			if (length > 0)
				(*this) *= ((real) 1 / length);
		}

		//Multiples vector by scalar value
		void operator*=(const real value)
		{
			x *= value;
			y *= value;
			z *= value;
		}

		//returns copy of vector scaled to value
		Vector3 operator*(const real value) const
		{
			return Vector3(x * value, y * value, z * value);
		}

		//Adds the given vector to this vector
		void operator+=(const Vector3& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
		}

		//returns the value of the given vector added to this vector
		Vector3 operator+(const Vector3& v) const
		{
			return Vector3(x+v.x, y+v.y, z+v.z);
		}

		//Subtracts the given vector from this vector
		void operator-=(const Vector3& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
		}

		//returns the value of the given vector subtracted from this vector
		Vector3 operator-(const Vector3& v) const
		{
			return Vector3(x-v.x, y-v.y, z-v.z);
		}

		//Adds the given vector to this vector, scaled by the given amount
		void AddScaledVector(const Vector3& vector, real scale)
		{
			x += vector.x * scale;
			y += vector.y * scale;
			z += vector.z * scale;
		}

		//Calculate and return a component-wise product of this vector with the given vector
		Vector3 ComponentProduct(const Vector3 &vec) const
		{
			return Vector3(x * vec.x, y * vec.y, z * vec.z);
		}

		//performs a component-wise product with the given vector and sets this vector to its result
		void ComponentProductUpdate(const Vector3 &vec)
		{
			x *= vec.x;
			y *= vec.y;
			z *= vec.z;
		}

		//Calculate and return the scalar product of this vector with the given vector
		real ScalarProduct(const Vector3 &vec) const
		{
			return x*vec.x + y*vec.y + z*vec.z;
		}

		//Calculate and return the scalar product of this vector with the given vector
		real operator*(const Vector3 &vec) const
		{
			return x * vec.x + y * vec.y + z * vec.z;
		}

		//Calculates and returns the vector product of this vector with the given vector
		Vector3 VectorProduct(const Vector3 &vec) const
		{
			return Vector3(y*vec.z - z*vec.y,
						   z*vec.x - x*vec.z,
						   x*vec.y - y*vec.x);
		}

		//Updates this vector to be the vector product of its current value and the given vector
		void operator %=(const Vector3& vec)
		{
			*this = VectorProduct(vec);
		}

		//Calculate and return the vector product of this vector with the given vector
		Vector3 operator%(const Vector3 &vec) const
		{
			return Vector3(y*vec.z - z*vec.y,
						   z*vec.x - x*vec.z,
						   x*vec.y - y*vec.z);
		}
	};
};