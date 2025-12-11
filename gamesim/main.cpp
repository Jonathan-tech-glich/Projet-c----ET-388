//============================================================================
// Name        : game_sim.cpp
// Author      : An, sam
// Description : Simple game sim template
//============================================================================

#include <iostream>
#include <iomanip>
#include <vector>
#include <ctime>
#include <string>
#include <algorithm>
#include "unitlist.h"
using namespace std;

// ******************** Function Defs ******************** //
static void decideTurnOrder(Unit* knight, Unit* wizard);
static bool isAllDead(vector<Unit*>& team);
static int sortInitiative(vector<Unit*>& unitGroup);

// ******************** Global Variables ***************** //
uid distAction(0,1);
vector<Unit*> unitOrder; // vector storing the Unit class

int main()
{
    cout << fixed << setprecision(2);
	hybridRandomShuffle(); // don't worry about this
	Knight knight(1);
    //Knight knight2(2);
    //Knight knight3(3);
	Wizard wizard(1);
    //Wizard wizard2(2);
    //Wizard wizard3(3);
	int roundCount = 0;

    // The following is for 1v1
	cout << "Knight begins with " << knight.getHealth() << " HP" << endl;
	cout << "Wizard begins with " << wizard.getHealth() << " HP" << endl;
	cout << "Checking Initiative ... " << endl;
	cout << "Knight's Initiative " << knight.initiative << endl;
	cout << "Wizard's Initiative " << wizard.initiative << endl;
	// Arrange turn order based in initiative (only for 1v1)
	//decideTurnOrder(&knight, &wizard); // use unitOrder to initiate attack
    
    // For multiple v. multiple
    // Use loop to display each unit's stats
    
    // For multiple units, sort based on initiative value (ascending)
    // First push the objects in unit order
    unitOrder.push_back(&knight);
    //unitOrder.push_back(&knight2);
    //unitOrder.push_back(&knight3);
    unitOrder.push_back(&wizard);
    //unitOrder.push_back(&wizard2);
    //unitOrder.push_back(&wizard3);
    int first = sortInitiative(unitOrder);

	cout << unitOrder[0]->name << "#" << first << " (Initiative: " << unitOrder[0]->initiative << ") will act first" << endl;

	// Create teams
	vector<Unit*> teamKnights;
	teamKnights.push_back(&knight);
	vector<Unit*> teamWizards;
	teamWizards.push_back(&wizard);
    
    // For multiple units in each team, have to loop and push_back() multiple times for each team

	// Create empty targets
	vector<Unit*> targets;

    // battle continue until last unit standing
    // This is only for 1v1 while (unitOrder.size() > 1)
    // Check for team exclusivity for mutliple units vs multiple units
	while(true) // while non of the teams are empty
	{
		roundCount++;
        cout << endl;
		cout << "=========== Start of Round " << roundCount << " ===========" << endl;
        cout << "Knight: " << knight.getHealth() << " HP " << endl;
        cout << "Wizard: " << wizard.getHealth() << " HP " << endl;
        cout << "Begin >>>>>>>>>>>>>>>>>>>>>>>" << endl;

		// for(short i : unitOrder) OR
		for(size_t i = 0; i < unitOrder.size(); i++) // use size_t from <vector> to avoid type cast warning
		{
			Unit* thisUnit = unitOrder[i]; // the current acting unit

			// Check if this unit is already dead
            // checkIfAlive return 1 for alive and 0 for dead
            // The logic !thisUnit->checkIfAlive() (! is NOT operator) return 1 for dead and 0 for alive
			if(!thisUnit->checkIfAlive()) //
			{
				continue; // Skip if unit is dead
			}

			string thisUnitName = thisUnit->name; // check the name of current acting unit
            targets = thisUnitName == "Knight" ? teamWizards : teamKnights; // identify targets
            
            /* //OR
			if(thisUnitName == "Knight")
			{
				targets = teamWizards;
			}
			else
			{
				targets = teamKnights;
			}
            */

			// Get live targets only
			vector<Unit*> liveTargets;
			for(Unit* t : targets)
			{
				if(t->checkIfAlive())
				{
					liveTargets.push_back(t);
				}
			}

			if (liveTargets.empty()) // check if all live targets are dead
			{
			    continue; // nothing to attack
			}

			// Pick a random target from live targets
			uid distTarget(0, liveTargets.size() - 1);
            // generate random integer representing target in the opposing team
            short targetIdx = distTarget(genRand);
            // store selected target in current target (thisTarget)
			Unit* thisTarget = liveTargets[targetIdx];

			// Random Action
            short thisAction = distAction(genRand);
			if(thisAction == 0)
			{
				thisUnit->heal();
			}
			else
			{
				thisUnit->attack(*thisTarget);
			}
			
            // Remove dead units from unitOrder stack
			for (size_t i = unitOrder.size(); i-- > 0;) // reverse iteration C++ syntax that works with size_t
            // OR for(int i = unitOrder.size()-1; i >= 0; i--) // triggers warning cuz size_t is unsigned
			{
			    if (!unitOrder[i]->checkIfAlive())
			    {
			        unitOrder.erase(unitOrder.begin() + i);
			    }
			}
            
            /* //OR using function handle (more complicated but more C++-ish)
            // Remove dead units from unitOrder stack (alternate: using remove_if())
            unitOrder.erase(
                remove_if(unitOrder.begin(), unitOrder.end(),
                          [](Unit* u){ return !u->checkIfAlive(); }),
                unitOrder.end()
            );
            */
		}
        cout << "=========== End of Round " << roundCount << " ===========" << endl;
        
        if (isAllDead(teamKnights))
        {
            cout << "Team Wizard WON!" << endl;
            break;
        }
        if (isAllDead(teamWizards))
        {
            cout << "Team Knight WON!" << endl;
            break;
        }
	}

    cout << endl;
    
    /* // This is for 1v1
	if(unitOrder[0]->name == "Knight")
	{
		cout << "Team Knight WON!" << endl;
	}
	else
	{
		cout << "Team Wizard WON!" << endl;
	}
    */
    
    // OR team check for multiple units in each team
    // Check the team vector for empty array
    // check if a team has been wiped


	return 0;
}


