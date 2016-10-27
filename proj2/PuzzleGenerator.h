#include <stdio.h>
#include <stdlib.h>
#include <climits>
#include "Puzzle.h"
#include "Timer.h"

// Constant global define
const double e = 2.71828;
const double alpha = 0.99999;
const double InitTemp = 2000;

class PuzzleGenerator{
public:
	PuzzleGenerator(int nRows, int nColumns, int minVal, int maxVal);
	Puzzle GeneratePuzzle(); 
	Puzzle HillClimb(double timelimit);	// Time limit given in seconds.
	Puzzle SimulatedAnnealing(double timelimit);
	double CalculateProbability(int oldValue, int newValue);
	
private:
	int nRows, nColumns, minVal, maxVal;
	Timer timer;
	double maxtime;

	double temperature;
};
