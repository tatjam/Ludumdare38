#pragma once
#include <map>

#include "../ImGui/imgui.h"
#include "PlanetElement.h"
#include "UniverseElement.h"
#include <SFML/Graphics.hpp>

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

#define STARTING_MONEY 1500
#define STARTING_FOOD 5000
#define STARTING_TECH 50
#define STARTING_METAL 50

#define FOOD_PER_PERSON 0.3f


#define METAL_FOR_LAUNCH 5000
#define TECH_FOR_LAUNCH 3500
#define CASH_FOR_LAUNCH 8000

enum EventSeverity
{
	INFO,
	WARN,
	FATAL
};

enum Personality
{
	FRIENDLY,
	NORMAL, 
	HOSTILE
};

struct Event
{
	EventSeverity severity;
	std::string text;

	Event(EventSeverity severity, std::string text)
	{
		this->severity = severity;
		this->text = text;
	}
};

class EmpireAI;


#define CASH_PER_SOLDIER 200
#define METAL_PER_SOLDIER 100
#define TECH_PER_SOLDIER 100

class Empire
{
public:


	// Multiplies every positive economic action
	// Make it lower to make AI have trouble being 
	// SO FUCKING FAST
	float handicap = 1.0f;

	EmpireAI* aicontroller;

	std::vector<Event> events;

	std::vector<Empire*> otherEmpires;

	// Used for plots

	std::vector<int> previousMoney = std::vector<int>();
	std::vector<int> previousFood = std::vector<int>();
	std::vector<int> previousTech = std::vector<int>();
	std::vector<int> previousMetal = std::vector<int>();

	std::string name = "NOT_SET";

	int money = STARTING_MONEY;
	int moneydaily = 0;
	int food = STARTING_FOOD;
	int fooddaily = 0;
	int tech = STARTING_TECH;
	int techdaily = 0;
	int metal = STARTING_METAL;
	int metaldaily = 0;

	int happiness = 100;

	int workplaces = 0;

	int workers = 0;
	int housed = 0;
	int population = 10;
	int soldiers = 5;


	int launcherCount = 0;
	int launchCount = 0;
	int daysSinceLaunch = 0;

	bool canLaunch = false;

	int total_houses = 0;

	float wantedTaxes = 0.5f;
	float taxes = 0.5f;

	int timeSinceLastLauncher = 0;

	int timeSinceAttack = 0;

	float workEfficiency = 1.0f;

	std::vector<Planet*> planets;

	Planet* toBeAdded;
	bool waitingToAdd = false;

	Empire()
	{
		planets = std::vector<Planet*>();
		previousMoney.push_back(STARTING_MONEY);
		previousFood.push_back(STARTING_FOOD);
		previousMetal.push_back(STARTING_METAL);
		previousTech.push_back(STARTING_TECH);
	}

	void receiveAttack(Empire* attacker)
	{
		std::string message = "You have been attacked by: ";
		message.append(attacker->name.c_str());
		message.append(". War has been declared");
		events.push_back(Event(EventSeverity::FATAL, message));
	}

	void destroyBuildings(Planet* target, int count)
	{
		int destroyed = 0;
		bool done = false;
		if (target->usedtiles < count)
		{
			count = target->usedtiles;
		}
		printf("Deleting %i tiles\n", count);

		while(!done)
		{
			printf("DELETING TILE?");
			int pos = rand() % target->size;
			if (target->tiles[pos] != 0)
			{
				target->tiles[pos] = 0;
				target->tilesBuilding[pos] = 0;
				destroyed++;
				printf("OK!\n");
			}
			else
			{
				printf("NAY!\n");
			}
			if (destroyed >= count)
			{
				done = true;
			}
		}
	}

	void attack(Planet* target, int sold, bool annex)
	{
		if (target->owner != NULL && target->owner != this)
		{


			float ourPower = (float)sold;
			float theirPower = (float)target->owner->soldiers;

			float ourLuck = ((float)(rand() % 100) / 100.0f) + 0.7f;
			float theirLuck = ((float)(rand() % 100) / 100.0f) + 0.7f;


			ourPower *= ourLuck;
			theirPower *= theirLuck;


			if (ourPower > 30)
			{


				if (ourPower > theirPower)
				{


					if (annex)
					{
						// We can take over the planet!
						// Also some stuff will be damaged and a portion 
						// of their population will move to our civilization
						int buildingsToDestroy = (theirPower / ourPower) * 10;

						std::string message = "Our forces sucessfully annexed planet ";
						message.append(target->name.c_str());
						message.append(". Buildings destroyed: ");
						message.append(std::to_string((int)((ourPower / theirPower) * 10)));
						events.push_back(Event(EventSeverity::FATAL, message));

						target->owner->planets.erase(
							std::remove(target->owner->planets.begin(),
								target->owner->planets.end(),
								target), target->owner->planets.end());

						target->owner = this;
						population += target->owner->population / 6;
						target->owner->population -= target->owner->population / 6;
						planets.push_back(target);

						int toDestroy = (int)((ourPower / theirPower) * 10);

						destroyBuildings(target, toDestroy);
					}
					else
					{
						// We will do a lot of damage but not
						// destroy it completely
						int buildingsToDestroy = (theirPower / ourPower) * 30;
						std::string message = "Our forces sucessfully damaged planet ";
						message.append(target->name.c_str());
						message.append(". Buildings destroyed: ");
						message.append(std::to_string((int)((ourPower / theirPower) * 30)));
						events.push_back(Event(EventSeverity::FATAL, message));

						int toDestroy = (int)((ourPower / theirPower) * 30);

						destroyBuildings(target, toDestroy);
					}
				}
				else
				{
					std::string message = "We could not win the defensive forces at ";
					message.append(target->name.c_str());
					message.append(". Buildings destroyed: ");
					message.append(std::to_string((int)((ourPower / theirPower) * 10)));
					events.push_back(Event(EventSeverity::FATAL, message));
					// We will do a bit of damage to buildings
					int buildingsToDestroy = (ourPower / theirPower) * 10;

					int toDestroy = (int)((ourPower / theirPower) * 10);

					destroyBuildings(target, toDestroy);
				}
			}
			else
			{
				events.push_back(Event(EventSeverity::FATAL, "We couldn't do any damage to land, but managed to get a few soldiers"));
			}

			soldiers -= sold;

			target->owner->soldiers -= (int)ourPower;
			if (target->owner->soldiers < 0)
			{
				target->owner->soldiers = 0;
			}

			/*if (ourPower > theirPower)
			{

			}
			else
			{
				// All our units are annihilated
				soldiers -= sold;
			}*/

		}
	}

	void updateMonthly()
	{
		if (waitingToAdd)
		{
			planets.push_back(toBeAdded);
			toBeAdded->alreadyOwned = true;
			waitingToAdd = false;
			toBeAdded = NULL;
			events.push_back(Event(EventSeverity::FATAL, "Your vessel has arrived!"));


		}

		taxes = wantedTaxes;

		// You pay for every citizen a bit (monthly)? It's exponential so that late game empires are not
		// cash mountains
		money -= (int)((float)population * (float)population * 0.005f);
		
		if (food < population)
		{
			events.push_back(Event(EventSeverity::FATAL, "Your food stock is very low! Your population will starve!"));
			happiness -= 10;
		}

		if (money < 0)
		{
			events.push_back(Event(EventSeverity::FATAL, "Your balance is negative. This will get your population unhappy!"));

			// Every 100 negative money takes away 1 happiness point
			happiness += money / 100;
		}

		if (happiness > 0)
		{
			// Every 20 happiness points brings efficiency to a double
			workEfficiency = happiness / 20 + 1.0f;
		}
		else
		{
			// Negative happiness halfs efficiency
			workEfficiency = 0.5f;
		}

		if (happiness >= 0)
		{
			// Every 1 happiness points you get you 2 person
			// 0 gets you stable population
			population += (int)(2.0f * happiness);
		}
		else
		{
			events.push_back(Event(EventSeverity::FATAL, "Your population is unhappy! Work efficiency is halved and you are losing population!"));
			if (population > 15)
			{

				if (population + (int)(0.25f * happiness) <= 15)
				{
					population = 15;
				}
				else
				{
					// Every 4 negative happiness points you lose a person (migration, or mighty death!)
					population += (int)(0.25f * happiness);
				}
			}
		}
	}

	bool launchReady()
	{
		if (metal > METAL_FOR_LAUNCH && tech > TECH_FOR_LAUNCH && money > CASH_FOR_LAUNCH)
		{
			for (int i = 0; i < planets.size(); i++)
			{
				for (int t = 0; t < planets[i]->size; t++)
				{
					if (planets[i]->tiles[t] == BUILDING_LAUNCHER)
					{
						if (planets[i]->tilesBuilding[t] >= 1.0f)
						{
							return true;
						}
					}
				}
			}
		}

		return false;
	}

	void launchVehicle(Planet* target)
	{
		for (int i = 0; i < planets.size(); i++)
		{
			for (int t = 0; t < planets[i]->size; t++)
			{
				if (planets[i]->tiles[t] == BUILDING_LAUNCHER)
				{
					if (planets[i]->tilesBuilding[t] >= 1.0f)
					{
						if (target->owner == NULL)
						{
							events.push_back(Event(EventSeverity::FATAL, "You have launched a vehicle! It will arrive in a month"));
							waitingToAdd = true;
							toBeAdded = target;
							target->owner = this;
							target->alreadyOwned = false;
							planets[i]->tiles[t] = 0;
							planets[i]->tilesBuilding[t] = 0;
							money -= (float)CASH_FOR_LAUNCH * (1.0f / handicap);
							metal -= (float)METAL_FOR_LAUNCH * (1.0f / handicap);
							tech -= (float)TECH_FOR_LAUNCH * (1.0f / handicap);
						}
					}
				}
			}
		}
	}

