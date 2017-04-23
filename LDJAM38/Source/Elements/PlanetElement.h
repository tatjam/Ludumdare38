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
#define BUILDING_LAUNCHER 3
#define BUILDING_WARPLACE 4
#define BUILDING_MINE 5
#define BUILDING_MAGICPLACE 6
#define BUILDING_FARM 7
#define BUILDING_APPARTMENT 8

#define SECTOR_SIZE 32.0f

class Planet;

class Empire
{
	std::vector<Planet*> planets;
	std::string name;
	
	float money;

	float resources[5];

	void render();
};


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

	std::map<std::string, sf::Texture> buildings;

public:

	std::string name;

	bool easyHabitable = false;

	bool hasAtmosphere;
	sf::Color atmosphereColor;
	sf::Color crustColor;
	sf::Color surfaceColor;
	sf::Color cloudColor;


	sf::Vector2f worldPosition;

	int size;

	float radius;

	float distanceToStar = 0.0f;
	float angleInStar = 0.0f;

	float rotation;
	float rotationSpeed;

	int population;
	int soldiers;

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

