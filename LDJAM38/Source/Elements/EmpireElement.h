#pragma once
#include <map>

#include "PlanetElement.h"

enum Relation
{
	ALLIANCE,
	FRIENDS,
	GOOD,
	NEUTRAL,
	COMPETENCE,
	ENEMIES,
	HATRED
};


class EmpireAI
{
public:
	Empire* linked;
	std::map<Empire*, Relation> relations;

	void update(float dt)
	{
		// Complex sentient AI incoming

	}
};

class EmpirePlayer
{
public:
	Empire* linked;
};

class Empire
{
public:

	int money = 0;
	int food = 0;
	int tech = 0;
	int metal = 0;

	int population = 0;
	int soldiers = 0;

	std::vector<Planet*> planets;

};