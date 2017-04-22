#include "PlanetElement.h"



Planet::Planet(int size, sf::Texture shade, sf::Texture rock, sf::Texture night, sf::Texture atmo)
{
	this->size = size;
	population = 0;
	soldiers = 0;

	worldPosition = sf::Vector2f(0, 0);

	buildings = std::map<int, Building>();

	this->rotation = -90.0f;

	this->rotationSpeed = 90.f;

	this->shade = shade;
	this->rock = rock;
	this->nightShadeTex = night;
	this->atmo = atmo;
}

void Planet::update(float dt, bool rotate)
{
	if (rotate)
	{
		this->rotation += this->rotationSpeed * dt;
	}
}

Planet::~Planet()
{
	
}

sf::Vector2f Planet::getSectorPosition(int s)
{
	sf::Vector2f c = sf::Vector2f(radius, radius);
	sf::Vector2f out;
	if (s < 0 || s > size)
	{
		return out;
	}

	float travel = s * SECTOR_SIZE + SECTOR_SIZE / 2;
	float circ = size * SECTOR_SIZE;
	float angle = (travel / circ) * TAU;

	out.x = c.x + radius * cos(angle);
	out.y = c.y + radius * sin(angle);

	return out;
}

float Planet::getSectorAngle(int s)
{
	float angleInSectors = 360.f / (float)size;

	return angleInSectors * s + (angleInSectors / 2) + 90;
}

void Planet::drawSector(int s, sf::RenderWindow* win, sf::Vector2f offset)
{
	if (s >= 0 && s < size)
	{
		sf::RectangleShape sector = sf::RectangleShape(
			sf::Vector2f(SECTOR_SIZE, 5.0f));
		sf::Vector2f center = getSectorPosition(s);
		sector.setOrigin(SECTOR_SIZE / 2, 2.5f);

		sector.setPosition(center + offset);
		sector.setRotation(getSectorAngle(s));

		win->draw(sector);
	}

}

int Planet::getClosestSector(sf::Vector2f mousePos)
{
	std::vector<sf::Vector2f> points;
	sf::Vector2f c = sf::Vector2f(radius, radius);
	for (int i = 0; i < size; i++)
	{
		// Get position of point
		// Calculate angle:
		float travel = i * SECTOR_SIZE + SECTOR_SIZE / 2;
		float circ = size * SECTOR_SIZE;
		float angle = (travel / circ) * TAU;


		sf::Vector2f p;
		p.x = c.x + radius * cos(angle);
		p.y = c.y + radius * sin(angle);

		points.push_back(p);
	}

	float minDistance = 999999.0f;
	int minI = 99999999;
	for (int i = 0; i < size; i++)
	{

		// \  /----------------------------------
		//  \/  (x1-x2)(x1-x2) + (y1-y2)(y1-y2)
		float distance =
			std::sqrtf(
				std::powf(points[i].x - mousePos.x, 2.0f) +
				std::powf(points[i].y - mousePos.y, 2.0f)
				);
		if (distance < minDistance)
		{
			minDistance = distance;
			minI = i;
		}
	}

	return minI;

}

void Planet::makeDrawables()
{
	// radius = circ / TAU
	radius = (SECTOR_SIZE * size) / TAU;

	land = sf::CircleShape(radius, size);
	land.setFillColor(surfaceColor);

	crust = sf::CircleShape(radius * 0.95f, size);
	crust.setFillColor(crustColor);

	crust.setTexture(&rock);

	inner = sf::Sprite(shade);
	inner.setScale(sf::Vector2f(radius / 64, radius / 64));

	nightShade = sf::CircleShape(radius, size);
	nightShade.setTexture(&nightShadeTex);

	nightShade.setOrigin(radius, radius);
	land.setOrigin(radius, radius);
	crust.setOrigin(radius * 0.95f, radius * 0.95f);
	inner.setOrigin(64.0f, 64.0f);

	atmoSpr = sf::Sprite(atmo);
	atmoSpr.setColor(atmosphereColor);
	atmoSpr.setScale(radius / 64, radius / 64);
	atmoSpr.setOrigin(256 / 2, 256 / 2);

}

void Planet::draw(sf::RenderWindow* win, sf::Vector2f sun)
{
	//TODO proper night time shade drawing

	land.setPosition(sf::Vector2f(worldPosition.x + radius,
		worldPosition.y + radius));
	crust.setPosition(sf::Vector2f(worldPosition.x + (radius * 0.01f) + radius, 
		worldPosition.y + (radius * 0.01f) + radius));
	inner.setPosition(sf::Vector2f(worldPosition.x + radius,
		worldPosition.y + radius));
	nightShade.setPosition(sf::Vector2f(worldPosition.x + radius, worldPosition.y + radius));
	atmoSpr.setPosition(sf::Vector2f(worldPosition.x + radius, worldPosition.y + radius));

	nightShade.setRotation(rotation);
	crust.setRotation(rotation);
	land.setRotation(rotation);

	atmoSpr.setRotation(rotation);

	if (hasAtmosphere)
	{
		win->draw(atmoSpr);
	}
	win->draw(land);
	win->draw(crust);
	win->draw(inner);
	win->draw(nightShade);
}