	// Called every day
	void updateDaily()
	{
		/*if (events.size() > 5)
		{
			events.erase(events.begin());
		}*/

		happiness = 0;

		previousMetal.push_back(metal);
		previousFood.push_back(food);
		previousTech.push_back(tech);
		previousMoney.push_back(money);


		if (food < population)
		{
			happiness -= 10;
			population -= 1;
		}


		if (launchCount != 0)
		{
			daysSinceLaunch++;
		}

		if (launchCount > 0)
		{
			if (daysSinceLaunch > (int)((1.0f / (float)launcherCount) * 365.f))
			{
				canLaunch = true;
			}
		}

		int usedPopulation = 0;

		int housedPopulation = 0;


		int houses = 0;

		techdaily = 0;
		moneydaily = 0;
		fooddaily = 0;
		metaldaily = 0;


		fooddaily -= (float)population * FOOD_PER_PERSON;

		food -= (float)population * FOOD_PER_PERSON;

		workplaces = 0;

		int freePopulation = population;

		for (int i = 0; i < planets.size(); i++)
		{
			if (planets[i]->easyHabitable)
			{
				// 2 free happiness for habitable planet. Maybe more or less required
				happiness += 2;
			}
			// Do daily maintenance and daily generations
			// Also calculate employment and run buildings as required
			for (int t = 0; t < planets[i]->size; t++)
			{
				if (planets[i]->tiles[t] != 0)
				{
					if (planets[i]->tilesBuilding[t] >= 1.0f)
					{
						switch (planets[i]->tiles[t])
						{
						case BUILDING_HOUSE:
							money -= (float)HOUSE_MAINTENANCE * (1.0f / handicap);
							moneydaily -= (float)HOUSE_MAINTENANCE * (1.0f/handicap);
							if (money >= HOUSE_MAINTENANCE)
							{
								if (housedPopulation + HOUSE_HOUSING > population)
								{
									housedPopulation = population;
								}
								else
								{
									housedPopulation += HOUSE_HOUSING;
								}

								houses += HOUSE_HOUSING;
							}
							break;
						case BUILDING_LABORATORY:

							workplaces += LAB_EMPLOYMENT;
							money -= (float)LAB_MAINTENANCE * (1.0f / handicap);
							moneydaily -= (float)LAB_MAINTENANCE * (1.0f / handicap);
							if (money >= LAB_MAINTENANCE && usedPopulation < population)
							{
								tech += (int)((float)LAB_GENERATION * planets[i]->scienceBoost);
								techdaily += (int)((float)LAB_GENERATION * planets[i]->scienceBoost);

							}
							if (usedPopulation + LAB_EMPLOYMENT < population)
							{
								usedPopulation += LAB_EMPLOYMENT;
							}
							else
							{
								usedPopulation = population;
							}
							break;
						case BUILDING_APPARTMENT:
							money -= (float)APP_MAINTENANCE * (1.0f / handicap);
							moneydaily -= (float)APP_MAINTENANCE * (1.0f / handicap);
							if (money >= APP_MAINTENANCE) {
								if (housedPopulation + APP_HOUSING > population)
								{
									housedPopulation = population;
								}
								else
								{
									housedPopulation += APP_HOUSING;
								}

								houses += APP_HOUSING;
							}
							break;
						case BUILDING_FARM:
							workplaces += FARM_EMPLOYMENT;
							money -= (float)FARM_MAINTENANCE * (1.0f / handicap);
							moneydaily -= (float)FARM_MAINTENANCE * (1.0f / handicap);

							if (money >= FARM_MAINTENANCE && usedPopulation < population)
							{
								food += (int)((float)FARM_GENERATION * planets[i]->foodBoost);
								fooddaily += (int)((float)FARM_GENERATION * planets[i]->foodBoost);
							}

							if (usedPopulation + FARM_EMPLOYMENT < population)
							{
								usedPopulation += FARM_EMPLOYMENT;
							}
							else
							{
								usedPopulation = population;
							}

							break;
						case BUILDING_MINE:
							workplaces += MINE_EMPLOYMENT;
							money -= (float)MINE_MAINTENANCE * (1.0f / handicap);
							moneydaily -= (float)MINE_MAINTENANCE * (1.0f / handicap);

							if (money >= MINE_MAINTENANCE && usedPopulation < population)
							{
								metal += (int)((float)MINE_GENERATION * planets[i]->mineralBoost);
								metaldaily += (int)((float)MINE_GENERATION * planets[i]->mineralBoost);
							}

							if (usedPopulation + MINE_EMPLOYMENT < population)
							{
								usedPopulation += MINE_EMPLOYMENT;
							}
							else
							{
								usedPopulation = population;
							}
							break;
						case BUILDING_MARKET:
							workplaces += MARKET_EMPLOYMENT;

							if (usedPopulation < population)
							{
								money += MARKET_GENERATION;
								moneydaily += MARKET_GENERATION;
							}

							if (usedPopulation + MARKET_EMPLOYMENT < population)
							{
								usedPopulation += MARKET_EMPLOYMENT;
							}
							else
							{
								usedPopulation = population;
							}
							break;
						case BUILDING_SMARKET:
							workplaces += SMARKET_EMPLOYMENT;

							if (usedPopulation < population)
							{
								money += SMARKET_GENERATION;
								moneydaily += SMARKET_GENERATION;
							}

							if (usedPopulation + SMARKET_EMPLOYMENT < population)
							{
								usedPopulation += SMARKET_EMPLOYMENT;
							}
							else
							{
								usedPopulation = population;
							}
							break;
						case BUILDING_HFARM:
							workplaces += HFARM_EMPLOYMENT;
							money -= (float)HFARM_MAINTENANCE * (1.0f / handicap);
							moneydaily -= (float)HFARM_MAINTENANCE * (1.0f / handicap);

							if (money >= HFARM_MAINTENANCE && usedPopulation < population)
							{
								food += (int)((float)HFARM_GENERATION * planets[i]->foodBoost);
								fooddaily += (int)((float)HFARM_GENERATION * planets[i]->foodBoost);
							}

							if (usedPopulation + HFARM_EMPLOYMENT < population)
							{
								usedPopulation += HFARM_EMPLOYMENT;
							}
							else
							{
								usedPopulation = population;
							}
							break;
						}
					}
					else
					{
						float increase = 1 / (float)Planet::getBuildingTime(planets[i]->tiles[t]);
						planets[i]->tilesBuilding[t] += increase;

						if (planets[i]->tilesBuilding[t] >= 1.0f)
						{
							if (planets[i]->tiles[t] == BUILDING_LAUNCHER)
							{
								events.push_back(Event(EventSeverity::FATAL, "Your space launch center is ready!"));
							}
						}
					}
				}
			}
		}

		if (housed >= population)
		{
			happiness += 10;
		}
		else if (housed >= population * 0.8)
		{
			happiness += 3;
		}
		else if (housed >= population * 0.5)
		{
			happiness -= 1;
		}
		else
		{
			happiness -= 15;
		}

		if (workers >= population)
		{
			happiness += 10;
		}
		else if (workers >= population * 0.8)
		{
			happiness += 3;
		}
		else if (workers >= population * 0.5)
		{
			happiness -= 2;
		}
		else
		{
			happiness -= 10;
		}

		if (taxes == 0)
		{
			happiness += 25;
		}
		else
		{
			// We use -1.5(x-2)^3 as our happiness formula
			happiness += powf(-1.5f*(taxes - 2.0f), 3.0f);
		}

		workers = usedPopulation;
		housed = housedPopulation;

		total_houses = houses;
		// Generate money from taxes (only employed pays)
		money += (int)((float)taxes * (float)usedPopulation);
		moneydaily += (int)((float)taxes * (float)usedPopulation);
	}


};


#define AI_SAFE_MARGIN 50

enum AIFocus
{
	MAKE_MONEY,
	MAKE_HAPPY,
	MAKE_SOLDIERS,
	EXPAND,
};


class EmpireAI
{
public:

	Personality personality = Personality::NORMAL;

	AIFocus focus = MAKE_HAPPY;

	bool firstframe = true;

	Empire* linked;
	std::map<Empire*, Relation> relations;

	Universe* universe;

	int timeSinceUpdate = 99;

	void receiveAttack(Empire* attacker)
	{
		if (relations[attacker] == Relation::ENEMIES)
		{
			relations[attacker] = Relation::HATRED;
		}
		else if (relations[attacker] == Relation::ALLIANCE)
		{
			if (personality == Personality::FRIENDLY)
			{
				relations[attacker] = Relation::COMPETENCE;
			}
			else
			{
				// Traitors!!
				relations[attacker] = Relation::HATRED;
			}
		}
		else
		{
			relations[attacker] = Relation::ENEMIES;
		}

	}

	bool aiBuild(int buildingType)
	{
		// Will find free space and build a building duh
		// If the building extracts resource it will prioritize 
		// planets with a boost of some kinds

		bool freeSpace = false;
		for (int i = 0; i < linked->planets.size(); i++)
		{
			if (linked->planets[i] != NULL)
			{
				for (int t = 0; t < linked->planets[i]->size; t++)
				{
					if (linked->planets[i]->tiles[t] == 0)
					{
						freeSpace = true;
					}
				}
			}
		}

		
		if(linked->money - Planet::getBuildingPrice(buildingType) > AI_SAFE_MARGIN)
		{
			if (buildingType == BUILDING_LAUNCHER && !freeSpace)
			{
				for (int i = 0; i < linked->planets.size(); i++)
				{
					if (linked->planets[i] != NULL)
					{
						for (int t = 0; t < linked->planets[i]->size; t++)
						{
							if (linked->planets[i]->tiles[t] != BUILDING_LAUNCHER)
							{
								linked->planets[i]->tiles[t] = BUILDING_LAUNCHER;
								linked->planets[i]->tilesBuilding[t] = 0;
								linked->money -= Planet::getBuildingPrice(BUILDING_LAUNCHER);
								//linked->planets[i]->usedtiles++; // TILES STAY THE SAME
								return true;
							}
						}
					}
				}
			}

			// Anywhere with free space!
			for (int i = 0; i < linked->planets.size(); i++)
			{
				if (linked->planets[i] != NULL)
				{
					for (int t = 0; t < linked->planets[i]->size; t++)
					{
						if (linked->planets[i]->tiles[t] == 0)
						{
							// Build here
							linked->planets[i]->tiles[t] = buildingType;
							linked->planets[i]->tilesBuilding[t] = 0.0f;
							linked->money -= Planet::getBuildingPrice(buildingType);
							linked->planets[i]->usedtiles++;
							return true;
						}
					}
				}
			}

		}

		return false;

	}

