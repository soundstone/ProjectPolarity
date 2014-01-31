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
#include "Spaceship.h"

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
const int BUTTON_TIME = 2.5f;

enum KEYS {UP, DOWN, LEFT, RIGHT, SPACE };
bool keys[5] = {false, false, false, false, false};

TopMagnetFactory topFactory;
BottomMagnetFactory bottomFactory;

//================================================================

#pragma endregion

#pragma region Prototypes

//Tunnel generation functions
void PlotPointsTop(Vector3 oldPosition, Vector3 newPosition, Vector3 pointsTop[]);
int  GenerateNewPointTop();
void ConnectPointsTop(Vector3 pointsTop[]);
void PlotPointsBottom(Vector3 oldPosition, Vector3 newPosition, Vector3 pointsBottom[]);
int  GenerateNewPointBottom();
void ConnectPointsBottom(Vector3 pointsBottom[]);
void DrawObsticles(Vector3 vectorObsticles[]);
void GenerateObsticles(Vector3 vectorObsticles[]);

//camera functions
Point TranslateWorldToScreen(int objectX, int objectY, int cameraX, int cameraY);
Point UpdateCamera(int x, int y, SpaceShip &ship);

//Magnet Functions
int GetMagnetLocationX();
int GetMagnetLocationY();
void DrawMagnets(Magnet magnets[], Magnet magnetsBot[]);
void SetupMagnetsTop();
void SetUpMagnetsBottom();

//Helper Functions
Vector3 GetVectorDistance(Vector3 firstPosition, Vector3 secondPosition);
bool CollideTunnelTop(Point points[], SpaceShip &ship);
bool CollideTunnelBottom(Point points[], SpaceShip &ship);
float GetLineSlope(Point p1, Point p2);
float GetYIntercept(Point p1, float slope);
bool IsOnLine(int boxX, int boxY, Point p1, Point p2);

float GetPerpedicularSlope(float slope);
Point GetLineIntersection(int x1, int y1, float slope, float intercept, int x2, int y2, float perpSlope, float perpIntercept);
//Vector3 GetLineIntersection(Vector3 originalPosition, float originalSlope, float originalEquationIntercept, Vector3 perpendicularPosition, 
//							float perpendicularSlope, float perpendicularIntercept);


//==========================================================================================

#pragma endregion 

