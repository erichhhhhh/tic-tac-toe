#include "Menu.h"
#include "Config.h"

Menu::Menu(std::vector<std::string> components, MenuInput inputType)
{
	this->components = components;
	this->inputType = inputType;
}

Menu Menu::rawMenu(std::vector<std::string> components, MenuInput inputType)
{
	return Menu(components, inputType);
}

std::string Menu::render()
{
	std::string cumulatedOutput;

	for (std::string component : this->components)
	{
		cumulatedOutput.append(Config::Language::getTranslation(component) + "\n");
	}

	return cumulatedOutput;
}
