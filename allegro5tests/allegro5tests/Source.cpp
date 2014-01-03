#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include "objects.h"
#include <math.h>



#pragma region Globals

//screen dimensions
const int WIDTH = 1280;
const int HEIGHT = 500;
ALLEGRO_FONT *font;
//level dimensions
const int LEVELWIDTH = 4000;
const int LEVELHEIGHT = 500;

//Camera position
int cameraX = 0;
int cameraY = 0;

//how many points and spacing between points for lines drawn
const int PLOT_INTERVAL = 36;
const int NUM_POINTS = 333;

//Magnet consts
const int NUM_MAGNETS = 3;

//obstacle consts
const int NUM_OBSTICLES = 2;
const int BUTTON_TIME = 1.0f;

enum KEYS {UP, DOWN, LEFT, RIGHT, SPACE };
bool keys[5] = {false, false, false, false, false};

//================================================================

#pragma endregion

#pragma region Prototypes

//Ship functions
void InitShip(SpaceShip &ship);
void DrawShip(SpaceShip &ship, bool polarity);
void MoveShipUp(SpaceShip &ship);
void MoveShipDown(SpaceShip &ship);
void MoveShipLeft(SpaceShip &ship);
void MoveShipRight(SpaceShip &ship);

//Tunnel generation functions
void PlotPointsAndConnectTop(Point oldP, Point newP, Point points[]);
int  GenerateNewPointTop();
void ConnectPointsTop(Point points[]);
void PlotPointsAndConnectBottom(Point oldP, Point newP, Point points[]);
int  GenerateNewPointBottom();
void ConnectPointsBottom(Point points[]);
bool CollideLineTop(SpaceShip &ship);
void DrawObsticles(Point obsticles[]);
void GenerateObsticles(Point obsticles[]);

//camera functions
Point TranslateWorldToScreen(int objectX, int objectY, int cameraX, int cameraY);
Point UpdateCamera(int x, int y, SpaceShip &ship);

//Magnet Functions
void InitMagnets(Magnet magnets[], Magnet magnetsBot[]);
int GetMagnetLocationX();
int GetMagnetLocationY();
void DrawMagnets(Magnet magnets[], Magnet magnetsBot[]);

//Helper Functions
double GetPointDistance(Point p1, Point p2);
bool CollideTunnelTop(Point points[], SpaceShip &ship);
bool CollideTunnelBottom(Point points[], SpaceShip &ship);
float GetLineSlope(Point p1, Point p2);
float GetYIntercept(Point p1, float slope);
//bool IsOnLine(int x, int y, float slope, float intercept);
bool IsOnLine(int boxX, int boxY, Point p1, Point p2);


//==========================================================================================

#pragma endregion 

int main(void)
{
	//primitive variable
	bool done = false;
	bool redraw = true;
	const int FPS = 60;
	

	//procedual tunnel variables
	Point oldP, newP;
	Point pointsTop[NUM_POINTS] = {};
	Point pointsBottom[NUM_POINTS] = {};
	Point obsticles[NUM_OBSTICLES];

	//object variables
	SpaceShip ship;
	Magnet magnets[NUM_MAGNETS];
	Magnet magnetsBot[NUM_MAGNETS];
	bool polarity = false;
	float buttonTimer = 0.0f;
	bool collide = false;
	bool collideBot = false;
	

	//allegro variables
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;	

	//Initialization functions
	if(!al_init())
		return -1;

	display = al_create_display(WIDTH, HEIGHT);
	
	if(!display)
		return -1;

	al_init_primitives_addon();
	al_install_keyboard();
	al_init_font_addon();
	al_init_ttf_addon();
	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);

	//srand(time(NULL));
	/*
		Seed set to 24 for testing purposes.
		//TODO: need to set up so that all coords print to a text file. 
	*/
	srand(24);

	//Initialize the ship to center screen to start game.
	InitShip(ship);

	//Font used for Debug display
	font = al_load_font("arial.ttf", 16, 0);
	
	//Initialize the magnets on the map
	InitMagnets(magnets, magnetsBot);

	//Place the center obstacles
	GenerateObsticles(obsticles);

	//set up points to initial null status
	oldP.x = NULL;
	oldP.y = NULL;
	newP.x = NULL;
	newP.y = NULL;

	//These two function calls generate and draw the points in the array and connect them by drawing a line to each of them.
	PlotPointsAndConnectTop(oldP, newP, pointsTop);
	PlotPointsAndConnectBottom(oldP, newP, pointsBottom);

	
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));

	al_start_timer(timer);