int main(void)
{
	//primitive variable
	bool done = false;
	bool redraw = true;
	const int FPS = 60;
	
	//procedual tunnel variables
	Vector3 oldPosition, newPosition;
	Vector3 topPoints[NUM_POINTS] = {};
	Vector3 bottomPoints[NUM_POINTS] = {};
	Vector3 vectorObsticles[NUM_OBSTICLES];

	//object variables
	PolarisEngine::Vector3 shipStartingPosition;
	shipStartingPosition.x = 20;
	shipStartingPosition.y = HEIGHT / 2;
	shipStartingPosition.z = 0;
	SpaceShip ship(shipStartingPosition, 15, 15, 7, 3, NEGATIVE);
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

	Logger::Log("Seed is 64789", Logger::logLevelInfo);
	Logger::ShutdownLogger();
	
	//Font used for Debug display
	font = al_load_font("arial.ttf", 16, 0);
	
	//Initialize the magnets on the map
	SetupMagnetsTop();
	SetUpMagnetsBottom();

	//Place the center obstacles
	GenerateObsticles(vectorObsticles);

	//set up points to initial null status
	oldPosition.x = oldPosition.y = NULL;
	oldPosition.z = 0;
	newPosition.x = newPosition.y = NULL;
	newPosition.z = 0;

	//These two function calls generate and draw the points in the array and connect them by drawing a line to each of them.
	PlotPointsTop(oldPosition, newPosition, topPoints);
	PlotPointsBottom(oldPosition, newPosition, bottomPoints);
	
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
				ship.MoveShipUp();
			if(keys[DOWN])
				ship.MoveShipDown();
			if(keys[LEFT])
				ship.MoveShipLeft();
			if(keys[RIGHT])
				ship.MoveShipRight();
			if(keys[SPACE])
			{
				if(buttonTimer >= BUTTON_TIME)
				{
					buttonTimer = 0.0f;
					ship.TogglePolaricCharge();	
				}
			}		

			//collide = CollideTunnelTop(topPoints, ship);
			//collideBot = CollideTunnelBottom(bottomPoints, ship);

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

			ship.DrawShip();

			ConnectPointsTop(topPoints);
			ConnectPointsBottom(bottomPoints);
			
			DrawMagnets(topMagnets, botMagnets);
			
			DrawObsticles(vectorObsticles);

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
void PlotPointsBottom(Vector3 oldPosition, Vector3 newPosition, Vector3 pointsBottom[])
{
	char logStringBuffer[50];
	logStringBuffer[0] = 0;

	if(newPosition.x >= 4000)
		return;
	
	//Generate the starting point. Located at the far left of screen. 
	if(oldPosition.x == NULL)
	{
		oldPosition.x = 0;
		oldPosition.y = GenerateNewPointBottom();
	}

	pointsBottom[0] = oldPosition;
	newPosition.x = oldPosition.x;

	Logger::Log("Points - BOTTOM", Logger::logLevelInfo);
	for (int i = 1; i < NUM_POINTS; i++)
	{
		//One in 66 the points will not vary in height. This cause flat areas, and makes the lines look more cavelike. 
		if(rand() % 66 == 0)
		{
			for(int j = 0; j < 3; j++)
			{
				//Move the X coord to the next PLOT_INTERVAL. Adding 12 during testing. Might need to be implemented through PLOT_INTERVAL
				newPosition.x = newPosition.x + PLOT_INTERVAL + 12;
				
				//Keeps the height the same. No change in slope.
				newPosition.y = oldPosition.y;
				
				//add the new point to the bottom of the map
				pointsBottom[i] = newPosition;
			}
		}
		//Generates new points until pointsBot[i] reaches 4000 points. These vary in height.
		else 
		{
			newPosition.x = newPosition.x + PLOT_INTERVAL;

			oldPosition.y = GenerateNewPointBottom();
			newPosition.y = GenerateNewPointBottom();
			
			pointsBottom[i] = newPosition;
		}
		sprintf_s(logStringBuffer, "PointsBot[ %i ] = (%d,%d, %d)", i, newPosition.x, newPosition.y, newPosition.z);  
		Logger::Log(logStringBuffer, Logger::logLevelInfo);
		memset(logStringBuffer, 0, sizeof(logStringBuffer));
	}	
	Logger::ShutdownLogger();
}

/*Main generation of top of tunnel. Sets number of points to plot, hard sets first point in level, moves the current point to the new point
  once generated. 1 in 66 points generated will result in == Y values, this creates flat spots in the tunnel design. */
void PlotPointsTop(Vector3 oldPosition, Vector3 newPosition, Vector3 pointsTop[])
{
	char logStringBuffer[50];
	logStringBuffer[0] = 0;

	if(newPosition.x >= 4000)
		return;
	
	//Generate the starting point. Located at the far left of screen. 
	if(oldPosition.x == NULL)
	{
		oldPosition.x = 0;
		oldPosition.y = GenerateNewPointTop();
	}

	pointsTop[0] = oldPosition;
	newPosition.x = oldPosition.x;

	Logger::Log("Points - BOTTOM", Logger::logLevelInfo);
	for (int i = 1; i < NUM_POINTS; i++)
	{
		//One in 66 the points will not vary in height. This cause flat areas, and makes the lines look more cavelike. 
		if(rand() % 66 == 0)
		{
			for(int j = 0; j < 3; j++)
			{
				//Move the X coord to the next PLOT_INTERVAL. Adding 12 during testing. Might need to be implemented through PLOT_INTERVAL
				newPosition.x = newPosition.x + PLOT_INTERVAL + 12;
				
				//Keeps the height the same. No change in slope.
				newPosition.y = oldPosition.y;
				
				//add the new point to the bottom of the map
				pointsTop[i] = newPosition;
			}
		}
		//Generates new points until pointsBot[i] reaches 4000 points. These vary in height.
		else 
		{
			newPosition.x = newPosition.x + PLOT_INTERVAL;

			oldPosition.y = GenerateNewPointTop();
			newPosition.y = GenerateNewPointTop();
			
			pointsTop[i] = newPosition;
		}
		sprintf_s(logStringBuffer, "PointsBot[ %i ] = (%d,%d, %d)", i, newPosition.x, newPosition.y, newPosition.z);  
		Logger::Log(logStringBuffer, Logger::logLevelInfo);
		memset(logStringBuffer, 0, sizeof(logStringBuffer));
	}	
	Logger::ShutdownLogger();
}

void ConnectPointsTop(Vector3 points[])
{
	for (int i = 0; i < NUM_POINTS; i++)
	{
		al_draw_line(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y, al_map_rgb(255,0,255), 2);
	}
}

//Draws lines from point to point. Connects bottom of tunnel. Uses Allegro al_draw_line() to do it. 
void ConnectPointsBottom(Vector3 pointsBot[])
{
	for (int i = 0; i < NUM_POINTS; i++)
	{
		al_draw_line(pointsBot[i].x, pointsBot[i].y, pointsBot[i + 1].x, pointsBot[i + 1].y, al_map_rgb(255,0,255),2);
	}
}

//Takes array of obsticles generated and draws them to the screen. Obsticles reside inside the tunnel for the player to dodge.
void DrawObsticles(Vector3 obsticles[])
{
	for (int i = 0; i < NUM_OBSTICLES; i++)
	{
		al_draw_filled_rectangle(obsticles[i].x, obsticles[i].y, obsticles[i].x + 20, obsticles[i].y + 20, al_map_rgb(255,0,255));
	}
}

//Generates and populates obsticles[]. Randomly places obsticles within the level. 
void GenerateObsticles(Vector3 obsticles[])
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
	x = ship.GetShipPosition().x - WIDTH / 2;
	y = ship.GetShipPosition().y - HEIGHT / 2;

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
			polaricCharge = !polaricCharge;

		topMagnet->InitializeMagnet(i, GetMagnetLocationX(), GetMagnetLocationY(), 10, 1.9f, polaricCharge);
		topMagnets[i] = *topMagnet;


		sprintf_s(logStringBuffer, "MagnetTop[ %i ] = (%i,%i)", i, topMagnet->magnetX, topMagnet->magnetY);  
		Logger::Log(logStringBuffer, Logger::logLevelInfo);
		memset(logStringBuffer, 0, sizeof(logStringBuffer));
	}

	Logger::ShutdownLogger();
}

