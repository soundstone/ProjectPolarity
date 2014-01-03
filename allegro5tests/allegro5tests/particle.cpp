//Particle.cpp
#include <assert.h>
#include "particle.h"

using namespace PolarisEngine;

void Particle::Integrate(real duration)
{
	if(inverseMass <= 0.0f) return;

	assert(duration > 0.0);

	//update linear position
	position.AddScaledVector(velocity, duration);

	//Work out the acceleration from the force
	Vector3 resultingAcceleration = acceleration;
	resultingAcceleration.AddScaledVector(forceAccum, inverseMass);

	//Update linear velocity from the accerlation
	velocity.AddScaledVector(resultingAcceleration, duration);

	//Impose Drag
	velocity *= real_pow(damping, duration);

	//clear the forces
	ClearAccumlator();
}

void Particle::SetMass(const real mass)
{
	assert(mass != 0);
	Particle::inverseMass = ((real)1.0) / mass;
}

real Particle::GetMass() const
{
	if(inverseMass == 0) 
	{
		return REAL_MAX;
	}
	else 
	{
		return ((real)1.0) / inverseMass;
	}
}

void Particle::SetInverseMass(const real inverseMass)
{
	Particle::inverseMass = inverseMass;
}

real Particle::GetInverseMass() const
{
	return inverseMass;
}

bool Particle::HasFiniteMass() const
{
	return inverseMass >= 0.0f;
}

void Particle::SetDamping(const real damping)
{
	Particle::damping = damping;
}

real Particle::GetDamping() const
{
	return damping;
}

void Particle::SetPosition(const Vector3 &position)
{
	Particle::position = position;
}

void Particle::SetPosition(const real x, const real y, const real z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Particle::GetPosition(Vector3 *position) const
{
	*position = Particle::position;
}

Vector3 Particle::GetPosition() const
{
	return position;
}

void Particle::SetVelocity(const Vector3 &velocity)
{
	Particle::velocity = velocity;
}

void Particle::SetVelocity(const real x, const real y, const real z)
{
	velocity.x = x;
	velocity.y = y;
	velocity.z = z;
}

void Particle::GetVelocity(Vector3 *velocity) const
{
	*velocity = Particle::velocity;
}

Vector3 Particle::GetVelocity() const
{
	return velocity;
}

void Particle::SetAcceleration(const Vector3 &acceleration)
{
	Particle::acceleration = acceleration;
}

void Particle::SetAcceleration(const real x, const real y, const real z)
{
	acceleration.x = x;
	acceleration.y = y;
	acceleration.z = z;
}

void Particle::GetAcceleration(Vector3 *acceleration) const
{
	*acceleration = Particle::acceleration;
}

Vector3 Particle::GetAcceleration() const
{
	return acceleration;
}

void Particle::ClearAccumlator()
{
	forceAccum.Clear();
}

void Particle::AddForce(const Vector3 &force)
{
	forceAccum += force;
}