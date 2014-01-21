//polarish.h
//contains declarations for point charge and all magnetic / electic force calculations

#include <math.h>
#include <cmath>

#ifndef POLARIS_ENGINE_POLARIS_H
#define POLARIS_ENGINE_POLARIS_H

#include "core.h"
#include "precision.h"
#include "particle.h"

namespace PolarisEngine
{
	const double kConst(8.99);
	const double pi(3.1415926);

	struct PointCharge
	{
		double charge;
		Vector3 *pos;

		PointCharge(double x, double y, double c)
		{
			pos->x = x;
			pos->y = y;
			pos->z = 0;

			if(c < 0)
				charge = c * -1;
			else 
				charge = c;
		}
	};

	class Polaris
	{
	public:
		
		//returns the force between 2 point charges
		static double Get_Force(PointCharge p1, PointCharge p2, double distance);

		//returns the Electric field generated by a force on a charge
		static double Get_Electric_Field(double f, PointCharge p);
		
		//returns Magnetic Field from wire carrying current
		static double Get_Magnetic_Field(double current, double distance);

		//return Electric Potential of a point charge
		static double Get_Electric_Potential(PointCharge p, double distance);

		//returns force on a current within a magnetic field
		static double Get_Force_On_Current_In_Field(double current, double magneticField, double angle);

		//returns distance between two points in space
		static double Get_Distance(Vector3 v1, Vector3 v2);

		//returns distance between two PointCharges in space
		static double Get_Distance(PointCharge p1, PointCharge p2);
	};
};

#endif