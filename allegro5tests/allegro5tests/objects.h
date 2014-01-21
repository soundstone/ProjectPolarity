#include "polaris.h"

//Object IDs
enum IDS {PLAYER, BULLET, ENEMY, MAGNET};
enum Polarity {POSITIVE, NEGATIVE};


struct Point
{
	int x, y;
};

//player
struct SpaceShip
{
	int ID;
	Point pos;
	Vector3 *shipPos;
	int lives;
	int speed;
	int width;
	int height;
	int score;
	bool polarity;

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