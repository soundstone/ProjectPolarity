#include "Spaceship.h"

SpaceShip::SpaceShip()
{
}

SpaceShip::SpaceShip(PolarisEngine::Vector3 pos, int widthOfShip, int heightOfShip, float spd, int life, bool polaricCharge)
{
	shipPos = pos;
	width = widthOfShip;
	height = heightOfShip;
	speed = spd;
	lives = life;
	polarity = polaricCharge;
	score = 0;
	buttonTimer = 0.0f;
}

void SpaceShip::MoveShipUp()
{
	shipPos.y -= speed;
	if( shipPos.y < 66)
		shipPos.y = 66;
}

void SpaceShip::MoveShipDown()
{
	shipPos.y += speed;
	if (shipPos.y > SCREENHEIGHT - 100)
		shipPos.y = SCREENHEIGHT - 100;
}

void SpaceShip::MoveShipLeft()
{
	shipPos.x -= speed;
	if (shipPos.x < 20)
		shipPos.x = 20;
}

void SpaceShip::MoveShipRight()
{
	shipPos.x += speed;
	if (shipPos.x > SCREENWIDTH - width)
		shipPos.x = SCREENWIDTH - width;
}

void SpaceShip::DrawShip()
{
	if(polarity)
	{
		al_draw_filled_rectangle(shipPos.x, shipPos.y, shipPos.x + width, shipPos.y + height, al_map_rgb(255,0,0));
		al_draw_pixel(shipPos.x + (width / 2), shipPos.y + (height / 2), al_map_rgb(0,0,0));
	}
	else
	{
		al_draw_filled_rectangle(shipPos.x, shipPos.y, shipPos.x + width, shipPos.y + height, al_map_rgb(255,255,255));
		al_draw_pixel(shipPos.x + (width / 2), shipPos.y + (height / 2), al_map_rgb(0,0,0));
	}
}
