#include <iostream>
#include <SFML/Graphics.hpp>
#include "ImGui/imgui.h"
#include "ImGui/imgui-SFML.h"

#include "Elements/PlanetElement.h"
#include "Elements/UniverseElement.h"
#include "Elements/EmpireElement.h"

void resize(int width, int height, sf::Rect<float> &empireWindow, sf::Rect<float> &systemWindow)
{
	empireWindow = sf::Rect<float>(sf::Vector2f(0, (float)height -
		((float)height / 8.0f)),
		sf::Vector2f((float)width, (float)height / 7.0f));

	systemWindow = sf::Rect<float>(sf::Vector2f(0, 0), 
		sf::Vector2f((float)width, 85));
}

int main()
{
	float backdropScale = 0.0f;

	std::map<std::string, sf::Texture> buildings;

	sf::Texture shade = sf::Texture();
	sf::Texture rock = sf::Texture();
	sf::Texture night = sf::Texture();
	sf::Texture space = sf::Texture();
	sf::Texture atmo = sf::Texture();
	sf::Texture star = sf::Texture();
	sf::Texture appartment = sf::Texture();
	sf::Texture farm = sf::Texture();
	sf::Texture house = sf::Texture();
	sf::Texture lab = sf::Texture();
	sf::Texture magic = sf::Texture();

	shade.loadFromFile("Resource/shade.png");
	rock.loadFromFile("Resource/rock.png");
	night.loadFromFile("Resource/nightshadeflat.png");
	atmo.loadFromFile("Resource/atmosphere.png");
	star.loadFromFile("Resource/star.png");
	space.loadFromFile("Resource/space.png");
	magic.loadFromFile("Resource/magic.png");
	appartment.loadFromFile("Resource/building/appartment.png");
	farm.loadFromFile("Resource/building/farm.png");
	house.loadFromFile("Resource/building/house.png");
	lab.loadFromFile("Resource/building/lab.png");
	space.setRepeated(true);

	buildings["apps"] = appartment;
	buildings["farm"] = farm;
	buildings["house"] = house;
	buildings["lab"] = lab;

	sf::Rect<float> empireWindow;
	sf::Rect<float> systemWindow;


	sf::Sprite starSpr = sf::Sprite(star);
	sf::Sprite backdrop = sf::Sprite(space);

	sf::Vector2f startSize = sf::Vector2f(1024, 768);

	backdropScale = std::max(startSize.x / 512, startSize.y / 512);

	sf::RenderWindow window(sf::VideoMode(startSize.x, startSize.y), "SFML works!");

	resize((int)startSize.x, (int)startSize.y, empireWindow, systemWindow);
	
	sf::View renderView = sf::View(sf::FloatRect(0, 0, startSize.x, startSize.y));

	window.setView(renderView);

	ImGui::SFML::Init(window);



	sf::Clock dtClock;
	float dt = 0.0f;
	sf::Time dtt;

	// Can be removed safely (Used for ImGui, but not requried at all)
	//window.resetGLStates();


	Planet p = Planet(8, shade, rock, night, atmo, buildings);
	p.hasAtmosphere = true;
	p.atmosphereColor = sf::Color(195, 244, 255, 128);
	p.surfaceColor = sf::Color(170, 213, 119);
	p.crustColor = sf::Color(219, 165, 117);
	p.worldPosition = sf::Vector2f(128, 320);
	p.makeDrawables();
	sf::Vector2f mousePos;

	Universe universe = Universe(star, shade, rock, night, atmo, buildings, time(NULL));
	universe.generate();

	starSpr.setPosition(sf::Vector2f(32, 32));
	starSpr.setColor(sf::Color(252, 255, 158));

	float viewZoom = 1.0f;

	float timeSpeed = 1.0f;


	int day = 1;
	int month = 1;
	int year = 2100;
	
	float timeStepper = 0.0f;

	Planet* focused = NULL;

	float frameAverage = 0.0f;

	Empire empire0 = Empire();

	EmpirePlayer empirePlayer = EmpirePlayer(&universe);

	empirePlayer.resize(startSize.x, startSize.y);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::Resized)
			{
				renderView.reset(sf::FloatRect(0, 0, event.size.width, event.size.height));
				window.setView(renderView);
				if ((float)event.size.width / (float)512 > (float)event.size.height / (float)512)
				{
					backdropScale = (float)event.size.width / (float)512;
				}
				else
				{
					backdropScale = event.size.height / 512;
				}
				

				resize((int)event.size.width, (int)event.size.height, empireWindow, systemWindow);
				empirePlayer.resize((int)event.size.width, (int)event.size.height);
				printf("Now scale %f\n", backdropScale);
			}
		}


		dtt = dtClock.restart();
		dt = dtt.asSeconds();


		ImGui::SFML::Update(window, dtt);

		frameAverage += dt;
		frameAverage *= 0.5f;

		
		empirePlayer.update(&renderView, dt);


	/*	backdrop.setTextureRect(sf::IntRect(
			renderView.getCenter().x - (renderView.getSize().x/2), 
			renderView.getCenter().y - (renderView.getSize().y/2), 
			renderView.getCenter().x + (renderView.getSize().x / 2),
			renderView.getCenter().y + (renderView.getSize().y / 2)));
	
	*/

		backdrop.setTextureRect(sf::IntRect(
			renderView.getCenter().x - (renderView.getSize().x / 2),
			renderView.getCenter().y - (renderView.getSize().y / 2),
			renderView.getSize().x,
			renderView.getSize().y
			));

		backdrop.setPosition(
			renderView.getCenter().x - renderView.getSize().x / 2,
			renderView.getCenter().y - renderView.getSize().y / 2);

		window.setView(renderView);

		mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);


		timeStepper += dt * timeSpeed;
		if (timeStepper >= 0.5f)
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


		

		

		mousePos -= p.worldPosition;
		universe.update(dt * timeSpeed);
		//p.update(dt, false);

		window.clear();
		window.draw(backdrop);
		//window.draw(starSpr);

		/*p.drawSector(p.getClosestSector(mousePos), &window, p.worldPosition);
		p.draw(&window, sf::Vector2f(0, 0));*/

		universe.draw(&window);

		ImGui::Render();
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}
