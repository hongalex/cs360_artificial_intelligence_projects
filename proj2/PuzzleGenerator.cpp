#include <iostream>
#include <cmath>
#include "PuzzleGenerator.h"
using namespace std;

PuzzleGenerator::PuzzleGenerator(int _nRows, int _nColumns, int _minVal, int _maxVal)
	:nRows(_nRows), nColumns(_nColumns), minVal(_minVal), maxVal(_maxVal)
{
	temperature = InitTemp; //Initial temperature 
}

Puzzle PuzzleGenerator::GeneratePuzzle()
{
	//timer.StartTimer();
	maxtime = 59.9;	// To make sure we don't exceed a minute
	
	// Do as many Hill Climbs as we can within the given time limit.
	/* Puzzle bestPuzzle = HillClimb(5); 

	while (timer.GetElapsedTime() +5 < maxtime)
	{
		Puzzle p = HillClimb(5);
		// Check if p is better than the best puzzle we have found so far.
		if(p.GetValue() > bestPuzzle.GetValue())
		{
			bestPuzzle = p;
			cout << "Found better puzzle with score: " << p.GetValue() << endl;
		}
		else
		{
			cout << "Found worse puzzle with score: " << p.GetValue() << endl;
		}
	} */


	// Do Simulated Annealings in the time limit
	// Puzzle firstPuzzle = SimulatedAnnealing(29.8);
	// temperature = InitTemp;
	// cout << "First puzzle: " << firstPuzzle.GetValue() << endl;
	// Puzzle secondPuzzle = SimulatedAnnealing(29.8);
	// cout << "Second puzzle: " << secondPuzzle.GetValue() << endl;

	// Puzzle max = (firstPuzzle.GetValue() > secondPuzzle.GetValue()) ? firstPuzzle : secondPuzzle;

	Puzzle max = SimulatedAnnealing(maxtime);
	return max;
}

Puzzle PuzzleGenerator::HillClimb(double timelimit)
{
	// A very simple function that starts at a random configuration and keeps randomly modifying it
	// until the time limit is reached. Returns the best solution found so far.

	Puzzle p(nRows, nColumns, minVal, maxVal);	// Generate a random puzzle with the specified values.
	
	// Keep track of the best puzzle found so far (and its value).
	Puzzle bestPuzzle = p;			
	int bestValue = p.GetValue();
	
	// Keep track of the time so we don't exceed it.
	Timer t;
	t.StartTimer();
	
	// Loop until we hit the time limit.
	while (t.GetElapsedTime() < timelimit-0.1)	
	{
		// Generate a successor of p by randomly changing the value of a random cell, three times
		Puzzle successor = p.GetRandomSuccessor();	
		successor = successor.GetRandomSuccessor();
		successor = successor.GetRandomSuccessor();
		
		// Update the current best solution.
		// Calling GetValue() for the first time is costly
		// since the puzzle has to be evaluated first
		if (successor.GetValue() > bestValue)	
		{
			bestValue = successor.GetValue();	// Calling it a second time simply returns the value that was computed before.
			bestPuzzle = successor;
			p = successor;
		}
	}
	
	return bestPuzzle;
}

Puzzle PuzzleGenerator::SimulatedAnnealing(double timelimit)
{
	Puzzle p(nRows, nColumns, minVal, maxVal);	// Generate a random puzzle with the specified values.

	// Keep track of the best puzzle found so far (and its value).
	Puzzle bestPuzzle = p;			
	int bestValue = p.GetValue();
	
	// Keep track of the time so we don't exceed it.
	Timer t;
	t.StartTimer();
	int jump = 0;
	
	// Loop until we hit the time limit.
	while (t.GetElapsedTime() < timelimit-0.1)	
	{
		// Generate a successor of p by randomly changing the value of a random cell
		Puzzle successor = p.GetRandomSuccessor();	
		// At lower temperatures, make three more changes (for fun)
		if(temperature < 0.1)
		{
			successor = p.GetRandomSuccessor();
			successor = p.GetRandomSuccessor();
			successor = p.GetRandomSuccessor();
		}
		
		// If the value is better, just accept it
		if (successor.GetValue() > bestValue)	
		{
			bestValue = successor.GetValue();	
			bestPuzzle = successor;
			p = successor;
		}
		// If the value is worse, consider it based on the temperature
		else
		{
			if(temperature > 0)
			{
				double random = (static_cast<double>(rand()) / static_cast<double>(RAND_MAX));
				double prob = CalculateProbability(bestPuzzle.GetValue(), successor.GetValue()) -1;
				// Jump sometimes based on this probability
				if(random > prob)
				{
					p = successor;
				}
			}
		}
		temperature*=alpha;
	}
	
	return bestPuzzle;
}

double PuzzleGenerator::CalculateProbability(int oldValue, int newValue)
{
	int difference = oldValue - newValue;
	double power = difference/temperature;
	return pow(e, power);
}
