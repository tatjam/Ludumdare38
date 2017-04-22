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

#define BUILDING_HOUSE 0
#define BUILDING_LABORATORY 1
#define BUILDING_LAUNCHER 2
#define BUILDING_WARPLACE 3
#define BUILDING_MINE 4
#define BUILDING_MAGICPLACE 5
#define BUILDING_FARM 6
#define BUILDING_APPARTMENT 7

#define SECTOR_SIZE 32.0f

class Planet;

class Building
{
public:

	int type = 0;

	void draw();

	Building()
	{}
	~Building()
	{}
};

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



public:

	bool hasAtmosphere;
	sf::Color atmosphereColor;
	sf::Color crustColor;
	sf::Color surfaceColor;
	sf::Color cloudColor;


	sf::Vector2f worldPosition;

	int size;

	float radius;

	float rotation;
	float rotationSpeed;

	int population;
	int soldiers;

	std::map<int, Building> buildings;

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
	Planet(int size, sf::Texture shade, sf::Texture rock, sf::Texture night, sf::Texture atmo);
	~Planet();
};

