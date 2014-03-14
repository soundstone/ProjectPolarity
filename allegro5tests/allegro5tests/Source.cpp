#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_image.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include "Logger.h"
#include "polaris.h"
#include "core.h"
#include "MagnetFactory.h"
#include "Spaceship.h"
#include "ScoreCounter.h"
#include "GameManager.h"
#include "MenuManager.h"

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
const int LEVELWIDTH = 4000 - SCREENWIDTH / 2;
const int LEVELHEIGHT = 500;

//Camera position
int cameraX = 0;
int cameraY = 0;

//how many points and spacing between points for lines drawn
const int PLOT_INTERVAL = 36;
const int NUM_POINTS = 120;

//Magnet variables
const int NUM_MAGNETS = 12;
Magnet topMagnets[NUM_MAGNETS];
Magnet botMagnets[NUM_MAGNETS];
PointCharge *topPointCharges[NUM_MAGNETS];
PointCharge *botPointCharges[NUM_MAGNETS];
bool inMagneticFieldTop;
bool inMagneticFieldBot;

//obstacle consts
const int NUM_OBSTACLES = 14;

const int BUTTON_TIME = 2.5f;
const int PAUSE_BUTTON_TIME = 1.0f;

enum KEYS {UP, DOWN, LEFT, RIGHT, SPACE, H, ENTER };
bool keys[7] = {false, false, false, false, false, false, false};

TopMagnetFactory topFactory;
BottomMagnetFactory bottomFactory;

//Object IDs
enum IDS {PLAYER, MAGNET};
enum Polarity {POSITIVE, NEGATIVE};

//Score increase per cycle amount
const float SCORE_INCREMENT = 1.0f;
const float timeDelayLimit = 1.0f;
float timeDelay = 0.0f;

struct BoundingBox
{
	int x1, x2;
	int y1, y2;

	BoundingBox()
	{
	}

	BoundingBox(int x, int y, int width, int height)
	{
		x1 = x;
		x2 = width;
		y1 = y;
		y2 = height;
	}
};


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
void Generateobstacles(Vector3 vectorobstacles[], BoundingBox boxes[]);

//Magnet Functions
int GetMagnetLocationX();
int GetMagnetLocationY();
void DrawMagnets(Magnet magnets[], Magnet magnetsBot[]);
void SetupMagnetsTop();
void SetUpMagnetsBottom();
void LogMagnet(Magnet magnet);


