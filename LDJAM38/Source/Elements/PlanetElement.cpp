#include "PlanetElement.h"



Planet::Planet(int size, sf::Texture shade, sf::Texture rock, sf::Texture night, 
	sf::Texture atmo, std::map<std::string, sf::Texture> buildings)
{
	this->size = size;
	population = 0;
	soldiers = 0;

	worldPosition = sf::Vector2f(0, 0);

	this->buildings = buildings;

	this->rotation = -90.0f;

	this->rotationSpeed = 90.f;

	this->shade = shade;
	this->rock = rock;
	this->nightShadeTex = night;
	this->atmo = atmo;

	this->tiles = std::vector<int>(size, 0);
	this->tilesBuilding = std::vector<float>(size, 0.0f);
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
	// We only draw if the planet is inside the view

	sf::View v = win->getView();

	if (
		worldPosition.x < v.getCenter().x + v.getSize().x
		&& worldPosition.y < v.getCenter().y + v.getSize().y
		&& worldPosition.x > v.getCenter().x - v.getSize().x
		&& worldPosition.y > v.getCenter().y - v.getSize().y)
	{

		land.setPosition(sf::Vector2f(worldPosition.x + radius,
			worldPosition.y + radius));
		crust.setPosition(sf::Vector2f(worldPosition.x + (radius * 0.01f) + radius,
			worldPosition.y + (radius * 0.01f) + radius));
		inner.setPosition(sf::Vector2f(worldPosition.x + radius,
			worldPosition.y + radius));
		nightShade.setPosition(sf::Vector2f(worldPosition.x + radius, worldPosition.y + radius));
		atmoSpr.setPosition(sf::Vector2f(worldPosition.x + radius, worldPosition.y + radius));

		int rotationN = 0;
		// Get angle to star (0, 0)
		float angle = angleInStar;

		rotationN = (angle * 180) / PI + 180;

		nightShade.setRotation(rotationN);
		crust.setRotation(rotation);
		land.setRotation(rotation);

		atmoSpr.setRotation(rotationN);


		// Draw all buildings
		for (int i = 0; i < size; i++)
		{
			int btype = tiles[i];
			if (btype != 0)
			{
				sf::Sprite b;
				if (btype == BUILDING_APPARTMENT)
					b = sf::Sprite(buildings["apps"]);
				if (btype == BUILDING_HOUSE)
					b = sf::Sprite(buildings["house"]);
				if (btype == BUILDING_LABORATORY)
					b = sf::Sprite(buildings["lab"]);
				if (btype == BUILDING_FARM)
					b = sf::Sprite(buildings["farm"]);
				if (btype == BUILDING_MINE)
					b = sf::Sprite(buildings["mine"]);
				if (btype == BUILDING_LAUNCHER)
					b = sf::Sprite(buildings["launcher"]);
				if (btype == BUILDING_MARKET)
					b = sf::Sprite(buildings["market"]);
				if (btype == BUILDING_SMARKET)
					b = sf::Sprite(buildings["smarket"]);
				b.setOrigin(16, 64);
				b.setPosition(getSectorPosition(i) + worldPosition);
				b.setRotation(getSectorAngle(i));
				int alpha = tilesBuilding[i] * 255;
				if (alpha > 255)
				{
					alpha = 255;
				}
				if (alpha < 255)
				{
					b.setColor(sf::Color(255, 64, 64, alpha));
				}
				else
				{
					b.setColor(sf::Color(255, 255, 255, alpha));
				}
				win->draw(b);
			}
		}
		if (hasAtmosphere)
		{
			win->draw(atmoSpr);
		}
		win->draw(land);
		win->draw(crust);
		win->draw(inner);

		win->draw(nightShade);
	}

	// Draw dummy thing to stop GL bug
	land.setPosition(sf::Vector2f(-99999.0f, -99999.f));
	win->draw(land);

}