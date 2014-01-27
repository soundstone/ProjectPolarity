#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include "objects.h"
#include <math.h>
#include <string>
#include "Logger.h"
#include "polaris.h"
#include "core.h"
#include "MagnetFactory.h"

using namespace std;
using namespace PolarisEngine;

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
Magnet topMagnets[NUM_MAGNETS];
Magnet botMagnets[NUM_MAGNETS];

//obstacle consts
const int NUM_OBSTICLES = 2;
const int BUTTON_TIME = 1.0f;

enum KEYS {UP, DOWN, LEFT, RIGHT, SPACE };
bool keys[5] = {false, false, false, false, false};

TopMagnetFactory topFactory;
BottomMagnetFactory bottomFactory;

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
void PlotPointsTop(Point oldP, Point newP, Point points[]);
int  GenerateNewPointTop();
void ConnectPointsTop(Point points[]);
void PlotPointsBottom(Point oldP, Point newP, Point points[]);
int  GenerateNewPointBottom();
void ConnectPointsBottom(Point points[]);
void DrawObsticles(Point obsticles[]);
void GenerateObsticles(Point obsticles[]);

//camera functions
Point TranslateWorldToScreen(int objectX, int objectY, int cameraX, int cameraY);
Point UpdateCamera(int x, int y, SpaceShip &ship);

//Magnet Functions
//void InitMagnets(Magnet magnets[], Magnet magnetsBot[]);
int GetMagnetLocationX();
int GetMagnetLocationY();
void DrawMagnets(Magnet magnets[], Magnet magnetsBot[]);
void SetupMagnetsTop();

//Helper Functions
Point GetPointDistance(Point p1, Point p2);
bool CollideTunnelTop(Point points[], SpaceShip &ship);
bool CollideTunnelBottom(Point points[], SpaceShip &ship);
float GetLineSlope(Point p1, Point p2);
float GetYIntercept(Point p1, float slope);
bool IsOnLine(int boxX, int boxY, Point p1, Point p2);

float GetPerpedicularSlope(float slope);
Point GetLineIntersection(int x1, int y1, float slope, float intercept, int x2, int y2, float perpSlope, float perpIntercept);


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
	//Magnet magnets[NUM_MAGNETS];
	//Magnet magnetsBot[NUM_MAGNETS];
	bool polarity = false;
	float buttonTimer = 0.0f;
	bool collide = false;
	bool collideBot = false;
	

	//allegro variables
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_BITMAP *backBuffer = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;	

	//Initialization functions
	if(!al_init())
		return -1;

	display = al_create_display(WIDTH, HEIGHT);
	backBuffer = al_create_bitmap(WIDTH, HEIGHT);
	
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
		Seed set to 64789 for testing purposes.
	*/
	srand(64789);

	//Initialize the ship to center screen to start game.
	InitShip(ship);

	//Font used for Debug display
	font = al_load_font("arial.ttf", 16, 0);
	
	//Initialize the magnets on the map
	//InitMagnets(magnets, magnetsBot);
	SetupMagnetsTop();

	//Place the center obstacles
	GenerateObsticles(obsticles);

	//set up points to initial null status
	oldP.x = NULL;
	oldP.y = NULL;
	newP.x = NULL;
	newP.y = NULL;

	//These two function calls generate and draw the points in the array and connect them by drawing a line to each of them.
	PlotPointsTop(oldP, newP, pointsTop);
	PlotPointsBottom(oldP, newP, pointsBottom);

	
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

			al_set_target_bitmap(backBuffer);
			al_clear_to_color(al_map_rgb(0,0,0));

			DrawShip(ship, polarity);

			ConnectPointsTop(pointsTop);
			ConnectPointsBottom(pointsBottom);
			
			DrawMagnets(topMagnets, botMagnets);
			
			DrawObsticles(obsticles);

			al_set_target_bitmap(al_get_backbuffer(display));

			al_draw_bitmap(backBuffer, 0, 0, 0);
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

//Initialize the ship's values to starting values
void InitShip(SpaceShip &ship)
{
	ship.pos.x = 20;
	ship.pos.y = HEIGHT / 2;
	ship.ID = PLAYER;
	ship.lives = 3;
	ship.score = 0;
	ship.speed = 7;

	ship.width = 15;
	ship.height = 15;

	ship.width = 15;
	ship.height = 15;
	ship.polarity = NEGATIVE;

}

