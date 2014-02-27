//particle.h
//class represents smallest object that can be simulated in the engine
#pragma once
#include "core.h"

namespace PolarisEngine
{
	class Particle
	{
	protected:
		//Holds the linear position of the particle in world space
		Vector3 position;

		//Holds the linear velocity of the particle in world space
		Vector3 velocity;

		//Holds the acceleration of the particle. This value can be used to set acceleration due to gravity or any other constant acceleration.
		Vector3 acceleration;

		//Holds the amount of damping applied to linear motion. Damping is required to remove energy added through numberical instability by the integrator.
		real damping;

		//Holds the inverse of the mass of the particle. 
		real inverseMass;

		//Holds the accumulated force to be applied at the next interation only. Value is zeroed at each step
		Vector3 forceAccum;

	public:
		//Integrates the particle forward in time by given amount. Uses Newton-Euler integration method.
		void Integrate(real duration);

		//Sets the mass of the particle
		/*
			This parameter must not be 0. This invalidates internal data for the particle.
			Either an integration function or the calculateInternals function should be called before trying to get 
			any settings from the particle. 
		*/
		void SetMass(const real mass);

		//Gets the mass of the particle
		real GetMass() const;

		//Set the inverse mass of the particle. May be 0. Either an integration function, or the calculateInternals
		//function should be called before tyring to get any settings from the particle
		void SetInverseMass(const real inverseMass);

		//Get inverse mass of particle
		real GetInverseMass() const;

		//return true if mass of particle is not-infinite
		bool HasFiniteMass() const;

		//Sets both the damping of the particle
		void SetDamping(const real damping);

		//Gets the current damping value
		real GetDamping() const;

		//Sets the position of the particle
		//Param position is the new position of the particle
		void SetPosition(const Vector3 &position);

		//Sets the position of the particle by component
		void SetPosition(const real x, const real y, const real z);

		//Fills the given vector with the position of the particle
		//Param position is a pointer to a vector3 into which to write the position
		void GetPosition(Vector3 *position) const;

		//Gets the position of the particle
		Vector3 GetPosition() const;

		//Sets the velocity of the particle
		//Param velocity is the new velocity of the particle
		void SetVelocity(const Vector3 &velocity);

		//Sets the velocity of the particle by component.
		void SetVelocity(const real x, const real y, const real z);

		//Fills the given vector with the velocity of the particle
		//Param velocity is a pointer to a vector3 into which to write the velocity. Given in world local space.
		void GetVelocity(Vector3 *velocity) const;

		//Gets velocity of the particle
		Vector3 GetVelocity() const;

		//Sets the constant acceleration of the particle
		void SetAcceleration(const Vector3 &acceleration);

		//Sets the constant acceleration of the particle by component
		void SetAcceleration(const real x, const real y, const real z);

		//Fills the given vector with the acceleration of the particle
		void GetAcceleration(Vector3 *acceleration) const;

		//Gets the acceleration of the particle
		Vector3 GetAcceleration() const;

		//Clears the forces applied to the particle. Called automatically after each integration
		void ClearAccumlator();

		//Adds the given force to the particle, to be applied at the next iteration only,
		void AddForce(const Vector3 &force);
	};
};