//Helper Functions
Vector3 GetVectorDistance(Vector3 firstPosition, Vector3 secondPosition);
bool CheckCollisionsTop(SpaceShip &ship, Vector3 pointOne, Vector3 pointTwo);
bool CheckCollisionsBottom(SpaceShip &ship, Vector3 pointOne, Vector3 pointTwo);
bool CheckObstacleCollision(BoundingBox shipBox, BoundingBox obstacle);
void SortMagnets(Magnet magnets[]);
void SpaceMagnets(Magnet magnets[], int size);
void SpaceObstacles(Vector3 o[], int size);
void Sort(Vector3 arr[], int size);
void ResetForce(SpaceShip &ship);
void ApplyForces(Magnet magnets, SpaceShip &ship);
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
	BoundingBox obstacleBoxes[NUM_OBSTACLES];

	//object variables
	PolarisEngine::Vector3 shipStartingPosition;
	shipStartingPosition.x = 20;
	shipStartingPosition.y = SCREENHEIGHT / 2;
	shipStartingPosition.z = 0;
	SpaceShip ship(shipStartingPosition, 15, 15, 7, 3, NEGATIVE, 0.5f, 1.0f);
	PointCharge shipPointCharge(shipStartingPosition.x, shipStartingPosition.y, 1);
	BoundingBox shipBoundingBox;
	GameManager gameManager;
	MenuManager menuManager;
	double distance  = 0;
	double force = 0;

	//Logo - SPLASH variables
	ALLEGRO_BITMAP *splashScreen = NULL;
	PolarisEngine::Vector3 logoCenter(SCREENWIDTH / 2, SCREENHEIGHT /2, 0);
	int alphaValue = 1;
	int fadeIncrement = 1;
	double fadeDelay = .030;
	int delayer = 200;
	int checkIntro = 0;
	double frameTime;
	
	//Setup game starting score
	ScoreCounter gameScore(0);

	bool polarity = false;
	float buttonTimer = 0.0f;
	bool collide = false;
	bool collideBot = false;
	bool collideObstacle = false;

	int currentX = 0;
	int currentY = 0;
	#pragma endregion

	#pragma region ALLEGRO Initialization
	//allegro variables
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_BITMAP *backBuffer = NULL;
	ALLEGRO_BITMAP *victoryFlash = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;	
	

	//Initialization functions
	if(!al_init())
		return -1;

	display = al_create_display(SCREENWIDTH, SCREENHEIGHT);
	backBuffer = al_create_bitmap(LEVELWIDTH + SCREENWIDTH / 2, LEVELHEIGHT);	

	if(!display)
		return -1;

	al_init_primitives_addon();
	al_install_keyboard();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);

	//Image for splash screen fade in.
	splashScreen = al_load_bitmap("RedTeam.png");
	if (!splashScreen)
		return -1;

	victoryFlash = al_load_bitmap("polaricFlash.png");
	if (!victoryFlash)
		return -1;

	//srand(time(NULL));
	/*
		Seed set to 64789 for testing purposes.
	*/
	srand(64789);
	#pragma endregion

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
	Generateobstacles(vectorobstacles, obstacleBoxes);

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

		#pragma region Receive_Input
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
			case ALLEGRO_KEY_ENTER:
				keys[ENTER] = true;
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
			case ALLEGRO_KEY_ENTER:
				keys[ENTER] = false;
			}
		}
		#pragma endregion

		#pragma region Splash
		if (gameManager.GetGameState() == SPLASH)
		{
			//fade in logo
			fadeDelay -= al_get_timer_count(timer);

			if (fadeDelay <= 0)
			{
				//reset fade delay
				fadeDelay = .045;

				//increment/decrement the fade value
				alphaValue += fadeIncrement;

				if (alphaValue <= 0)
					fadeIncrement *= -1;
			}

			if (delayer <= 400)
				delayer++;
			else 
				delayer = 401;

			frameTime = al_get_timer_count(timer);
			redraw = true;
		}
		#pragma endregion

		#pragma region MainMenu
		if (gameManager.GetGameState() == MAINMENU)
		{
			buttonTimer += 0.1f;
			if (buttonTimer >= BUTTON_TIME)
			{
				if (keys[UP])
				{
					buttonTimer = 0.0f;
					menuManager.currentMenuItem--;
					if (menuManager.currentMenuItem < 0)
						menuManager.currentMenuItem = 1;
					redraw = true;
				}
				if (keys[DOWN])
				{
					buttonTimer = 0.0f;
					menuManager.currentMenuItem++;
					if (menuManager.currentMenuItem > 1)
						menuManager.currentMenuItem = 0;
					redraw = true;
				}

				if (keys[ENTER])
				{
					buttonTimer = 0.0f;
					switch (menuManager.currentMenuItem)
					{
					case 0:
						//begin Game
						gameManager.SetGameState(PLAYING);
						break;
					case 1:
						//exit game
						return 0;
					}
				}
			}
		}
		#pragma endregion

		#pragma region Playing
		else if (gameManager.GetGameState() == PLAYING)
		{
			#pragma region Update Loop
			
			#pragma region Check Victory
			//check victory condition
			if (ship.shipPos.x >= LEVELWIDTH - 5)
			{
				gameManager.SetGameState(VICTORY);
				delayer = 0;
			}
			#pragma endregion

			if(ev.type == ALLEGRO_EVENT_TIMER)
			{

				buttonTimer += 0.1f;
				
				collide = false;
				collideObstacle = false;
				redraw = true;

				#pragma region Update ShipPointCharge and shipBoundingBox and reset force
				//update shipPointCharge for magnet calculations
				shipPointCharge.pos.x = ship.shipPos.x;
				shipPointCharge.pos.y = ship.shipPos.y;

				//update shipBoundingBox for collision calculations
				shipBoundingBox.x1 = ship.shipPos.x;
				shipBoundingBox.x2 = ship.shipPos.x + ship.GetWidth();
				shipBoundingBox.y1 = ship.shipPos.y;
				shipBoundingBox.y2 = ship.shipPos.y + ship.GetHeight();

				ResetForce(ship);
				#pragma endregion
				
				#pragma region Tunnel Collision Checks
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
				#pragma endregion

				#pragma region Obstacle Collision Checks
				for (int i = 0; i < NUM_OBSTACLES; i++)
				{
					if (shipBoundingBox.x1 > obstacleBoxes[i].x2)
						continue;
					if (shipBoundingBox.x2 < obstacleBoxes[i].x1)
						continue;

					collideObstacle = CheckObstacleCollision(shipBoundingBox, obstacleBoxes[i]);
				}
				#pragma endregion

				#pragma region Magnet Force Checks
				for ( int i = 0; i < NUM_MAGNETS; i++)
				{
					#pragma region TopMagnets
					if ( ((ship.shipPos.x < topMagnets[i].magnetPosition.x - topMagnets[i].radius) || 
						 (ship.shipPos.x > topMagnets[i].magnetPosition.x + topMagnets[i].radius) &&
						 (ship.shipPos.y > topMagnets[i].magnetPosition.y + topMagnets[i].radius)) ||
								( (ship.shipPos.x < botMagnets[i].magnetPosition.x - botMagnets[i].radius) || 
								(ship.shipPos.x > botMagnets[i].magnetPosition.x + botMagnets[i].radius) ) &&
								(ship.shipPos.y < botMagnets[i].magnetPosition.y - botMagnets[i].radius) )
					{
							Vector3 distanceVec = Polaris::Get_Distance_Vector(ship.shipPos, Vector3(LEVELWIDTH, LEVELHEIGHT / 2, 0));
							//ship.shipPos += distanceVec * 0.0005f;
							continue;
					}

					if ( ( (topMagnets[i].magnetPosition.x - ship.shipPos.x) * (topMagnets[i].magnetPosition.x - ship.shipPos.x) )
						+ ( (ship.shipPos.y - topMagnets[i].magnetPosition.y) * (ship.shipPos.y - topMagnets[i].magnetPosition.y) )
						< (topMagnets[i].radius * topMagnets[i].radius) )
					{ 
						//move ship based on polarity
						if (ship.GetPolarity() != topMagnets[i].magnetPolarity)
						{
							/*distance = Polaris::Get_Distance(ship.shipPos, topMagnets[i].magnetPosition);
							if (abs(distance) < 15)
								continue;

							force = Polaris::Get_Force(shipPointCharge.charge, topMagnets[i].force, distance);
							if (force < 0.007)
								force = 0.007;
							else if (force > 0.07)
								force = 0.07;
							Vector3 distanceVec = Polaris::Get_Distance_Vector(ship.shipPos, topMagnets[i].magnetPosition);
							if (!collide)
								ship.shipPos += distanceVec * (force);*/
							ApplyForces(topMagnets[i], ship);
							ship.SetxSpeed(ship.GetSpeedX() + (ship.GetxForce() / ship.GetMass()));
							ship.SetySpeed(ship.GetSpeedY() + (ship.GetyForce() / ship.GetMass()));

							ship.shipPos.x += ship.GetSpeedX();
							ship.shipPos.y += ship.GetSpeedY();
						}
					}
					else 
					{
					    /*distance = Polaris::Get_Distance(ship.shipPos, topMagnets[i].magnetPosition);
					    force = Polaris::Get_Force(shipPointCharge.charge, topMagnets[i].force, distance);
						if (force < 0.007)
							force = 0.007;
						else if (force > 0.07)
							force = 0.07;
						Vector3 distanceVec = Polaris::Get_Distance_Vector(ship.shipPos, topMagnets[i].magnetPosition);
						if (!collide)
							ship.shipPos -= distanceVec * force;*/
						ApplyForces(topMagnets[i], ship);
						ship.SetxSpeed(ship.GetSpeedX() + (ship.GetxForce() / ship.GetMass()));
						ship.SetySpeed(ship.GetSpeedY() + (ship.GetyForce() / ship.GetMass()));

						ship.shipPos.x -= ship.GetSpeedX();
						ship.shipPos.y -= ship.GetSpeedY();
					}
					#pragma endregion

					#pragma region Bottom Magnets
					if ((botMagnets[i].magnetPosition.x - ship.shipPos.x) + (ship.shipPos.y - botMagnets[i].magnetPosition.y)
						< botMagnets[i].radius)
					{ 
						//move ship based on polarity
						if (ship.GetPolarity() != botMagnets[i].magnetPolarity)
						{
							distance = Polaris::Get_Distance(ship.shipPos, botMagnets[i].magnetPosition);
							if (abs(distance) < 15)
								continue;

							force = Polaris::Get_Force(shipPointCharge.charge, botMagnets[i].force, distance);
							if (force < 0.007)
								force = 0.007;
							else if (force > 0.07)
								force = 0.07;
							Vector3 distanceVec = Polaris::Get_Distance_Vector(ship.shipPos, botMagnets[i].magnetPosition);
							if (!collide)
								ship.shipPos += distanceVec * (force);
						}
					}
					else 
					{
					    distance = Polaris::Get_Distance(ship.shipPos, botMagnets[i].magnetPosition);
					    force = Polaris::Get_Force(shipPointCharge.charge, botMagnets[i].force, distance);
						if (force < 0.007)
							force = 0.007;
						else if (force > 0.07)
							force = 0.07;
						Vector3 distanceVec = Polaris::Get_Distance_Vector(ship.shipPos, botMagnets[i].magnetPosition);
						if (!collide)
							ship.shipPos -= distanceVec * force;
					}
					#pragma endregion
				}
				#pragma endregion

				#pragma region Move Screen
				if (ship.shipPos.x > SCREENWIDTH / 2)
				{
					currentX += ship.GetSpeed();
					if (currentX >= ship.shipPos.x - 300)
						currentX = ship.shipPos.x - 300;						
				}
				else if (ship.shipPos.x < currentX + (SCREENWIDTH / 2))
				{
					if (currentX <= 0)
						currentX = 0;
					else
						currentX = ship.shipPos.x - 300;
				}

				if (LEVELWIDTH - (SCREENWIDTH / 2) < currentX)
					currentX = LEVELWIDTH - (SCREENWIDTH / 2);
				#pragma endregion				

				#pragma region Apply Input
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
				if(keys[ENTER])
				{
					if (buttonTimer >= PAUSE_BUTTON_TIME)
					{
						buttonTimer = 0.0f;
						gameManager.SetGameState(PAUSED);
					}
				}
				if (gameManager.GetGameState() != PLAYING)
					continue;
				#pragma endregion

				#pragma region Update Score
				timeDelay += 0.1f;
				if (timeDelay >= timeDelayLimit)
				{
					gameScore.UpdateScoreCounter(SCORE_INCREMENT);
					timeDelay = 0.0f;
				}
				#pragma endregion

				#pragma region Restart Level Upon Collision
				/*if (collide || collideObstacle)
				{
					gameScore = 0;
					ship.shipPos.x = 20;
					ship.shipPos.y = SCREENHEIGHT / 2;
				}*/
				#pragma endregion
			}
			#pragma endregion

			else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				done = true;
			}
		}
		#pragma endregion

		#pragma region Paused
		else if (gameManager.GetGameState() == PAUSED)
		{
			buttonTimer += 0.1f;
			al_draw_text(font, al_map_rgb(203,194,80), SCREENWIDTH / 2, SCREENHEIGHT / 2, 0, "P A U S E D");
			if (keys[ENTER])
			{
				if (buttonTimer >= PAUSE_BUTTON_TIME)
				{
					buttonTimer = 0.0f;
					gameManager.SetGameState(PLAYING);
				}
			}
		}
		#pragma endregion

		#pragma region Victory
		if (gameManager.GetGameState() == VICTORY)
		{
			fadeDelay -= al_get_timer_count(timer);

			if (fadeDelay <= 0)
			{
				//reset fade delay
				fadeDelay = .045;

				//increment/decrement the fade value
				alphaValue += fadeIncrement;

				if (alphaValue <= 0)
					fadeIncrement *= -1;
			}

			if (delayer <= 400)
				delayer++;
			else 
				delayer = 401;

			frameTime = al_get_timer_count(timer);
			redraw = true;
		}
		#pragma endregion

		#pragma region DrawLoop
		if(redraw && al_event_queue_is_empty(event_queue))
		{
			redraw = false; 

			al_set_target_bitmap(backBuffer);
			al_clear_to_color(al_map_rgb(0,0,0));

			if (gameManager.GetGameState() == SPLASH)
			{
				if (delayer >= 400)
					gameManager.SetGameState(MAINMENU);
				else
				{
					al_draw_tinted_bitmap(splashScreen, al_map_rgba_f(255 * alphaValue, 255 * alphaValue, 255 * alphaValue, alphaValue), 0, 0, 0);
				}
			}

			if (gameManager.GetGameState() == MAINMENU)
			{
				delayer = 0;
				al_draw_filled_rectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, al_map_rgb(14, 50, 200));
				if (menuManager.currentMenuItem == 0)
				{
					al_draw_text(font, al_map_rgb(34, 178, 134), 45, 50, 0, "BEGIN GAME");
					al_draw_text(font, al_map_rgb(255,255,255), 45, 120, 0, "EXIT GAME");
				}
				else if (menuManager.currentMenuItem == 1)
				{
					al_draw_text(font, al_map_rgb(255, 255, 255), 45, 50, 0, "BEGIN GAME");
					al_draw_text(font, al_map_rgb(34, 178, 134), 45, 120, 0, "EXIT GAME");
				}
			}
			else if (gameManager.GetGameState() == PLAYING)
			{
				//Draw static position score in bottom right corner of screen
				DrawScore(gameScore.GetScore(), currentX);
				
				ship.DrawShip();

				ConnectPointsTop(topPoints);
				ConnectPointsBottom(bottomPoints);
				
				
				al_draw_textf(font, al_map_rgb(255, 255, 110), currentX + 75, 30, 0, "Force = %g", force);
				
				al_draw_textf(font, al_map_rgb(255,255,100), currentX + 75, 60, 0, "Distance = %g", distance);

				if (collide)
				{
					al_draw_text(font, al_map_rgb(255, 255, 110), currentX + (SCREENWIDTH / 2), SCREENHEIGHT / 2, 0, "COLLISION ");
					al_draw_textf(font, al_map_rgb(255,100,100), currentX + 20, 50, 0, "ShipPos: ( %g, %g, %g)", ship.shipPos.x, ship.shipPos.y, ship.shipPos.z);
				}

				if (collideObstacle)
				{
					al_draw_text(font, al_map_rgb(255,255,255), currentX + 200, 300, 0, "Collide obstacles");
				}

				al_draw_textf(font, al_map_rgb(45, 120, 200), currentX + 20, SCREENHEIGHT - 100, 0, "currentX = %i", currentX);
				al_draw_line(currentX, 10, currentX, SCREENHEIGHT, al_map_rgb(255,0,0), 3);
				al_draw_line(LEVELWIDTH, 0, LEVELWIDTH, LEVELHEIGHT, al_map_rgb(0,0,255), 10);

				DrawMagnets(topMagnets, botMagnets);

				for (int i = 0; i < NUM_MAGNETS; i++)
				{
					al_draw_circle(topMagnets[i].magnetPosition.x, topMagnets[i].magnetPosition.y, topMagnets[i].radius, al_map_rgb(100,167, 99), 2);
					al_draw_circle(botMagnets[i].magnetPosition.x, botMagnets[i].magnetPosition.y, botMagnets[i].radius, al_map_rgb(100, 167,99), 2);
					al_draw_circle(topPointCharges[i]->pos.x + 5, topPointCharges[i]->pos.y + 5, 10, al_map_rgb(255,255,255), 2);
				}
				
				Drawobstacles(vectorobstacles);

			}
			if (gameManager.GetGameState() == PAUSED)
			{

				al_draw_text(font, al_map_rgb(405, 120, 200), currentX + 200, SCREENHEIGHT / 2, 0, "P A U S E D");
			}

			if (gameManager.GetGameState() == VICTORY)
			{
				if (delayer >= 400)
					gameManager.SetGameState(MAINMENU);
				else
				{
					//al_draw_text(font, al_map_rgb(405, 120, 200), currentX + 200, SCREENHEIGHT / 2, 0, "V I C T O R Y");
					al_draw_tinted_bitmap(victoryFlash, al_map_rgba_f(255 * alphaValue, 255 * alphaValue, 255 * alphaValue, alphaValue), currentX, currentY, 0);
					//al_draw_tinted_bitmap(victoryFlash, al_map_rgba_f(255 * alphaValue, 255 * alphaValue, 255 * alphaValue, alphaValue), 0, 0, 0);
				}
			}

			al_set_target_bitmap(al_get_backbuffer(display));

			//al_draw_bitmap(backBuffer, 0, 0, 0);
			al_draw_bitmap_region(backBuffer, currentX, currentY, currentX + SCREENWIDTH, currentY + SCREENHEIGHT, 0, 0, 0);
			al_flip_display();
			al_clear_to_color(al_map_rgb(0,0,0));
		}
		#pragma endregion
	}

	#pragma endregion

	#pragma region ALLEGRO Cleanup
	al_destroy_timer(timer);
	al_destroy_display(display);
	return 0;
	#pragma endregion

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
		if (points[i + 1].x < LEVELWIDTH)
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
		if (pointsBot[i + 1].x < LEVELWIDTH)
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
		if (obstacles[i].x < LEVELWIDTH)
			al_draw_filled_rectangle(obstacles[i].x, obstacles[i].y, obstacles[i].x + 20, obstacles[i].y + 20, al_map_rgb(255,0,255));
	}
}