	bool peaceful = false;

	void aiLaunch()
	{
		for (int i = 0; i < universe->planets.size(); i++)
		{
			if (universe->planets[i]->owner == NULL)
			{
				linked->launchVehicle(universe->planets[i]);
			}
		}
	}

	void aiAttack(Empire* e)
	{
		if (peaceful == false)
		{
			if (linked->soldiers < 35)
			{
				if (personality == Personality::HOSTILE || personality == Personality::NORMAL)
				{
					focus = AIFocus::MAKE_SOLDIERS;
				}
			}
			else
			{
				int estimated = e->soldiers;
				estimated += (rand() % e->soldiers / 2) - e->soldiers / 2;
				printf("estimated %i\n", estimated);
				// Attack random planet of empire
				int n = rand() % e->planets.size();
				linked->attack(e->planets[n], estimated + 100, rand() % 2);
				relations[e] = Relation::ENEMIES;
			}
		}
		else
		{
			// We don't attack in peaceful mode! Even if we are attacked
		}
	}

	void updateDaily()
	{

		linked->updateDaily();

		linked->timeSinceLastLauncher++;
		if (linked->launchReady())
		{
			aiLaunch();
		}


		linked->timeSinceAttack++;
		if (linked->timeSinceAttack > 25)
		{
			if (personality == Personality::FRIENDLY)
			{
				std::vector<Empire*> enemies;
				// Only attack enemies/hated
				for (std::map<Empire*, Relation>::iterator it = relations.begin(); it != relations.end(); ++it)
				{
					if (it->second == Relation::ENEMIES || it->second == Relation::HATRED)
					{
						enemies.push_back(it->first);
					}
				}
				if (enemies.size() != 0)
				{
					int n = rand() % enemies.size();

					aiAttack(enemies[n]);
				}

			}
			else if (personality == Personality::NORMAL)
			{
				// Attack enemies/hated or if no planet is free the weakest (TODO)
				std::vector<Empire*> enemies;
				// Only attack enemies/hated
				for (std::map<Empire*, Relation>::iterator it = relations.begin(); it != relations.end(); ++it)
				{
					if (it->second == Relation::ENEMIES || it->second == Relation::HATRED)
					{
						enemies.push_back(it->first);
					}
				}
				if (enemies.size() != 0)
				{
					int n = rand() % enemies.size();

					aiAttack(enemies[n]);
				}
			}
			else
			{
				// Attack enemies/hated/competency or at random anybody
				if (rand() < RAND_MAX / 6)
				{
					std::vector<Empire*> pep;
					// Attack a random person
					for (std::map<Empire*, Relation>::iterator it = relations.begin(); it != relations.end(); ++it)
					{
						pep.push_back(it->first);
					}

					int n = rand() % pep.size();

					aiAttack(pep[n]);
				}
				else
				{
					std::vector<Empire*> enemies;
					// Only attack enemies/hated
					for (std::map<Empire*, Relation>::iterator it = relations.begin(); it != relations.end(); ++it)
					{
						if (it->second == Relation::ENEMIES || it->second == Relation::HATRED)
						{
							enemies.push_back(it->first);
						}
					}
					if (enemies.size() != 0)
					{
						int n = rand() % enemies.size();

						aiAttack(enemies[n]);
					}
				}
				
			}
			linked->timeSinceAttack = 0;
		}

		/*printf("-----------------------\n");
		printf("AI State:\n");
		printf("-----------------------\n");
		printf("Cash: %i (%i) | Metal: %i (%i) | \nFood: %i (%i) | Tech: %i (%i)\n", 
			linked->money, linked->moneydaily, linked->metal, linked->metaldaily,
			linked->food, linked->fooddaily, linked->tech, linked->techdaily);
		printf("Happiness: %i | Population: %i\n", linked->happiness, linked->population);
		printf("AIFocus: %i\n", focus);*/

		// We only do shit every week cause reasons
		if (timeSinceUpdate >= 7)
		{
			if (linked->moneydaily < 0)
			{
				focus = AIFocus::MAKE_MONEY;
			}

			if (linked->happiness < 0)
			{
				focus = AIFocus::MAKE_HAPPY;
			}

			if (linked->fooddaily < 0)
			{
				//printf("AI building farm!");
				if (!aiBuild(BUILDING_HFARM))
				{
					aiBuild(BUILDING_FARM);
				}
			}

			if (linked->moneydaily > 4 && linked->metaldaily > 4 && linked->techdaily > 4 && linked->fooddaily > 4
				&& linked->timeSinceLastLauncher >= 240)
			{
				//printf("AI has stable economy, proceding to build launcher");
				focus = AIFocus::EXPAND;
			}

			if (focus == AIFocus::MAKE_HAPPY)
			{
				if (linked->happiness > 0)
				{
					if (linked->moneydaily > 0)
					{
						focus = AIFocus::EXPAND;
					}
					else
					{
						focus = AIFocus::MAKE_MONEY;
					}
				
				}
				// Find reason we are unhappy

				if (linked->money < 0)
				{
					// We need to make money! Try to sell stuff to other empires
					// and build markets. Also maybe make taxes bigger?
					//printf("AI making money (markets and trading)\n");

					if (aiBuild(BUILDING_MARKET) == false)
					{
						aiBuild(BUILDING_SMARKET);
					}
				}
				else if ((float)linked->housed / (float)linked->population < 0.8f)
				{
					// We need to build houses!
					//printf("AI building houses!\n");
					if (!aiBuild(BUILDING_APPARTMENT))
					{
						aiBuild(BUILDING_HOUSE);
					}
				}
				else if ((float)linked->workers / (float)linked->population < 0.8f)
				{
					// We need more places to work!
					//printf("AI is making workplaces!\n");

					// Build stuff that generates whatever we lack
					int lowestN = 0;
					int lowest = INT_MAX;
					if (linked->money < lowest)
					{	
						lowestN = 0;
						lowest = linked->money;
					}
					if (linked->food < lowest)
					{
						lowestN = 1;
						lowest = linked->food;
					}
					if (linked->tech < lowest)
					{
						lowestN = 2;
						lowest = linked->tech;
					}
					if (linked->metal < lowest)
					{
						lowestN = 3;
						lowest = linked->metal;
					}

					if (linked->moneydaily < 10)
					{
						lowestN = 0;
					}

					switch (lowestN)
					{
						case 0:
							//printf("Building money generator\n");
							if (!aiBuild(BUILDING_MARKET))
							{
								aiBuild(BUILDING_SMARKET);
							}
							break;
						case 1:
							//printf("Building farms\n");
							if (!aiBuild(BUILDING_HFARM))
							{
								aiBuild(BUILDING_FARM);
							}
							break;
						case 2:
							//printf("Building lab\n");
							if (!aiBuild(BUILDING_LABORATORY))
							{
								aiBuild(BUILDING_SMARKET);
							}
							break;
						case 3:
							//printf("Building mine!\n");
							aiBuild(BUILDING_MINE);
							break;
					}

				}
				else
				{
					// ???
					//printf("AI is decreasing taxes!\n");
					linked->wantedTaxes = linked->wantedTaxes - 0.4f;
				}
			}
			else if (focus == AIFocus::MAKE_MONEY)
			{
				if (linked->happiness > 8)
				{
					// We can increase taxes a bit
					//printf("AI is increasing taxes!\n");
					linked->wantedTaxes = linked->wantedTaxes + 0.4f;
					timeSinceUpdate = 5;
				}
				else
				{
					//printf("AI is building markets and trying to trade!\n");
					if (!aiBuild(BUILDING_MARKET))
					{
						if (!aiBuild(BUILDING_SMARKET))
						{
							linked->wantedTaxes += 0.4f;
						}
					}
				}
			}
			else if (focus == AIFocus::MAKE_SOLDIERS)
			{
				//printf("AI is building all kind of stuff but focusing on war stuff!\n");
				if (linked->happiness < 20)
				{
					focus = AIFocus::MAKE_HAPPY;
				}

				if (rand() > RAND_MAX / 2)
				{
					// WAR STUFF
					// Make some soldiers
					//printf("Making some soldiers!\n");
				}
				else
				{
					// Other stuff
					if (rand() > RAND_MAX / 2)
					{
						//printf("AI building market\n");
						if (!aiBuild(BUILDING_MARKET))
						{
							aiBuild(BUILDING_SMARKET);
						}
						else
						{
							printf("AI building mine\n");
							aiBuild(BUILDING_MINE);
						}
					}
					else
					{
						//printf("AI building lab!\n");
						aiBuild(BUILDING_LABORATORY);
					}
				}
			}
			else if (focus == AIFocus::EXPAND)
			{
				//printf("AI waiting to build launcher!\n");
				if (linked->money > LAUNCHER_PRICE + AI_SAFE_MARGIN && linked->timeSinceLastLauncher >= 240)
				{
					//printf("AI will build launcher!\n");
					aiBuild(BUILDING_LAUNCHER);
					linked->timeSinceLastLauncher = 0;
				}
				else
				{
					focus = AIFocus::MAKE_SOLDIERS;
					//WAIT
					if (linked->population > 200)
					{
						// Make some soldiers
					}
				}
			}
			timeSinceUpdate = 0;
		}

		timeSinceUpdate++;
	}

	void updateMonthly()
	{
		linked->updateMonthly();

		// Here we play the game for real
		// Decisions are monthly

		if (!firstframe)
		{

			if (linked->happiness > 10)
			{
				// If we are happy we can focus on other stuff
				if (linked->money > 0)
				{
					if (linked->metal > METAL_FOR_LAUNCH && linked->tech > METAL_FOR_LAUNCH)
					{
						printf("AI is launching vessel!\n");


						if (personality == Personality::HOSTILE || personality == Personality::NORMAL)
						{
							focus = MAKE_SOLDIERS;
						}
						else
						{
							focus = EXPAND;
						}
					}
					printf("AI focusing on making tech and metal to expand\n");
					if (linked->metaldaily > linked->techdaily)
					{
						// Build laboratory
						aiBuild(BUILDING_LABORATORY);
						focus = MAKE_SOLDIERS;
					}
					else
					{
						// Build mine
						aiBuild(BUILDING_MINE);
						focus = MAKE_SOLDIERS;
					}
				}
				else
				{
					printf("AI focusing in making money!\n");
					focus = MAKE_MONEY;
				}
			}
			else
			{
				// If we are not building economy we will try to make our country happy
				if (focus != MAKE_MONEY)
				{
					focus = MAKE_HAPPY;
				}
			}

		}
	}

