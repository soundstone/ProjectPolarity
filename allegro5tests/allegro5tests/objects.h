#include "core.h"

//Object IDs
enum IDS {PLAYER, BULLET, ENEMY, MAGNET};
enum Polarity {POSITIVE, NEGATIVE};

	///* Define basic vector struct. Used for line collision*/
	//struct Vector
	//{
	//	float x, y;

	//	
	//};

	/* Define a Point structure. This will be used to build lines and determine 2D Position */
	struct Point
	{
		int x, y;

		Point& operator-(const Point &rhs)
		{
			 Point temp;
			 temp.x = x - rhs.x;
			 temp.y = y - rhs.y;
			 return temp;
		}

		Point& operator+(const double &rhs)
		{
			Point temp;
			temp.x = x + rhs;
			temp.y = y + rhs;
			return temp;
		}
	};
	
	/* Define a Line structure. Consists of 2 points - start and end. Represents line between two points. */
	struct Line
	{
		Point startPoint;
		Point endPoint;
	};

//player
struct SpaceShip
{
	int ID;
	Point pos;
	int lives;
	int speed;
	int boundx;
	int boundy;
	int score;

};

struct Magnet
{
	int ID;
	int x, y;
	int radius;
	int force;
	bool polarity;
};

struct Bullet
{
	int ID;
	int x, y;
	bool live;
	int speed;
};

struct Comet
{
	int ID;
	int x, y;
	bool live;
	int speed;
	int boundx;
	int boundy;
};