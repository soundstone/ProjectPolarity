//core.h
//Basic building blocks of the engine. Holds Vector3, Quaternion, Matrix3 and Matrix4 structures
#include <math.h>

#ifndef POLARISENGINE_CORE_H
#define POLARISENGINE_CORE_H

#include "precision.h"


namespace PolarisEngine
{
	/* 
		Holds the value for energy under which a body will be put to sleep. This is a global value for
		the whole solution. By default it is 0.1, which is fine for simulation when graity is about 20 units per second squared, 
		masses are about one, and other forces are around that of gravity. It may need tweaking if simulation is drastically different. 
	*/
	extern real sleepEpsilon;

	/*
		Sets the current sleep epsilon value: the kinetic energy under which a body may be put to sleep. Bodies are put to sleep if
		they appear to have a stable kinetic energy less than this value. May need reduced. This value is global, all bodies use it.
	*/
	void SetSleepEpsilon(real value);

	//Gets the current value of the sleep epsilon parameter.
	real GetSleepEpsilon();


#pragma region Vector 3
	/*Holds a vector in 3D. Extra data members provides alignment in array.*/
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

#pragma region const static vector3s
		
		const static Vector3 GRAVITY;
		const static Vector3 HIGH_GRAVITY;
		const static Vector3 UP;
		const static Vector3 RIGHT;
		const static Vector3 OUT_OF_SCREEN;
		const static Vector3 X;
		const static Vector3 Y;
		const static Vector3 Z;

#pragma endregion

		//overload index operator for vector3 const
		real operator[](unsigned i) const
		{
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}

		//overload index operator for vector3
		real& operator[](unsigned i)
		{
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}

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

		//Limits the size of the vector to the given maximum.
		void Trim(real size)
		{
			if (SquareMagnitude() > size * size)
			{
				Normlize();
				x *= size;
				y *= size;
				z *= size;
			}
		}

		//Returns the normalized version of the vector
		Vector3 Unit() const
		{
			Vector3 result = *this;
			result.Normlize();
			return result;
		}

		//Checks if the two vectors are identical
		bool operator==(const Vector3 &other) const
		{
			return x == other.x &&
				y == other.y &&
				z == other.z;
		}

		//Checks if the two vectors have non-identical components.
		bool operator!=(const Vector3 &other) const
		{
			return !(*this == other);
		}

		//Checks if this vector is component by component less than the other
		bool operator<(const Vector3 &other) const
		{
			return x < other.x && y < other.y && z < other.z;
		}

		//Checks is this vector is component by component more than the other.
		bool operator>(const Vector3 &other) const
		{
			return x > other.x && y > other.y && z > other.z;
		}

		//Checks if this vector is compoent by component less than other
		bool operator<=(const Vector3 &other) const
		{
			return x <= other.x && y <= other.y && z <= other.z;
		}

		//Checks if this vector is component by componentn more than other.
		bool operator>=(const Vector3 &other) const
		{
			return x >= other.x && y >= other.y && z >= other.z;
		}

