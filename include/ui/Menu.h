#pragma once
#include <vector>
#include <string>

enum class MenuInput 
{
	NUMERIC_OPTION,
	ALPHABETIC_OPTION,
	FREEFIELD
};

class Menu
{
private:
	std::vector<std::string> components;
	MenuInput inputType;
	Menu(std::vector<std::string> components, MenuInput inputType);
public:

	static Menu rawMenu(std::vector<std::string> components, MenuInput inputType);
	std::string render();
	//bool configuredMenu();
};

/*class MenuItem
{
private:
	std::

};*/


