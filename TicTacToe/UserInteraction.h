#pragma once

#include <string>
#include <map>
#include <typeinfo>

#include "Field.h"
#include "Config.h"

inline std::map<int8_t, int8_t> keymap = { {0, 7},{1, 8},{2, 9},{3, 4},{4, 5},{5, 6},{6, 1},{7, 2},{8, 3} };

inline std::string tictactoe_title =
"   _____  _      _____           _____             \n"
"  |_   _|(_)    |_   _|         |_   _|            \n"
"    | |   _   ___ | |  __ _   ___ | |  ___    ___  \n"
"    | |  | | / __|| | / _` | / __|| | / _ \\  / _ \\ \n"
"    | |  | || (__ | || (_| || (__ | || (_) ||  __/ \n"
"    \\_/  |_| \\___|\\_/ \\__,_| \\___|\\_/ \\___/  \\___| \n"
"                                                   ";

void mainMenu(bool playDisabled, Config::Config& config);

void launchGame(Config::Config& config);

void settings(Config::Config& config, const bool& areTempSettings);


std::string table(Field& field);

void printError(std::string exception);

void pause();

void clear();


std::string getInput();

template <class T>
T numericInput()
{
	try
	{
		double ret = std::stod(getInput());
		return static_cast<T>(ret);
	}
	catch (std::exception e)
	{
		return -1;
	}
}