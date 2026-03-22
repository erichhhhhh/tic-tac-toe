#pragma once
#include <vector>
#include <string>

class Menu
{
private:
	std::vector<std::string> components;
	enum class MenuInput inputType;
	Menu(std::vector<std::string> components, enum class MenuInput inputType);
public:

	static Menu rawMenu(std::vector<std::string> components, enum class MenuInput inputType);
	std::string render();
	//bool configuredMenu();
};

/*class MenuItem
{
private:
	std::

};*/

enum class MenuInput 
{
	NUMERIC_OPTION,
	ALPHABETIC_OPTION,
	FREEFIELD
};