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

#define STARTING_MONEY 5000
#define STARTING_FOOD 10000
#define STARTING_TECH 100
#define STARTING_METAL 1000



class Empire
{
public:


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

	int workers = 5;
	int population = 10;
	int soldiers = 0;


	int launcherCount = 0;
	int launchCount = 0;
	int daysSinceLaunch = 0;

	bool canLaunch = false;

	std::vector<Planet*> planets;

	Empire()
	{
		planets = std::vector<Planet*>();
	}

	// Called every day
	void updateDaily()
	{

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

		for (int i = 0; i < planets.size(); i++)
		{
			// Do daily maintenance and daily generations
			// Also calculate employment and run buildings as required
			for (int t = 0; t < planets[i]->size; t++)
			{
				
			}
		}
	}


};


class EmpireAI
{
public:



	bool firstframe = false;

	Empire* linked;
	std::map<Empire*, Relation> relations;

	Universe* universe;

	void update(float dt)
	{
		// Complex sentient AI incoming


		if (firstframe)
		{
			// Choose our planet

			bool valid = false;


			while (!valid)
			{
				int i = rand() % universe->count;
				if (universe->planets[i]->owner == NULL)
				{
					linked->planets.push_back(universe->planets[i]);
					universe->planets[i]->owner = linked;
				}
			}

			valid = false;

			while (!valid)
			{
				linked->name == universe->nameList[rand() % 11];
				if (std::find(universe->usedAINames.begin(), universe->usedAINames.end(), linked->name) 
					== universe->usedAINames.end())
				{
					valid = true;
				}
			}

		
		}
		else
		{
			// Play the game :3

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

	void drawEmpireWindow()
	{
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

		ImGui::Button("Economy", ImVec2(empireWindow.height - 40, empireWindow.height - 40));
		ImGui::SameLine();
		ImGui::BeginChild("EmpireSubEconomy", ImVec2(empireWindow.height + 40, empireWindow.height - 40), true);
		static float TestData[6] = { 0.f,-4.f,3.f,-2.f,0.f,4.f };
		ImGui::PlotLines("Cash", TestData, 6);
		ImGui::PlotLines("Food", TestData, 6);
		ImGui::PlotLines("Metal", TestData, 6);
		ImGui::PlotLines("Tech", TestData, 6);
		ImGui::EndChild();
		ImGui::SameLine();
		ImGui::Button("People", ImVec2(empireWindow.height - 40, empireWindow.height - 40));
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
		ImGui::EndChild();
		ImGui::SameLine();
		ImGui::Button("Relations", ImVec2(empireWindow.height - 40, empireWindow.height - 40));
		ImGui::SameLine();
		ImGui::BeginChild("EmpireSubRelations", ImVec2(empireWindow.height + 40, empireWindow.height - 40), true);
		ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Mr.Robot: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "GOOD");
		ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Comcast: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "AT WAR");
		ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Google: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "NEUTRAL");
		ImGui::EndChild();

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
			if (ImGui::Button(universe->planets[i]->name.c_str()))
			{
				focused = universe->planets[i];
				state = PlayerState::VIEW_MODE;
			}

			if (isPlanetOurs)
			{
				ImGui::PopStyleColor(1);
			}

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


		}
		else if (state == PlayerState::EDIT_MODE)
		{
			drawSystemWindow();
			drawEditorUI();
		}
		else if (state == PlayerState::CHOOSE_WORLD)
		{
			drawWorldChooserWindow();
		}
	}
	
	bool wasMousePressed = false;

	void update(sf::View* v, float dt, sf::Vector2f mousePos, sf::RenderWindow* win)
	{

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
							focused->tiles[tileC] = choosenTileType;
							focused->usedtiles++;
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

	std::vector<Empire> empires;

	Universe* universe;

	std::vector<EmpireAI> aiempires;

	void update(sf::View* v, float dt, sf::Vector2f mousepos, sf::RenderWindow* win)
	{
		player->update(v, dt, mousepos, win);

		for (int i = 0; i < aiempires.size(); i++)
		{
			aiempires[i].update(dt);
		}
	}

	void render(sf::RenderWindow* win)
	{
		player->draw(win);
	}

	void createAIEmpire()
	{
		Empire n = Empire();
		if (empires.size() != 0)
		{
			for (int i = 0; i < empires.size(); i++)
			{
				n.otherEmpires.push_back(&empires[i]);
			}
		}
		EmpireAI nAi = EmpireAI();
		empires.push_back(n);
		nAi.linked = &empires[empires.size() - 1];
		nAi.universe = universe;
		// Add us to every other empire
		for (int i = 0; i < empires.size() - 1; i++)
		{
			empires[i].otherEmpires.push_back(&empires[empires.size() - 1]);
		}
	}

	void createPlayerEmpire()
	{
		Empire n = Empire();
		
		if (empires.size() != 0)
		{
			// Add every other empire to our empire
			for (int i = 0; i < empires.size(); i++)
			{
				n.otherEmpires.push_back(&empires[i]);
			}
		}
		empires.push_back(n);
		player->linked = &empires[empires.size() - 1];
		// Add us to every other empire
		for (int i = 0; i < empires.size() - 1; i++)
		{
			empires[i].otherEmpires.push_back(&empires[empires.size() - 1]);
		}
	}

	EmpireManager(Universe* universe)
	{
		this->universe = universe;
	}
};