	void update(float dt)
	{
		// Complex sentient AI incoming


		if (firstframe)
		{
			// Choose our planet
			linked->aicontroller = this;

			bool valid = false;


			while (!valid)
			{
				int i = rand() % universe->count;
				if (universe->planets[i]->owner == NULL)
				{
					linked->planets.push_back(universe->planets[i]);
					universe->planets[i]->owner = linked;
					valid = true;
				}
			}

			valid = false;

			while (!valid)
			{
				linked->name = universe->empireNames[rand() % 11];
				if (std::find(universe->usedAINames.begin(), universe->usedAINames.end(), linked->name) 
					== universe->usedAINames.end())
				{
					valid = true;
					universe->usedAINames.push_back(linked->name);
				}
			}

			firstframe = false;
		}
		else
		{
			// Play the game :3

			// We play daily so nothing here duh

		}

	}
};

enum PlayerState
{
	EDIT_MODE,
	VIEW_MODE,
	CHOOSE_WORLD
};


class EmpirePlayer
{
public:


	std::vector<Event> events;

	Universe* universe;
	Empire* linked;

	PlayerState state = PlayerState::CHOOSE_WORLD;

	sf::FloatRect systemWindow;
	sf::FloatRect empireWindow;

	Planet* focused;

	float timeStepper = 0.0f;
	float timeSpeed = 1.0f;
	float frameAverage = 0.0f;
	int day = 1;
	int month = 1;
	int year = 2100;

	sf::Texture selector;

	sf::Sprite cursor;

	bool ecoWindow = false;
	bool popWindow = false;
	bool retWindow = false;


	// Gets first word of name (Mr.Robot enterprise -> Mr.Robot)
	std::string shortenName(std::string name)
	{
		std::string out;
		for (int i = 0; i < name.length(); i++)
		{
			char c = name[i];
			if (c == ' ' || c == 0)
			{
				break;
			}
			out += c;
		}
		return out;
	}

	bool contains(std::vector<Planet*>* planets, Planet* planet)
	{
		if (std::find(planets->begin(), planets->end(), planet) != planets->end())
		{
			return true;
		}
		return false;
	}

	bool shown = false;

	char nameBuff[256] = "My Empire";

	int choosenTileType = 0;

	int tileC = 0;

	std::string choosenTileName;

	sf::Texture* choosenTileImage;

	sf::Vector2f editorOffset = sf::Vector2f(0.0f, 0.0f);


	sf::Texture gear;
	sf::Texture cross;

	bool attackPlaner = false;

	bool showGuide = true;

	int guidePage = 0;

	int attackSoldierCount = 1;
	Planet* attackTarget;

	int selectedMode = 0;
	
	void receiveAttack(Empire* attacker)
	{

	}

