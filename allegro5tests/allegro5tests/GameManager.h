/* * * * * * * * * *GameManager.h * * * * * * * * * * * * * 
*  Class Definitions for the game manager static class. 
*  
*  - GameManager uses enum GameState to run code sections 
*      based on what state the game is currently in.
*  - Gamestates include: Splash, Main Menu, Playing, 
*	   Paused, and Game Over.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

enum GameState
{
	SPLASH,
	MAINMENU,
	PLAYING,
	PAUSED,
	GAMEOVER
};

class GameManager
{
public:
	GameManager()
		: gameState(SPLASH)
	{};

	int GetGameState();
	void SetGameState(GameState state);
	
private:
    int gameState;
};