//Generates and populates obstacles[]. Randomly places obstacles within the level. 
void Generateobstacles(Vector3 obstacles[], BoundingBox boxes[])
{
	char logStringBuffer[50];
	logStringBuffer[0] = 0;
	bool sorted = false;
	
	Logger::Log("\n", Logger::logLevelDebug);

	Logger::Log("Obstacles", Logger::logLevelInfo);

	int storeRandom[40];
	for (int i = 0; i < NUM_OBSTACLES; i++)
	{
		bool isRepeating;
		int random;
		do
		{
			random = (rand() % 50) * 100;
			isRepeating = true;
			for (int j = 0; j < i; j++)
			{
				if (random == storeRandom[j])
				{
					isRepeating = false;
					break;
				}
			}
		} while(!isRepeating);
		storeRandom[i] = random;
		obstacles[i].x = storeRandom[i];
		obstacles[i].y = Height / 2 - ((rand() % 25) + 20);
	}

	if (!sorted)
	{
		Sort(obstacles, NUM_OBSTACLES);
		sorted = true;
	}
	
	SpaceObstacles(obstacles, NUM_OBSTACLES);
	
	for (int i = 0; i < NUM_OBSTACLES; i++)
	{
		boxes[i].x1 = obstacles[i].x;
		boxes[i].x2 = obstacles[i].x + 20;
		boxes[i].y1 = obstacles[i].y;
		boxes[i].y2 = obstacles[i].y + 20;
	}
	
	
	Sort(obstacles, NUM_OBSTACLES);

	for (int i = 0; i < NUM_OBSTACLES; i++)
	{
		sprintf_s(logStringBuffer, "Obstacles[ %i ] = (%g,%g)", i, obstacles[i].x, obstacles[i].y);  
		Logger::Log(logStringBuffer, Logger::logLevelInfo);
		memset(logStringBuffer, 0, sizeof(logStringBuffer));
	}
	Logger::ShutdownLogger();
}

