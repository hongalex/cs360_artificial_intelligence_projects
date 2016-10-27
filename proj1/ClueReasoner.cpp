#include "ClueReasoner.h"
using namespace std;

// #define DEBUG false

// void print(int toPrint) {
// 	if(DEBUG)
// 		cout << toPrint << " ";
// }

// void printLine() {
// 	if(DEBUG)
// 		cout << endl;
// }

int ClueReasoner::GetPlayerNum(string player)
{
	if (player == case_file)
		return num_players;
	
	for (int i = 0; i < num_players; i++)
		if (player == players[i])
			return i;
			
	cout<<"Illegal player: "<<player<<endl;
	return -1;
}
int ClueReasoner::GetCardNum(string card)
{
	for (int i = 0; i < num_cards; i++)
		if (card == cards[i])
			return i;
			
	cout<<"Illegal card: "<<card<<endl;
	return -1;
}

string ClueReasoner::QueryString(int return_code)
{
	if (return_code == kFalse)
		return "n";
	else if (return_code == kTrue)
		return "Y";
	else
		return "-";
}

void ClueReasoner::PrintNotepad()
{
	for (int i = 0; i < num_players; i++)
		cout<<"\t"<<players[i];
	cout<<"\t"<<case_file<<endl;
	
	for (int i = 0; i < num_cards; i++)
	{
		cout<<cards[i]<<"\t";
		for (int j = 0; j < num_players; j++)
			cout<<QueryString(Query(players[j], cards[i]))<<"\t";
		
		cout<<QueryString(Query(case_file, cards[i]))<<endl;
	}
}
	
