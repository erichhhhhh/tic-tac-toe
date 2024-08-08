#include "UserInteraction.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include <rang.hpp>

#include "Field.h"
#include "Player.h"
#include "FieldAnalyser.h"
#include "Config.h"

void mainMenu(bool playDisabled, Config::Config& config)
{
	bool abort = false;
	while (!abort)
	{
		if(playDisabled)
		{
			if (config.deserialize())
			{
				playDisabled = false;
			}
		}
		std::cout << "Erik Gerk pr\204sentiert:" << std::endl;

		std::string tictactoe_title =
			"   _____  _      _____           _____             \n"
			"  |_   _|(_)    |_   _|         |_   _|            \n"
			"    | |   _   ___ | |  __ _   ___ | |  ___    ___  \n"
			"    | |  | | / __|| | / _` | / __|| | / _ \\  / _ \\ \n"
			"    | |  | || (__ | || (_| || (__ | || (_) ||  __/ \n"
			"    \\_/  |_| \\___|\\_/ \\__,_| \\___|\\_/ \\___/  \\___| \n"
			"                                                   ";

		std::cout << rang::fg::green << rang::style::blink << tictactoe_title << rang::style::reset << std::endl;


		rang::style style = rang::style::reset;

		if (playDisabled)
		{
			style = rang::style::crossed;
		}

		std::cout
			<< style
			<< "1)    Spielen" << std::endl
			<< "2)    Onlinespiel" << std::endl
			<< rang::style::reset
			<< "3)    Einstellungen" << std::endl
			<< "0)    Beenden" << std::endl;

		if (playDisabled)
		{
			std::cout
				<< rang::style::bold
				<< "\nSpiel und Onlinespiel sind aufgrund fehlender Einstellungen deaktiviert."
				"\nBitte hinterlege deine Pr\204ferenzen in den Einstellungen."
				<< rang::style::reset
				<< std::endl;
		}

		int ipt = numericInput<int>();

		switch (ipt)
		{
			case 1: //Intenional fall-through
			case 2:
				if (playDisabled)
				{
					std::cout << rang::fgB::red << "Entschludigung\nDu kannst nicht spielen" << rang::style::reset << std::endl;
					pause();
				}
				else
				{
					if (ipt == 1)
					{
						if (config.areSettingsBeforeGameShown())
						{
							try
							{
								Config::Config tmpConfig = config;
								settings(tmpConfig, true);
								launchGame(tmpConfig);
							}
							catch(std::exception e){}
						}
						else
						{
							launchGame(config);
						}
					}
					else if (ipt == 2)
					{
						std::cout << "Sorry! Noch nicht implementiert" << std::endl;
						pause();
						//Online game init code here
					}
				}
				break;
			case 3:
				settings(config, false);
				break;
			case 0:
				abort = true;
				return;
				break;
			default:
				break;
		}

		clear();
	}
}

void launchGame(Config::Config& config)
{
	bool playerIsX = config.getFirstPlayer() == FieldType::PLAYER1;

	enum Config::PlayerAmount testVal = config.getPlayerAmount();
	enum Config::Difficulty difficulty = config.getDifficulty();

	if (testVal == Config::PlayerAmount::COMPUTER_ONLY)
	{
		bool player1Max = static_cast<bool>(std::rand() % 2);
		Player player1(FieldType::PLAYER1, PlayerType::COMPUTER, Symbol::X, player1Max ? MinimaxRole::MAX : MinimaxRole::MIN, !player1Max ? WinningDetection::ENABLED : WinningDetection::DISABLED);
		Player player2(FieldType::PLAYER2, PlayerType::COMPUTER, Symbol::O, !player1Max ? MinimaxRole::MAX : MinimaxRole::MIN, player1Max ? WinningDetection::ENABLED : WinningDetection::DISABLED);
	}
	else if (testVal == Config::PlayerAmount::COMPUTER_PLAYER)
	{
		/*
		This code has to be rewritten after implementation of a difficulty level
		*/
		Player player1(FieldType::PLAYER1, PlayerType::PLAYER, playerIsX == true ? Symbol::X : Symbol::O, MinimaxRole::NOPART, WinningDetection::NONE);
		Player player2(FieldType::PLAYER2, PlayerType::COMPUTER, playerIsX == false ? Symbol::X : Symbol::O, difficulty == Config::Difficulty::EASY ? MinimaxRole::MIN : MinimaxRole::MAX, difficulty == Config::Difficulty::HARD ? WinningDetection::ENABLED : WinningDetection::DISABLED);
	}
	else if (testVal == Config::PlayerAmount::PLAYER_ONLY)
	{
		Player player1(FieldType::PLAYER1, PlayerType::PLAYER, playerIsX == true ? Symbol::X : Symbol::O, MinimaxRole::NOPART, WinningDetection::NONE);
		Player player2(FieldType::PLAYER2, PlayerType::PLAYER, playerIsX == false ? Symbol::X : Symbol::O, MinimaxRole::NOPART, WinningDetection::NONE);
	}

	Field field;

	while (true)
	{
		for (int i = 1; i < 3; i++)
		{
			clear();
			std::cout << "Spieler " << Player::getPlayer((enum FieldType)i).getSymbolString() << ":" << std::endl;
			std::cout << table(field) << std::endl;

			if (Player::getPlayer(static_cast<FieldType>(i)).isComputer())
			{
				std::cout << "Der Computer muss noch \201berlegen" << std::endl;
				pause();
			}
			else
			{
				std::cout << "Welches Feld?" << std::endl;
			}

			Player::getPlayer((enum FieldType)i).play(field);

			if (isGameWon(field))
			{
				clear();
				std::cout << table(field) << std::endl;
				std::cout << printWinningMessage(field) << std::endl;
				pause();
				Player::flushPlayerlist();
				return;
			}
		}
	}
}