#pragma endregion

#pragma region Magnets

//Generate random X value for Magnet location
int GetMagnetLocationX()
{
	int x = (rand() % 50) * 50;

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
		if (magnets[i].magnetPosition.x < LEVELWIDTH)
		{
			if (magnets[i].magnetPolarity == POSITIVE)
				al_draw_filled_rectangle(magnets[i].magnetPosition.x, magnets[i].magnetPosition.y, magnets[i].magnetPosition.x + 20, magnets[i].magnetPosition.y + 20, al_map_rgb(255,0,0));
			else
				al_draw_filled_rectangle(magnets[i].magnetPosition.x, magnets[i].magnetPosition.y, magnets[i].magnetPosition.x + 20, magnets[i].magnetPosition.y + 20, al_map_rgb(255,255,255));
		}
		if (magnetsBot[i].magnetPosition.x < LEVELWIDTH)
		{
			if (magnetsBot[i].magnetPolarity == POSITIVE)
				al_draw_filled_rectangle(magnetsBot[i].magnetPosition.x, magnetsBot[i].magnetPosition.y, magnetsBot[i].magnetPosition.x + 20, magnetsBot[i].magnetPosition.y + 20, al_map_rgb(255, 0, 0));
			else 
				al_draw_filled_rectangle(magnetsBot[i].magnetPosition.x, magnetsBot[i].magnetPosition.y, magnetsBot[i].magnetPosition.x + 20, magnetsBot[i].magnetPosition.y + 20, al_map_rgb(255,255,255));
		}
	}
}

