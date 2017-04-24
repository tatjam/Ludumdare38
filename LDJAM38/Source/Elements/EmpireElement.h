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

#define STARTING_MONEY 2500
#define STARTING_FOOD 1000
#define STARTING_TECH 50
#define STARTING_METAL 50

#define FOOD_PER_PERSON 0.5f



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

class Empire
{
public:

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
	int soldiers = 0;


	int launcherCount = 0;
	int launchCount = 0;
	int daysSinceLaunch = 0;

	bool canLaunch = false;

	int total_houses = 0;

	float wantedTaxes = 0.5f;
	float taxes = 0.5f;

	float workEfficiency = 1.0f;

	std::vector<Planet*> planets;

	Empire()
	{
		planets = std::vector<Planet*>();
		previousMoney.push_back(STARTING_MONEY);
		previousFood.push_back(STARTING_FOOD);
		previousMetal.push_back(STARTING_METAL);
		previousTech.push_back(STARTING_TECH);
	}

	void updateMonthly()
	{
		taxes = wantedTaxes;

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

		if (happiness >= 1)
		{
			// Every 2 happiness points you get a person
			population += (int)(0.5f * happiness);
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

	// Called every day
	void updateDaily()
	{

		happiness = 0;

		previousMetal.push_back(metal);
		previousFood.push_back(food);
		previousTech.push_back(tech);
		previousMoney.push_back(money);

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
							money -= HOUSE_MAINTENANCE;
							moneydaily -= HOUSE_MAINTENANCE;
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
							money -= LAB_MAINTENANCE;
							moneydaily -= LAB_MAINTENANCE;
							if (money >= LAB_MAINTENANCE && usedPopulation < population)
							{
								tech += LAB_GENERATION;
								techdaily += LAB_GENERATION;

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
							money -= APP_MAINTENANCE;
							moneydaily -= APP_MAINTENANCE;
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
							money -= FARM_MAINTENANCE;
							moneydaily -= FARM_MAINTENANCE;

							if (money >= FARM_MAINTENANCE && usedPopulation < population)
							{
								food += FARM_GENERATION;
								fooddaily += FARM_GENERATION;
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
							money -= MINE_MAINTENANCE;
							moneydaily -= MINE_MAINTENANCE;

							if (money >= MINE_MAINTENANCE && usedPopulation < population)
							{
								metal += MINE_GENERATION;
								metaldaily += MINE_GENERATION;
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
						}
					}
					else
					{
						float increase = 1 / (float)Planet::getBuildingTime(planets[i]->tiles[t]);
						planets[i]->tilesBuilding[t] += increase;
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

#define METAL_FOR_LAUNCH 2000
#define TECH_FOR_LAUNCH 2000

#define AI_SAFE_MARGIN 500

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

	void aiBuild(int buildingType)
	{
		// Will find free space and build a building duh
		// If the building extracts resource it will prioritize 
		// planets with a boost of some kinds
		if(linked->money - Planet::getBuildingPrice(buildingType) > AI_SAFE_MARGIN)
		{
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
							return;
						}
					}
				}
			}

		}

	}

	void updateDaily()
	{
		linked->updateDaily();

		printf("-----------------------\n");
		printf("AI State:\n");
		printf("-----------------------\n");
		printf("Cash: %i (%i) | Metal: %i (%i) | \nFood: %i (%i) | Tech: %i (%i)\n", 
			linked->money, linked->moneydaily, linked->metal, linked->metaldaily,
			linked->food, linked->fooddaily, linked->tech, linked->techdaily);
		printf("Happiness: %i | Population: %i\n", linked->happiness, linked->population);

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
				aiBuild(BUILDING_FARM);
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
					printf("AI making money (markets and trading)\n");

					aiBuild(BUILDING_MARKET);
				}
				else if ((float)linked->housed / (float)linked->population < 0.8f)
				{
					// We need to build houses!
					printf("AI building houses!\n");
					aiBuild(BUILDING_HOUSE);
				}
				else if ((float)linked->workers / (float)linked->population < 0.8f)
				{
					// We need more places to work!
					printf("AI is making workplaces!\n");

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

					switch (lowestN)
					{
						case 0:
							printf("Building money generator\n");
							break;
						case 1:
							printf("Building farms\n");
							aiBuild(BUILDING_FARM);
							break;
						case 2:
							printf("Building lab\n");
							aiBuild(BUILDING_LABORATORY);
							break;
						case 3:
							printf("Building mine!\n");
							break;
					}

				}
				else
				{
					// ???
					printf("AI is decreasing taxes!\n");
					linked->wantedTaxes = linked->taxes - 0.1f;
				}
			}
			else if (focus == AIFocus::MAKE_MONEY)
			{
				if (linked->happiness > 8)
				{
					// We can increase taxes a bit
					printf("AI is increasing taxes!\n");
					linked->wantedTaxes = linked->taxes + 0.2f;
				}
				else
				{
					printf("AI is building markets and trying to trade!\n");
				}
			}
			else if (focus == AIFocus::MAKE_SOLDIERS)
			{
				printf("AI is building all kind of stuff but focusing on war stuff!\n");
			}
			else if (focus == AIFocus::EXPAND)
			{
				printf("AI is making resources to expand!\n");
				if (linked->money > LAUNCHER_PRICE + AI_SAFE_MARGIN)
				{
					printf("AI will build launcher!\n");
				}
				else
				{
					printf("AI will only build all kind of resource builders!\n");
					if (rand() > RAND_MAX / 2)
					{
						printf("AI building market\n");
						aiBuild(BUILDING_MARKET);
					}
					else
					{
						printf("AI building lab!\n");
						aiBuild(BUILDING_LABORATORY);
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
							focus = MAKE_MONEY;
						}
					}
					printf("AI focusing on making tech and metal to expand\n");
					focus = EXPAND;
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

	char nameBuff[256];

	int choosenTileType = 0;

	int tileC = 0;

	std::string choosenTileName;

	sf::Texture* choosenTileImage;

	sf::Vector2f editorOffset = sf::Vector2f(0.0f, 0.0f);


	sf::Texture gear;
	sf::Texture cross;

	void drawEditorUI()
	{
		ImGui::Begin("Tiles");

		for (int i = 1; i < BUILDING_TYPES; i++)
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
				ImGui::TextWrapped("Extracts metal depending on the planet's concentration (Base %i). %iC every month",
					MINE_GENERATION, MINE_MAINTENANCE);
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
		}

		if (focused->tiles[tileC] != 0)
		{
			if (ImGui::Button("Destroy"))
			{
				focused->tiles[tileC] = 0;
				focused->usedtiles--;
			}
		}

		ImGui::End();
	}

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
			ImGui::Text("Tip: Check the manual for help");
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

