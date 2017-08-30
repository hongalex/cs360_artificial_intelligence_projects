#include "GridPathPlanner.h"
#include <cmath>
#include <climits>

using namespace std;

GridPathPlanner::GridPathPlanner(PartiallyKnownGrid* grid, bool use_adaptive_a_star) {
	mAdaptiveAStar = use_adaptive_a_star;
	height = grid->GetHeight();
	width = grid->GetWidth();
	mNumExpansions = 0;
	mTotalExpansions = 0;

	// Dynamically allocate the 2d array 
	mGrid = new Node**[height];
	for(int i = 0; i < height; i++)
	{
		mGrid[i] = new Node*[width];
	}

	// Dynamically allocate the individual Nodes 
	xyLoc goal = grid->GetGoalLocation();
	// Initialize the locations and values 
	for(int i=0; i<height; i++)
	{
		for(int j=0; j<width; j++)
		{
			Node* newNode = new Node;
			newNode->mLoc = xyLoc(j,i);
			newNode->f = INT_MAX;
			newNode->g = INT_MAX;
			// Initialize the node's Manhattan distance
			newNode->h = abs(j - goal.x) + abs(i - goal.y);
			mGrid[i][j] = newNode;
		}
	}

}
GridPathPlanner::~GridPathPlanner(){
	for(int i=0; i<height; i++) 
	{
		for(int j=0; j<width; j++)
		{
			delete mGrid[i][j];
		}
	}
	for(int i=0; i<height; i++) 
	{
		delete[] mGrid[i];
	}
	delete[] mGrid;
}

xyLoc GridPathPlanner::GetNextMove(PartiallyKnownGrid* grid) {
	// Reset mNumExpansions
	mNumExpansions = 0;

	/* Run A* search */

	/* Initialize and add the start node */
	xyLoc start = grid->GetCurrentLocation();
	Node* startNode = mGrid[start.y][start.x];
	startNode->g = 0;
	startNode->f = startNode->g + startNode->h;
	mOpen.insert(startNode);

	/* Main Loop */
	while(!mOpen.empty())
	{
		std::set<Node*>::iterator top= mOpen.begin();
		Node* curr = *top;

		// Found Goal Node
		if(curr->mLoc == grid->GetGoalLocation())
		{
			//std::cout << "Found the goal node" << std::endl;
			while(curr->mParent != startNode)
			{
				curr = curr->mParent;
			}

			// Use Adaptive A*
			if(mAdaptiveAStar)
			{
				// Update all the Node's H values
				xyLoc goal = grid->GetGoalLocation();
				Node* goalNode = mGrid[goal.y][goal.x];

				for(auto& expanded : mClosed)
				{
					expanded->h = goalNode->g - expanded->g;
					//expanded->f = expanded->g + expanded->h;
				}
			}
			mOpen.clear();
			mClosed.clear();
			
			return curr->mLoc;
		}
		mOpen.erase(top);
		mClosed.insert(curr);
		mNumExpansions++;	
		mTotalExpansions++;

		// Generate neighbors 
		std::vector<xyLoc> neighbors;
		xyLoc currLoc = curr->mLoc;
		neighbors.push_back(xyLoc(currLoc.x+1, currLoc.y));
		neighbors.push_back(xyLoc(currLoc.x-1, currLoc.y));
		neighbors.push_back(xyLoc(currLoc.x, currLoc.y+1));
		neighbors.push_back(xyLoc(currLoc.x, currLoc.y-1));
		for (int i = 0; i < neighbors.size(); i++) 
		{
			xyLoc n = neighbors[i];
			if (!grid->IsValidLocation(n) || grid->IsBlocked(n)) 
			{
				neighbors[i] = neighbors.back();
				neighbors.pop_back();
				i--;
			}
		}
		for(xyLoc& neighbor : neighbors)
		{
			Node* successor = mGrid[neighbor.y][neighbor.x];
			// Ignore any successors that have already been evaluated 
			if(mClosed.find(successor) != mClosed.end())
			{
				continue;
			}
			// Calculate a temporary gScore
			int gScore = curr->g + 1;
			// If the successor is not in the open set, add to open set
			if(mOpen.find(successor) == mOpen.end())
			{
				successor->mParent = curr;
				successor->g = gScore;
				successor->f = successor->g + successor->h;

				mOpen.insert(successor);
			}
			// If the new gScore is better than the existing, replace it 
			else if(gScore < successor->g)
			{
				mOpen.erase(successor);
				successor->mParent = curr;
				successor->g = gScore;
				successor->f = successor->g + successor->h;

				mOpen.insert(successor);
			}
		}
	}

	mOpen.clear();
	mClosed.clear();
	return kInvalidXYLoc;
}

int GridPathPlanner::GetNumExpansions() {
	return mNumExpansions;
}

int GridPathPlanner::GetTotalExpansions() {
	return mTotalExpansions;
}
