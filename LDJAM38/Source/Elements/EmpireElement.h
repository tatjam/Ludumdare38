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

	int money;
	int food;
	int tech;
	int metal;

	int population;
	int soldiers;

	std::vector<Planet*> planets;

};