#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include "Logger.h"
#include "polaris.h"
#include "core.h"
#include "MagnetFactory.h"
#include "Spaceship.h"
#include "ScoreCounter.h"

using namespace std;
using namespace PolarisEngine;

#pragma region Globals

//screen dimensions
int Width = 1280;
int Height = 500;

const int SCREENWIDTH = 1280;
const int SCREENHEIGHT = 500;

ALLEGRO_FONT *font;

//level dimensions
const int LEVELWIDTH = 8190;
const int LEVELHEIGHT = 500;

//Camera position
int cameraX = 0;
int cameraY = 0;

//how many points and spacing between points for lines drawn
const int PLOT_INTERVAL = 36;
const int NUM_POINTS = 220;

//Magnet consts
const int NUM_MAGNETS = 3;
Magnet topMagnets[NUM_MAGNETS];
Magnet botMagnets[NUM_MAGNETS];

//obstacle consts
const int NUM_OBSTACLES = 2;
const int BUTTON_TIME = 2.5f;

enum KEYS {UP, DOWN, LEFT, RIGHT, SPACE, H };
bool keys[5] = {false, false, false, false, false};

TopMagnetFactory topFactory;
BottomMagnetFactory bottomFactory;

//Object IDs
enum IDS {PLAYER, MAGNET};
enum Polarity {POSITIVE, NEGATIVE};

//Score increase per cycle amount
const float SCORE_INCREMENT = 1.0f;
const float timeDelayLimit = 1.0f;
float timeDelay = 0.0f;

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
void Drawobstacles(Vector3 vectorobstacles[]);
void Generateobstacles(Vector3 vectorobstacles[]);

//camera functions
//Point TranslateWorldToScreen(int objectX, int objectY, int cameraX, int cameraY);
//Point UpdateCamera(int x, int y, SpaceShip &ship);

//Magnet Functions
int GetMagnetLocationX();
int GetMagnetLocationY();
void DrawMagnets(Magnet magnets[], Magnet magnetsBot[]);
void SetupMagnetsTop();
void SetUpMagnetsBottom();

//Helper Functions
Vector3 GetVectorDistance(Vector3 firstPosition, Vector3 secondPosition);
bool CheckCollisionsTop(SpaceShip &ship, Vector3 pointOne, Vector3 pointTwo);
bool CheckCollisionsBottom(SpaceShip &ship, Vector3 pointOne, Vector3 pointTwo);
void DrawScore(float score, int currentX);

//==========================================================================================

#pragma endregion 

int main(void)
{
	#pragma region GameVariables
	//primitive variable
	bool done = false;
	bool redraw = true;
	const int FPS = 60;
	
	//procedual tunnel variables
	Vector3 oldPosition, newPosition;
	Vector3 topPoints[NUM_POINTS] = {};
	Vector3 bottomPoints[NUM_POINTS] = {};
	Vector3 vectorobstacles[NUM_OBSTACLES];

	//object variables
	PolarisEngine::Vector3 shipStartingPosition;
	shipStartingPosition.x = 20;
	shipStartingPosition.y = SCREENHEIGHT / 2;
	shipStartingPosition.z = 0;
	SpaceShip ship(shipStartingPosition, 15, 15, 7, 3, NEGATIVE);
	
	//Setup game starting score
	ScoreCounter gameScore(0);

	bool polarity = false;
	float buttonTimer = 0.0f;
	bool collide = false;
	bool collideBot = false;

	int currentX = 0;
	int currentY = 0;
#pragma endregion

	//allegro variables
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_BITMAP *backBuffer = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;	

	//Initialization functions
	if(!al_init())
		return -1;

	display = al_create_display(SCREENWIDTH, SCREENHEIGHT);
	backBuffer = al_create_bitmap(LEVELWIDTH, LEVELHEIGHT);

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

	#pragma region LoggerInitialize
	//If log file currently exists, delete it for a fresh copy
	if(remove("log.txt") != 0)
		cout << "error deleting log.txt file";
	else 
		cout << "file successfully deleted";

	Logger::Log("Seed is 64789", Logger::logLevelInfo);
	Logger::ShutdownLogger();
	
#pragma endregion

	#pragma region GameInitialize
	//Font used for Debug display
	font = al_load_font("arial.ttf", 16, 0);
	
	//Initialize the magnets on the map
	SetupMagnetsTop();
	SetUpMagnetsBottom();

	//Place the center obstacles
	Generateobstacles(vectorobstacles);

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
#pragma endregion

	#pragma region Game Loop

	while(!done)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		#pragma region Update Loop
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
			{
				ship.MoveShipLeft();
				if (ship.shipPos.x <= 20)
					ship.shipPos.x = 20;
			}
			if(keys[RIGHT])
			{
				ship.MoveShipRight();
				if (ship.shipPos.x >= LEVELWIDTH)
					ship.shipPos.x = LEVELWIDTH;
			}
			if(keys[SPACE])
			{
				if(buttonTimer >= BUTTON_TIME)
				{
					buttonTimer = 0.0f;
					ship.TogglePolaricCharge();	
				}
			}		
			if (ship.shipPos.y < 190)
			{
				for (int i = 0; i < NUM_POINTS; i++)
				{
					if (ship.shipPos.x > topPoints[i + 1].x)
						continue;
					if (ship.shipPos.x < topPoints[i].x)
						continue;
					
					collide = CheckCollisionsTop(ship, topPoints[i], topPoints[i + 1]);
				}
			}
			else if (ship.shipPos.y > SCREENHEIGHT / 2 + 60)
			{
				for (int i = 0; i < NUM_POINTS; i++)
				{
					if (ship.shipPos.x > bottomPoints[i + 1].x)
						continue;
					if (ship.shipPos.x < bottomPoints[i].x)
						continue;
					collide = CheckCollisionsBottom(ship, bottomPoints[i], bottomPoints[i + 1]);
				}
			}		
			
			timeDelay += 0.1f;
			if (timeDelay >= timeDelayLimit)
			{
				gameScore.UpdateScoreCounter(SCORE_INCREMENT);
				timeDelay = 0.0f;
			}

			if (ship.shipPos.x > SCREENWIDTH / 2 && keys[RIGHT])
			{
				currentX += ship.GetSpeed();
			}
			else if (ship.shipPos.x < currentX + (SCREENWIDTH / 2) && keys[LEFT])
			{
				if (currentX <= 0)
					currentX = 0;
				else
					currentX -= ship.GetSpeed();
			}

			if (LEVELWIDTH - (SCREENWIDTH / 2) < currentX)
				currentX = LEVELWIDTH - (SCREENWIDTH / 2);
		}