#pragma region Game Loop

	while(!done)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_TIMER)
		{
			buttonTimer += 0.1f;
			
			collide = false;
			redraw = true;
			if(keys[UP])
				MoveShipUp(ship);
			if(keys[DOWN])
				MoveShipDown(ship);
			if(keys[LEFT])
				MoveShipLeft(ship);
			if(keys[RIGHT])
				MoveShipRight(ship);
			if(keys[SPACE])
			{
				if(buttonTimer >= BUTTON_TIME)
				{
					buttonTimer = 0.0f;
					polarity = !polarity;
				}
			}		

			collide = CollideTunnelTop(pointsTop, ship);
			collideBot = CollideTunnelBottom(pointsBottom, ship);

			//CollideLineTop(ship);
			UpdateCamera(cameraX, cameraY, ship);
		}
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			done = true;
		}
		if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch(ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			case ALLEGRO_KEY_UP:
				keys[UP] = true;
				break;
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = true;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = true;
				break;
			case ALLEGRO_KEY_SPACE:
				keys[SPACE] = true;
				break;
			}
		}
		else if(ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch(ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				done = false;
				break;
			case ALLEGRO_KEY_UP:
				keys[UP] = false;
				break;
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = false;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = false;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = false;
				break;
			case ALLEGRO_KEY_SPACE:
				keys[SPACE] = false;
				break;
			}
		}

		if(redraw && al_event_queue_is_empty(event_queue))
		{
			redraw = false;
			
			/*if(collide)
				al_draw_textf(font, al_map_rgb(255,0,0), WIDTH / 2, HEIGHT / 2, 0, "COLLISION AT %5d, %5d", ship.pos.x, ship.pos.y);
			if(collideBot)
				al_draw_textf(font, al_map_rgb(0,255,50), WIDTH / 2, HEIGHT / 2, 0, "Collision at %5d, %5d", ship.pos.x, ship.pos.y);*/

			DrawShip(ship, polarity);

			//TODO: list out what is in the array

			ConnectPointsTop(pointsTop);
			ConnectPointsBottom(pointsBottom);
			
			DrawMagnets(magnets, magnetsBot);
			
			DrawObsticles(obsticles);
			
			al_flip_display();
			al_clear_to_color(al_map_rgb(0,0,0));
		}
	}

#pragma endregion

	al_destroy_timer(timer);
	al_destroy_display(display);
	return 0;
}

#pragma region Ship

void InitShip(SpaceShip &ship)
{
	ship.pos.x = 20;
	ship.pos.y = HEIGHT / 2;
	ship.ID = PLAYER;
	ship.lives = 3;
	ship.score = 0;
	ship.speed = 7;
	ship.boundx = 15;
	ship.boundy = 15;
}

void DrawShip(SpaceShip &ship, bool polarity)
{
	if(polarity)
	{
		al_draw_filled_rectangle(ship.pos.x, ship.pos.y, ship.pos.x + ship.boundx, ship.pos.y + ship.boundy, al_map_rgb(255,0,0));
		al_draw_pixel(ship.pos.x + (ship.boundx / 2), ship.pos.y + (ship.boundy / 2), al_map_rgb(0,0,0));
		//turrets
		//al_draw_filled_rectangle(ship.x, ship.y - 9, ship.x + 10, ship.y - 7, al_map_rgb(0,145,255));
		//al_draw_filled_rectangle(ship.x, ship.y + 9, ship.x + 10, ship.y + 7, al_map_rgb(0,145,255));

		//ship body
		//al_draw_filled_triangle(ship.x - 12, ship.y - 17, ship.x + 12, ship.y, ship.x - 12, ship.y + 17, al_map_rgb(255, 0, 0));
		//al_draw_filled_rectangle(ship.x - 12, ship.y - 2, ship.x + 15, ship.y + 2, al_map_rgb(0,0,255));
	}
	else
	{
		al_draw_filled_rectangle(ship.pos.x, ship.pos.y, ship.pos.x + ship.boundx, ship.pos.y + ship.boundy, al_map_rgb(255,255,255));
		al_draw_pixel(ship.pos.x + (ship.boundx / 2), ship.pos.y + (ship.boundy / 2), al_map_rgb(0,0,0));
		//turrets
		//al_draw_filled_rectangle(ship.x, ship.y - 9, ship.x + 10, ship.y - 7, al_map_rgb(0,0,255));
		//al_draw_filled_rectangle(ship.x, ship.y + 9, ship.x + 10, ship.y + 7, al_map_rgb(0,0,255));

		//ship body
		//al_draw_filled_triangle(ship.x - 12, ship.y - 17, ship.x + 12, ship.y, ship.x - 12, ship.y + 17, al_map_rgb(255, 255, 255));
		//al_draw_filled_rectangle(ship.x - 12, ship.y - 2, ship.x + 15, ship.y + 2, al_map_rgb(0,0,255));
	}
}