void ClueReasoner::AddInitialClauses()
{
	/* The following code is given as an example to show how to create Clauses and post them to the solver. SatSolver.h uses the following typedefs:
		typedef int Literal;
		typedef std::vector<Literal> Clause;
		
	That is, a Literal (a propositional variable or its negation) is defined as a positive or a negative (meaning that it is in negated form, as in -p or -q) integer, and a Clause is defined as a vector of Literals.
	
	The function GetPairNum(p, c) returns the literal that corresponds to card c being at location p (either a player's hand or the case_file). 
	See ClueReasoner.h, lines 7-29 for a definition of the arrays and variables that you can use in your implementation. 
	*/

	// Each card is in at least one place (including the case file).
	for (int c = 0; c < num_cards; c++)	// Iterate over all cards.
	{
		Clause clause;
		for (int p = 0; p <= num_players; p++) 
		{	// Iterate over all players, including the case file (as a possible place for a card).
			clause.push_back(GetPairNum(p, c));
		}
		solver->AddClause(clause);
	}

	// If a card is in one place, it cannot be in another place.
	for(int c = 0; c < num_cards; c++) {
		for (int p1 = 0; p1 <= num_players; p1++) {	
			for(int p2 = p1+1; p2 <= num_players; p2++) {
				if(p1 != p2) { 
					Clause clause;
					clause.push_back(-1*GetPairNum(p1,c));
					clause.push_back(-1*GetPairNum(p2,c));
					solver->AddClause(clause);
				}
			}
		}
	}

	// At least one card of each category is in the case file.
	Clause sClause, wClause, rClause;
	for(int s = 0; s < num_suspects; s++) {
		sClause.push_back(GetPairNum("cf", suspects[s]));
	}
	solver->AddClause(sClause);


	for(int w = 0; w < num_weapons; w++) {
		wClause.push_back(GetPairNum("cf", weapons[w]));
	}
	solver->AddClause(wClause);


	for(int r = 0; r < num_rooms; r++) {
		rClause.push_back(GetPairNum("cf", rooms[r]));
	}
	solver->AddClause(rClause);


	// No two cards in each category can both be in the case file.
	for(int s = 0; s < num_suspects; s++) {
		for(int i= s+1; i < num_suspects; i++) {
			if(s != i) {
				Clause clause;
				clause.push_back(-1*GetPairNum("cf", suspects[s]));
				clause.push_back(-1*GetPairNum("cf", suspects[i]));
				solver->AddClause(clause);
			}
		}
	}

	for(int w = 0; w < num_weapons; w++) {
		for(int i= w+1; i < num_weapons; i++) {
			if(w != i) {
				Clause clause;
				clause.push_back(-1*GetPairNum("cf", weapons[w]));
				clause.push_back(-1*GetPairNum("cf", weapons[i]));
				solver->AddClause(clause);
			}
		}
	}

	for(int r = 0; r < num_rooms; r++) {
		for(int i= r+1; i < num_rooms; i++) {
			if(r != i) {
				Clause clause;
				clause.push_back(-1*GetPairNum("cf", rooms[r]));
				clause.push_back(-1*GetPairNum("cf", rooms[i]));
				solver->AddClause(clause);
			}
		}
	}

}
void ClueReasoner::Hand(string player, string cards[3])
{
	// GetPlayerNum returns the index of the player in the players array (not the suspects array). Remember that the players array is sorted wrt the order that the players play. Also note that, player_num (not to be confused with num_players) is a private variable of the ClueReasoner class that is initialized when this function is called.
	player_num = GetPlayerNum(player);
	
	for(int c = 0; c < num_cards; c++) {
		Clause clause;

		if(c == GetCardNum(cards[0]) || c == GetCardNum(cards[1])|| c == GetCardNum(cards[2])) {
			clause.push_back(GetPairNum(player_num, c));
		} 

		solver->AddClause(clause);
	}	
}
void ClueReasoner::Suggest(string suggester, string card1, string card2, string card3, string refuter, string card_shown)
{
	// Note that in the Java implementation, the refuter and the card_shown can be NULL. 
	// In this C++ implementation, NULL is translated to be the empty string "".
	// To check if refuter is NULL or card_shown is NULL, you should use if(refuter == "") or if(card_shown == ""), respectively.
	
	int suggesterNum = GetPlayerNum(suggester);

	//First, if there was no refuter, everyone else other than the suggester cannot have the cards 
	if(refuter == "" && card_shown == "") {
		for(int p = 0; p < num_players; p++) {
			if(suggester != players[p]) {
				Clause clause1, clause2, clause3;
				clause1.push_back(-1*GetPairNum(p,GetCardNum(card1)));
				clause2.push_back(-1*GetPairNum(p,GetCardNum(card2)));
				clause3.push_back(-1*GetPairNum(p,GetCardNum(card3)));
				solver->AddClause(clause1);
				solver->AddClause(clause2);
				solver->AddClause(clause3);
			}
		}
	} else {  
		int refuterNum = GetPlayerNum(refuter);
		//Increment first so it's immediately the next player
		if(suggesterNum==num_players-1) {
			suggesterNum = 0;
		} else {
			suggesterNum++;
		}
		//Players in the middle cannot have any of the suggested cards 
		while(suggesterNum!=refuterNum) {

			Clause clause1, clause2, clause3;
			clause1.push_back(-1*GetPairNum(suggesterNum,GetCardNum(card1)));
			clause2.push_back(-1*GetPairNum(suggesterNum,GetCardNum(card2)));
			clause3.push_back(-1*GetPairNum(suggesterNum,GetCardNum(card3)));
			solver->AddClause(clause1);
			solver->AddClause(clause2);
			solver->AddClause(clause3);

			if(suggesterNum==num_players-1) {
				suggesterNum = 0;
			} else {
				suggesterNum++;
			}
		}
		//The refuter has one of the three cards if card_shown == "", or has the card_shown
		if(card_shown=="") {
			Clause clause;
			clause.push_back(GetPairNum(suggesterNum,GetCardNum(card1)));
			clause.push_back(GetPairNum(suggesterNum,GetCardNum(card2)));
			clause.push_back(GetPairNum(suggesterNum,GetCardNum(card3)));
			solver->AddClause(clause);
		}
		else 
		{
			Clause clause;
			clause.push_back(GetPairNum(refuterNum, GetCardNum(card_shown)));
			solver->AddClause(clause);
		}
	}

}
void ClueReasoner::Accuse(string suggester, string card1, string card2, string card3, bool is_correct)
{
	// Optional
}
