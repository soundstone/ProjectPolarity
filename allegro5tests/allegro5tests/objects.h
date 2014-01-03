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