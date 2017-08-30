#ifndef GRID_PATH_PLANNER_H
#define GRID_PATH_PLANNER_H

#include "PartiallyKnownGrid.h"
#include <set>
#include <list>

struct Node {
	int f, g, h;
	Node* mParent;
	xyLoc mLoc;
};

struct NodeComparator {
	bool operator()(const Node* lhs, const Node* rhs ) {
		if(lhs->f < rhs->f)
		{
			return true;
		}
		else if(lhs->f == rhs->f)
		{
			if(lhs->g > rhs->g)
			{
				return true;
			}	
			else if(lhs->g == rhs->g)
			{
				return lhs->mLoc < rhs->mLoc;
			}
		}
		return false;
	}
};


class GridPathPlanner{
public:
	GridPathPlanner(PartiallyKnownGrid* grid, bool use_adaptive_a_star = false);
	~GridPathPlanner();

	xyLoc GetNextMove(PartiallyKnownGrid* grid);
	int GetNumExpansions();
	int GetTotalExpansions();

private:
	bool mAdaptiveAStar; 
	int height;
	int width;
	std::set<Node*, NodeComparator> mOpen; // Set for the to-visit nodes
	std::set<Node*> mClosed; // Set for the visited nodes 
	Node*** mGrid; // Dynamically allocated 2D array of Node*

	int mNumExpansions;
	int mTotalExpansions;
};

#endif