	void drawAttackPlaner()
	{
		ImGui::Begin("Attack Planner");

		
		ImGui::Text("Soldiers: %i", linked->soldiers);
		if (linked->soldiers != 0)
		{
			ImGui::SliderInt("Soldier Count", &attackSoldierCount, 1, linked->soldiers);

			if (attackTarget != NULL)
			{
				ImGui::Text("About to attack: %s", attackTarget->name.c_str());
				if (ImGui::Button("Do Attack! (Damage)"))
				{
					linked->attack(attackTarget, attackSoldierCount, false);
					attackTarget->owner->aicontroller->receiveAttack(this->linked);
				}

				ImGui::SameLine();

				if (ImGui::Button("Do Attack! (Annex)"))
				{
					linked->attack(attackTarget, attackSoldierCount, true);
					attackTarget->owner->aicontroller->receiveAttack(this->linked);
				}
			}
			else
			{
				ImGui::Text("Please choose a target from the list below.");
			}

		}
		else
		{
			ImGui::Text("You have no soldiers!");
		}
	

		ImGui::Text("Target: ");
		ImGui::Separator();
		for (int i = 0; i < universe->planets.size(); i++)
		{
			if (universe->planets[i]->owner != NULL && universe->planets[i]->owner != linked)
			{
				if (ImGui::Button(universe->planets[i]->name.c_str()))
				{
					attackTarget = universe->planets[i];
				}
				
				ImGui::Text("Owned by: %s", universe->planets[i]->owner->name.c_str());
				ImGui::Text("Relation: ");
				ImGui::SameLine();
				if (universe->planets[i]->owner->aicontroller->relations[linked] == Relation::HATRED)
				{
					ImGui::TextColored(ImVec4(1.0, 0.2, 0.2, 1.0), "AT WAR");
				}
				else if (universe->planets[i]->owner->aicontroller->relations[linked] == Relation::ENEMIES)
				{
					ImGui::TextColored(ImVec4(1.0, 0.2, 0.2, 1.0), "AT WAR");
				}
				else if (universe->planets[i]->owner->aicontroller->relations[linked] == Relation::COMPETENCE)
				{
					ImGui::TextColored(ImVec4(1.0, 0.5, 0, 1.0), "COMPETENCE");
				}
				else if (universe->planets[i]->owner->aicontroller->relations[linked] == Relation::NEUTRAL)
				{
					ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1.0), "NEUTRAL");
				}
				else if (universe->planets[i]->owner->aicontroller->relations[linked] == Relation::GOOD)
				{
					ImGui::TextColored(ImVec4(0.5, 1.0, 0.0, 1.0), "GOOD");
				}
				else if (universe->planets[i]->owner->aicontroller->relations[linked] == Relation::FRIENDS)
				{
					ImGui::TextColored(ImVec4(0.3, 1.0, 0.3, 1.0), "FRIENDS");
				}
				else if (universe->planets[i]->owner->aicontroller->relations[linked] == Relation::ALLIANCE)
				{
					ImGui::TextColored(ImVec4(0.1, 1.0, 0.1, 1.0), "ALLIANCE");
				}
				ImGui::Separator();
			}
		}

		ImGui::End();
	}

	void drawGuide()
	{
		ImGui::SetNextWindowSize(ImVec2(290, 400));
		ImGui::Begin("Guide", NULL, ImGuiWindowFlags_NoResize);

		ImGui::Text("You can close/open this guide with F1");
		ImGui::Spacing();
		ImGui::Separator();

		if (guidePage == 0)
		{
			ImGui::Text("Getting Started:");
			ImGui::BeginChild("GuideContent", ImVec2(250, 300), true);
			ImGui::TextWrapped("TinyPlanets is a RTS game set in a solar system made of tiny planets");
			ImGui::Spacing();
			ImGui::TextWrapped("Your objective is to either annex every other corporation in the system or launch a vessel to another solar system");
			ImGui::Spacing();
			ImGui::TextWrapped("To do this you must build on planets, and expand into other ones using the tools provided");
			ImGui::Spacing();
			ImGui::TextWrapped("Most windows can be moved around, including this guide. (<3 ImGui)");
			ImGui::Spacing();
			ImGui::TextWrapped("Right now you should choose a planet and continue reading the guide (Use the buttons below!)");
			ImGui::EndChild();
		}
		else if (guidePage == 1)
		{
			ImGui::Text("Basic Usage:");
			ImGui::BeginChild("GuideContent", ImVec2(250, 300), true);
			ImGui::TextWrapped("There are two views: World view and Edit view. Chances are you are in World view right now");
			ImGui::Spacing();
			ImGui::TextWrapped("At the top you can see the solar system panel, time control buttons and resource statistics");
			ImGui::Spacing();
			ImGui::TextWrapped("At the bottom you can see your empire's stats, and clicking on the buttons, show more information");
			ImGui::Spacing();
			ImGui::TextWrapped("You can select planets at the top, green ones are owned by you, gray ones have no owner and red ones are from another corporation");
			ImGui::Spacing();
			ImGui::TextWrapped("Choose your home-planet (green) and click edit at the bottom left to get into edit mode. Please continue reading.");
			ImGui::EndChild();
		}
		else if (guidePage == 2)
		{
			ImGui::Text("Edit Mode:");
			ImGui::BeginChild("GuideContent", ImVec2(250, 300), true);
			ImGui::TextWrapped("In Edit mode you can build and destroy buildings in your planet");
			ImGui::Spacing();
			ImGui::TextWrapped("You have two panels: Tiles and Choosen Tile");
			ImGui::Spacing();
			ImGui::TextWrapped("The first allows you to select which building you want to build");
			ImGui::Spacing();
			ImGui::TextWrapped("The second allows you to see what are the contents of the selected tile");
			ImGui::Spacing();
			ImGui::TextWrapped("Left-Click to build, Right-click to lock mouse cursor. Escape to exit build mode");
			ImGui::Spacing();
			ImGui::TextWrapped("You can move around with WASD and zoom with R/F");
			ImGui::EndChild();
		}
		else if (guidePage == 3)
		{
			ImGui::Text("Strategy:");
			ImGui::BeginChild("GuideContent", ImVec2(250, 300), true);
			ImGui::TextWrapped("You should make sure you always have enough of every resource, and that your population is happy");
			ImGui::Spacing();
			ImGui::TextWrapped("Your money can be keep up by the increase of taxes or the building of markets");
			ImGui::Spacing();
			ImGui::TextWrapped("Minerals can be optained by building mines");
			ImGui::Spacing();
			ImGui::TextWrapped("Food is generated in farms and Algae Domes");
			ImGui::Spacing();
			ImGui::TextWrapped("Technology is generated in laboratories");
			ImGui::Spacing();
			ImGui::TextWrapped("In order to expand into another planet you need a decent ammount of every resource.");
			ImGui::Spacing();
			ImGui::TextWrapped("You should also build a millitary. It will require a fair ammount of metal, food and population.");
			ImGui::EndChild();
		}
		else if (guidePage == 4)
		{
			ImGui::Text("War:");
			ImGui::BeginChild("GuideContent", ImVec2(250, 300), true);
			ImGui::TextWrapped("War in this game is done via the 'People' menu");
			ImGui::Spacing();
			ImGui::TextWrapped("There you can create soldiers from your population and some resources and launch attacks");
			ImGui::Spacing();
			ImGui::TextWrapped("To launch attacks you use the 'Attack Planner' window");
			ImGui::Spacing();
			ImGui::TextWrapped("There you can attack any planet from other civilization in two ways:");
			ImGui::Spacing();
			ImGui::TextWrapped("- Annex: If you win the planet (and a part of its population) will become yours");
			ImGui::Spacing();
			ImGui::TextWrapped("- Destroy: You will focus on damaging buildings on the target planet");
			ImGui::Spacing();
			ImGui::TextWrapped("All of these will damage buildings in the target planet");
			ImGui::EndChild();

		}
		if (ImGui::Button("<"))
		{
			if (guidePage > 0)
			{
				guidePage--;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(">"))
		{
			guidePage++;
		}

		ImGui::End();
	}

	void drawEditorUI()
	{
		ImGui::Begin("Tiles");

		for (int i = 1; i < BUILDING_TYPES_TOTAL; i++)
		{
			/*if (i == choosenTileType)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.3f, 1.0f));
			}*/

			if (i == BUILDING_APPARTMENT)
			{
				if (ImGui::ImageButton(focused->buildings["apps"]))
				{
					choosenTileType = i;
					choosenTileImage = &focused->buildings["apps"];
					choosenTileName = "Appartment Block";
				}
				ImGui::SameLine();
				ImGui::BeginChild("APPSSUBFRAME", ImVec2(180, 70), true);
				ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1.0), "Price: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0, 0.8, 0.0, 1.0), "%iC", APP_PRICE);
				ImGui::TextWrapped("%i people can live inside. %iC maintenance", APP_HOUSING, APP_MAINTENANCE);
				ImGui::EndChild();
			}
			else if (i == BUILDING_FARM)
			{
				if (ImGui::ImageButton(focused->buildings["farm"]))
				{
					choosenTileType = i;
					choosenTileImage = &focused->buildings["farm"];
					choosenTileName = "Farm";
				}
				ImGui::SameLine();
				ImGui::BeginChild("FARMSSUBFRAME", ImVec2(180, 70), true);
				ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1.0), "Price: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0, 0.8, 0.0, 1.0), "%iC", FARM_PRICE);
				ImGui::TextWrapped("Generates %i food every day. Uses %iC to do so. Employs %i persons", 
					FARM_GENERATION, FARM_MAINTENANCE, FARM_EMPLOYMENT);
				ImGui::EndChild();
			}
			else if (i == BUILDING_LABORATORY)
			{
				if (ImGui::ImageButton(focused->buildings["lab"]))
				{
					choosenTileType = i;
					choosenTileImage = &focused->buildings["lab"];
					choosenTileName = "Laboratory";
				}
				ImGui::SameLine();
				ImGui::BeginChild("LABSSUBFRAME", ImVec2(180, 70), true);
				ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1.0), "Price: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0, 0.8, 0.0, 1.0), "%iC", LAB_PRICE);
				ImGui::TextWrapped("Generates %i science every day. Uses %iC to do so. Employs %i persons",
					LAB_GENERATION, LAB_MAINTENANCE, LAB_EMPLOYMENT);
				ImGui::EndChild();
			}
			else if (i == BUILDING_HOUSE)
			{
				if (ImGui::ImageButton(focused->buildings["house"]))
				{
					choosenTileType = i;
					choosenTileImage = &focused->buildings["house"];
					choosenTileName = "House";
				}
				ImGui::SameLine();
				ImGui::BeginChild("HOUSESSUBFRAME", ImVec2(180, 70), true);
				ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1.0), "Price: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0, 0.8, 0.0, 1.0), "%iC", HOUSE_PRICE);
				ImGui::TextWrapped("%i people can live inside. %iC maintenance", HOUSE_HOUSING, HOUSE_MAINTENANCE);
				ImGui::EndChild();
			}
			else if (i == BUILDING_MINE)
			{
				if (ImGui::ImageButton(focused->buildings["mine"]))
				{
					choosenTileType = i;
					choosenTileImage = &focused->buildings["mine"];
					choosenTileName = "Mining Station";
				}
				ImGui::SameLine();
				ImGui::BeginChild("MINESSUBFRAME", ImVec2(180, 70), true);
				ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1.0), "Price: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0, 0.8, 0.0, 1.0), "%iC", MINE_PRICE);
				ImGui::TextWrapped("Extracts metal depending on the planet's concentration (Base %i). %iC maintenance every day. Employs %i people!",
					MINE_GENERATION, MINE_MAINTENANCE, MINE_EMPLOYMENT);
				ImGui::EndChild();
			}
			else if (i == BUILDING_LAUNCHER)
			{
				if (ImGui::ImageButton(focused->buildings["launcher"]))
				{
					choosenTileType = i;
					choosenTileImage = &focused->buildings["launcher"];
					choosenTileName = "Space Launch Site";
				}
				ImGui::SameLine();
				ImGui::BeginChild("LAUNCHSSUBFRAME", ImVec2(180, 70), true);
				ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1.0), "Price: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0, 0.8, 0.0, 1.0), "%iC", LAUNCHER_PRICE);
				ImGui::TextWrapped("Allows you to launch a vehicle twice every year. 15000C every launch and %iC maintenance",
					LAUNCHER_MAINTENANCE);
				ImGui::EndChild();
			}
			else if (i == BUILDING_MARKET)
			{
				if (ImGui::ImageButton(focused->buildings["market"]))
				{
					choosenTileType = i;
					choosenTileImage = &focused->buildings["market"];
					choosenTileName = "Market";
				}
				ImGui::SameLine();
				ImGui::BeginChild("MARKETSSUBFRAME", ImVec2(180, 70), true);
				ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1.0), "Price: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0, 0.8, 0.0, 1.0), "%iC", MARKET_PRICE);
				ImGui::TextWrapped("Generates %iC on a daily basis. Requires %i employees", 
					MARKET_GENERATION, MARKET_EMPLOYMENT);
				ImGui::EndChild();
			}
			else if (i == BUILDING_SMARKET)
			{
				if (ImGui::ImageButton(focused->buildings["smarket"]))
				{
					choosenTileType = i;
					choosenTileImage = &focused->buildings["smarket"];
					choosenTileName = "Small Market";
				}
				ImGui::SameLine();
				ImGui::BeginChild("SMARKETSSUBFRAME", ImVec2(180, 70), true);
				ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1.0), "Price: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0, 0.8, 0.0, 1.0), "%iC", SMARKET_PRICE);
				ImGui::TextWrapped("Generates %iC on a daily basis. Requires %i employees",
					SMARKET_GENERATION, SMARKET_EMPLOYMENT);
				ImGui::EndChild();
			}
			else if (i == BUILDING_HFARM)
			{
				if (ImGui::ImageButton(focused->buildings["hydrofarm"]))
				{
					choosenTileType = i;
					choosenTileImage = &focused->buildings["hydrofarm"];
					choosenTileName = "Hydroponic Farm";
				}
				ImGui::SameLine();
				ImGui::BeginChild("HFARMSSUBFRAME", ImVec2(180, 70), true);
				ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1.0), "Price: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0, 0.8, 0.0, 1.0), "%iC", HFARM_PRICE);
				ImGui::TextWrapped("Generates %i food every day. Uses %iC to do so. Employs %i persons",
					HFARM_GENERATION, HFARM_MAINTENANCE, HFARM_EMPLOYMENT);
				ImGui::EndChild();
			}
			/*if (i == choosenTileType)
			{
				ImGui::PopStyleColor(1);
			}*/
		}

		ImGui::End();

		ImGui::Begin("Choosen Tile");
		if (choosenTileType != 0)
		{
			ImGui::Image(*choosenTileImage);
			ImGui::Text("%s selected", choosenTileName.c_str());
		}
		else
		{
			ImGui::Text("No tile selected");
		}

		ImGui::Separator();

		ImGui::Text("Tile: %i", tileC);
		switch (focused->tiles[tileC])
		{
		case BUILDING_EMPTY:
			ImGui::Text("Nothing built");
			break;
		case BUILDING_APPARTMENT:
			ImGui::Image(focused->buildings["apps"]);
			ImGui::Text("Appartment Block");
			break;
		case BUILDING_FARM:
			ImGui::Image(focused->buildings["farm"]);
			ImGui::Text("Farm");;
			break;
		case BUILDING_LABORATORY:
			ImGui::Image(focused->buildings["lab"]);
			ImGui::Text("Laboratory");
			break;
		case BUILDING_HOUSE:
			ImGui::Image(focused->buildings["house"]);
			ImGui::Text("House");
			break;
		case BUILDING_MARKET:
			ImGui::Image(focused->buildings["market"]);
			ImGui::Text("Market");
			break;
		case BUILDING_SMARKET:
			ImGui::Image(focused->buildings["smarket"]);
			ImGui::Text("Small Market");
			break;
		case BUILDING_HFARM:
			ImGui::Image(focused->buildings["hydrofarm"]);
			ImGui::Text("Hydroponic Farms");
			break;
		}

		if (focused->tiles[tileC] != 0)
		{
			ImGui::Text("Build Progress: %i%%", (int)(focused->tilesBuilding[tileC] * 100.0f));
			if (ImGui::Button("Destroy"))
			{
				focused->tiles[tileC] = 0;
				focused->usedtiles--;
			}
		}

		ImGui::End();
	}

	bool peaceful = false;
	int aiCount = 4;

	void drawWorldChooserWindow()
	{
		int citem;




		if (shown == true)
		{
			ImGui::Begin(focused->name.c_str(), NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
			ImGui::Text("Info about %s", focused->name.c_str());
			ImGui::Text("Size: %i", focused->size);
			if (focused->easyHabitable)
			{
				ImGui::TextColored(ImVec4(0.2, 1.0, 0.2, 1.0), "Habitable");
			}
			else
			{
				ImGui::TextColored(ImVec4(1.0, 0.2, 0.2, 1.0), "Not-Habitable");
			}
			if (focused->hasAtmosphere)
			{
				ImGui::TextColored(ImVec4(0.2, 1.0, 0.2, 1.0), "Has Atmosphere");
			}
			else
			{
				ImGui::TextColored(ImVec4(1.0, 0.2, 0.2, 1.0), "No Atmosphere");
			}

			ImGui::Text("Food Multiplier: %f", focused->foodBoost);
			ImGui::Text("Mineral Multiplier: %f", focused->mineralBoost);
			ImGui::Text("Science Multiplier: %f", focused->scienceBoost);

			ImGui::InputText("Civ Name", nameBuff, 256);

			ImGui::Checkbox("Peaceful Mode", &peaceful);
			ImGui::InputInt("AI count: ", &aiCount, 1, 2);

			if (ImGui::Button("Choose"))
			{
				// Choose this planet
				linked->planets.push_back(focused);
				focused->owner = linked;
				state = PlayerState::VIEW_MODE;

				linked->name = std::string(nameBuff);
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				focused = NULL;
				shown = false;
			}
			ImGui::Separator();
			ImGui::Text("Tip: Check the guide for help");
			ImGui::End();
		}

		ImGui::Begin("Choose starting world", NULL, ImGuiWindowFlags_NoCollapse);

		ImGui::TextWrapped("Habitable worlds are easier to start with, but have less minerals than Non-Habitable ones");
		ImGui::Separator();

		for (int i = 0; i < universe->count; i++)
		{
			if (ImGui::Button(universe->planets[i]->name.c_str()))
			{
				focused = universe->planets[i];
				shown = true;
			}

			ImGui::Text("Size: %i", universe->planets[i]->size);

			if (universe->planets[i]->easyHabitable == true)
			{
				ImGui::TextColored(ImVec4(0.2, 1.0, 0.2, 1.0), "Habitable");
			}
			else
			{
				ImGui::TextColored(ImVec4(1.0, 0.2, 0.2, 1.0), "Non-Habitable");
			}

			ImGui::Separator();
		}

		ImGui::End();
	}

	void drawRelationsWindow()
	{
		ImGui::Begin("Relations");
		for (int i = 0; i < linked->otherEmpires.size(); i++)
		{
			ImGui::Text("%s:", linked->otherEmpires[i]->name.c_str());
			ImGui::SameLine();
			if (linked->otherEmpires[i]->aicontroller->relations[linked] == Relation::HATRED)
			{
				ImGui::TextColored(ImVec4(1.0, 0.2, 0.2, 1.0), "AT WAR");
			}
			else if (linked->otherEmpires[i]->aicontroller->relations[linked] == Relation::ENEMIES)
			{
				ImGui::TextColored(ImVec4(1.0, 0.2, 0.2, 1.0), "AT WAR");
			}
			else if (linked->otherEmpires[i]->aicontroller->relations[linked] == Relation::COMPETENCE)
			{
				ImGui::TextColored(ImVec4(1.0, 0.5, 0, 1.0), "COMPETENCE");
			}
			else if (linked->otherEmpires[i]->aicontroller->relations[linked] == Relation::NEUTRAL)
			{
				ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1.0), "NEUTRAL");
			}
			else if (linked->otherEmpires[i]->aicontroller->relations[linked] == Relation::GOOD)
			{
				ImGui::TextColored(ImVec4(0.5, 1.0, 0.0, 1.0), "GOOD");
			}
			else if (linked->otherEmpires[i]->aicontroller->relations[linked] == Relation::FRIENDS)
			{
				ImGui::TextColored(ImVec4(0.3, 1.0, 0.3, 1.0), "FRIENDS");
			}
			else if (linked->otherEmpires[i]->aicontroller->relations[linked] == Relation::ALLIANCE)
			{
				ImGui::TextColored(ImVec4(0.1, 1.0, 0.1, 1.0), "ALLIANCE");
			}
		}
		ImGui::End();
	}

	int choosenSoldiers = 1;

	void drawPeopleWindow()
	{
		ImGui::Begin("Population");

		ImGui::TextColored(ImVec4(0.7, 0.7, 0.7, 1.0), "Population:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), "%i", linked->population);

		ImGui::TextColored(ImVec4(0.7, 0.7, 0.7, 1.0), "Soldiers: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), "%i", linked->soldiers);

		ImGui::TextColored(ImVec4(0.7, 0.7, 0.7, 1.0), "Employment: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), "%i%% (%i/%i)", 
			(int)(((float)linked->workers / (float)linked->population) * 100.f),
			linked->workers, linked->population);
		
		ImGui::TextColored(ImVec4(0.7, 0.7, 0.7, 1.0), "Workers/Workplaces: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), "%i/%i", linked->workers, linked->workplaces);

		ImGui::TextColored(ImVec4(0.7, 0.7, 0.7, 1.0), "Housing: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), "%i%%", 
			(int)( ( (float)linked->housed / (float)linked->population) * 100.f));


		ImGui::TextColored(ImVec4(0.7, 0.7, 0.7, 1.0), "Houses Used: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), "%i%% (%i/%i)",
			(int)(((float)linked->housed / (float)linked->total_houses) * 100.f),
			linked->housed, linked->total_houses);


		ImGui::Separator();
		ImGui::Text("Soldier Creation:");
		ImGui::SliderInt("Ammount", &choosenSoldiers, 1, linked->population - 1);
		int cashCost = choosenSoldiers * CASH_PER_SOLDIER;
		int metalCost = choosenSoldiers * METAL_PER_SOLDIER;
		int techCost = choosenSoldiers * TECH_PER_SOLDIER;
		ImGui::Text("Cost: %iC | %iM | %iT", cashCost, metalCost, techCost);
		ImGui::Spacing();


		if (cashCost < linked->money && metalCost < linked->metal && techCost < linked->tech)
		{
			if (ImGui::Button("Create Soldiers"))
			{
				linked->money -= cashCost;
				linked->metal -= metalCost;
				linked->tech -= techCost;

				linked->population -= choosenSoldiers;
				linked->soldiers += choosenSoldiers;
			}
		}
		else
		{
			ImGui::Text("Not enough resources!");
		}



		ImGui::Spacing();

		if (ImGui::Button("Attack Planner"))
		{
			attackPlaner = !attackPlaner;
		}


		ImGui::Separator();

		if (ImGui::Button("Close"))
		{
			popWindow = false;
		}

		ImGui::End();
	}


	void drawEconomyWindow()
	{
		ImGui::Begin("Economy");

		ImGui::Text("Actual taxes: %f", linked->taxes);
		ImGui::SliderFloat("Wanted Taxes", &linked->wantedTaxes, 0.0f, 5.0f);

		ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1.0), "Population x Taxes:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%i/d", (int)((float)linked->population * linked->taxes));

		ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1.0), "Taxes will change when next month starts");

		ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1.0), "Happiness Effect:");
		ImGui::SameLine();
		if (linked->wantedTaxes == 0)
		{
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "25");
		}
		else
		{
			// We use -1.5(x-2)^3 as our happiness formula
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%i",
				(int)(powf(-1.5f*(linked->taxes-2.0f), 3.0f)));
		}


		ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1.0), "Monthly Maintenance Costs:");
		ImGui::SameLine();
		int change = (int)((float)linked->population * (float)linked->population * 0.005f);
		ImGui::Text("%i", change);

		ImGui::Separator();

		ImGui::PlotLines("Cash", (float*)linked->previousMoney.data(), linked->previousMoney.size(), 
			0, "", (3.402823466e+38f), (3.402823466e+38f), ImVec2(160, 50));
		ImGui::PlotLines("Food", (float*)linked->previousFood.data(), linked->previousFood.size(),
			0, "", (3.402823466e+38f), (3.402823466e+38f), ImVec2(160, 50));
		ImGui::PlotLines("Metal", (float*)linked->previousMetal.data(), linked->previousMetal.size(),
			0, "", (3.402823466e+38f), (3.402823466e+38f), ImVec2(160, 50));
		ImGui::PlotLines("Tech", (float*)linked->previousTech.data(), linked->previousTech.size(),
			0, "", (3.402823466e+38f), (3.402823466e+38f), ImVec2(160, 50));
		if (ImGui::Button("Clear Graphs"))
		{
			linked->previousMoney.clear();
			linked->previousFood.clear();
			linked->previousMetal.clear();
			linked->previousTech.clear();
			linked->previousMoney.push_back(linked->money);
			linked->previousFood.push_back(linked->food);
			linked->previousMetal.push_back(linked->metal);
			linked->previousTech.push_back(linked->tech);
		}
	
		ImGui::SameLine();

		if (ImGui::Button("Close"))
		{
			ecoWindow = false;
		}

		ImGui::End();
	}

	Planet* launchTarget = NULL;

	void drawEmpireWindow()
	{

		if (linked->launchReady())
		{
			ImGui::Begin("Space Launch Ready");

			ImGui::Text("You have built a space launcher and it's ready to launch a vessel!");
			ImGui::Text("%i Metal and %i Science will be consumed!", METAL_FOR_LAUNCH, TECH_FOR_LAUNCH);
			ImGui::Text("Select your target from the list:");
			ImGui::Separator();
			for (int i = 0; i < universe->planets.size(); i++)
			{
				if(universe->planets[i]->owner == NULL)
				{
					if(ImGui::Button(universe->planets[i]->name.c_str()))
					{
						linked->launchVehicle(universe->planets[i]);
					}
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::Text("Planet Information");
						ImGui::Separator();
						ImGui::TextColored(ImVec4(0.7, 0.7, 0.7, 1.0), "Size: ");
						ImGui::SameLine();
						ImGui::Text("%i", universe->planets[i]->size);

						if (universe->planets[i]->easyHabitable)
						{
							ImGui::TextColored(ImVec4(0.2, 1.0, 0.2, 1.0), "Habitable");
						}
						else
						{
							ImGui::TextColored(ImVec4(1.0, 0.2, 0.2, 1.0), "Non-Habitable");
						}

						ImGui::Text("Food Multiplier: %f", focused->foodBoost);
						ImGui::Text("Mineral Multiplier: %f", focused->mineralBoost);
						ImGui::Text("Science Multiplier: %f", focused->scienceBoost);



						ImGui::EndTooltip();
					}
					ImGui::Spacing();
				}
			}

			ImGui::End();
		}

		// Event window
		int eventsShown = 0;

		for (int i = 0; i < linked->events.size(); i++)
		{
			if (linked->events[i].text != "" && eventsShown < 5)
			{
				eventsShown++;
				std::string composite = linked->events[i].text;
				composite = shortenName(composite);
				composite.append(std::to_string(i));

				ImGui::SetNextWindowPos(ImVec2(systemWindow.width - 256.0f, eventsShown * 90 + systemWindow.height),
					ImGuiSetCond_Always);
				ImGui::SetNextWindowSize(ImVec2(256.0f, 80.0f),
					ImGuiSetCond_Always);
				ImGui::Begin(composite.c_str(), NULL, 
					ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
				ImGui::TextWrapped(linked->events[i].text.c_str());
				if (ImGui::Button("Dismiss"))
				{
					linked->events[i] = Event(EventSeverity::FATAL, "");
				}
				ImGui::End();
			}
		}

		ImGui::SetNextWindowPos(ImVec2(empireWindow.left, empireWindow.top),
			ImGuiSetCond_Always);
		ImGui::SetNextWindowSize(ImVec2(empireWindow.width, empireWindow.height),
			ImGuiSetCond_Always);

		ImGui::Begin("Empire", NULL,
			ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove);
		ImGui::BeginChild("EmpireSubFocusData", ImVec2(empireWindow.width / 4, empireWindow.height - 40), true);
		if (focused != NULL)
		{
			ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Focused on ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", focused->name.c_str());
			if (focused->owner == linked && focused->alreadyOwned)
			{
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				if (ImGui::Button("Edit", ImVec2(40, 16)))
				{
					state = PlayerState::EDIT_MODE;
				}
			}
			ImGui::Separator();
			ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Owner: ");
			ImGui::SameLine();
			if (focused->owner != NULL)
			{
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", focused->owner->name.c_str());
			}
			else
			{
				ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "No Owner");
			}
			ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Population: ");
			ImGui::SameLine();
			if (focused->owner != NULL)
			{
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%i", focused->owner->population);
			}
			else
			{
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "0");
			}
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "| Tiles: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%i/%i", focused->usedtiles, focused->size);
		}
		else
		{
			ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Focused on no planet");
		}
		ImGui::EndChild();
		ImGui::SameLine();

		if (ImGui::Button("Economy", ImVec2(empireWindow.height - 40, empireWindow.height - 40)))
		{
			ecoWindow = !ecoWindow;
		}
		ImGui::SameLine();
		ImGui::BeginChild("EmpireSubEconomy", ImVec2(empireWindow.height + 40, empireWindow.height - 40), true);
		ImGui::PlotLines("Cash", (float*)linked->previousMoney.data(), linked->previousMoney.size());
		ImGui::PlotLines("Food", (float*)linked->previousFood.data(), linked->previousFood.size());
		ImGui::PlotLines("Metal", (float*)linked->previousMetal.data(), linked->previousMetal.size());
		ImGui::PlotLines("Tech", (float*)linked->previousTech.data(), linked->previousTech.size());
		if (ImGui::Button("Clear Graphs"))
		{
			linked->previousMoney.clear();
			linked->previousFood.clear();
			linked->previousMetal.clear();
			linked->previousTech.clear();
			linked->previousMoney.push_back(linked->money);
			linked->previousFood.push_back(linked->food);
			linked->previousMetal.push_back(linked->metal);
			linked->previousTech.push_back(linked->tech);
		}
		ImGui::EndChild();
		ImGui::SameLine();
		if (ImGui::Button("People", ImVec2(empireWindow.height - 40, empireWindow.height - 40)))
		{
			popWindow = !popWindow;
		}
		ImGui::SameLine();
		ImGui::BeginChild("EmpireSubPopulation", ImVec2(empireWindow.height + 40, empireWindow.height - 40), true);
		ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1.0), "Population: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), "%i", linked->population);
		ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1.0), "Soldiers: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), "%i", linked->soldiers);
		ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1.0), "Workers: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), "%i%%", (int)(((float)linked->workers / (float)linked->population) * 100.f));
		ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1.0), "Housed: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), "%i%%", (int)(((float)linked->housed / (float)linked->population) * 100.f));
		ImGui::EndChild();
		ImGui::SameLine();
		if (ImGui::Button("Relations", ImVec2(empireWindow.height - 40, empireWindow.height - 40)))
		{
			retWindow = !retWindow;
		}
		ImGui::SameLine();
		ImGui::BeginChild("EmpireSubRelations", ImVec2(empireWindow.height + 40, empireWindow.height - 40), true);
		for (int i = 0; i < linked->otherEmpires.size(); i++)
		{
			ImGui::Text("%s:", shortenName(linked->otherEmpires[i]->name).c_str());
			ImGui::SameLine();
			if (linked->otherEmpires[i]->aicontroller->relations[linked] == Relation::HATRED)
			{
				ImGui::TextColored(ImVec4(1.0, 0.2, 0.2, 1.0), "AT WAR");
			}
			else if (linked->otherEmpires[i]->aicontroller->relations[linked] == Relation::ENEMIES)
			{
				ImGui::TextColored(ImVec4(1.0, 0.2, 0.2, 1.0), "AT WAR");
			}
			else if (linked->otherEmpires[i]->aicontroller->relations[linked] == Relation::COMPETENCE)
			{
				ImGui::TextColored(ImVec4(1.0, 0.5, 0, 1.0), "COMPETENCE");
			}
			else if (linked->otherEmpires[i]->aicontroller->relations[linked] == Relation::NEUTRAL)
			{
				ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1.0), "NEUTRAL");
			}
			else if (linked->otherEmpires[i]->aicontroller->relations[linked] == Relation::GOOD)
			{
				ImGui::TextColored(ImVec4(0.5, 1.0, 0.0, 1.0), "GOOD");
			}
			else if (linked->otherEmpires[i]->aicontroller->relations[linked] == Relation::FRIENDS)
			{
				ImGui::TextColored(ImVec4(0.3, 1.0, 0.3, 1.0), "FRIENDS");
			}
			else if (linked->otherEmpires[i]->aicontroller->relations[linked] == Relation::ALLIANCE)
			{
				ImGui::TextColored(ImVec4(0.1, 1.0, 0.1, 1.0), "ALLIANCE");
			}
		}
		ImGui::EndChild();

		ImGui::SameLine();
		ImGui::BeginChild("Menu&Stuff");
		if (ImGui::ImageButton(gear, (empireWindow.height - 64) / 10))
		{
			settingsOpen = !settingsOpen;
		}
		if (ImGui::ImageButton(cross, (empireWindow.height - 64) / 10))
		{
			exit(0);
		}
		ImGui::EndChild();

		ImGui::End();
	}

	bool fullscreen = false;
	bool settingsOpen = false;

	float musicVolume = 0.25f;
	float fxVolume = 0.25f;
	sf::Sound errorP;

	void drawSettingsWindow()
	{
		ImGui::Begin("Settings");
		ImGui::Checkbox("Fullscreen", &fullscreen);
		ImGui::Checkbox("Limit FPS", &limitFPS);
		ImGui::SliderFloat("Music Vol.", &musicVolume, 0.0f, 1.0f);
		ImGui::SliderFloat("Effects Vol.", &fxVolume, 0.0f, 1.0f);
		if (ImGui::Button("Close"))
		{
			settingsOpen = false;
		}
		ImGui::End();
	}

	void drawSystemWindow()
	{
		ImGui::SetNextWindowPos(ImVec2(systemWindow.left, systemWindow.top));
		ImGui::SetNextWindowSize(ImVec2(systemWindow.width, systemWindow.height));
		ImGui::Begin("System", NULL, ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_HorizontalScrollbar);

		ImGui::BeginChild("SubSystemData", ImVec2(systemWindow.width * 0.7f, 50),
			true, ImGuiWindowFlags_HorizontalScrollbar);

		for (int i = 0; i < universe->count; i++)
		{
			bool isPlanetOurs = universe->planets[i]->owner == linked;

			if (isPlanetOurs)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.4f, 0.2f, 1.0f));
			}
			else if (universe->planets[i]->owner == NULL)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25, 0.25f, 0.25f, 1.0f));
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.25f, 0.25f, 1.0f));
			}
			if (ImGui::Button(universe->planets[i]->name.c_str()))
			{
				focused = universe->planets[i];
				state = PlayerState::VIEW_MODE;
			}

			ImGui::PopStyleColor(1);

			if (i < universe->count - 1)
			{
				ImGui::SameLine();
			}
			else
			{

			}
		}
		ImGui::EndChild();
		ImGui::SameLine();
		if (timeSpeed == 1)
		{
			if (ImGui::Button("[]"))
			{
				timeSpeed = 0;
			}
		}
		else if (timeSpeed == 2)
		{
			if (ImGui::Button("|<"))
			{
				timeSpeed = 1;
			}
		}
		else if (timeSpeed == 6)
		{
			if (ImGui::Button("<<"))
			{
				timeSpeed = 2;
			}
		}
		else
		{
			if (ImGui::Button(">|"))
			{
				timeSpeed = 1;
			}
		}
		ImGui::SameLine();
		if (timeSpeed == 0)
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "%02i:%02i:%04i", day, month, year);
		}
		else if (timeSpeed == 1)
		{
			ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1), "%02i:%02i:%04i", day, month, year);
		}
		else if (timeSpeed == 2)
		{
			ImGui::TextColored(ImVec4(1, 1, 1, 1), "%02i:%02i:%04i", day, month, year);
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.5, 0.1f, 1), "%02i:%02i:%04i", day, month, year);
		}
		ImGui::SameLine();
		if (timeSpeed == 1)
		{
			if (ImGui::Button("> "))
			{
				timeSpeed = 2;
			}
		}
		else if (timeSpeed == 2)
		{
			if (ImGui::Button(">>"))
			{
				timeSpeed = 6;
			}
		}
		else if (timeSpeed == 6)
		{
			if (ImGui::Button("-|"))
			{
				timeSpeed = 6;
			}
		}
		else
		{
			if (ImGui::Button("|<"))
			{
				timeSpeed = 1;
			}
		}
		// Bottom Empire Stats
		ImGui::Separator();
		ImGui::TextColored(ImVec4(255, 255, 255, 1.0f), "Empire: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%i (%i/d)", linked->money, linked->moneydaily);
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(255, 255, 0, 1.0f), "C");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "| %i (%i/d)", linked->food, linked->fooddaily);
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "F");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "| %i (%i/d)", linked->tech, linked->techdaily);
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.9f, 1.0f), "T");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "| %i (%i/d)", linked->metal, linked->metaldaily);
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "M");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "| %i", linked->happiness);
		ImGui::SameLine();
		if (linked->happiness >= 0)
		{
			ImGui::TextColored(ImVec4(0.2, 1.0f, 0.2, 1.0f), ":)");
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.2, 0.2, 1.0f), "):");
		}
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "| %i", (int)(1 / frameAverage));
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "FPS");

		//	ImGui::Image(magic, sf::Vector2f(16, 16), sf::FloatRect(0, 0, 16, 16));
		ImGui::End();
	}


	bool allowMove = true;
	bool wasRightDown = false;
	
	bool wasGuideButtonDown = false;

	void drawUI(PlayerState state)
	{
		if (state == PlayerState::VIEW_MODE)
		{
			drawEmpireWindow();
			drawSystemWindow();
			

		}
		else if (state == PlayerState::EDIT_MODE)
		{
			drawSystemWindow();
			drawEmpireWindow();
			drawEditorUI();
		}
		else if (state == PlayerState::CHOOSE_WORLD)
		{
			drawWorldChooserWindow();
		}

		if (ecoWindow)
		{
			drawEconomyWindow();
		}

		if (attackPlaner)
		{
			drawAttackPlaner();
		}

		if (popWindow)
		{
			drawPeopleWindow();
		}

		if (retWindow)
		{
			drawRelationsWindow();
		}

		if (settingsOpen)
		{
			drawSettingsWindow();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
		{
			if (wasGuideButtonDown == false)
			{
				showGuide = !showGuide;
				wasGuideButtonDown = true;
			}
		}
		else
		{
			wasGuideButtonDown = false;
		}

		if (showGuide)
		{
			drawGuide();
		}
	}
	
	bool wasMousePressed = false;

	bool limitFPS = true;

	void updateDaily()
	{
		linked->updateDaily();
	}

	void updateMonthly()
	{
		linked->updateMonthly();
	}

	bool updateWindow = false;

	void update(sf::View* v, float dt, sf::Vector2f mousePos, sf::RenderWindow* win)
	{

		if (fullscreen && !previousFullscreen)
		{
			win->create(sf::VideoMode::getFullscreenModes()[0], "FULLSCREEN TinyPlanets", sf::Style::Fullscreen);
			previousFullscreen = true;
			updateWindow = true;

		}
		else if (!fullscreen && previousFullscreen)
		{
			win->create(sf::VideoMode(1000, 650), "TinyPlanets", sf::Style::Default);
			previousFullscreen = false;
			updateWindow = true;
		}
		else
		{
			updateWindow = false;
		}



		frameAverage += dt;
		frameAverage /= 2;

		if (state != PlayerState::CHOOSE_WORLD)
		{

			timeStepper += dt * timeSpeed;
			if (timeStepper >= 1.0f)
			{
				timeStepper = 0.0f;
				day++;
				if (day > 30)
				{
					month++;
					day = 1;
				}
				if (month > 12)
				{
					month = 1;
					year++;
				}
			}
		}

		drawUI(state);



		if (state == PlayerState::VIEW_MODE)
		{

			if (focused != NULL)
			{
				v->setCenter(focused->worldPosition + sf::Vector2f(focused->radius, focused->radius));
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			{
				if (focused == NULL)
					v->move(sf::Vector2f(0, -1000.0f) * dt);
				focused = NULL;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			{
				if (focused == NULL)
					v->move(sf::Vector2f(-1000.0, 0.0f) * dt);
				focused = NULL;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			{
				if (focused == NULL)
					v->move(sf::Vector2f(0, 1000.0f) * dt);
				focused = NULL;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			{
				if (focused == NULL)
					v->move(sf::Vector2f(1000.0f, 0.0f) * dt);
				focused = NULL;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			{
				v->zoom(1.002f);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
			{
				v->zoom(0.998f);
			}
		}
		else if (state == PlayerState::EDIT_MODE)
		{
			/*if (focused != NULL)
			{
				v->setCenter(focused->worldPosition + sf::Vector2f(focused->radius, focused->radius));
			}*/

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			{
				editorOffset.y -= 100.0f * dt;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			{
				editorOffset.x -= 100.0f * dt;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			{
				editorOffset.y += 100.0f * dt;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			{
				editorOffset.x += 100.0f * dt;
			}

			v->setCenter(sf::Vector2f(focused->radius + editorOffset.x, focused->radius + editorOffset.y));

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			{
				v->zoom(1.002f);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
			{
				v->zoom(0.998f);
			}

			//mousePos += sf::Vector2f(focused->radius * 5, focused->radius * 5);

			mousePos = win->mapPixelToCoords((sf::Vector2i)mousePos);

			if (allowMove)
			{

				tileC = focused->getClosestSector(mousePos);

			}

			if (tileC > focused->size)
			{
				tileC = focused->size;
			}

			cursor.setOrigin(16, 64);


			cursor.setPosition(focused->getSectorPosition(tileC));
			cursor.setRotation(focused->getSectorAngle(tileC));

			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !ImGui::GetIO().WantCaptureMouse)
			{
				if (!wasMousePressed)
				{


					if (choosenTileType != 0)
					{
						if (focused->tiles[tileC] == 0)
						{
							if (linked->money - Planet::getBuildingPrice(choosenTileType) > 0)
							{
								focused->tiles[tileC] = choosenTileType;
								focused->usedtiles++;
								linked->money -= Planet::getBuildingPrice(choosenTileType);
								focused->tilesBuilding[tileC] = 0.0f;
							}
							else
							{
								// Play sound
								errorP.play();
							}
						}
						// BUY THE TILE
					}

					wasMousePressed = true;
				}
				
			}
			else
			{
				wasMousePressed = false;
			}
			
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) && !ImGui::GetIO().WantCaptureMouse)
			{
				if (!wasRightDown)
				{
					allowMove = !allowMove;
					wasRightDown = true;
				}
			}
			else
			{
				wasRightDown = false;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				state = PlayerState::VIEW_MODE;
			}




			/*printf("Drawing cursor at tile: %i {%f, %f} {%f, %f}\n", tileC, cursor.getPosition().x, 
				cursor.getPosition().y, focused->worldPosition.x, focused->worldPosition.y);
			printf("Mouse pos {%f, %f}\n", mousePos.x, mousePos.y);*/
		}
		else if(state == PlayerState::CHOOSE_WORLD)
		{
			if (focused != NULL)
			{
				v->setCenter(focused->worldPosition + sf::Vector2f(focused->radius, focused->radius));
			}
		}
	}

	bool previousFullscreen = false;

	void draw(sf::RenderWindow* win)
	{
		if (focused != NULL)
		{
			win->draw(cursor);
		}


	}

	void resize(int width, int height)
	{
		empireWindow = sf::Rect<float>(sf::Vector2f(0, (float)height -
			((float)height / 8.0f)),
			sf::Vector2f((float)width, (float)height / 7.0f));

		systemWindow = sf::Rect<float>(sf::Vector2f(0, 0),
			sf::Vector2f((float)width, 85));
	}

	EmpirePlayer(Universe* universe)
	{
		this->universe = universe;

		selector = sf::Texture();
		selector.loadFromFile("Resource/selector.png");

		cursor = sf::Sprite(selector);
	}
};



class EmpireManager
{
	

public:

	//Universe* universe;

	EmpirePlayer* player;

	std::vector<Empire*> empires;

	Universe* universe;

	std::vector<EmpireAI*> aiempires;

	void update(sf::View* v, float dt, sf::Vector2f mousepos, sf::RenderWindow* win)
	{
		player->update(v, dt, mousepos, win);

		if (player->state != PlayerState::CHOOSE_WORLD)
		{
			for (int i = 0; i < aiempires.size(); i++)
			{
				aiempires[i]->update(dt);
			}
		}
	}

	void render(sf::RenderWindow* win)
	{
		player->draw(win);
	}

	void updateDaily()
	{
		player->updateDaily();
		for (int i = 0; i < aiempires.size(); i++)
		{
			aiempires[i]->updateDaily();
		}
	}

	void updateMonthly()
	{
		player->updateMonthly();
		for (int i = 0; i < aiempires.size(); i++)
		{
			aiempires[i]->updateMonthly();
		}


	}

	void createAIEmpire()
	{
		Empire* n = new Empire();
		if (empires.size() != 0)
		{
			for (int i = 0; i < empires.size(); i++)
			{
				n->otherEmpires.push_back(empires[i]);
			}
		}

		n->handicap = 1.0f;
		EmpireAI* nAi = new EmpireAI();
		
		if (rand() > RAND_MAX / 2)
		{
			if (rand() > RAND_MAX / 2)
			{
				nAi->personality = Personality::HOSTILE;
			}
			else
			{
				nAi->personality = Personality::FRIENDLY;
			}
		}
		else
		{
			nAi->personality = Personality::NORMAL;
		}

		empires.push_back(n);
		nAi->linked = empires[empires.size() - 1];
		nAi->universe = universe;
		// Add us to every other empire
		for (int i = 0; i < empires.size() - 1; i++)
		{
			empires[i]->otherEmpires.push_back(empires[empires.size() - 1]);
			nAi->relations[empires[i]] = Relation::NEUTRAL;
			if (empires[i]->aicontroller != NULL)
			{
				empires[i]->aicontroller->relations[empires[empires.size() - 1]] = Relation::NEUTRAL;
			}
		}


		aiempires.push_back(nAi);

		empires[empires.size() - 1]->aicontroller = aiempires[aiempires.size() - 1];
	}

	void createPlayerEmpire()
	{
		Empire* n = new Empire();
		
		if (empires.size() != 0)
		{
			// Add every other empire to our empire
			for (int i = 0; i < empires.size(); i++)
			{
				n->otherEmpires.push_back(empires[i]);
			}
		}
		empires.push_back(n);
		player->linked = empires[empires.size() - 1];

		// Add us to every other empire
		for (int i = 0; i < empires.size() - 1; i++)
		{
			empires[i]->otherEmpires.push_back(empires[empires.size() - 1]);
		}
	}

	EmpireManager(Universe* universe)
	{
		this->universe = universe;
	}
};