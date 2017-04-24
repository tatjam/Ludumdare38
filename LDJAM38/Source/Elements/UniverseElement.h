#pragma once

#include "PlanetElement.h"


#define MIN_PLANETS 8
#define MAX_PLANETS 20
#define MAX_POS 10000
#define MIN_DISTANCE_TO_STAR 1000

#define MAX_PLANET_SIZE 50
#define MIN_PLANET_SIZE 20

#define GRASS_BASE_COLOR sf::Color(169, 232, 118)
#define DUST_BASE_COLOR sf::Color(198, 198, 198)

#define EARTHSKY_BASE_COLOR sf::Color(195, 253, 255)
#define ALIENSKY_BASE_COLOR sf::Color(145, 217, 158)

#define ROCK_BASE_COLOR sf::Color(219, 183, 141)
#define REGOLITH_BASE_COLOR sf::Color(140, 140, 140)

#define BASE_STAR_COLOR sf::Color(255, 255, 145)

#define G 100



// Everything happens in a single solar system
// so shoudl be named "SolarSystem"
// Star is at {0, 0}
class Universe
{
public:

	std::string empireNames[12]
	{
		"Kerbal Space Program",
		"Mr.Robot Enterprise",
		"Guamedo's Electric Company",
		"TinyPlanet Corps",
		"Jupiter Exploration",
		"ComCast Communications",
		"Space-Explorer 9",
		"Mann's Enterprise",
		"Kepler's Vision",
		"Xin-Yang Mining Corps",
		"Nanotrasen",
		"Syndicate",

	};

	std::vector<std::string> usedAINames;

	std::string nameList[64]
	{
		"Awhuinus",
		"Xuploutania",
		"Xasparth",
		"Faskion",
		"Piowei",   //5
		"Feayama",
		"Clalanov",
		"Claxalara",
		"Spilles DQ",
		"Crides DIS6", //10
		"Eranin",
		"Hoskuaphus",
		"Asnoybos",
		"Uchinda",
		"Xubrion", //15
		"Oulia",
		"Gouliv",
		"Croxoyama",
		"Swesagawa",
		"Flion 5EC", //20
		"Sliuq NZ8Y",
		"Jucreyama",
		"Ugluaruta",
		"Waclides",
		"Fostypso", //25
		"Vugawa",
		"Xeynerth",
		"Flogonides",
		"Flehanus",
		"Strao 0NRN", //30
		"Shomia FGR3",
		"Snadalea", // 32 - All planet with "habitable" names
		"Aguilar",
		"Ahrensa",
		"Barnardiana", // 35
		"Belton",
		"Berounka",
		"Bohuska",
		"Cuffey",
		"Denisyuk", // 40
		"Entsuji",
		"Ethiopia",
		"Euler",
		"Euryanthe",
		"Excalibur", // 45
		"Fechner",
		"Fienga",
		"Fraknoi",
		"Goodricke",
		"Hagihara", // 50
		"Jewitt",
		"Jia-xiang",
		"Jugurtha",
		"Karma",
		"Lambert", //55
		"Lew Allen",
		"Pythagoras",
		"Heron",
		"Descartes",
		"Gauss", //60
		"Tesla",
		"Edison",
		"Turing", // 63
		"Tatjamus"
	};

	sf::Texture star; sf::Texture shade;
	sf::Texture rock; sf::Texture night; 
	sf::Texture atmo;

	std::map<std::string, sf::Texture> buildings;

	sf::Sprite starSpr;

	int seed, count, habitables;

	std::vector<Planet*> planets;