void MoveShipUp(SpaceShip &ship)
{
	ship.pos.y -= ship.speed;
	if( ship.pos.y < 66)
		ship.pos.y = 66;
}

void MoveShipDown(SpaceShip &ship)
{
	ship.pos.y += ship.speed;
	if( ship.pos.y > HEIGHT - 100)
		ship.pos.y = HEIGHT - 100;
}

void MoveShipLeft(SpaceShip &ship)
{
	ship.pos.x -= ship.speed;
	if( ship.pos.x < 20)
		ship.pos.x = 20;
}

void MoveShipRight(SpaceShip &ship)
{
	ship.pos.x += ship.speed;
	if( ship.pos.x > WIDTH)
		ship.pos.x = WIDTH;
}

bool CollideLineTop(SpaceShip &ship)
{
	return true;
}

#pragma endregion

#pragma region Procedual Tunnel
	
int GenerateNewPointTop()
{
	int y = NULL;

	//Constrain the Y placement so that there is room for potential obstacles, magnets or player movement.
	y = (rand() % 110) + 80;

	return y;
}

int GenerateNewPointBottom()
{
	int y = NULL;

	//Constrain the Y placement so that there is room for potential obstacles, magnets or player movement.
	y = (rand() % 66) + 320;

	return y;
}

void PlotPointsAndConnectBottom(Point oldP, Point newP, Point pointsBot[])
{
	if(newP.x >= 4000)
		return;
	
	//Generate the starting point. Located at the far left of screen. 
	if(oldP.x == NULL)
	{
		oldP.x = 0;
		oldP.y = GenerateNewPointBottom();
	}

	pointsBot[0] = oldP;
	newP.x = oldP.x;

	for (int i = 1; i < NUM_POINTS; i++)
	{
		//One in 66 the points will not vary in height. This cause flat areas, and makes the lines look more cavelike. 
		if(rand() % 66 == 0)
		{
			for(int j = 0; j < 3; j++)
			{
				//Move the X coord to the next PLOT_INTERVAL. Adding 12 during testing. Might need to be implemented through PLOT_INTERVAL
				newP.x = newP.x + PLOT_INTERVAL + 12;
				
				//Keeps the height the same. No change in slope.
				newP.y = oldP.y;
				
				//add the new point to the bottom of the map
				pointsBot[i] = newP;
			}
		}
		//Generates new points until pointsBot[i] reaches 4000 points. These vary in height.
		else 
		{
			newP.x = newP.x + PLOT_INTERVAL;

			oldP.y = GenerateNewPointBottom();
			newP.y = GenerateNewPointBottom();
			
			pointsBot[i] = newP;
		}
	}	
}

void PlotPointsAndConnectTop(Point oldP, Point newP, Point points[])
{
	if(newP.x >= 4000)
		return;

	//Generate starting point for bottomw lines. Far left of the screen. 
	if(oldP.x == NULL)
	{
		oldP.x = 0;
		oldP.y = GenerateNewPointTop();
	}

	points[0] = oldP;
	newP.x = oldP.x;

	for (int i = 1; i < NUM_POINTS; i++)
	{

		//One in 66, the Y coord will remain the same. This causes flat areas within the map. 
		if(rand() % 66 == 0)
		{
			for(int j = 0; j < 3; j++)
			{
				newP.x = newP.x + PLOT_INTERVAL + 12;
				newP.y = oldP.y;
				points[i] = newP;
			}
			
		}
		//Generate the remainder of the points to fill points[i] to 4000. Vary in height.
		else 
		{
			newP.x = newP.x + PLOT_INTERVAL;

			oldP.y = GenerateNewPointTop();
			newP.y = GenerateNewPointTop();
			
			points[i] = newP;
		}
	}	
}

void ConnectPointsTop(Point points[])
{
	for(int i = 0; i < NUM_POINTS; i++)
	{
		al_draw_line(points[i].x, points[i].y, points[i+1].x, points[i + 1].y, al_map_rgb(255,0,255), 2);
	}
}

void ConnectPointsBottom(Point pointsBot[])
{
	for (int i = 0; i < NUM_POINTS; i++)
	{
		al_draw_line(pointsBot[i].x, pointsBot[i].y, pointsBot[i + 1].x, pointsBot[i + 1].y, al_map_rgb(255,0,255), 2);
	}
}