void SetUpMagnetsBottom()
{
	char logStringBuffer[50];
	logStringBuffer[0] = 0;

	bool polaricCharge = NEGATIVE;
	
	Logger::Log("Magnets - BOTTOM", Logger::logLevelInfo);

	int magnetYPositionOffset = 370;

	for (int i = 0; i < NUM_MAGNETS; i++)
	{
		Magnet *bottomMagnet = bottomFactory.requestMagnet();

		if (i % 2 == 0)
			polaricCharge = !polaricCharge;

		bottomMagnet->InitializeMagnet(i, GetMagnetLocationX(), GetMagnetLocationY() + magnetYPositionOffset, 10, 1.9f, polaricCharge);
		botMagnets[i] = *bottomMagnet;


		sprintf_s(logStringBuffer, "MagnetTop[ %i ] = (%i,%i)", i, bottomMagnet->magnetX, bottomMagnet->magnetY);  
		Logger::Log(logStringBuffer, Logger::logLevelInfo);
		memset(logStringBuffer, 0, sizeof(logStringBuffer));
	}

	Logger::ShutdownLogger();
}

#pragma endregion

#pragma region Helper Functions

//Returns distance between two points p1 and p2
Vector3 GetVectorDistance(Vector3 vectorOne, Vector3 vectorTwo)
{
	Vector3 temp;

	temp.x = abs((vectorTwo.x - vectorOne.x));
	temp.x /= 2;
	temp.y = abs((vectorTwo.y - vectorOne.y));
	temp.y /= 2;
	temp.x = vectorTwo.x - temp.x;
	if (vectorTwo.y > vectorOne.y)
	{
		temp.y = vectorTwo.y - temp.y;
	}
	else
	{
		temp.y = vectorOne.y - temp.y;
	}
	return temp;
}

//Point GetPointDistance(Point p1, Point p2)
//{
//	Point temp;
//
//	if(p2.y > p1.y)
//	{
//		temp.x = abs((p2.x - p1.x));
//		temp.x /= 2;
//		temp.y = abs((p2.y - p1.y));
//		temp.y /= 2;
//		temp.x = p2.x - temp.x;
//		temp.y = p2.y - temp.y;
//	}
//	else
//	{
//		temp.x = abs((p2.x - p1.x));
//		temp.x /= 2;
//		temp.y = abs((p2.y - p1.y));
//		temp.y /= 2;
//		temp.x = p2.x - temp.x;
//		temp.y = p1.y - temp.y;
//	}
//	return temp;
//
//}

