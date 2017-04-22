#include <iostream>
#include <SFML/Graphics.hpp>
#include "ImGui/imgui.h"
#include "ImGui/imgui-SFML.h"

#include "Elements/PlanetElement.h"

void resize(int width, int height, sf::Rect<float> &empireWindow)
{
	empireWindow = sf::Rect<float>(sf::Vector2f(0, (float)height -
		((float)height / 8.0f)),
		sf::Vector2f((float)width, (float)height / 8.0f));
}

int main()
{
	float backdropScale = 0.0f;

	sf::Texture shade = sf::Texture();
	sf::Texture rock = sf::Texture();
	sf::Texture night = sf::Texture();
	sf::Texture space = sf::Texture();
	sf::Texture atmo = sf::Texture();

	shade.loadFromFile("Resource/shade.png");
	rock.loadFromFile("Resource/rock.png");
	night.loadFromFile("Resource/nightshadeflat.png");
	atmo.loadFromFile("Resource/atmosphere.png");
	space.loadFromFile("Resource/space.png");
	space.setRepeated(true);

	sf::Rect<float> empireWindow;



	sf::Sprite backdrop = sf::Sprite(space);

	sf::Vector2f startSize = sf::Vector2f(512, 512);

	backdropScale = std::max(startSize.x / 512, startSize.y / 512);

	sf::RenderWindow window(sf::VideoMode(startSize.x, startSize.y), "SFML works!");

	resize((int)startSize.x, (int)startSize.y, empireWindow);

	sf::View renderView = sf::View(sf::FloatRect(0, 0, startSize.x, startSize.y));

	window.setView(renderView);

	ImGui::SFML::Init(window);



	sf::Clock dtClock;
	float dt = 0.0f;
	sf::Time dtt;

	// Can be removed safely (Used for ImGui, but not requried at all)
	//window.resetGLStates();


	Planet p = Planet(8, shade, rock, night, atmo);
	p.hasAtmosphere = true;
	p.atmosphereColor = sf::Color(255, 0, 0, 128);
	p.surfaceColor = sf::Color(0, 255, 0);
	p.crustColor = sf::Color(0, 0, 128);
	p.worldPosition = sf::Vector2f(128, 128);
	p.makeDrawables();
	sf::Vector2f mousePos;

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
				backdrop.setTextureRect(sf::IntRect(0, 0, event.size.width, 
					event.size.height));

				resize((int)event.size.width, (int)event.size.height, empireWindow);
				printf("Now scale %f\n", backdropScale);
			}
		}

		mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);

		dtt = dtClock.restart();
		dt = dtt.asSeconds();

		ImGui::SFML::Update(window, dtt);

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

		mousePos -= p.worldPosition;

		p.update(dt, false);

		window.clear();
		window.draw(backdrop);


		p.drawSector(p.getClosestSector(mousePos), &window, p.worldPosition);
		p.draw(&window, sf::Vector2f(0, 0));
		printf("Closest: %i\n", p.getClosestSector(mousePos));

		ImGui::Render();
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}

