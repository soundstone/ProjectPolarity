//polaris.cpp
//contains definitions for Polaris class

#include "polaris.h"
#include <stdexcept>

using namespace std;

namespace PolarisEngine
{
	//Returns the total force between two PointCharge objects
	double Polaris::Get_Force(PointCharge *p1, PointCharge *p2, double dist)
	{
		//Hold force of calculation
		double force;

		//square distance for force formula use
		double distance = dist * dist;

		//calculate force based on position and charge of 2 point charges
		force = ( (kConst * p1->charge * p2->charge) / distance);

		return force;
	}

	double Polaris::Get_Force(double p1, double p2, double dist)
	{
		//Hold force of calculation
		double force;

		//square distance for force formula use
		double distance = dist * dist;

		//calculate force based on position and charge of 2 point charges
		force = ( (kConst * p1 * p2) / distance);

		return force;
	}

	//Is a vector field. Field vector at a given point is defined as the
	//force vector per unit charge that would be exerted on a stationary 
	//Pointcharge at that point. 
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

		x = (v2.x - v1.x);
		y = (v2.y - v1.y);
		z = (v2.z - v1.z);

		distance = x + y + z;

		return distance;
	}

	double Polaris::Get_Distance(PointCharge p1, PointCharge p2)
	{
		double distance;
		double x, y;

		x = (p2.pos.x - p1.pos.x);
		y = (p2.pos.y - p1.pos.y);
		
		distance = x + y;

		return distance;
	}

	Vector3 Polaris::Get_Distance_Vector(Vector3 v1, Vector3 v2)
	{
		Vector3 temp;

		temp.x = v2.x - v1.x;
		temp.y = v2.y - v1.y;
		temp.z = 0;

		return temp;
	}
};