void SetupMagnetsTop()
{
	bool polaricCharge = POSITIVE;
	bool sorted = false;

	Logger::Log("\n", Logger::logLevelDebug);

	Logger::Log("MAGNETS - TOP", Logger::logLevelInfo);

	Logger::ShutdownLogger();

	for (int i = 0; i < NUM_MAGNETS; i++)
	{
		Magnet *topMagnet = topFactory.requestMagnet();

		if (i % 2 == 0)
			polaricCharge = !polaricCharge;
		
		topMagnet->InitializeMagnet(i, GetMagnetLocationX(), GetMagnetLocationY(), 200, 0.09f, polaricCharge);
		topMagnets[i] = *topMagnet;
	}

	if (!sorted)
		{
			SortMagnets(topMagnets);
			sorted = true;
		}
	
	SpaceMagnets(topMagnets, NUM_MAGNETS);

	for (int i = 0; i < NUM_MAGNETS; i++)
	{
		topPointCharges[i] = new PointCharge(topMagnets[i].magnetPosition.x + 5, topMagnets[i].magnetPosition.y + 5, topMagnets[i].force);
		LogMagnet(topMagnets[i]);
	}
}

void LogMagnet(Magnet magnet)
{
	char logStringBuffer[50];
	logStringBuffer[0] = 0;

	sprintf_s(logStringBuffer, "Magnet = (%g,%g)", magnet.magnetPosition.x, magnet.magnetPosition.y);  
	Logger::Log(logStringBuffer, Logger::logLevelInfo);
	memset(logStringBuffer, 0, sizeof(logStringBuffer));

	Logger::ShutdownLogger();
}