static void decideTurnOrder(Unit* knight, Unit* wizard)
{
	if(knight->initiative > wizard->initiative)
	{
		cout << "Knight has higher initiative ("<< knight->initiative <<"), will act first" << endl;
		// push_back() method creates stack by adding element at the end
		unitOrder.push_back(knight); // push_back knight first
		unitOrder.push_back(wizard); // push_back wizard second
	}
	else if(knight->initiative < wizard->initiative)
	{
		cout << "Wizard has higher initiative ("<< wizard->initiative <<"), will act first" << endl;
		unitOrder.push_back(wizard);
		unitOrder.push_back(knight);
	}
	else
	{
		cout << "Equal Initiative. Unit with lower health will act first" << endl;
		if(knight->health < wizard->health)
		{
			cout << "Knight will act first" << endl;
			unitOrder.push_back(knight); // push_back knight first
			unitOrder.push_back(wizard); // push_back wizard second
		}
		else
		{
			cout << "Wizard will act first" << endl;
			unitOrder.push_back(wizard); // push_back wizard first
			unitOrder.push_back(knight); // push_back knight second
		}
	}
}

static bool isAllDead(vector<Unit*>& team)
{
    for (Unit* i : team)
    {
        if (i->checkIfAlive())
        {
            return false;
        }
    }
    return true;
}

static int sortInitiative(vector<Unit*>& unitGroup)
{
    // Sort unit based on initiative and return the ID of the one with highest initiative
    sort(unitGroup.begin(), unitGroup.end(),
         [](Unit* a, Unit* b){ return a->initiative > b->initiative; });
    return unitGroup[0]->unitID;
}