//Designs and draw's the ship. 
void DrawShip(SpaceShip &ship, bool polarity)
{
	if(polarity)
	{
		al_draw_filled_rectangle(ship.pos.x, ship.pos.y, ship.pos.x + ship.width, ship.pos.y + ship.height, al_map_rgb(255,0,0));
		al_draw_pixel(ship.pos.x + (ship.width / 2), ship.pos.y + (ship.height / 2), al_map_rgb(0,0,0));
	}
	else
	{
		al_draw_filled_rectangle(ship.pos.x, ship.pos.y, ship.pos.x + ship.width, ship.pos.y + ship.height, al_map_rgb(255,255,255));
		al_draw_pixel(ship.pos.x + (ship.width / 2), ship.pos.y + (ship.height / 2), al_map_rgb(0,0,0));
	}
}

//Keyboard input to move the player forcefully up
void MoveShipUp(SpaceShip &ship)
{
	ship.pos.y -= ship.speed;
	if( ship.pos.y < 66)
		ship.pos.y = 66;
}

//Keyboard input to move the player forcefully down
void MoveShipDown(SpaceShip &ship)
{
	ship.pos.y += ship.speed;
	if( ship.pos.y > HEIGHT - 100)
		ship.pos.y = HEIGHT - 100;
}

//Keyboard input to move the player forcefully left
void MoveShipLeft(SpaceShip &ship)
{
	ship.pos.x -= ship.speed;
	if( ship.pos.x < 20)
		ship.pos.x = 20;
}

//Keyboard input to move the player forcefully right
void MoveShipRight(SpaceShip &ship)
{
	ship.pos.x += ship.speed;
	if( ship.pos.x > WIDTH)
		ship.pos.x = WIDTH;
}

#pragma endregion

#pragma region Procedual Tunnel

//Randomly generates new Y value for the top of the tunnel generation
int GenerateNewPointTop()
{
	int y = NULL;

	//Constrain the Y placement so that there is room for potential obstacles, magnets or player movement.
	y = (rand() % 110) + 80;

	return y;
}

//Randomly generates new Y value for the bottom of the tunnel generation
int GenerateNewPointBottom()
{
	int y = NULL;

	//Constrain the Y placement so that there is room for potential obstacles, magnets or player movement.
	y = (rand() % 66) + 320;

	return y;
}

/*main generation of bottom of tunnel. Sets number of points to plot, hard sets first point in level, moves the current point to the new point 
  once generated. 1 in 66 points generated will result in == Y values, this creates flat spots in the tunnel design. */
void PlotPointsBottom(Point oldP, Point newP, Point pointsBot[])
{
	char logStringbuffer[50];
	logStringbuffer[0] = 0;

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

	Logger::Log("Points - BOTTOM", Logger::logLevelInfo);
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
		sprintf(logStringbuffer, "PointsBot[ %i ] = (%i,%i)", i, newP.x, newP.y);  
		Logger::Log(logStringbuffer, Logger::logLevelInfo);
		memset(logStringbuffer, 0, sizeof(logStringbuffer));
	}	
	Logger::ShutdownLogger();
}

/*Main generation of top of tunnel. Sets number of points to plot, hard sets first point in level, moves the current point to the new point
  once generated. 1 in 66 points generated will result in == Y values, this creates flat spots in the tunnel design. */
void PlotPointsTop(Point oldP, Point newP, Point points[])

{
	char logStringbuffer[50];
	logStringbuffer[0] = 0;

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

	Logger::Log("Points - TOP", Logger::logLevelInfo);

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
		sprintf(logStringbuffer, "PointsTop[ %i ] = (%i,%i)", i, newP.x, newP.y);  
		Logger::Log(logStringbuffer, Logger::logLevelInfo);
		memset(logStringbuffer, 0, sizeof(logStringbuffer));
	}	
	Logger::ShutdownLogger();
}

//Draws lines from point to point. Connects top of tunnel. Uses Allegro al_draw_line() to do it. 
void ConnectPointsTop(Point points[])
{
	for(int i = 0; i < NUM_POINTS; i++)
	{
		al_draw_line(points[i].x, points[i].y, points[i+1].x, points[i + 1].y, al_map_rgb(255,0,255), 2);
	}
}