void DrawObsticles(Point obsticles[])
{
	for (int i = 0; i < NUM_OBSTICLES; i++)
	{
		al_draw_filled_rectangle(obsticles[i].x, obsticles[i].y, obsticles[i].x + 20, obsticles[i].y + 20, al_map_rgb(255,0,255));
	}
}

void GenerateObsticles(Point obsticles[])
{
	for (int i = 0; i < NUM_OBSTICLES; i++)
	{
		obsticles[i].x = ((rand() % (WIDTH - 30)) + 100);
		obsticles[i].y = HEIGHT / 2 - 20;
	}
}

#pragma endregion

#pragma region Camera

Point TranslateWorldToScreen(int objectX, int objectY, int cameraX, int cameraY)
{
	Point newPoint;
	newPoint.x = objectX - cameraX;
	newPoint.y = objectY - cameraY;

	return newPoint;
}

Point UpdateCamera(int x, int y, SpaceShip &ship)
{
	x = ship.pos.x - WIDTH / 2;
	y = ship.pos.y - HEIGHT / 2;

	if(x < 0) x = 0;
	if(y < 0) y = 0;
	if(x > LEVELWIDTH - WIDTH) x = LEVELWIDTH - WIDTH;
	if(y > LEVELHEIGHT - HEIGHT) y = LEVELHEIGHT - HEIGHT;

	Point CameraPoint;
	CameraPoint.x = x;
	CameraPoint.y = y;

	return CameraPoint;
}

#pragma endregion

#pragma region Magnets

void InitMagnets(Magnet magnets[], Magnet magnetsBot[])
{
	for(int i = 0; i < NUM_MAGNETS; ++i)
	{
		//Top
		magnets[i].ID = MAGNET;
		magnets[i].radius = 10;
		magnets[i].polarity = POSITIVE;
		magnets[i].x = GetMagnetLocationX();
		magnets[i].y = GetMagnetLocationY();

		//Bottom
		magnetsBot[i].ID = MAGNET;
		magnetsBot[i].radius = 10;
		magnetsBot[i].polarity = POSITIVE;
		magnetsBot[i].x = GetMagnetLocationX();
		magnetsBot[i].y = GetMagnetLocationY() + 370;
	}

}

int GetMagnetLocationX()
{
	int x = (rand() % WIDTH) + 30;

	return x;
}

int GetMagnetLocationY()
{
	int y = (rand() % 60) + 15;

	return y;
}

void DrawMagnets(Magnet magnets[], Magnet magnetsBot[])
{
	for(int i = 0; i < NUM_MAGNETS; ++i)
	{
		al_draw_filled_rectangle(magnets[i].x, magnets[i].y, magnets[i].x + 20, magnets[i].y + 20, al_map_rgb(0,255,255));
		al_draw_filled_rectangle(magnetsBot[i].x, magnetsBot[i].y, magnetsBot[i].x + 20, magnetsBot[i].y + 20, al_map_rgb(0, 255, 255));
	}
}

#pragma endregion

#pragma region Helper Functions

double GetPointDistance(Point p1, Point p2)
{
	double temp;
	double x, y;


	x = abs((p2.x - p1.x)*(p2.x - p1.x));
	y = abs((p2.y - p1.y)*(p2.y - p1.y));
	temp = sqrt(x + y);

	return temp;

}

