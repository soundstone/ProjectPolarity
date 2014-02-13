/* * * * * *GameManager.cpp* * * * * * */

#include "GameManager.h"


int GameManager::GetGameState()
{
	return gameState;
}

void GameManager::SetGameState(GameState state)
{
	gameState = state;
}

