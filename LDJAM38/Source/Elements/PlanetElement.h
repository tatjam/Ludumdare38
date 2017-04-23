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
#define BUILDING_WARPLACE 7
#define BUILDING_MAGICPLACE 8

#define BUILDING_TYPES 7


// HOUSE
#define HOUSE_PRICE 100
#define HOUSE_MAINTENANCE 0
#define HOUSE_HOUSING 5
// LABORATORY
#define LAB_PRICE 1000
#define LAB_MAINTENANCE 5
#define LAB_GENERATION 2
#define LAB_EMPLOYMENT 10
// APPARTMENT
#define APP_PRICE 1000
#define APP_MAINTENANCE 2
#define APP_HOUSING 25
// FARM
#define FARM_PRICE 250
#define FARM_MAINTENANCE 5
#define FARM_GENERATION 10
#define FARM_EMPLOYMENT 10
// LAUNCHER
#define LAUNCHER_PRICE 25000
#define LAUNCHER_MAINTENANCE 50
#define LAUNCHER_EMPLOYMENT 30
// MINE
#define MINE_PRICE 2000
#define MINE_MAINTENANCE 20
#define MINE_GENERATION 15
#define MINE_EMPLOYMENT 20


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

	std::map<std::string, sf::Texture> buildings;

	std::string name;

	bool easyHabitable = false;

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

	int population;
	int soldiers;

	Empire* owner;

	std::vector<int> tiles;

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

