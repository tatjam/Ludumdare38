#pragma once
#include <map>

#include "../ImGui/imgui.h"
#include "PlanetElement.h"
#include "UniverseElement.h"
#include <SFML/Graphics.hpp>

enum Relation
{
	ALLIANCE,
	FRIENDS,
	GOOD,
	NEUTRAL,
	COMPETENCE,
	ENEMIES,
	HATRED
};

class Empire
{
public:

	int money = 0;
	int food = 0;
	int tech = 0;
	int metal = 0;

	int population = 0;
	int soldiers = 0;

	std::vector<Planet*> planets;

};


class EmpireAI
{
public:
	Empire* linked;
	std::map<Empire*, Relation> relations;

	void update(float dt)
	{
		// Complex sentient AI incoming

	}
};

enum PlayerState
{
	EDIT_MODE,
	VIEW_MODE
};



class EmpirePlayer
{
public:
	Universe* universe;
	Empire* linked;

	PlayerState state = PlayerState::VIEW_MODE;

	sf::FloatRect systemWindow;
	sf::FloatRect empireWindow;

	Planet* focused;

	float timeStepper = 0.0f;
	float timeSpeed = 1.0f;
	float frameAverage = 0.0f;
	int day = 1;
	int month = 1;
	int year = 2100;


	bool ecoWindow = false;
	bool popWindow = false;
	bool retWindow = false;

	void drawEmpireWindow()
	{
		ImGui::SetNextWindowPos(ImVec2(empireWindow.left, empireWindow.top),
			ImGuiSetCond_Always);
		ImGui::SetNextWindowSize(ImVec2(empireWindow.width, empireWindow.height),
			ImGuiSetCond_Always);
		ImGui::Begin("Empire", NULL,
			ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove);
		ImGui::BeginChild("EmpireSubFocusData", ImVec2(empireWindow.width / 4, empireWindow.height - 40), true);
		if (focused != NULL)
		{
			ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Focused on ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", focused->name.c_str());
			ImGui::SameLine();
			ImGui::Spacing();
			ImGui::SameLine();
			ImGui::Button("Edit", ImVec2(40, 16));
			ImGui::Separator();
			ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Owner: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "MR.Robot enterprise");
			ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Population: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "12312");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "| Tiles: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%i/%i", 10, focused->size);
		}
		else
		{
			ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Focused on no planet");
		}
		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::Button("Economy", ImVec2(empireWindow.height - 40, empireWindow.height - 40));
		ImGui::SameLine();
		ImGui::BeginChild("EmpireSubEconomy", ImVec2(empireWindow.height + 40, empireWindow.height - 40), true);
		static float TestData[6] = { 0.f,-4.f,3.f,-2.f,0.f,4.f };
		ImGui::PlotLines("Cash", TestData, 6);
		ImGui::PlotLines("Food", TestData, 6);
		ImGui::PlotLines("Metal", TestData, 6);
		ImGui::PlotLines("Tech", TestData, 6);
		ImGui::EndChild();
		ImGui::SameLine();
		ImGui::Button("People", ImVec2(empireWindow.height - 40, empireWindow.height - 40));
		ImGui::SameLine();
		ImGui::BeginChild("EmpireSubPopulation", ImVec2(empireWindow.height + 40, empireWindow.height - 40), true);
		ImGui::EndChild();
		ImGui::SameLine();
		ImGui::Button("Relations", ImVec2(empireWindow.height - 40, empireWindow.height - 40));
		ImGui::SameLine();
		ImGui::BeginChild("EmpireSubRelations", ImVec2(empireWindow.height + 40, empireWindow.height - 40), true);
		ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Mr.Robot: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "GOOD");
		ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Comcast: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "AT WAR");
		ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Google: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "NEUTRAL");
		ImGui::EndChild();

		ImGui::End();
	}

	void drawSystemWindow()
	{
		ImGui::SetNextWindowPos(ImVec2(systemWindow.left, systemWindow.top));
		ImGui::SetNextWindowSize(ImVec2(systemWindow.width, systemWindow.height));
		ImGui::Begin("System", NULL, ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_HorizontalScrollbar);

		ImGui::BeginChild("SubSystemData", ImVec2(systemWindow.width * 0.7f, 50),
			true, ImGuiWindowFlags_HorizontalScrollbar);

		for (int i = 0; i < universe->count; i++)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.4f, 0.2f, 1.0f));

			if (ImGui::Button(universe->planets[i]->name.c_str()))
			{
				focused = universe->planets[i];
			}

			ImGui::PopStyleColor(1);

			if (i < universe->count - 1)
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
		ImGui::TextColored(ImVec4(255, 255, 0, 1.0f), "C");
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
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "| %i", (int)(1 / frameAverage));
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "FPS");
		//	ImGui::Image(magic, sf::Vector2f(16, 16), sf::FloatRect(0, 0, 16, 16));
		ImGui::End();
	}

	void drawUI(PlayerState state)
	{
		if (state == PlayerState::VIEW_MODE)
		{
			drawEmpireWindow();
			drawSystemWindow();


		}
		else if (state == PlayerState::EDIT_MODE)
		{
			drawSystemWindow();
		}
	}

	void update(sf::View* v, float dt)
	{

		frameAverage += dt;
		frameAverage /= 2;

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

		drawUI(state);

		if (focused != NULL)
		{
			v->setCenter(focused->worldPosition + sf::Vector2f(focused->radius, focused->radius));
		}

		if (state == PlayerState::VIEW_MODE)
		{

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			{
				if (focused == NULL)
					v->move(sf::Vector2f(0, -1000.0f) * dt);
				focused = NULL;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			{
				if (focused == NULL)
					v->move(sf::Vector2f(-1000.0, 0.0f) * dt);
				focused = NULL;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			{
				if (focused == NULL)
					v->move(sf::Vector2f(0, 1000.0f) * dt);
				focused = NULL;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			{
				if (focused == NULL)
					v->move(sf::Vector2f(1000.0f, 0.0f) * dt);
				focused = NULL;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			{
				v->zoom(1.002f);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
			{
				v->zoom(0.998f);
			}
		}
	}

	void resize(int width, int height)
	{
		empireWindow = sf::Rect<float>(sf::Vector2f(0, (float)height -
			((float)height / 8.0f)),
			sf::Vector2f((float)width, (float)height / 7.0f));

		systemWindow = sf::Rect<float>(sf::Vector2f(0, 0),
			sf::Vector2f((float)width, 85));
	}

	EmpirePlayer(Universe* universe)
	{
		this->universe = universe;
	}
};
