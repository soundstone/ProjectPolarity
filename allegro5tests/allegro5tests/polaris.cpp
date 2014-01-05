//polaris.cpp
//contains definitions for Polaris class

#include "polaris.h"
#include <stdexcept>

using namespace std;

namespace PolarisEngine
{
	const double kConst = 8.99;
	const double pi = 3.1415926;

	double Polaris::Get_Force(PointCharge p1, PointCharge p2, double dist)
	{
		//Hold force of calculation
		double force;

		//square distance for force formula use
		double distance = dist * dist;

		//calculate force based on position and charge of 2 point charges
		force = ( (kConst * p1.charge * p2.charge) / distance);

		return force;
	}

	double Polaris::Get_Electric_Field(double force, PointCharge p)
	{
		//Hold force value of calculation
		double eField;

		//calculate electric field based on force of charge
		eField = force / p.charge;

		return eField;
	}

	double Polaris::Get_Magnetic_Field(double current, double distance)
	{
		//Hold strength of magentic field
		double magneticField;

		//used in calculation along with electric current 
		const double newZero = 4 * (3.13e-7);

		double numerator;
		double denomitor;


		numerator = newZero * current;
		denomitor = 2 * pi * distance;

		magneticField = numerator / denomitor;

		return magneticField;
	}

	double Polaris::Get_Electric_Potential(PointCharge p, double distance)
	{
		double potential;
		double numerator;

		numerator = kConst * p.charge;
		potential = numerator / distance;

		return potential;
	}

	double Polaris::Get_Force_On_Current_In_Field(double current, double magneticField, double angle)
	{
		double force;

		force = current * magneticField * sin(angle);

		return force;
	}

	double Polaris::Get_Distance(Vector3 v1, Vector3 v2)
	{
		double distance;
		double x, y, z;

		x = (v2.x - v1.x) * (v2.x - v1.x);
		y = (v2.y - v1.y) * (v2.y - v1.y);
		z = (v2.z - v1.z) * (v2.z - v1.z);

		distance = sqrt(x + y + z);

		return distance;
	}
};