#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
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

	sf::Texture cross = sf::Texture();
	sf::Texture gear = sf::Texture();
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
	sf::Texture launcher = sf::Texture();
	sf::Texture mine = sf::Texture();
	sf::Texture market = sf::Texture();
	sf::Texture logo = sf::Texture();
	sf::Texture smarket = sf::Texture();
	sf::Texture hydrofarm = sf::Texture();

	cross.loadFromFile("Resource/cross.png");
	gear.loadFromFile("Resource/gear.png");
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
	mine.loadFromFile("Resource/building/mine.png");
	launcher.loadFromFile("Resource/building/launcher.png");
	market.loadFromFile("Resource/building/market.png");
	smarket.loadFromFile("Resource/building/smarket.png");
	logo.loadFromFile("Resource/logo.png");
	hydrofarm.loadFromFile("Resource/building/hydrophonic.png");
	space.setRepeated(true);

	buildings["apps"] = appartment;
	buildings["farm"] = farm;
	buildings["house"] = house;
	buildings["lab"] = lab;
	buildings["mine"] = mine;
	buildings["launcher"] = launcher;
	buildings["market"] = market;
	buildings["smarket"] = smarket;
	buildings["hydrofarm"] = hydrofarm;

	sf::Rect<float> empireWindow;
	sf::Rect<float> systemWindow;


	sf::Sprite logoSpr = sf::Sprite(logo);
	sf::Sprite starSpr = sf::Sprite(star);
	sf::Sprite backdrop = sf::Sprite(space);

	sf::Vector2f startSize = sf::Vector2f(1024, 768);

	backdropScale = std::max(startSize.x / 512, startSize.y / 512);

	sf::RenderWindow window(sf::VideoMode(startSize.x, startSize.y), "TinyPlanets");

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

	EmpirePlayer empirePlayer = EmpirePlayer(&universe);
	

	EmpireManager manager = EmpireManager(&universe);
	manager.player = &empirePlayer;
	manager.createPlayerEmpire();

	empirePlayer.resize(startSize.x, startSize.y);

	empirePlayer.gear = gear;
	empirePlayer.cross = cross;

	sf::SoundBuffer menuBuffer;
	sf::Sound menuMusic;

	sf::SoundBuffer editBuffer;
	sf::SoundBuffer playBuffer;

	sf::SoundBuffer errorBuffer;

	errorBuffer.loadFromFile("Resource/music/error.wav");

	empirePlayer.errorP = sf::Sound(errorBuffer);

	//sf::Sound editMusic;

	menuBuffer.loadFromFile("Resource/music/menumusic.ogg");
	menuMusic.setBuffer(menuBuffer);

	editBuffer.loadFromFile("Resource/music/editmusic.ogg");
	playBuffer.loadFromFile("Resource/music/playmusic.ogg");
	/*editMusic.setBuffer(editBuffer);

	menuMusic.setLoop(true);
	editMusic.setLoop(true);
	*/

	menuMusic.setVolume(0.25f);
	menuMusic.setLoop(true);
	menuMusic.play();



	bool hasStarted = false;

	if(empirePlayer.limitFPS)
		window.setFramerateLimit(60);

	bool lose = false;

	while (window.isOpen())
	{
		sf::Event event;

		if (empirePlayer.lose)
		{
			while (window.pollEvent(event))
			{
				ImGui::SFML::ProcessEvent(event);

				if (event.type == sf::Event::Closed)
				{
					window.close();
				}
			}

			dtt = dtClock.restart();
			dt = dtt.asSeconds();

			sf::RectangleShape rect = sf::RectangleShape(sf::Vector2f(0, 0));


			ImGui::SFML::Update(window, dtt);

			ImGui::SetNextWindowPos(ImVec2(20, 20));
			ImGui::SetNextWindowSize(ImVec2(400, 400));
			ImGui::Begin("You lost!");
			ImGui::TextWrapped("Sadly due to lack of time a restart feature is not available!");
			ImGui::TextWrapped("You probably got killed by AI, make sure you have a big enough army before attacking.");
			if (ImGui::Button("Quit"))
			{
				window.close();
			}
			ImGui::End();

			window.clear();
			window.draw(rect);
			ImGui::Render();
			window.display();
		}
		else
		{
			if (empirePlayer.updateWindow)
			{
				renderView.reset(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
				window.setView(renderView);
				if (window.getSize().x / (float)512 > window.getSize().y / (float)512)
				{
					backdropScale = window.getSize().x / (float)512;
				}
				else
				{
					backdropScale = window.getSize().y / 512;
				}


				//resize((int)event.size.width, (int)event.size.height, empireWindow, systemWindow);
				empirePlayer.resize(window.getSize().x, window.getSize().y);

			}

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

			menuMusic.setVolume(empirePlayer.musicVolume * 100);
			empirePlayer.errorP.setVolume(empirePlayer.fxVolume * 100);

			if (empirePlayer.limitFPS)
			{
				window.setFramerateLimit(60);
			}
			else
			{
				window.setFramerateLimit(-1);
			}

			dtt = dtClock.restart();
			dt = dtt.asSeconds();


			ImGui::SFML::Update(window, dtt);

			frameAverage += dt;
			frameAverage *= 0.5f;


			window.setView(renderView);

			mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);

			manager.update(&renderView, dt, mousePos, &window);

			/*	backdrop.setTextureRect(sf::IntRect(
					renderView.getCenter().x - (renderView.getSize().x/2),
					renderView.getCenter().y - (renderView.getSize().y/2),
					renderView.getCenter().x + (renderView.getSize().x / 2),
					renderView.getCenter().y + (renderView.getSize().y / 2)));

			*/

			{


				backdrop.setTextureRect(sf::IntRect(
					renderView.getCenter().x - (renderView.getSize().x / 2),
					renderView.getCenter().y - (renderView.getSize().y / 2),
					renderView.getSize().x,
					renderView.getSize().y
					));

				backdrop.setPosition(
					renderView.getCenter().x - renderView.getSize().x / 2,
					renderView.getCenter().y - renderView.getSize().y / 2);
			}

			if (manager.player->state == PlayerState::EDIT_MODE)
			{
				if (menuMusic.getBuffer() != &editBuffer)
				{
					menuMusic.setBuffer(editBuffer);
					menuMusic.setPlayingOffset(sf::seconds(0));
					menuMusic.play();
				}
			}
			else if (manager.player->state == PlayerState::VIEW_MODE)
			{
				if (menuMusic.getBuffer() != &playBuffer)
				{
					menuMusic.setBuffer(playBuffer);
					menuMusic.setPlayingOffset(sf::seconds(0));
					menuMusic.play();
				}
				// TODO
			}

			if (manager.player->state != PlayerState::CHOOSE_WORLD)
			{
				if (hasStarted == false)
				{
					for (int i = 0; i < manager.player->aiCount; i++)
					{
						manager.createAIEmpire();
					}
					hasStarted = true;
				}

				timeStepper += dt * empirePlayer.timeSpeed;
				if (timeStepper >= 1.0f)
				{
					timeStepper = 0.0f;
					day++;
					if (manager.player->state != PlayerState::CHOOSE_WORLD)
					{
						manager.updateDaily();
					}
					if (day > 30)
					{
						month++;
						day = 1;
						manager.updateMonthly();
					}
					if (month > 12)
					{
						month = 1;
						year++;
					}
				}
			}
			else
			{


				/*if (menuMusic.getStatus() != sf::Music::Playing)
				{
					/*menuMusic.play();
					printf("playing!");*/
					//}
			}






			mousePos -= p.worldPosition;
			universe.update(dt * empirePlayer.timeSpeed);
			//p.update(dt, false);

			window.clear(sf::Color(17, 10, 32));

			window.draw(backdrop);

			//window.draw(starSpr);

			/*p.drawSector(p.getClosestSector(mousePos), &window, p.worldPosition);
			p.draw(&window, sf::Vector2f(0, 0));*/


			if (empirePlayer.state == PlayerState::CHOOSE_WORLD)
			{
				logoSpr.setOrigin(120, 44);
				if (window.getSize().x > 1200.0f)
				{
					logoSpr.setScale(2.0f, 2.0f);
				}
				else
				{
					logoSpr.setScale(1.0f, 1.0f);
				}
				if (empirePlayer.focused == NULL)
				{

					logoSpr.setPosition(sf::Vector2f(renderView.getCenter().x, 90.0f));
				}
				else
				{
					logoSpr.setPosition(sf::Vector2f(
						empirePlayer.focused->worldPosition.x + empirePlayer.focused->radius,
						empirePlayer.focused->worldPosition.y -
						(renderView.getSize().y / 4) + (empirePlayer.focused->radius * 0.5f)));
				}

				window.draw(logoSpr);
			}

			if (empirePlayer.state != PlayerState::EDIT_MODE)
			{
				universe.draw(&window);
			}
			else
			{

				manager.render(&window);
				sf::Vector2f oldPos = empirePlayer.focused->worldPosition;
				empirePlayer.focused->worldPosition = sf::Vector2f(0.0f, 0.0f);
				empirePlayer.focused->draw(&window, sf::Vector2f(0.0f, 0.0f));
				empirePlayer.focused->worldPosition = oldPos;

			}



			ImGui::Render();
			window.display();
		}
	}

	ImGui::SFML::Shutdown();

	return 0;
}