		//Zero all the components of the vector
		void Clear()
		{
			x = y = z = 0;
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

#pragma endregion

#pragma region Quaternion
	class Quaternion
	{
	public:
		union 
		{
			struct 
			{
				//Holds the real component of the quaternion
				real r;

				//Holds the first complex component of the quaternion
				real i;

				//Holds the second complex component of the quaternion
				real j;

				//Holds the third complex component of the quaternion
				real k;
			};

			//Holds the quaternion data in array form
			real data[4];
		};

		//Default Ctor creates a quaternion representing a zero rotation
		Quaternion() : r(1), i(0), j(0), k(0) {}

		//The explicit Ctor creates a quaternion with the given components
		//The given orientation does not need to be normalized and can be zero.
		Quaternion(const real r, const real i, const real j, const real k)
			: r(r), i(i), j(j), k(k)
		{
		}

		//Normalizes the quaternion to unit length, making it a valid orientation 
		void Normalize()
		{
			real d = r*r+i*i+j*j+k*k;

			//Check for zero length quaternion and use the no-rotation quaterion in that case
			if (d < real_epsilon) 
			{
				r = 1;
				return;
			}

			d = ( (real)1.0 / real_sqrt(d) );
			r *= d;
			i *= d;
			j *= d;
			k *= d;
		}

		//Multiplies the quaternion by the given quaternion
		void operator *=(const Quaternion &multiplier)
		{
			Quaternion q = *this;
			r = q.r*multiplier.r - q.i*multiplier.i -
				q.j*multiplier.j - q.k*multiplier.k;
			i = q.r*multiplier.i + q.i*multiplier.r +
				q.j*multiplier.k - q.k*multiplier.j;
			j = q.r*multiplier.j + q.j*multiplier.r +
				q.k*multiplier.i - q.i*multiplier.k;
			k = q.r*multiplier.k + q.k*multiplier.r + 
				q.i*multiplier.j - q.j*multiplier.i;
		}

		//Adds the given vector to this quaternion, scaled by the given amount.
		//Used to update the orientation quaternion by a rotation and time.
		void AddScaledVector(const Vector3 &vec, real scale)
		{
			Quaternion q(0,
				vec.x * scale,
				vec.y * scale,
				vec.z * scale);
			q *= *this;
			r *= q.r * ((real)0.5);
			i += q.i * ((real)0.5);
			j += q.j * ((real)0.5);
			k += q.k * ((real)0.5);
		}

		void RotateByVector(const Vector3 &vec)
		{
			Quaternion q(0, vec.x, vec.y, vec.z);
			(*this) *= q;
		}
	};

#pragma endregion

#pragma region Matrix4

	//Holds a transform matrix, consisting of a rotation matrix and a position.
	class Matrix4
	{
	public:
		//Holds the transform matrix data in array form
		real data[12];

		//Creates an indentity matrix
		Matrix4()
		{
			data[1] = data[2] = data[3] = data[4] = data[6] =
				data[7] = data[8] = data[9] = data[11] = 0;
			data[0] = data[5] = data[10] = 1;
		}

		//Sets the matrix to be a diagonal matrix with the given coefficients
		void SetDiagonal(real a, real b, real c)
		{
			data[0] = a;
			data[5] = b;
			data[10] = c;
		}

		//Returns a matrix which is this matrix multiplied by the given other matrix
		Matrix4 operator*(const Matrix4 &other) const
		{
			Matrix4 result;
			result.data[0] = (other.data[0]* data[0]) + (other.data[4]*data[1]) + (other.data[8] *data[2]);
			result.data[4] = (other.data[0]*data[4])  + (other.data[4]*data[5]) + (other.data[8] * data[6]);
			result.data[8] = (other.data[0]* data[8]) + (other.data[4]*data[9]) + (other.data[8] *data[10]);

			result.data[1] = (other.data[1]*data[0]) +  (other.data[5]*data[1]) + (other.data[9]*data[2]);
			result.data[5] = (other.data[1]*data[4]) +  (other.data[5]*data[5]) + (other.data[9] * data[6]);
			result.data[9] = (other.data[1]* data[8]) + (other.data[5]*data[9]) + (other.data[9] *data[10]);

			result.data[2] =  (other.data[2]*data[0]) +  (other.data[6]*data[1]) + (other.data[10]*data[2]);
			result.data[6] =  (other.data[2]*data[4]) +  (other.data[6]*data[5]) + (other.data[10] * data[6]);
			result.data[10] = (other.data[2]* data[8]) + (other.data[6]*data[9]) + (other.data[10] *data[10]);

			result.data[3] =  (other.data[3]*data[0]) +  (other.data[7]*data[1]) + (other.data[11]*data[2]);
			result.data[7] =  (other.data[3]*data[4]) +  (other.data[7]*data[5]) + (other.data[11] * data[6]);
			result.data[11] = (other.data[3]* data[8]) + (other.data[7]*data[9]) + (other.data[11] *data[10]);

			return result;
		}

		//Transform the given vector by this matrix
		Vector3 operator*(const Vector3 &vec) const
		{
			return Vector3(
				vec.x * data[0] +
				vec.y * data[1] + 
				vec.z * data[2] + data[3],

				vec.x * data[4] + 
				vec.y * data[5] +
				vec.z * data[6] + data[7],

				vec.x * data[8] +
				vec.y * data[9] + 
				vec.z * data[10] + data[11]
			);
		}

		//Transform the given vector by this matrix
		Vector3 Transform(const Vector3 &vec) const
		{
			return (*this) * vec;
		}

		//Returns the determinant of the matrix
		real GetDeterminant() const;

		//Sets the matrix to be the inverse of the given matrix
		void SetInverse(const Matrix4 &m);

		//Returns a new matrix containing the inverse of this matrix
		Matrix4 inverse() const
		{
			Matrix4 result;
			result.SetInverse(*this);
			return result;
		}

		//Invert matrix
		void Invert()
		{
			SetInverse(*this);
		}

		//Transform the given direction vector by this matrix
		Vector3 TransformDirection(const Vector3 &vec) const
		{
			return Vector3(
				vec.x * data[0] +
				vec.y * data[1] +
				vec.z * data[2],

				vec.x * data[4] +
				vec.y * data[5] +
				vec.z * data[6],

				vec.x * data[8] +
				vec.y * data[9] +
				vec.z * data[10]
			);		
		}

		//Transform the given direction vector by the transformations inverse of this matrix
		Vector3 TransformInverseDirection(const Vector3 &vec) const
		{
			return Vector3(
				vec.x * data[0] +
				vec.y * data[4] +
				vec.z * data[8],

				vec.x * data[1] +
				vec.y * data[5] +
				vec.z * data[9],

				vec.x * data[2] +
				vec.y * data[6] +
				vec.z * data[10]
			);
		}

		//Transform the given vector by the transformations inverse of this matrix
		Vector3 TransformInverse(const Vector3 &vec) const
		{
			Vector3 temp = vec;
			temp.x -= data[3];
			temp.y -= data[7];
			temp.z -= data[11];

			return Vector3(
				temp.x * data[0] + 
				temp.y * data[4] +
				temp.z * data[8],

				temp.x * data[1] + 
				temp.y * data[5] + 
				temp.z * data[9],

				temp.x * data[2] +
				temp.y * data[6] +
				temp.z * data[10]
			);
		}

		//Gets a vector representing one axis (one column) in matrix
		Vector3 GetAxisVector(int i) const
		{
			return Vector3(data[i], data[i + 4], data[i + 8]);
		}

		//Sets this matrix to be the rotation matrix corresponding to the given quaternion
		void SetOrientationAndPos(const Quaternion &q, const Vector3 &pos)
		{
			data[0] = 1 - (2*q.j * q.j + 2*q.k *q.k);
			data[1] = 2*q.i * q.j + 2*q.k*q.r;
			data[2] = 2*q.i*q.k - 2*q.j *q.r;
			data[3] = pos.x;

			data[4]= 2*q.i*q.j - 2*q.k *q.r;
			data[5] = 1 - (2*q.i*q.i + 2*q.k*q.k);
			data[6] = 2*q.j*q.k + 2*q.i*q.r;
			data[7] = pos.y;

			data[8] = 2*q.i*q.k + 2*q.j*q.r;
			data[9] = 2*q.j*q.k - 2*q.i*q.r;
			data[10] = 1 - (2*q.i*q.i + 2*q.j*q.j);
			data[11] = pos.z;
		}

		//Fills the given array with this transform matrix, so it is usuable as transform matrix.
		void FillGLArray(float a[16]) const
		{
			a[0] = (float)data[0];
			a[1] = (float)data[4];
			a[2] = (float)data[8];
			a[3] = (float)0;

			a[4] = (float)data[1];
			a[5] = (float)data[5];
			a[6] = (float)data[9];
			a[7] = (float)0;

			a[8] = (float)data[2];
			a[9] = (float)data[6];
			a[10] = (float)data[10];
			a[11] = (float)0;

			a[12] = (float)data[3];
			a[13] = (float)data[7];
			a[14] = (float)data[11];
			a[15] = (float)1;
		}
	};

#pragma endregion

#pragma region Matrix3

	class Matrix3
	{
	public:
		//Holds the tensor matrix data in array form
		real data[9];

		//Default Matrix
		Matrix3()
		{
			data[0] = data[1] = data[2] = data[3] = data[4] = data[5] =
				data[6] = data[7] = data[8] = 0;
		}

		//Create matrix with 3 given vectors for columns
		Matrix3(const Vector3 &one, const Vector3 &two,
			const Vector3 &three)
		{
			SetComponents(one, two, three);
		}

		//Create matrix with coefficients
		Matrix3(real c0, real c1, real c2, real c3, real c4, real c5,
			real c6, real c7, real c8)
		{
			data[0] = c0;
			data[1] = c1;
			data[2] = c2;
			data[3] = c3;
			data[4] = c4;
			data[5] = c5;
			data[6] = c6;
			data[7] = c7;
			data[8] = c8;
		}

		//Set matrix to be diagonal matrix with given values in leading diagonal
		void SetDiagonal(real a, real b, real c)
		{
			SetInertiaTensorCoeffs(a, b, c);
		}

		//Sets the value of matrix from inertia tensor
		void SetInertiaTensorCoeffs(real ix, real iy, real iz,
			real ixy=0, real ixz= 0, real iyz = 0)
		{
			data[0] = ix;
			data[1] = data[3] = -ixy;
			data[2] = data[6] = -ixz;
			data[4] = iy;
			data[5] = data[7] = -iyz;
			data[8] = iz;
		}

		//Sets the value of matrix as an inertia tensor of a rectangle. Block
		//is aligned with the body's coordinate system with the given axis half-sizes and mass.
		void SetBlockInertiaTensor(const Vector3 &half, real mass)
		{
			Vector3 squares = half.ComponentProduct(half);
			SetInertiaTensorCoeffs(0.3f*mass*(squares.y + squares.z),
				0.3f*mass*(squares.x + squares.z),
				0.3f*mass*(squares.x + squares.y));
		}

		//Sets the matrix to be a skew symmetric matrix based on given vector.
		void SetSkewSymmetric(const Vector3 vec)
		{
			data[0] = data[4] = data[8] = 0;
			data[1] = -vec.z;
			data[2] = vec.y;
			data[3] = vec.z;
			data[5] = -vec.x;
			data[6] = -vec.y;
			data[7] = vec.x;
		}

		//Sets the matrix values from the given components
		void SetComponents(const Vector3 &one, const Vector3 &two,
			const Vector3 &three)
		{
			data[0] = one.x;
			data[1] = two.x;
			data[2] = three.x;
			data[3] = one.y;
			data[4] = two.y;
			data[5] = three.y;
			data[6] = one.z;
			data[7] = two.z;
			data[8] = three.z;
		}

		//Transform the given vector by this matrix
		Vector3 operator*(const Vector3 &vec) const
		{
			return Vector3
			(
				vec.x * data[0] + vec.y * data[1] + vec.z * data[2],
				vec.x * data[3] + vec.y * data[4] + vec.z * data[5],
				vec.x * data[6] + vec.y * data[7] + vec.z * data[8]
			);
		}

		//Transform the given vector by this matrix
		Vector3 transform(const Vector3 &vec) const
		{
			return (*this) * vec;
		}

		Vector3 TransformTranspose(const Vector3 &vec) const
		{
			return Vector3
			(
				vec.x * data[0] + vec.y * data[3] + vec.z * data[6],
				vec.x * data[1] + vec.y * data[4] + vec.z * data[7],
				vec.x * data[2] + vec.y * data[5] + vec.z * data[8]
			);
		}

		//Gets a vector representing one row
		// i = row to return
		Vector3 GetRowVector(int i) const
		{
			return Vector3(data[i*3], data[i*3+1], data[i*3+2]);
		}

		//Get a vector representing one axis
		Vector3 GetAxisVector(int i) const
		{
			return Vector3(data[i], data[i + 3], data[i+6]);
		}

		//Sets the matrix to its inverse
		void SetInverse(const Matrix3 &m)
		{
			real t4 = m.data[0]*m.data[4];
			real t6 = m.data[0]*m.data[5];
			real t8 = m.data[1]*m.data[3];
			real t10 = m.data[2]*m.data[3];
			real t12 = m.data[1]*m.data[6];
			real t14 = m.data[2]*m.data[6];

			//calculate the determinant
			real t16 = (t4 * m.data[8] - t6*m.data[7] - t8*m.data[8] +
				t10*m.data[7] + t12 * m.data[5] - t14*m.data[4]);

			//Make sure the determinant is not 0
			if ( t16 == (real)0.0f )
				return;
			real t17 = 1 / t16;

			data[0] = (m.data[4]*m.data[8]-m.data[5]*m.data[7]) * t17;
			data[1] = -(m.data[1]*m.data[8]-m.data[2]*m.data[7]) * t17;
			data[2] = (m.data[1]*m.data[5]-m.data[2]*m.data[4]) * t17;
			data[3] = -(m.data[3]*m.data[8]-m.data[5]*m.data[6]) * t17;
			data[4] = (m.data[0]*m.data[8] - t14) * t17;
			data[5] = -(t6 - t10) * t17;
			data[6] = (m.data[3]*m.data[7]-m.data[4]*m.data[6]) * t17;
			data[7] = -(m.data[0]*m.data[7]-t12) * t17;
			data[8] = (t4 - t8) * t17;
		}

		//Return a new matrix containing the inverse
		Matrix3 Inverse() const
		{
			Matrix3 result;
			result.SetInverse(*this);
			return result;
		}

		//Inverts Matrix
		void Invert()
		{
			SetInverse(*this);
		}

		//Sets the matrix to be the transpose of given matrix
		void SetTranspose(const Matrix3 &m)
		{
			data[0] = m.data[0];
			data[1] = m.data[3];
			data[2] = m.data[6];
			data[3] = m.data[1];
			data[4] = m.data[4];
			data[5] = m.data[7];
			data[6] = m.data[2];
			data[7] = m.data[5];
			data[8] = m.data[8];
		}

		//Returns a new matrix containing the transpose of this matrix
		Matrix3 Transpose() const
		{
			Matrix3 result;
			result.SetTranspose(*this);
			return result;
		}

		//Returns a matrix which is this matrix multplied by other matrix
		Matrix3 operator*(const Matrix3 &other) const
		{
			return Matrix3 
				(
					data[0]*other.data[0] + data[1]*other.data[3] + data[2]*other.data[6],
					data[0]*other.data[1] + data[1]*other.data[4] + data[2]*other.data[7],
					data[0]*other.data[2] + data[1]*other.data[5] + data[2]*other.data[8],

					data[3]*other.data[0] + data[4]*other.data[3] + data[5]*other.data[6],
					data[3]*other.data[1] + data[4]*other.data[4] + data[5]*other.data[7],
					data[3]*other.data[2] + data[4]*other.data[5] + data[5]*other.data[8],

					data[6]*other.data[0] + data[7]*other.data[3] + data[8]*other.data[6],
					data[6]*other.data[1] + data[7]*other.data[4] + data[8]*other.data[7],
					data[6]*other.data[2] + data[7]*other.data[5] + data[8]*other.data[8]
				);
		}

		//Multiplies this matrix in place by other matrix
		void operator*=(const Matrix3 &other)
		{
			real t1;
			real t2;
			real t3;

			t1 = data[0]*other.data[0] + data[1]*other.data[3] + data[2]*other.data[6];
			t2 = data[0]*other.data[1] + data[1]*other.data[4] + data[2]*other.data[7];
			t3 = data[0]*other.data[2] + data[1]*other.data[5] + data[2]*other.data[8];
			data[0] = t1;
			data[1] = t2;
			data[2] = t3;

			t1 = data[3]*other.data[0] + data[4]*other.data[3] + data[5]*other.data[6];
			t2 = data[3]*other.data[1] + data[4]*other.data[4] + data[5]*other.data[7];
			t3 = data[3]*other.data[2] + data[4]*other.data[5] + data[5]*other.data[8];
			data[3] = t1;
			data[4] = t2;
			data[5] = t3;

			
			t1 = data[6]*other.data[0] + data[7]*other.data[3] + data[8]*other.data[6];
			t2 = data[6]*other.data[1] + data[7]*other.data[4] + data[8]*other.data[7];
			t3 = data[6]*other.data[2] + data[7]*other.data[5] + data[8]*other.data[8];
			data[6] = t1;
			data[7] = t2;
			data[8] = t3;
		}

		//Multiply matrix by scalar value
		void operator*=(const real scalar)
		{
			data[0] *= scalar; data[1] *= scalar; data[2] *= scalar;
			data[3] *= scalar; data[4] *= scalar; data[5] *= scalar;
			data[6] *= scalar; data[7] *= scalar; data[8] *= scalar;
		}

		//component addition of matrix by matrix
		void operator+=(const Matrix3 &other)
		{
			data[0] += other.data[0]; data[1] += other.data[1]; data[2] += other.data[2];
			data[0] += other.data[0]; data[3] += other.data[1]; data[4] += other.data[5];
			data[0] += other.data[0]; data[6] += other.data[1]; data[7] += other.data[8];
		}

		//Sets the matrix to be the rotation matrix corresponding to quaternion
		void SetOrientation(const Quaternion &q)
		{
			data[0] = 1 - (2*q.j*q.j + 2*q.k *q.k);
			data[1] = 2*q.i*q.j + 2*q.k*q.r;
			data[2] = 2*q.i*q.k - 2*q.j*q.r;
			data[3] = 2*q.i*q.j - 2*q.k*q.r;
			data[4] = 1 - (2*q.i*q.i + 2*q.k*q.k);
			data[5] = 2*q.j*q.k + 2*q.i*q.r;
			data[6] = 2*q.i*q.k + 2*q.j*q.r;
			data[7] = 2*q.j*q.k - 2*q.i*q.r;
			data[8] = 1 - (2*q.i*q.i + 2*q.j*q.j);
		}

		//Interpolates two matricies
		static Matrix3 LinearInterpolate(const Matrix3 &a, const Matrix3 &b, real prop);
	};

#pragma endregion
};

#endif