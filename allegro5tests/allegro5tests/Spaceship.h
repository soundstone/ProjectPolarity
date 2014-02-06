//Spaceship.h
//Contains header declarations of the player's avatar - a ship

#include "polaris.h"
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>


class SpaceShip
{
public:

	SpaceShip();
	SpaceShip(PolarisEngine::Vector3 pos, int widthOfShip, int heightOfShip, float spd, int life, bool polaricCharge);


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
	float speed;
	int width;
	int height;
	int score;
	bool polarity;
	float buttonTimer;

	static const int SCREENHEIGHT = 500;
	static const int SCREENWIDTH = 1280;
	static const int BUTTON_TIME = 1.0f;
};