#pragma endregion

		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			done = true;
		}

		#pragma region Keysets
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
#pragma endregion

		#pragma region DrawLoop
		if(redraw && al_event_queue_is_empty(event_queue))
		{
			redraw = false;

			al_set_target_bitmap(backBuffer);
			al_clear_to_color(al_map_rgb(0,0,0));
			
			//Draw static position score in bottom right corner of screen
			DrawScore(gameScore.GetScore(), currentX);
			
			ship.DrawShip();

			ConnectPointsTop(topPoints);
			ConnectPointsBottom(bottomPoints);
			
			if (collide)
			{
				al_draw_text(font, al_map_rgb(255, 255, 110), currentX + (SCREENWIDTH / 2), SCREENHEIGHT / 2, 0, "COLLISION ");
				al_draw_textf(font, al_map_rgb(255,100,100), currentX + 20, 50, 0, "ShipPos: ( %g, %g, %g)", ship.shipPos.x, ship.shipPos.y, ship.shipPos.z);
			}

			al_draw_textf(font, al_map_rgb(45, 120, 200), currentX + 20, SCREENHEIGHT - 100, 0, "currentX = %i", currentX);
			al_draw_line(currentX, 10, currentX, SCREENHEIGHT, al_map_rgb(255,0,0), 3);
			al_draw_line(LEVELWIDTH, 0, LEVELWIDTH, LEVELHEIGHT, al_map_rgb(0,0,255), 10);

			DrawMagnets(topMagnets, botMagnets);
			
			Drawobstacles(vectorobstacles);

			al_set_target_bitmap(al_get_backbuffer(display));

			//al_draw_bitmap(backBuffer, 0, 0, 0);
			al_draw_bitmap_region(backBuffer, currentX, currentY, currentX + SCREENWIDTH, currentY + SCREENHEIGHT, 0, 0, 0);
			al_flip_display();
			al_clear_to_color(al_map_rgb(0,0,0));
		}
#pragma endregion
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
	
	Logger::Log("\n", Logger::logLevelDebug);

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
		sprintf_s(logStringBuffer, "PointsBot[ %i ] = (%g,%g)", i, newPosition.x, newPosition.y);  
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

	Logger::Log("\n", Logger::logLevelDebug);
	
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

	Logger::Log("Points - TOP", Logger::logLevelInfo);
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
		sprintf_s(logStringBuffer, "PointsTop[ %i ] = (%g,%g)", i, pointsTop[i].x, pointsTop[i].y);  
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
		/*
			The line below is only used for debugging to visually see where each point is located throughout the ceiling.
		al_draw_line(points[i].x, points[i].y - 20, points[i].x, points[i].y + 20, al_map_rgb(0,255,255), 3);

		*/
	}
}

//Draws lines from point to point. Connects bottom of tunnel. Uses Allegro al_draw_line() to do it. 
void ConnectPointsBottom(Vector3 pointsBot[])
{
	for (int i = 0; i < NUM_POINTS; i++)
	{
		al_draw_line(pointsBot[i].x, pointsBot[i].y, pointsBot[i + 1].x, pointsBot[i + 1].y, al_map_rgb(255,0,255),2);
		/*
			The line below is only used for visually displaying where each point is on the floor. 
		al_draw_line(pointsBot[i].x, pointsBot[i].y - 20, pointsBot[i].x, pointsBot[i].y + 20, al_map_rgb(0,255,255), 3);
		*/
	}
}

