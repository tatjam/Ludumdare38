#include <iostream>
#include <SFML/Graphics.hpp>
#include "ImGui/imgui.h"
#include "ImGui/imgui-SFML.h"

#include "Elements/PlanetElement.h"
#include "Elements/UniverseElement.h"
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
	sf::Texture magic = sf::Texture();

	shade.loadFromFile("Resource/shade.png");
	rock.loadFromFile("Resource/rock.png");
	night.loadFromFile("Resource/nightshadeflat.png");
	atmo.loadFromFile("Resource/atmosphere.png");
	star.loadFromFile("Resource/star.png");
	space.loadFromFile("Resource/space.png");
	magic.loadFromFile("Resource/magic.png");
	appartment.loadFromFile("Resource/building/appartment.png");
	space.setRepeated(true);

	buildings["apps"] = appartment;

	sf::Rect<float> empireWindow;
	sf::Rect<float> systemWindow;


	sf::Sprite starSpr = sf::Sprite(star);
	sf::Sprite backdrop = sf::Sprite(space);

	sf::Vector2f startSize = sf::Vector2f(512, 512);

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
				printf("Now scale %f\n", backdropScale);
			}
		}

		if (focused != NULL)
		{
			renderView.setCenter(focused->worldPosition + sf::Vector2f(focused->radius, focused->radius));
			printf("Focused on: %s", focused->name.c_str());
		}
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			if(focused == NULL)
				renderView.move(sf::Vector2f(0, -1000.0f) * dt);
			focused = NULL;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			if (focused == NULL)
				renderView.move(sf::Vector2f(-1000.0, 0.0f) * dt);
			focused = NULL;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			if (focused == NULL)
				renderView.move(sf::Vector2f(0, 1000.0f) * dt);
			focused = NULL;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			if (focused == NULL)
				renderView.move(sf::Vector2f(1000.0f, 0.0f) * dt);
			focused = NULL;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
		{
			renderView.zoom(1.002f);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
		{
			renderView.zoom(0.998f);
		}

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

		dtt = dtClock.restart();
		dt = dtt.asSeconds();

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

		ImGui::SFML::Update(window, dtt * timeSpeed);

		ImGui::SetNextWindowPos(ImVec2(empireWindow.left, empireWindow.top), 
			ImGuiSetCond_Always);
		ImGui::SetNextWindowSize(ImVec2(empireWindow.width, empireWindow.height), 
			ImGuiSetCond_Always);
		ImGui::Begin("Empire", NULL, 
			ImGuiWindowFlags_NoTitleBar 
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove);
		ImGui::Button("Lol");
		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(systemWindow.left, systemWindow.top));
		ImGui::SetNextWindowSize(ImVec2(systemWindow.width, systemWindow.height));
		ImGui::Begin("System", NULL, ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_HorizontalScrollbar);

		ImGui::BeginChild("SubSystemData", ImVec2(systemWindow.width * 0.7f, 50), 
			true, ImGuiWindowFlags_HorizontalScrollbar);

		for (int i = 0; i < universe.count; i++)
		{
			if (i % 2 == 0)
			{
				if (ImGui::Button(universe.planets[i]->name.c_str()))
				{
					focused = universe.planets[i];
				}
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.4f, 0.2f, 1.0f));
				ImGui::Button(universe.planets[i]->name.c_str());
				ImGui::PopStyleColor(1);
			}
			if (i < universe.count - 1)
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
		else if (timeSpeed == 4)
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
				timeSpeed = 4;
			}
		}
		else if (timeSpeed == 4)
		{
			if (ImGui::Button("-|"))
			{
				timeSpeed = 4;
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
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "123456");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(255, 255, 0, 1.0f), "$");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "| 123456");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8f, 0.7f, 0.1f, 1.0f), "F");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "| 123456");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.9f, 1.0f), "T");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "| 123456");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "M");
		//	ImGui::Image(magic, sf::Vector2f(16, 16), sf::FloatRect(0, 0, 16, 16));
		ImGui::End();

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