void SetUpMagnetsBottom()
{
	Logger::Log("\n", Logger::logLevelDebug);

	bool polaricCharge = NEGATIVE;
	bool sorted = false;
	
	Logger::Log("Magnets - BOTTOM", Logger::logLevelInfo);
	Logger::ShutdownLogger();

	int magnetYPositionOffset = 370;

	for (int i = 0; i < NUM_MAGNETS; i++)
	{
		Magnet *bottomMagnet = bottomFactory.requestMagnet();

		if (i % 2 == 0)
			polaricCharge = !polaricCharge;
		
		bottomMagnet->InitializeMagnet(i, GetMagnetLocationX(), GetMagnetLocationY() + magnetYPositionOffset, 200, 0.9f, polaricCharge);
		botMagnets[i] = *bottomMagnet;
	}

	if (!sorted)
	{
		SortMagnets(botMagnets);
		sorted = true;
	}

	SpaceMagnets(botMagnets, NUM_MAGNETS);

	for (int i = 0; i < NUM_MAGNETS; i++)
	{
		botPointCharges[i] = new PointCharge(botMagnets[i].magnetPosition.x + 5, botMagnets[i].magnetPosition.y - 5, botMagnets[i].force);
		LogMagnet(botMagnets[i]);
	}	
}

#pragma endregion