//bool CollideTunnelTop(Point points[], SpaceShip &ship)
//{
//	for(int i = 0; i < NUM_POINTS; i++)
//	{
//		if(points[i + 1].y < points[i].y)
//		{
//			if( (ship.GetShipPosition().x > points[i].x) &&
//				(ship.GetShipPosition().x < points[i + 1].x))
//			{
//				if( (ship.GetShipPosition().y > points[i + 1].y) &&
//					(ship.GetShipPosition().y < points[i].y))
//				{
//					Point distanceBetweenLinePoints = ( (GetPointDistance(points[i], points[i + 1])));
//					float slope = GetLineSlope(points[i], points[i + 1]);
//					float perpSlope = GetPerpedicularSlope(slope);
//					float lineIntercept =  GetYIntercept(points[i], slope);
//					//float perpLineIntercept = GetYIntercept(ship.pos, perpSlope);
//				//	Point intersection = GetLineIntersection(points[i].x, points[i].y, slope, lineIntercept, 
//					/*						points[i + 1].x, points[i + 1].y, perpSlope, perpLineIntercept);
//
//					al_draw_line(intersection.x, intersection.y, ship.pos.x, ship.pos.y, al_map_rgb(255, 255, 0), 1.0f);
//					al_draw_line(points[i].x, points[i].y, ship.pos.x, ship.pos.y, al_map_rgb(0,255,255), 1.0f);
//					al_draw_line(points[i + 1].x, points[i+1].y, ship.pos.x, ship.pos.y, al_map_rgb(0,255,255), 1.0f);
//					al_draw_line(distanceBetweenLinePoints.x, distanceBetweenLinePoints.y, ship.pos.x, ship.pos.y, al_map_rgb(255,0,0), 1.0f);*/
//				}
//			}
//		}
//		else 
//		{
//			if( (ship.GetShipPosition().x < points[i + 1].x) &&
//				(ship.GetShipPosition().x > points[i].x))
//			{
//				if( (ship.GetShipPosition().y < points[i + 1].y) &&
//					(ship.GetShipPosition().y > points[i].y))
//				{
//					
//					Point distanceBetweenLinePoints = ( (GetPointDistance(points[i], points[i + 1])));
//					float slope = GetLineSlope(points[i], points[i + 1]);
//					float perpSlope = GetPerpedicularSlope(slope);
//					float lineIntercept =  GetYIntercept(points[i], slope);
//					//float perpLineIntercept = GetYIntercept(ship.pos, perpSlope);
//					//Point intersection = GetLineIntersection(points[i].x, points[i].y, slope, lineIntercept, 
//					//						points[i + 1].x, points[i + 1].y, perpSlope, perpLineIntercept);
//
//					//al_draw_line(intersection.x, intersection.y, ship.pos.x, ship.pos.y, al_map_rgb(255, 255, 0), 1.0f);
//					//al_draw_line(points[i].x, points[i].y, ship.pos.x, ship.pos.y, al_map_rgb(0,255,255), 1.0f);
//					//al_draw_line(points[i + 1].x, points[i+1].y, ship.pos.x, ship.pos.y, al_map_rgb(0,255,255), 1.0f);
//					//al_draw_line(distanceBetweenLinePoints.x, distanceBetweenLinePoints.y, ship.pos.x, ship.pos.y, al_map_rgb(255,0,0), 1.0f);
//				}
//			}
//		}
//	}
//	return false;
//}
//
//bool CollideTunnelBottom(Point points[], SpaceShip &ship)
//{
//	for (int i = 0; i < NUM_POINTS; i++)
//	{
//		if(points[i + 1].y < points[i].y)
//		{
//			//This is a preliminary check. Is the ship close enough to check for collision?
//			if(( ship.GetShipPosition().x > points[i].x) &&
//				(ship.GetShipPosition().x < points[i + 1].x) &&
//				(ship.GetShipPosition().y > points[i].y) &&
//				(ship.GetShipPosition().y < points[i + 1].y))
//			{
//				//Collide
//				return true;
//			}
//		}
//		else if(points[i + 1].y > points[i].y)
//		{
//			//preliminary check. Is ship close enough to check for collision?
//			if( (ship.GetShipPosition().x > points[i].x) &&
//				(ship.GetShipPosition().x < points[i + 1].x) &&
//				(ship.GetShipPosition().y + 10 < points[i + 1].y) &&
//				(ship.GetShipPosition().y - 10> points[i].y))
//			{
//				//collide
//				return true;
//			}
//		}
//	}
//	return false;
//}

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