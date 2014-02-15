/* * * * * * *Menu Manager.h * * * * * */
/* Class definitions for menu manager  *
 * handles game before gameplay and    *
 * after splash						   *
 * * * * * * * * * * * * * * * * * * * */
#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include <string>
#include <vector>
#include "core.h"

enum MenuState
{
	TitleScreen,
	MainMenu
};

struct MenuItem
{
public:
	std::string text;
	PolarisEngine::Vector3 position;
	bool selected;

	MenuItem(std::string displayText, PolarisEngine::Vector3 pos)
	{
		text = displayText;
		position = pos;
	}
};

class MenuManager
{
public:
	MenuManager();
	~MenuManager();

	int GetMenuState() const;
	void SetMenuState(MenuState state);

	int currentMenuItem;
	MenuItem* menuItems[2];

	void Update();
	void CreateMenuItems();

private:
	MenuState menuState;

};

#endif