#pragma region Helper Functions

bool CheckCollisionsTop(SpaceShip &ship, Vector3 pointOne, Vector3 pointTwo)
{
	Vector3 lineSegment = GetVectorDistance(pointOne, pointTwo);
	Vector3 shipWidthPosition(ship.shipPos.x + ship.GetWidth(), ship.shipPos.y, ship.shipPos.z);

	if (pointTwo.y <= pointOne.y) //upwards line
	{
		Vector3 lineNormal(lineSegment.y, -lineSegment.x, 0);
		double u = GetVectorDistance(ship.shipPos, pointTwo) * lineNormal;

		if (u > 0)
			return false;
		else if (u < 0)
			return true;
	}
	else if (pointTwo.y >= pointOne.y) //downwards line
	{
		Vector3 lineNormal(lineSegment.y, -lineSegment.x, 0);
		double u = GetVectorDistance(shipWidthPosition, pointOne) * lineNormal;

		if (u > 0)
			return false;
		else if (u < 0)
			return true;
	}
	return false;
}

bool CheckCollisionsBottom(SpaceShip &ship, Vector3 pointOne, Vector3 pointTwo)
{
	Vector3 lineSegment = GetVectorDistance(pointOne, pointTwo);
	Vector3 shipHeightPosition(ship.shipPos.x, ship.shipPos.y + ship.GetHeight(), ship.shipPos.z);
	Vector3 shipWidthHeightPosition(ship.shipPos.x + ship.GetWidth(), ship.shipPos.y + ship.GetHeight(), ship.shipPos.z);

	if (pointTwo.y <= pointOne.y) //upwards line
	{
		Vector3 lineNormal(-lineSegment.y, lineSegment.x, 0);
		double u = GetVectorDistance(shipWidthHeightPosition, pointTwo) * lineNormal;

		if (u > 0)
			return false;
		else if (u < 0)
			return true;
	}
	else if (pointTwo.y >= pointOne.y) //downwards line
	{
		Vector3 lineNormal(-lineSegment.y, lineSegment.x, 0);
		double u = GetVectorDistance(shipHeightPosition, pointTwo) * lineNormal;

		if (u > 0)
			return false;
		else if (u < 0)
			return true;
	}
	return false;
}

