/* * * * * *MenuManager.cpp * * * * * */

#include "MenuManager.h"

MenuManager::MenuManager()
	:menuState(MainMenu), currentMenuItem(0)
{

}

MenuManager::~MenuManager()
{
}

int MenuManager::GetMenuState() const
{
	return menuState;
}

void MenuManager::SetMenuState(MenuState state)
{
	menuState = state;
}

void MenuManager::Update()
{
	switch (menuState)
	{
	case 0: //menuState == TitleScreen
		break;
	case 1: //menuState == Playing
		break;
	}
}

void MenuManager::CreateMenuItems()
{
	MenuItem* itemOne = new MenuItem("Begin Game", PolarisEngine::Vector3(45, 50, 0));
	MenuItem* itemTwo = new MenuItem("Exit Game", PolarisEngine::Vector3(45, 120, 0));

	menuItems[0] = itemOne;
	menuItems[1] = itemTwo;
}