//Takes array of obstacles generated and draws them to the screen. obstacles reside inside the tunnel for the player to dodge.
void Drawobstacles(Vector3 obstacles[])
{
	for (int i = 0; i < NUM_OBSTACLES; i++)
	{
		al_draw_filled_rectangle(obstacles[i].x, obstacles[i].y, obstacles[i].x + 20, obstacles[i].y + 20, al_map_rgb(255,0,255));
	}
}

//Generates and populates obstacles[]. Randomly places obstacles within the level. 
void Generateobstacles(Vector3 obstacles[])
{
	for (int i = 0; i < NUM_OBSTACLES; i++)
	{
		obstacles[i].x = ((rand() % (Width - 30)) + 100);
		obstacles[i].y = Height / 2 - ((rand() % 25) + 20);
	}
}

#pragma endregion

#pragma region Camera

//Point TranslateWorldToScreen(int objectX, int objectY, int cameraX, int cameraY)
//{
//	Point newPoint;
//	newPoint.x = objectX - cameraX;
//	newPoint.y = objectY - cameraY;
//
//	return newPoint;
//}
//
//Point UpdateCamera(int x, int y, SpaceShip &ship)
//{
//	x = ship.GetShipPosition().x - WIDTH / 2;
//	y = ship.GetShipPosition().y - HEIGHT / 2;
//
//	if(x < 0) x = 0;
//	if(y < 0) y = 0;
//	if(x > LEVELWIDTH - WIDTH) x = LEVELWIDTH - WIDTH;
//	if(y > LEVELHEIGHT - HEIGHT) y = LEVELHEIGHT - HEIGHT;
//
//	Point CameraPoint;
//	CameraPoint.x = x;
//	CameraPoint.y = y;
//
//	return CameraPoint;
//}

#pragma endregion

#pragma region Magnets

//Generate random X value for Magnet location
int GetMagnetLocationX()
{
	int x = (rand() % Width) + 30;

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

	Logger::Log("\n", Logger::logLevelDebug);

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

	Logger::Log("\n", Logger::logLevelDebug);

	bool polaricCharge = NEGATIVE;
	
	Logger::Log("Magnets - BOTTOM", Logger::logLevelInfo);

	int magnetYPositionOffset = 370;

	for (int i = 0; i < NUM_MAGNETS; i++)
	{
		Magnet *bottomMagnet = bottomFactory.requestMagnet();

		if (i % 2 == 0)
			polaricCharge = !polaricCharge;
		
		int position = bottomMagnet[i - 1].magnetX;
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

bool CheckCollisionsTop(SpaceShip &ship, Vector3 pointOne, Vector3 pointTwo)
{
	Vector3 lineSegment = GetVectorDistance(pointOne, pointTwo);
	Vector3 shipWidthPosition(ship.shipPos.x + ship.GetWidth(), ship.shipPos.y, ship.shipPos.z);

	if (pointTwo.y < pointOne.y) //upwards line
	{
		Vector3 lineNormal(lineSegment.y, -lineSegment.x, 0);
		double u = GetVectorDistance(ship.shipPos, pointTwo) * lineNormal;

		if (u > 0)
			return false;
		else if (u < 0)
			return true;
	}
	else if (pointTwo.y > pointOne.y) //downwards line
	{
		Vector3 lineNormal(lineSegment.y, -lineSegment.x, 0);
		double u = GetVectorDistance(shipWidthPosition, pointOne) * lineNormal;

		if (u > 0)
			return false;
		else if (u < 0)
			return true;
	}
}

bool CheckCollisionsBottom(SpaceShip &ship, Vector3 pointOne, Vector3 pointTwo)
{
	Vector3 lineSegment = GetVectorDistance(pointOne, pointTwo);
	Vector3 shipHeightPosition(ship.shipPos.x, ship.shipPos.y + ship.GetHeight(), ship.shipPos.z);
	Vector3 shipWidthHeightPosition(ship.shipPos.x + ship.GetWidth(), ship.shipPos.y + ship.GetHeight(), ship.shipPos.z);

	if (pointTwo.y < pointOne.y) //upwards line
	{
		Vector3 lineNormal(-lineSegment.y, lineSegment.x, 0);
		double u = GetVectorDistance(shipWidthHeightPosition, pointTwo) * lineNormal;

		if (u > 0)
			return false;
		else if (u < 0)
			return true;
	}
	else if (pointTwo.y > pointOne.y) //downwards line
	{
		Vector3 lineNormal(-lineSegment.y, lineSegment.x, 0);
		double u = GetVectorDistance(shipHeightPosition, pointTwo) * lineNormal;

		if (u > 0)
			return false;
		else if (u < 0)
			return true;
	}
}

//Returns distance between two points p1 and p2
Vector3 GetVectorDistance(Vector3 vectorOne, Vector3 vectorTwo)
{
	Vector3 temp;

	temp.x = vectorTwo.x - vectorOne.x;
	temp.y = vectorTwo.y - vectorOne.y;
	temp.z = vectorTwo.z - vectorOne.z;
	return temp;
}

void DrawScore(float score, int currentX)
{
	al_draw_textf(font, al_map_rgb(145,58,83), currentX + (SCREENWIDTH - 100), SCREENHEIGHT - 30, 0, "Score: %g", score);
}

#pragma endregion