//Draws lines from point to point. Connects bottom of tunnel. Uses Allegro al_draw_line() to do it. 
void ConnectPointsBottom(Point pointsBot[])
{
	for (int i = 0; i < NUM_POINTS; i++)
	{
		al_draw_line(pointsBot[i].x, pointsBot[i].y, pointsBot[i + 1].x, pointsBot[i + 1].y, al_map_rgb(255,0,255), 2);
	}
}

//Takes array of obsticles generated and draws them to the screen. Obsticles reside inside the tunnel for the player to dodge.
void DrawObsticles(Point obsticles[])
{
	for (int i = 0; i < NUM_OBSTICLES; i++)
	{
		al_draw_filled_rectangle(obsticles[i].x, obsticles[i].y, obsticles[i].x + 20, obsticles[i].y + 20, al_map_rgb(255,0,255));
	}
}

//Generates and populates obsticles[]. Randomly places obsticles within the level. 
void GenerateObsticles(Point obsticles[])
{
	for (int i = 0; i < NUM_OBSTICLES; i++)
	{
		obsticles[i].x = ((rand() % (WIDTH - 30)) + 100);
		obsticles[i].y = HEIGHT / 2 - ((rand() % 25) + 20);
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

//TODO: Turn magnets into a Factory class
#pragma region Magnets

//Generate random X value for Magnet location
int GetMagnetLocationX()
{
	int x = (rand() % WIDTH) + 30;

	return x;
}

//Generate random Y value for magnet location
int GetMagnetLocationY()
{
	int y = (rand() % 60) + 15;

	return y;
}

//Draw the magnets to the screen, using Allegro's al_draw_filled_rectangle
void DrawMagnets(Magnet magnets[], Magnet magnetsBot[])
{
	for(int i = 0; i < NUM_MAGNETS; ++i)
	{
		al_draw_filled_rectangle(magnets[i].magnetX, magnets[i].magnetY, magnets[i].magnetX + 20, magnets[i].magnetY + 20, al_map_rgb(0,255,255));
		al_draw_filled_rectangle(magnetsBot[i].magnetX, magnetsBot[i].magnetY, magnetsBot[i].magnetX + 20, magnetsBot[i].magnetY + 20, al_map_rgb(0, 255, 255));
	}
}

void SetupMagnetsTop()
{
	char logStringBuffer[50];
	logStringBuffer[0] = 0;

	bool polaricCharge = POSITIVE;

	Logger::Log("MAGNETS - TOP", Logger::logLevelInfo);

	for (int i = 0; i < NUM_MAGNETS; i++)
	{
		Magnet *topMagnet = topFactory.requestMagnet();

		if (i % 2 == 0)
			polaricCharge != polaricCharge;

		topMagnet->InitializeMagnet(i, GetMagnetLocationX(), GetMagnetLocationY(), 10, 1.9f, polaricCharge);
		topMagnets[i] = *topMagnet;


		sprintf(logStringBuffer, "MagnetTop[ %i ] = (%i,%i)", i, topMagnet->magnetX, topMagnet->magnetY);  
		Logger::Log(logStringBuffer, Logger::logLevelInfo);
		memset(logStringBuffer, 0, sizeof(logStringBuffer));
	}

	Logger::ShutdownLogger();
}

#pragma endregion

#pragma region Helper Functions

//Returns distance between two points p1 and p2
Point GetPointDistance(Point p1, Point p2)
{
	Point temp;

	if(p2.y > p1.y)
	{
		temp.x = abs((p2.x - p1.x));
		temp.x /= 2;
		temp.y = abs((p2.y - p1.y));
		temp.y /= 2;
		temp.x = p2.x - temp.x;
		temp.y = p2.y - temp.y;
	}
	else
	{
		temp.x = abs((p2.x - p1.x));
		temp.x /= 2;
		temp.y = abs((p2.y - p1.y));
		temp.y /= 2;
		temp.x = p2.x - temp.x;
		temp.y = p1.y - temp.y;
	}
	return temp;

}

bool CollideTunnelTop(Point points[], SpaceShip &ship)
{
	for(int i = 0; i < NUM_POINTS; i++)
	{
		if(points[i + 1].y < points[i].y)
		{
			if( (ship.pos.x > points[i].x) &&
				(ship.pos.x < points[i + 1].x))
			{
				if( (ship.pos.y > points[i + 1].y) &&
					(ship.pos.y < points[i].y))
				{
					Point distanceBetweenLinePoints = ( (GetPointDistance(points[i], points[i + 1])));
					float slope = GetLineSlope(points[i], points[i + 1]);
					float perpSlope = GetPerpedicularSlope(slope);
					float lineIntercept =  GetYIntercept(points[i], slope);
					float perpLineIntercept = GetYIntercept(ship.pos, perpSlope);
					Point intersection = GetLineIntersection(points[i].x, points[i].y, slope, lineIntercept, 
											points[i + 1].x, points[i + 1].y, perpSlope, perpLineIntercept);

					al_draw_line(intersection.x, intersection.y, ship.pos.x, ship.pos.y, al_map_rgb(255, 255, 0), 1.0f);
					al_draw_line(points[i].x, points[i].y, ship.pos.x, ship.pos.y, al_map_rgb(0,255,255), 1.0f);
					al_draw_line(points[i + 1].x, points[i+1].y, ship.pos.x, ship.pos.y, al_map_rgb(0,255,255), 1.0f);
					al_draw_line(distanceBetweenLinePoints.x, distanceBetweenLinePoints.y, ship.pos.x, ship.pos.y, al_map_rgb(255,0,0), 1.0f);
				}
			}
		}
		else 
		{
			if( (ship.pos.x < points[i + 1].x) &&
				(ship.pos.x > points[i].x))
			{
				if( (ship.pos.y < points[i + 1].y) &&
					(ship.pos.y > points[i].y))
				{
					
					Point distanceBetweenLinePoints = ( (GetPointDistance(points[i], points[i + 1])));
					float slope = GetLineSlope(points[i], points[i + 1]);
					float perpSlope = GetPerpedicularSlope(slope);
					float lineIntercept =  GetYIntercept(points[i], slope);
					float perpLineIntercept = GetYIntercept(ship.pos, perpSlope);
					Point intersection = GetLineIntersection(points[i].x, points[i].y, slope, lineIntercept, 
											points[i + 1].x, points[i + 1].y, perpSlope, perpLineIntercept);

					al_draw_line(intersection.x, intersection.y, ship.pos.x, ship.pos.y, al_map_rgb(255, 255, 0), 1.0f);
					al_draw_line(points[i].x, points[i].y, ship.pos.x, ship.pos.y, al_map_rgb(0,255,255), 1.0f);
					al_draw_line(points[i + 1].x, points[i+1].y, ship.pos.x, ship.pos.y, al_map_rgb(0,255,255), 1.0f);
					al_draw_line(distanceBetweenLinePoints.x, distanceBetweenLinePoints.y, ship.pos.x, ship.pos.y, al_map_rgb(255,0,0), 1.0f);
				}
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
				//collide
				return true;
			}
		}
	}
	return false;
}

//Returns the slope of the line based on points p1 and p2
float GetLineSlope(Point p1, Point p2)
{	
	float slope;
	float yDistance, xDistance;
	yDistance = (p2.y - p1.y);
	xDistance = (p2.x - p1.x);
	slope = yDistance / xDistance;
	
	return slope;
}

//Returns the Y intercept of line based on a point of the line and its slope
float GetYIntercept(Point p1, float slope)
{
	float intercept;

	intercept = (slope * p1.x) - p1.y;
	intercept *= -1;

	return intercept;
}

//returns the slope of a line that is perpendicular to slope of line passed to function.
float GetPerpedicularSlope(float slope)
{
	return ( -1 / slope);
}

Point GetLineIntersection(int x1, int y1, float slope, float intercept, int x2, int y2, float perpSlope, float perpIntercept)
{
	Point intersection;
	//y1 = slope * x1 + intercept - convert to y1 - slope * x1 = intercept
	intersection.y = slope * x1 + intercept;
	intersection.x = y2 = perpSlope * intersection.y + perpIntercept;
	intersection.y = slope * intersection.x + intercept;

	return intersection;

}

#pragma endregion