	void generate()
	{

		std::vector<std::string> usedNames;

		starSpr = sf::Sprite(this->star);
		starSpr.setPosition(-128 * 4, -128 * 4);
		starSpr.setScale(4, 4);
		starSpr.setColor(BASE_STAR_COLOR);

		habitables = 0;
		srand(seed);
		count = rand() % (MAX_PLANETS - MIN_PLANETS) + MIN_PLANETS;

		printf("Generating %i planets: \n", count);

		for (int i = 0; i < count; i++)
		{
			int size;
			size = rand() % (MAX_PLANET_SIZE - MIN_PLANET_SIZE) + MIN_PLANET_SIZE;
			if (size % 2 != 0)
			{
				size += 1;
			}
			Planet* p = new Planet(size, shade, rock, night, atmo, buildings);
			sf::Vector2f pos;
			bool valid = false;
			while (!valid)
			{
				pos.x = rand() % MAX_POS * 2 - MAX_POS;
				pos.y = rand() % MAX_POS * 2 - MAX_POS;
				
				printf("POS: {%f, %f}\n", pos.x, pos.y);

				// Get distance to star:
				if (sqrtf(powf(pos.x - 0, 2) + powf(pos.y - 0, 2)) > MIN_DISTANCE_TO_STAR)
				{
					valid = true;
				}
			}
			p->worldPosition = pos;
			
			p->distanceToStar = sqrtf(powf(pos.x - 0, 2) + powf(pos.y - 0, 2));
			p->angleInStar = rand() % 360;
			// Generate stuff

			// Habitable 25% chance
			p->easyHabitable = rand() < RAND_MAX / 4;

			if (p->easyHabitable)
			{
				valid = false;
				while (!valid)
				{
					p->name = nameList[rand() % 32];
					if (std::find(usedNames.begin(), usedNames.end(), p->name) == usedNames.end())
					{
						valid = true;
						usedNames.push_back(p->name);
					}
				}
				habitables++;
				// Earth like planet
				p->hasAtmosphere = true;
				p->surfaceColor = GRASS_BASE_COLOR;
				p->atmosphereColor = EARTHSKY_BASE_COLOR;
				p->crustColor = ROCK_BASE_COLOR;

				p->foodBoost += (float)(rand() % 50) / 70;
				p->mineralBoost -= (float)(rand() % 50) / 70;
			}
			else
			{
				valid = false;
				while (!valid)
				{
					p->name = nameList[rand() % (64 - 32) + 32];
					if (std::find(usedNames.begin(), usedNames.end(), p->name) == usedNames.end())
					{
						valid = true;
						usedNames.push_back(p->name);
					}
				}

				p->foodBoost -= (float)(rand() % 50) / 70;
				p->mineralBoost += (float)(rand() % 50) / 50;

				// Non-earth like planet
				p->hasAtmosphere = rand() < RAND_MAX / 8;
				p->surfaceColor = DUST_BASE_COLOR;
				if (rand() < RAND_MAX / 8)
				{
					p->atmosphereColor = EARTHSKY_BASE_COLOR;
				}
				else
				{
					p->atmosphereColor = ALIENSKY_BASE_COLOR;
				}

				if (rand() < RAND_MAX / 8)
				{
					p->crustColor = ROCK_BASE_COLOR;
				}
				else
				{
					p->crustColor = REGOLITH_BASE_COLOR;
				}
			}

			/*for (int i = 0; i < p->size; i++)
			{
				p->tiles[i] = rand() % 5;
			}*/

			printf("Generated planet (%i) %s: \n", i, p->name.c_str());
			printf("Habitable: %i \n", p->easyHabitable);
			printf("Has Atmosphere: %i\n", p->hasAtmosphere);
			printf("Size: %i\n", p->size);


			p->makeDrawables();
			planets.push_back(p);
		}

		printf("Total habitables: %i\n", habitables);
	}

	void draw(sf::RenderWindow* win)
	{
		win->draw(starSpr);
		for (int i = 0; i < count; i++)
		{
			planets[i]->draw(win, sf::Vector2f(0, 0));
		}
	}

	void update(float dt)
	{
		// Rotate planets around star
		for (int i = 0; i < count; i++)
		{
	
			planets[i]->angleInStar += (G / planets[i]->distanceToStar) * dt;
			

			planets[i]->worldPosition.x = 
				cos(planets[i]->angleInStar) * planets[i]->distanceToStar;

			planets[i]->worldPosition.y =
				sin(planets[i]->angleInStar) * planets[i]->distanceToStar;

			planets[i]->update(dt, false);
		}
	}

	Universe(sf::Texture star, sf::Texture shade, 
		sf::Texture rock, sf::Texture night, sf::Texture atmo, 
		std::map<std::string, sf::Texture> buildings, int seed)
	{
		this->star = star;
		this->shade = shade;
		this->rock = rock;
		this->night = night;
		this->atmo = atmo;
		this->seed = seed;
		this->buildings = buildings;

	}

	Universe()
	{

	}

	~Universe()
	{

	}
};