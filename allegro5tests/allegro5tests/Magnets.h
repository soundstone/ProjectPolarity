//Abstract class Magnet with child classes TopMagnet and BottomMagnet
//Class declarations

class Magnet
{
public:
	virtual void InitializeMagnet(int id, int xcoord, int ycoord, int rad, float f, bool polarity) 
	{
		magnetID = id;
		magnetX = xcoord;
		magnetY = ycoord;
		radius = rad;
		force = f;
		magnetPolarity = polarity;
	};
	
	int magnetID;
	int magnetX;
	int magnetY;
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
		magnetX = xCoord;
		magnetY = yCoord;
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
		magnetX = xcoord;
		magnetY = ycoord;
		radius = rad;
		force = f;
		magnetPolarity = polarity;
	}
};