//ScoreCounter.h
//Class declaration for score counter. This counter will be updated each frame to increase the players score overtime

#include "allegro5\allegro.h";

class ScoreCounter
{
public:
	ScoreCounter();
	ScoreCounter(int score);

	~ScoreCounter();

	void UpdateScoreCounter(float scoreOverTime, int collectableValue = 0);

	float GetScore() const  
	{
		return score;
	}

private:
	int score;

};