void settings(Config::Config& config, const bool& areTempSettings)
{
	clear();

	std::string title = areTempSettings ? "Spielparameter" : "Standardeinstellungen";
	while (true)
	{
		std::cout
			<< title
			<< ":\n"
			<< "\t1) Symbol\t\t(Spieler 1): [" << ((config.getPreferedSymbol() == Symbol::X) ? "X" : "O") << "]"
			<< "\n\t\t\t\t(Spieler 2): [" << ((config.getPreferedSymbol() != Symbol::X) ? "X" : "O") << "]\n"
			<< std::endl;

		std::cout
			<< "\t2) Spielerkonfiguration\t[" << ((config.getPlayerAmount() == Config::PlayerAmount::COMPUTER_ONLY) 
				? "Nur Computer" : (config.getPlayerAmount() == Config::PlayerAmount::COMPUTER_PLAYER) 
				? "Computer und Spieler" : "Nur Spieler") << "]\n"
			<< std::endl;

		std::cout
			<< "\t3) Symbol erzwingen\t[" << (config.isSymbolEnforced() ? "Ja" : "Nein") 
			<< "] (Symbol wird im Onlinespiel entgegen Darstellung des Gegners erzwungen)\n"
			<< std::endl;

		std::cout
			<< "\t4) Klassische\n\t   Spielerreihenfolge\t[" << ((config.getFirstPlayer() == FieldType::PLAYER1) ? "Ja" : "Nein") << "]"
			<< " (Wenn diese Option aktiviert ist, beginnt Spieler 1 das Spiel,\n\t\t\t\t\tansonsten Spieler 2)\n"
			<< std::endl;

		if (!areTempSettings)
		{
			std::cout
				<< "\t5) Parameter vor\n\t   Spielbeginn\t\t[" << (config.areSettingsBeforeGameShown() ? "Ja" : "Nein") 
				<< "] (Wenn aktiviert, werden vor jedem Spiel die Einstellungen gezeigt)\n"
				<< std::endl;
		}

		std::cout
			<< "\t6) Schwierigkeitsgrad\t[" << ((config.getDifficulty() == Config::Difficulty::EASY) 
				? "Einfach" : (config.getDifficulty() == Config::Difficulty::MIDDLE) 
				? "Mittel" : "Schwer") << "]\n"
			<< std::endl;

		if (areTempSettings)
		{
			std::cout
				<< "\t9) Spiel abbrechen\n"
				<< std::endl;
			std::cout
				<< "\t0) Zum Spiel"
				<< std::endl;
		}
		else
		{
			std::cout
				<< "\t0) Einstellungen speichern"
				<< std::endl;
		}

		int input = numericInput<int>();

		switch (input)
		{
		case 1:
			config.setPreferedSymbol((config.getPreferedSymbol() == Symbol::X) ? Symbol::O : Symbol::X);
			break;
		case 2:
			config.setPlayerAmount((config.getPlayerAmount() == Config::PlayerAmount::COMPUTER_ONLY) 
				? Config::PlayerAmount::COMPUTER_PLAYER : (config.getPlayerAmount() == Config::PlayerAmount::COMPUTER_PLAYER) 
				? Config::PlayerAmount::PLAYER_ONLY : (config.getPlayerAmount() == Config::PlayerAmount::PLAYER_ONLY) 
				? Config::PlayerAmount::COMPUTER_ONLY : Config::PlayerAmount::COMPUTER_ONLY);
			break;
		case 3:
			config.setIfSymbolEnforced(!config.isSymbolEnforced());
			break;
		case 4:
			config.setFirstPlayer((config.getFirstPlayer() == FieldType::PLAYER1) ? FieldType::PLAYER2 : FieldType::PLAYER1);
			break;
		case 5:
			if (!areTempSettings)
			{
				config.setIfSettingsAreShownBeforeGame(!config.areSettingsBeforeGameShown());
				break;
			}
			else
			{
				break;
			}
		case 6:
			config.setDifficulty((config.getDifficulty() == Config::Difficulty::EASY) 
				? Config::Difficulty::MIDDLE : (config.getDifficulty() == Config::Difficulty::MIDDLE) 
				? Config::Difficulty::HARD : (config.getDifficulty() == Config::Difficulty::HARD) 
				? Config::Difficulty::EASY : Config::Difficulty::HARD);
			break;
		case 9:
			if (areTempSettings)
			{
				throw std::exception("Scheiß Wichser hat den Schwanz eingezogen und will nicht spielen");
				break;
			}
			else
			{
				break;
			}
		case 0:
			if (!areTempSettings)
			{
				config.serialize();
			}
			return;
			break;
		default:
			break;
		}

		clear();
	}
}

