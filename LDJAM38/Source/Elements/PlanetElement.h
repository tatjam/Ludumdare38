#pragma once

#include <map>

#include <SFML/Graphics.hpp>

#define RESOURCE_BUILDING 0
#define RESOURCE_FOOD 1
#define RESOURCE_METAL 2
#define RESOURCE_TECH 3
#define RESOURCE_MAGIC 4

// Mathematical constants ¬_¬
#define PI 3.14159
#define TAU 6.283185

#define BUILDING_EMPTY 0
#define BUILDING_HOUSE 1
#define BUILDING_LABORATORY 2
#define BUILDING_APPARTMENT 3
#define BUILDING_FARM 4
#define BUILDING_LAUNCHER 5
#define BUILDING_MINE 6
#define BUILDING_MARKET 7
#define BUILDING_SMARKET 8
#define BUILDING_HFARM 9


#define BUILDING_TYPES_TOTAL 10


// HOUSE
#define HOUSE_PRICE 100
#define HOUSE_MAINTENANCE 0
#define HOUSE_HOUSING 5
#define HOUSE_BUILD_TIME 3
// LABORATORY
#define LAB_PRICE 500
#define LAB_MAINTENANCE 10
#define LAB_GENERATION 8
#define LAB_EMPLOYMENT 10
#define LAB_BUILD_TIME 7
// APPARTMENT
#define APP_PRICE 500
#define APP_MAINTENANCE 2
#define APP_HOUSING 25
#define APP_BUILD_TIME 10
// FARM
#define FARM_PRICE 250
#define FARM_MAINTENANCE 5
#define FARM_GENERATION 10
#define FARM_EMPLOYMENT 5
#define FARM_BUILD_TIME 5
// LAUNCHER
#define LAUNCHER_PRICE 4500
#define LAUNCHER_MAINTENANCE 15
#define LAUNCHER_EMPLOYMENT 15
#define LAUNCHER_BUILD_TIME 70
// MINE
#define MINE_PRICE 500
#define MINE_MAINTENANCE 20
#define MINE_GENERATION 10
#define MINE_EMPLOYMENT 20
#define MINE_BUILD_TIME 20
// MARKET
#define MARKET_PRICE  900
#define MARKET_MAINTENANCE 0
#define MARKET_GENERATION 10
#define MARKET_EMPLOYMENT 5
#define MARKET_BUILD_TIME 10
// SMARKET
#define SMARKET_PRICE  300
#define SMARKET_MAINTENANCE 0
#define SMARKET_GENERATION 3
#define SMARKET_EMPLOYMENT 2
#define SMARKET_BUILD_TIME 4
// HFARM
#define HFARM_PRICE 600
#define HFARM_MAINTENANCE 10
#define HFARM_GENERATION 25
#define HFARM_EMPLOYMENT 9
#define HFARM_BUILD_TIME 8

#define SECTOR_SIZE 32.0f

class Planet;

class Empire;

class Planet
{

private:
	sf::CircleShape crust;
	sf::CircleShape land;
	sf::Sprite inner;
	sf::CircleShape nightShade;
	sf::Sprite atmoSpr;

	sf::Texture shade;
	sf::Texture rock;
	sf::Texture nightShadeTex;
	sf::Texture atmo;

	

public:

	static int getBuildingPrice(int type)
	{
		switch(type)
		{
		case BUILDING_HOUSE:
			return HOUSE_PRICE;
		case BUILDING_APPARTMENT:
			return APP_PRICE;
		case BUILDING_FARM:
			return FARM_PRICE;
		case BUILDING_LABORATORY:
			return LAB_PRICE;
		case BUILDING_MARKET:
			return MARKET_PRICE;
		case BUILDING_MINE:
			return MINE_PRICE;
		case BUILDING_LAUNCHER:
			return LAUNCHER_PRICE;
		case BUILDING_SMARKET:
			return SMARKET_PRICE;
		case BUILDING_HFARM:
			return HFARM_PRICE;
		}

		return 0;
	}

	static int getBuildingTime(int type)
	{
		switch (type)
		{
		case BUILDING_HOUSE:
			return HOUSE_BUILD_TIME;
		case BUILDING_APPARTMENT:
			return APP_BUILD_TIME;
		case BUILDING_FARM:
			return FARM_BUILD_TIME;
		case BUILDING_LABORATORY:
			return LAB_BUILD_TIME;
		case BUILDING_MARKET:
			return MARKET_BUILD_TIME;
		case BUILDING_MINE:
			return MINE_BUILD_TIME;
		case BUILDING_LAUNCHER:
			return LAUNCHER_BUILD_TIME;
		case BUILDING_SMARKET:
			return SMARKET_BUILD_TIME;
		case BUILDING_HFARM:
			return HFARM_BUILD_TIME;
		}

		return 0;
	}

	std::map<std::string, sf::Texture> buildings;

	std::string name;

	bool easyHabitable = false;

	bool alreadyOwned = true;

	bool hasAtmosphere;
	sf::Color atmosphereColor;
	sf::Color crustColor;
	sf::Color surfaceColor;
	sf::Color cloudColor;


	sf::Vector2f worldPosition;

	int size;

	int usedtiles = 0;

	float radius;

	float distanceToStar = 0.0f;
	float angleInStar = 0.0f;

	float rotation;
	float rotationSpeed;

	float mineralBoost = 1.0f;
	float scienceBoost = 1.0f;
	float foodBoost = 1.0f;

	int population;
	int soldiers;

	Empire* owner;

	std::vector<int> tiles;

	// From 0 to 1.0f. Calculated in Empire
	std::vector<float> tilesBuilding;

	void makeDrawables();
	void draw(sf::RenderWindow* win, sf::Vector2f sunloc);

	// Give mouse pos relative to planet
	// Ignores rotation
	// Ignores worldPosition
	int getClosestSector(sf::Vector2f mousePos);

	// Gets center of sector
	sf::Vector2f getSectorPosition(int s);
	float getSectorAngle(int s);

	void drawSector(int s, sf::RenderWindow* win, sf::Vector2f offset);

	void update(float dt, bool rotate);

	// Size must be even not uneven!
	Planet(int size, sf::Texture shade, sf::Texture rock, 
		sf::Texture night, sf::Texture atmo, std::map<std::string, sf::Texture> buildings);
	~Planet();
};

