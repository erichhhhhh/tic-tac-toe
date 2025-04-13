#include "UserInteraction.h"

#include <iostream>
#include <format>
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
		std::cout << Config::Language::getTranslation("title.presentation") << std::endl; // Erik Gerk pr\204sentiert:

		std::cout << rang::fg::green << rang::style::blink << Config::Language::getTranslation("title.title_art") << rang::style::reset << std::endl;


		rang::style style = rang::style::reset;

		if (playDisabled)
		{
			style = rang::style::crossed;
		}

		std::cout
			<< style
			<< Config::Language::getTranslation("title.play") << std::endl // 1) Spielen
			<< Config::Language::getTranslation("title.multiplayer") << std::endl // 2) Onlinespiel
			<< rang::style::reset
			<< Config::Language::getTranslation("title.settings") << std::endl // 3) Einstellungen
			<< Config::Language::getTranslation("title.exit") << std::endl; // 0) Beenden

		if (playDisabled)
		{
			std::cout
				<< rang::style::bold
				<< Config::Language::getTranslation("title.play_disabled") //"\nSpiel und Onlinespiel sind aufgrund fehlender Einstellungen deaktiviert.\nBitte hinterlege deine Pr\204ferenzen in den Einstellungen."
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
								Config::Config tmpConfig = config;
								settings(tmpConfig, true);
								launchGame(tmpConfig);
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

	std::string title = areTempSettings ? Config::Language::getTranslation("settings.temp_settings") : Config::Language::getTranslation("settings.settings"); // Spielparameter ; Standardeinstellungen
	while (true)
	{
		std::string smbl = ((config.getPreferedSymbol() == Symbol::X) ? Config::Language::getTranslation("symbol.X") : Config::Language::getTranslation("symbol.O"));
		std::string smbl2 = ((config.getPreferedSymbol() != Symbol::X) ? Config::Language::getTranslation("symbol.X") : Config::Language::getTranslation("symbol.O"));
		std::string symbol = std::vformat(Config::Language::getTranslation("settings.symbol"),
			std::make_format_args(smbl, smbl2));

		std::cout
			<< title
			<< symbol
			<< std::endl;

		std::string plAmount = ((config.getPlayerAmount() == Config::PlayerAmount::COMPUTER_ONLY) ? Config::Language::getTranslation("settings.player_amount.computer_only") : (config.getPlayerAmount() == Config::PlayerAmount::COMPUTER_PLAYER) ? Config::Language::getTranslation("settings.player_amount.computer_and_player") : Config::Language::getTranslation("settings.player_amount.player_only"));
		std::string playerAmount = std::vformat(Config::Language::getTranslation("settings.player_amount"), std::make_format_args(plAmount));
		std::cout
			<< playerAmount
			<< std::endl;

		std::string isSymbolEnfrcd = (config.isSymbolEnforced() ? Config::Language::getTranslation("settings.yes") : Config::Language::getTranslation("settings.no"));
		std::string isSymbolEnforced = std::vformat(Config::Language::getTranslation("settings.is_symbol_enforced"),
			std::make_format_args(isSymbolEnfrcd));
		std::cout
			<< isSymbolEnforced
			<< std::endl;

		std::string fstPlayer = ((config.getFirstPlayer() == FieldType::PLAYER1) ? Config::Language::getTranslation("settings.yes") : Config::Language::getTranslation("settings.no"));
		std::string firstPlayer = std::vformat(Config::Language::getTranslation("settings.first_player"),
			std::make_format_args(fstPlayer));
		std::cout
			<< firstPlayer
			<< std::endl;

		if (!areTempSettings)
		{
			std::string sttngsBfrGm = (config.areSettingsBeforeGameShown() ? Config::Language::getTranslation("settings.yes") : Config::Language::getTranslation("settings.no"));
			std::string settingsBeforeGame = std::vformat(Config::Language::getTranslation("settings.settings_before_game"),
				std::make_format_args(sttngsBfrGm));
			std::cout
				<< settingsBeforeGame
				<< std::endl;
		}

		std::string dffclty = ((config.getDifficulty() == Config::Difficulty::EASY)
			? Config::Language::getTranslation("settings.difficulty.easy") : (config.getDifficulty() == Config::Difficulty::MIDDLE)
			? Config::Language::getTranslation("settings.difficulty.medium") : Config::Language::getTranslation("settings.difficulty.hard"));
		std::string difficulty = std::vformat(Config::Language::getTranslation("settings.difficulty"),
			std::make_format_args(dffclty));
		std::cout
			<< difficulty
			<< std::endl;

		if (areTempSettings)
		{
			std::cout
				<< Config::Language::getTranslation("settings.abort_game")
				<< std::endl;
			std::cout
				<< Config::Language::getTranslation("settings.start_game")
				<< std::endl;
		}
		else
		{
			std::cout
				<< Config::Language::getTranslation("settings.save_settings")
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
				throw std::exception("Shit happens");
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

void printError(std::string exception)
{
	clear();
	std::cout << rang::fg::red << rang::style::bold << tictactoe_title << rang::style::reset << std::endl;
	std::cout << "There was an exception whilst running the program" << std::endl << std::endl;
	std::cout << rang::style::bold << exception << rang::style::reset << std::endl;
	pause();
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