bool CollideTunnelTop(Point points[], SpaceShip &ship)
{

	for (int i = 0; i < NUM_POINTS; i++)
	{
		if(points[i + 1].y < points[i].y)
		{
			//preliminary check for collision, is ship within checking range?
			if( (ship.pos.x > points[i].x) &&
				(ship.pos.x < points[i + 1].x) &&
				(ship.pos.y > points[i + 1].y) &&
				(ship.pos.y < points[i].y))
			{
				//TODO: need to place another if statment here checking the slope of the line and if the point lies on the line. 
				float slope = GetLineSlope(points[i], points[i + 1]);
				float intercept = GetYIntercept(points[i], slope);
				//if( IsOnLine(ship.pos.x, ship.pos.y, slope, intercept) )
				if(IsOnLine(ship.pos.x, ship.pos.y, points[i], points[i + 1]))
				{
					//Collide
					al_draw_textf(font, al_map_rgb(255,0,0), 100, 10, 0, "Line points (%5d, %5d)", points[i].x, points[i].y);
					al_draw_textf(font, al_map_rgb(255,0,0), 100, 35, 0, "Line points (%5d, %5d)", points[i + 1].x, points[i + 1].y);
					al_draw_textf(font, al_map_rgb(255,0,0), WIDTH / 2, HEIGHT / 2, 0, "COLLISION AT %5d, %5d", ship.pos.x, ship.pos.y);
					return true;
				}
				//else if ( IsOnLine((ship.pos.x + ship.boundx) / 2, ship.pos.y, slope, intercept) )
				else if( IsOnLine( (ship.pos.x + ship.boundx) / 2, ship.pos.y, points[i], points[i + 1]) )
				{
					//Collide
					al_draw_textf(font, al_map_rgb(255,0,0), 100, 10, 0, "Line points (%5d, %5d)", points[i].x, points[i].y);
					al_draw_textf(font, al_map_rgb(255,0,0), 100, 35, 0, "Line points (%5d, %5d)", points[i + 1].x, points[i + 1].y);
					al_draw_textf(font, al_map_rgb(255,0,0), WIDTH / 2, HEIGHT / 2, 0, "COLLISION AT %5d, %5d", ship.pos.x + ship.boundx, ship.pos.y);
					return true;
				}
				//else if ( IsOnLine(ship.pos.x + ship.boundx, ship.pos.y, slope, intercept) )
				else if( IsOnLine(ship.pos.x + ship.boundx, ship.pos.y, points[i], points[i + 1]) )
				{
					//Collide
					al_draw_textf(font, al_map_rgb(255,0,0), 100, 10, 0, "Line points (%5d, %5d)", points[i].x, points[i].y);
					al_draw_textf(font, al_map_rgb(255,0,0), 100, 35, 0, "Line points (%5d, %5d)", points[i + 1].x, points[i + 1].y);
					al_draw_textf(font, al_map_rgb(255,0,0), WIDTH / 2, HEIGHT / 2, 0, "COLLISION AT %5d, %5d", (ship.pos.x + ship.boundx) / 2, ship.pos.y);
					return true;
				}
			}
		}
		else if(points[i + 1].y > points[i].y)
		{
			//preliminary check for collision. Is ship within checking range?
			if( (ship.pos.x > points[i].x) &&
				(ship.pos.x < points[i + 1].x) &&
				(ship.pos.y < points[i + 1].y) &&
				(ship.pos.y > points[i].y))
			{
				//TODO: need to place another if statment here checking the slope of the line and if the point lies on the line. 
				//collide
				al_draw_textf(font, al_map_rgb(255,0,0), 100, 10, 0, "Line points (%5d, %5d)", points[i].x, points[i].y);
				al_draw_textf(font, al_map_rgb(255,0,0), 100, 35, 0, "Line points (%5d, %5d)", points[i + 1].x, points[i + 1].y);
				return true;
			}
		}
	}
	return false;
}

bool CollideTunnelBottom(Point points[], SpaceShip &ship)
{
	for (int i = 0; i < NUM_POINTS; i++)
	{
		if(points[i + 1].y < points[i].y)
		{
			//This is a preliminary check. Is the ship close enough to check for collision?
			if(( ship.pos.x > points[i].x) &&
				(ship.pos.x < points[i + 1].x) &&
				(ship.pos.y > points[i].y) &&
				(ship.pos.y < points[i + 1].y))
			{
				//TODO: need to place another if statement here checking the slope of the line and if the point lies on it
				//Collide
				return true;
			}
		}
		else if(points[i + 1].y > points[i].y)
		{
			//preliminary check. Is ship close enough to check for collision?
			if( (ship.pos.x > points[i].x) &&
				(ship.pos.x < points[i + 1].x) &&
				(ship.pos.y + 10 < points[i + 1].y) &&
				(ship.pos.y - 10> points[i].y))
			{
				//TODO: need to place another if statement here checking the slope of the line and if the point lies on it.
				//collide
				return true;
			}
		}
	}
	return false;
}

float GetLineSlope(Point p1, Point p2)
{	
	return ( (p2.y - p1.y) / (p2.x - p1.x) );
}

float GetYIntercept(Point p1, float slope)
{
	float intercept;

	intercept = (slope * p1.x) - p1.y;
	intercept *= -1;

	return intercept;
}

/*
bool IsOnLine(int x, int y, float slope, float intercept)
{
	return y == (slope * x) + intercept;
}
*/

bool IsOnLine(int boxX, int boxY, Point p1, Point p2)
{
	Point temp;
	temp.x = boxX;
	temp.y = boxY;
	return ( (p1.y - temp.y) == GetLineSlope(p1, p2) * (p1.x - temp.x) );
}
#pragma endregion