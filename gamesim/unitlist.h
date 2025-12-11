// Unit class definitions

#ifndef UNITLIST_H_
#define UNITLIST_H_

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <random>
#include <string>
#include <chrono>
#include <algorithm>

// ************************* Namespace *********************** //
using namespace std;

// ************************* Typedef ************************* //
typedef uniform_real_distribution<double> urd;
typedef uniform_int_distribution<short>   uid;
// OR Modern C++:
//using urd = uniform_real_distribution<double>;
//using uid = uniform_int_distribution<short>;
typedef unsigned int uint32_t;

// ************************* Defines ************************* //
#define KNIGHT_HP_MAX			(double)200.00
#define KNIGHT_HP_MIN			(double)100.00
#define KNIGHT_DMG_MAX			(double)70.00
#define KNIGHT_DMG_MIN 			(double)35.00
#define WIZARD_HP_MAX			(double)120.00
#define WIZARD_HP_MIN			(double)80.00
#define WIZARD_DMG_MAX			(double)100.00
#define WIZARD_DMG_MIN 			(double)60.00
#define HEAL_MAX				(double)40.00
#define HEAL_MIN				(double)15.00
#define INITIATIVE_MIN			(short)0
#define INITIATIVE_MAX			(short)200
#define USE_RNG_SHUFFLING		1 // set this to 0 if you want the same random number everytime
#define USE_RNG_SEED            (uint32_t)0 // can be any positive integer
#define CHECKNEGATIVE(a) 		(bool)(a < 0 ? true : false)
#define MINZERO(a)   			(a < 0 ? 0 : a)

// *********************** Function defs ********************** //
void hybridRandomShuffle();

// *********************** Global Variables *******************//
mt19937 genRand; // Global random generator engine

// ************************ Class Definitions **************************** //
class Unit
{
protected:
	double damage; // damage dealt per attack
	short action; // 0 for heal, 1 for attack, -1 for init
public:
	double health; // health point
	double initiative; // 0 to 10, the higher takes the first turn
	string name;
    int unitID;
public:
	Unit()
	{
		health = 0;
		damage = 0;
		action = -1;
		initiative = 0;
		name = "";
	}

	virtual ~Unit() {};

	// set to 0 for pure virtual fcn (abstract base method)
	virtual void attack(Unit &target) = 0; // attack action
	virtual void heal() = 0; // heal action
	virtual bool checkIfAlive() = 0; // check if dead or alive

	void setHealth(double hp)
	{
		health = hp;
	}

	double getHealth()
	{
		return health;
	}

	double getDamage()
	{
		return damage;
	}
};

class Knight : public Unit
{
public:
	Knight(int ID)
	{
		// Initialize random distribution between [min max]
		urd distHP(KNIGHT_HP_MIN, KNIGHT_HP_MAX);
		urd distDmg(KNIGHT_DMG_MIN, KNIGHT_DMG_MAX);
		uid distInit(INITIATIVE_MIN,INITIATIVE_MAX);

		// Initialize Props
        unitID = ID;
		health = distHP(genRand);
		damage = distDmg(genRand);
		action = -1;
		initiative = distInit(genRand);
		name = "Knight";
	}

	~Knight() {};

	void attack(Unit &target) override
	{
		urd distDmg(KNIGHT_DMG_MIN, KNIGHT_DMG_MAX); // Initialize random distribution
		double dmg = distDmg(genRand); // Set damage for THIS attack action
		cout << "> Knight attack for " << dmg << " damage point.";
		double targetHealth = target.getHealth(); // Get health of the target
		target.setHealth(targetHealth - dmg);	  // Apply damage to target health
        double tmpHealth = target.getHealth();
        double tmp = MINZERO(tmpHealth);
        target.setHealth(tmp); // Health cannot be negative
		cout << " Targeted wizard's health point is now " << target.getHealth() << " HP" << endl;
	}

    void heal() override
    {
        urd distHeal(HEAL_MIN, HEAL_MAX); // Initialize random distribution
        double healAmount = distHeal(genRand); // Set heal amount for THIS heal action
        health += healAmount; // Add heal amount to health
        health = min(health, KNIGHT_HP_MAX); // restrict health to max if exceed
        cout << "> Knight heal for " << healAmount <<
        		" HP. Total Knight's health is now " << health << " HP" << endl;
    }

    bool checkIfAlive() override
    {
    	return health > 0 ? true : false; // returns true if unit is dead
    }
};

class Wizard : public Unit
{

public:
    Wizard(int ID)
    {
        urd distHP(WIZARD_HP_MIN, WIZARD_HP_MAX);
        urd distDmg(WIZARD_DMG_MIN, WIZARD_DMG_MAX);
		uid distInit(INITIATIVE_MIN,INITIATIVE_MAX);

		// Initialize Props
        unitID = ID;
        health = distHP(genRand);
        damage = distDmg(genRand);
        action = -1;
		initiative = distInit(genRand);
    	name = "Wizard";
    }

    ~Wizard() {};

    void attack(Unit &target) override
    {
        urd distDmg(WIZARD_DMG_MIN, WIZARD_DMG_MAX);
        double dmg = distDmg(genRand);
        cout << "> Wizard attack for " << dmg << " damage point.";
        double targetHealth = target.getHealth(); // Get health of the target
        target.setHealth(targetHealth - dmg);  // Apply damage to target health
        double tmpHealth = target.getHealth();
        double tmp = MINZERO(tmpHealth);
        target.setHealth(tmp); // Health cannot be negative
        cout << " Targeted knight's health point is now " << target.getHealth() << " HP" << endl;
    }

    void heal() override
    {
        uniform_real_distribution<double> distHeal(20.0, 50.0);
        double healAmount = distHeal(genRand);
        health += healAmount; // Add heal amount to health
        health = min(health, WIZARD_HP_MAX); // restrict health to max if exceed
        cout << "> Wizard heal for " << healAmount <<
               	" HP. Total Wizard's health is now " << health << " HP" << endl;
    }

    bool checkIfAlive() override
    {
    	return health > 0 ? true : false;
    }
};
// ************************ End Class Definitions **************************** //

// ************************ Function Implementations ************************** //
void hybridRandomShuffle()
{
	random_device randDev;
	uint32_t rdSeed = randDev();

#if USE_RNG_SHUFFLING
	// Simply use
	//genRand.seed(static_cast<unsigned>(time(nullptr)));
	// OR
	// Use system_clock instead of high_resolution_clock
	auto nowTime = std::chrono::system_clock::now();
	auto nowNanosec = std::chrono::duration_cast<std::chrono::nanoseconds>(nowTime.time_since_epoch()).count();
	uint32_t nowSeed = static_cast<unsigned int>(nowNanosec);

	// Combine seeds
	uint32_t hybridSeed = rdSeed ^ nowSeed; // ^ : bit-wise OR
	genRand.seed(hybridSeed);
#else
	genRand.seed(USE_RNG_SEED);
#endif
}
// ************************ End Function Implementations ************************** //


#endif /* UNITLIST_H_ */