std::string table(Field& field)
{
	std::vector<std::string> xandosString;
	std::string output;
	for (int i = 0; i < 9; i++)
	{
		if (field.getPlayAt(i) == FieldType::EMPTY)
		{
			xandosString.push_back(std::to_string(keymap.at(i)));
		}
		else if (field.getPlayAt(i) == FieldType::PLAYER1)
		{
			xandosString.push_back(Player::getPlayer(FieldType::PLAYER1).getSymbolString());
		}
		else if (field.getPlayAt(i) == FieldType::PLAYER2)
		{
			xandosString.push_back(Player::getPlayer(FieldType::PLAYER2).getSymbolString());
		}
		else
		{
			xandosString.push_back("e");
		}
	}

	std::stringstream ostream;
	auto backup = std::cout.rdbuf();
	auto sstreamBuf = ostream.rdbuf();

	std::cout.rdbuf(sstreamBuf);

	std::cout <<	"\u00DA\u00C4\u00C4\u00C4"
					"\u00C2\u00C4\u00C4\u00C4"
					"\u00C2\u00C4\u00C4\u00C4\u00BF\n";

	for (int i = 0; i < xandosString.size(); i++)
	{
		auto fg = rang::fg::reset;
		if (xandosString.at(i) == "X")
			fg = rang::fg::cyan;
		else if (xandosString.at(i) == "O")
			fg =  rang::fg::yellow;

		if (i % 3 == 2 && i != 0)
		{
			std::cout << " " << fg << xandosString.at(i) << rang::fg::reset << " \u00B3";
			if (i != 8)
			{
				std::cout <<	"\n\u00C3\u00C4\u00C4\u00C4"
								"\u00C5\u00C4\u00C4\u00C4"
								"\u00C5\u00C4\u00C4\u00C4\u00B4\n";
			}
		}
		else if (i % 3 == 0)
		{
			std::cout << "\u00B3 " << fg << xandosString.at(i) << rang::fg::reset << " \u00B3";
		}
		else
		{
			std::cout << " " << fg << xandosString.at(i) << rang::fg::reset << " \u00B3";
		}
	}

	std::cout <<	"\n\u00C0\u00C4\u00C4\u00C4"
					"\u00C1\u00C4\u00C4\u00C4"
					"\u00C1\u00C4\u00C4\u00C4\u00D9\n";

	std::cout.rdbuf(backup);

	return ostream.str();
}

void pause()
{
	std::cout << "Mit Eingabe (Enter) fortsetzen ..." << std::endl;
	std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
}

void clear()
{
	std::cout << "\x1b[2J\x1b[H" << std::flush;
}

std::string getInput()
{
	std::string input;
	std::getline(std::cin, input);
	return input;
}