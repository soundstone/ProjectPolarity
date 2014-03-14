//Spaceship.h
//Contains header declarations of the player's avatar - a ship
#pragma once
#include "polaris.h"
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>


class SpaceShip
{
public:

	SpaceShip();
	SpaceShip(PolarisEngine::Vector3 pos, int widthOfShip, int heightOfShip, float spd, int life, bool polaricCharge, float chargeValue, float massValue);

	int GetNumberOfLives() const 
	{ 
		return lives;
	}
	int GetWidth() const
	{
		return width;
	}
	int GetHeight() const
	{
		return height;
	}
	int GetScore() const
	{
		return score;
	}
	bool GetPolarity() const
	{
		return polarity;
	}
	float GetSpeedX() const 
	{
		return xSpeed; 
	}
	float GetSpeedY() const
	{
		return ySpeed;
	}
	float GetSpeed() const
	{
		return speed;
	}
	float GetCharge() const
	{
		return charge;
	}
	float GetxForce() const
	{
		return xForce;
	}
	float GetyForce() const
	{
		return yForce;
	}
	float GetMass() const
	{
		return mass;
	}

	void SetxForce(float value)
	{
		xForce = value;
	}
	void SetyForce(float value)
	{
		yForce = value;
	}
	void SetxSpeed(float value)
	{
		xSpeed = value;
	}
	void SetySpeed(float value)
	{
		ySpeed = value;
	}

	void MoveShipUp();
	void MoveShipDown();
	void MoveShipLeft();
	void MoveShipRight();

	void TogglePolaricCharge()
	{
		polarity = !polarity;
	}

	void LoseLife()
	{
		lives--;
		if (lives <= 0)
			lives = 0;
	}
	void GainLife()
	{
		lives++;
	}

	void UpdateScore(int addScore)
	{
		score += addScore;
	}
	void DrawShip();

	PolarisEngine::Vector3 shipPos;

private:

	
	int lives;
	//used for forced input movement.
	float speed;
	int width;
	int height;
	int score;
	bool polarity;
	float buttonTimer;

	
	//xSpeed and ySpeed are used for force calculations from magnet's influence.
	float xSpeed;
	float ySpeed;

	//xForce and yForce are used for force calculations from magnet's influence. Ship will be moved by these amounts while in field.
	float xForce;
	float yForce;

	//charge and mass are used in magnet force calculations while in magnetic field.
	float charge;
	float mass;

	static const int SCREENHEIGHT = 500;
	static const int SCREENWIDTH = 1280;
	static const int BUTTON_TIME = 1.0f;
};