bool CheckObstacleCollision(BoundingBox ship, BoundingBox obstacle)
{
  /*return (abs(ship.x1 - obstacle.x1) * 2 < (ship.x2 + obstacle.x2)) &&
         (abs(ship.y1 - obstacle.y1) * 2 < (ship.y2 + obstacle.y2));*/
	if ( ((ship.x2 > obstacle.x1) && (ship.x2 < obstacle.x2)) &&
			(ship.y1 > obstacle.y1) && (ship.y1 < obstacle.y2) ||
			(ship.y2 > obstacle.y1) && (ship.y2 < obstacle.y2))
			return true;
	else if ( ((ship.x1 > obstacle.x1) && (ship.x1 < obstacle.x2)) &&
				(ship.y1 > obstacle.y1) && (ship.y1 < obstacle.y2) ||
				(ship.y2 > obstacle.y1) && (ship.y2 < obstacle.y2))
				return true;
	else
		return false;
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

void SortMagnets(Magnet magnets[])
{
	bool sorted = false;
	int swapped = 0;
	while(!sorted)
	{
		swapped = 0;
		for (int i = 0; i < NUM_MAGNETS; i++)
		{
			if (i + 1 < NUM_MAGNETS)
			{
				if (magnets[i + 1].magnetPosition.x < magnets[i].magnetPosition.x)
				{
					swap(magnets[i], magnets[i+1]);
					sorted = false;
					++swapped;
				}
				else
					continue;
			}
			if (swapped == 0)
			{
				sorted = true;
			}
		}
	}
	for (int i = 0; i < NUM_MAGNETS; i++)
	{
		if (i + 1 < NUM_MAGNETS)
			magnets[i + 1].magnetPolarity = (!(magnets[i].magnetPolarity));
	}
}

void Sort(Vector3 arr[], int size)
{
	bool sorted = true;
	int swapped = 0;
	while(true)
	{
		swapped = 0;
		for (int i = 0; i < size; i++)
		{
			if (i + 1 < size)
			{
				if (arr[i + 1].x < arr[i].x)
				{
					swap(arr[i], arr[i+1]);
					swapped++;
				}
			}
		}
		if (!(swapped > 0))
			break;
	}
}

void SpaceMagnets(Magnet m[], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (i + 1 < size)
		{
			if (m[i + 1].magnetPosition.x > (m[i].magnetPosition.x + (m[i].radius * 2) + 100) )
			{
				m[i + 1].magnetPosition.x = (m[i].magnetPosition.x + (m[i].radius * 2) + rand() % 100 );
			}
			else if (m[i + 1].magnetPosition.x < (m[i].magnetPosition.x + (m[i].radius * 1.5)))
			{
				m[i + 1].magnetPosition.x = m[i].magnetPosition.x + (m[i].radius * 1.5);
			}
		}
	}
}

void SpaceObstacles(Vector3 o[], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (i + 1 < size)
		{
			if (o[i + 1].x < o[i].x + 300)
				o[i + 1].x = o[i].x + 300;
		}
	}
}

void ResetForce(SpaceShip &ship)
{
	ship.SetxForce(0);
	ship.SetyForce(0);
}

void ApplyForces(Magnet m, SpaceShip &ship)
{
	float dX = m.magnetPosition.x - ship.shipPos.x;
	float dY = m.magnetPosition.y - ship.shipPos.y;

	float distance = Polaris::Get_Distance(m.magnetPosition, ship.shipPos);
	float force = (-ship.GetCharge() - m.force) / (distance * distance);

	if (force > 1000)
		force = 1000;

	float xDirectionForce = force * (dX / distance);
	float yDirectionForce = force * (dY / distance);

	ship.SetxForce(ship.GetxForce() * xDirectionForce);
	ship.SetyForce(ship.GetyForce() * yDirectionForce);
}

#pragma endregion