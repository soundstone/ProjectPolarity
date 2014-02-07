//ScoreCounter.cpp
//Class definition for Score Counter

#include "ScoreCounter.h"

ScoreCounter::ScoreCounter()
	: score(0)
{
}

ScoreCounter::ScoreCounter(int startingScore)
	: score(startingScore)
{
}

ScoreCounter::~ScoreCounter()
{
}

void ScoreCounter::UpdateScoreCounter(float scoreOverTime, int collectableValue)
{
	if (collectableValue != 0)
		score += collectableValue;
	else 
		score += scoreOverTime;
}
