//Abstract class Magnet with child classes TopMagnet and BottomMagnet
//Class declarations
#pragma once
#include "core.h"
#include "Polaris.h"

class Magnet
{
public:
	virtual void InitializeMagnet(int id, int xcoord, int ycoord, int rad, float f, bool polarity) 
	{
		magnetID = id;
		magnetPosition.x = xcoord;
		magnetPosition.y = ycoord;
		magnetPosition.z = 0;
		radius = rad;
		force = f;
		magnetPolarity = polarity;
	};
	
	int magnetID;
	PolarisEngine::Vector3 magnetPosition;
	int radius;
	float force;
	bool magnetPolarity;
};

class TopMagnet : public Magnet
{
public:
	virtual void InitializeMagnet(int id, int xCoord, int yCoord, int rad, float f, bool polarity)
	{
		magnetID = id;
		magnetPosition.x = xCoord;
		magnetPosition.y = yCoord;
		magnetPosition.z = 0;
		radius = rad;
		force = f;
		magnetPolarity = polarity;
	}
};

class BottomMagnet : public Magnet
{
public:
	virtual void InitializeMagnet(int id, int xcoord, int ycoord, int rad, float f, bool polarity)
	{
		magnetID = id;
		magnetPosition.x = xcoord;
		magnetPosition.y = ycoord;
		magnetPosition.z = 0;
		radius = rad;
		force = f;
		magnetPolarity = polarity;
	}
};