	void drawEmpireWindow()
	{

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
			if (focused->owner == linked)
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
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "12312");
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

	void drawSettingsWindow()
	{
		ImGui::Begin("Settings");
		ImGui::Checkbox("Fullscreen", &fullscreen);
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
			bool isPlanetOurs = contains(&linked->planets, universe->planets[i]);

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
		ImGui::TextColored(ImVec4(0.8f, 0.7f, 0.1f, 1.0f), "F");
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
		if (linked->happiness > 0)
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
	
	void drawUI(PlayerState state)
	{
		if (state == PlayerState::VIEW_MODE)
		{
			drawEmpireWindow();
			drawSystemWindow();
			if (ecoWindow)
			{
				drawEconomyWindow();
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
	}
	
	bool wasMousePressed = false;

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
			win->create(sf::VideoMode::getFullscreenModes()[0], "FULLSCREEN Tinyplanets", sf::Style::Fullscreen);
			previousFullscreen = true;
			updateWindow = true;

		}
		else if (!fullscreen && previousFullscreen)
		{
			win->create(sf::VideoMode(1000, 650), "Tinyplanets", sf::Style::Default);
			previousFullscreen = false;
			updateWindow = true;
		}
		else
		{
			updateWindow = false;
		}



		frameAverage += dt;
		frameAverage /= 2;

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

		for (int i = 0; i < aiempires.size(); i++)
		{
			aiempires[i]->update(dt);
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