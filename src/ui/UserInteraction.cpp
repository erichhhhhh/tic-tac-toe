#include "UserInteraction.h"

#include <iostream>
#include <vector>
#include <string>
#include <format>
#include <sstream>

#include <rang.hpp>

#include "Menu.h"


std::string buildOutput(std::string str, auto&&... args)
{
	return std::vformat(str, std::make_format_args(args...));
}

void languageSettings(Config::Config& config)
{
	std::vector<Config::Language> langlist = Config::Language::getLanguageList();

#ifdef _ADHSUID
	for (size_t i = 0; i < 3; i++)
	{
		langlist.insert(langlist.end(), langlist.begin(), langlist.end());
	}
#endif
	while (true)
	{
		for (size_t i = 0; i < langlist.size(); i += 10)
		{
			clear();
			std::string titleTranslation = Config::Language::getTranslation("settings.language.title");
			size_t page = (i / 10) + 1;
			size_t allPages = ((langlist.size() / 10) + ((langlist.size() % 10 > 0) ? 1 : 0));
			std::string outputTitle = std::vformat(titleTranslation, std::make_format_args(page, allPages));
			std::cout << outputTitle << "\n" << std::endl;
			for (size_t j = 0; j < 10 && (langlist.size() - i) > j; j++)
			{
				size_t target = i * 10 + j;
				std::string targetString = std::to_string(target+1);
				std::string displayName = langlist.at(target).getDisplayName();
				std::string region = langlist.at(target).getRegion();
				std::string output = buildOutput(Config::Language::getTranslation("settings.language.entry"), targetString, displayName, region);
				if (langlist.at(target).getFilename() == config.getLanguage())
				{
					std::cout << rang::style::bold << output << rang::style::reset << std::endl;
				}
				else
				{
					std::cout << output << std::endl;
				}
			}
			std::cout << Config::Language::getTranslation("settings.language.exit") << std::endl;
			if (allPages > 1)
				std::cout << Config::Language::getTranslation("settings.language.scroll") << std::endl;
			int input = numericInput<int>();
			if (input == 0)
			{
				return;
			}
			try
			{
				config.setLanguage(langlist.at(input-1 + i * 10).getFilename());
				return;
			}
			catch(std::exception e) {}
		}
	}
}

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

		
		std::vector<std::string> rawMenu = {
			"title.presentation",
			"title.title_art",
			"title.play",
			"title.multiplayer",
			"title.settings",
			"title.exit"
		};

		Menu mainmenu = Menu::rawMenu(rawMenu, MenuInput::FREEFIELD);

		std::cout << mainmenu.render() << std::endl;

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
					std::cout << rang::fgB::red << Config::Language::getTranslation("title.play_disabled.error") << rang::style::reset << std::endl;
					pause();
				}
				else
				{
					if (ipt == 1)
					{
						if (config.areSettingsBeforeGameShown())
						{
								Config::Config tmpConfig = config;
								try
								{
									settings(tmpConfig, true);
									launchGame(tmpConfig, Connectivity::LOCAL);
								}
								catch (std::exception e)
								{
									break;
								}
						}
						else
						{
							launchGame(config, Connectivity::LOCAL);
						}
					}
					else if (ipt == 2)
					{
						std::cout << "Not implemented" << std::endl;
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
void updateField(IEngine& engine, bool finished)
{
	clear();
	if (!finished)
	{
		std::string playerStr = Config::Language::getTranslation(engine.getSymbol(engine.getTurn()) == Symbol::X ? "symbol.X" : "symbol.O");
		std::string title = std::vformat(Config::Language::getTranslation("game.turn_title"), std::make_format_args(playerStr));
		std::cout << title << std::endl;
	}
	else
	{
		std::cout << printWinningMessage(engine) << std::endl;
	}
	std::cout << table(engine) << std::endl;
}

void launchGame(Config::Config& config, Connectivity connectivity)
{
	std::unique_ptr<IEngine> engine;
	std::unique_ptr<EngineConfig> engineConfig;

	if (connectivity == Connectivity::LOCAL) 
	{
		auto localConfig = std::make_unique<LocalEngineConfig>();
		auto localEngine = std::make_unique<LocalEngine>();
		localConfig->gameType = config.getGameType();
		localConfig->difficulty = config.getDifficulty();
		localConfig->firstPlayer = config.getFirstPlayer();
		engineConfig = std::move(localConfig);
		engine = std::move(localEngine);
	}
	else if (connectivity == Connectivity::REMOTE) 
	{
	}

	if (config.getPreferedSymbol() == Symbol::X)
	{
		engineConfig->playerSymbols.x = PlayerID::PLAYER1;
		engineConfig->playerSymbols.o = PlayerID::PLAYER2;
	}
	else
	{
		engineConfig->playerSymbols.o = PlayerID::PLAYER1;
		engineConfig->playerSymbols.x = PlayerID::PLAYER2;
	}

	engineConfig->callbacks.onUpdate = [](IEngine& engine) {};
	engineConfig->callbacks.onInteractiveTurn = [](IEngine& engine, bool firstTry) -> int8_t
		{
			static const std::map<int8_t, int8_t> keymap = { {7, 0}, {8, 1}, {9, 2}, {4, 3}, {5, 4}, {6, 5}, {1, 6}, {2, 7}, {3, 8} };
			updateField(engine);
			if (!firstTry)
			{
				std::cout << Config::Language::getTranslation("misc.wrong_input") << std::endl;
			}
			int8_t i_input = numericInput<int8_t>();
			if (keymap.contains(i_input))
			{
				return keymap.at(i_input);
			}

			return -1;
		};
	engineConfig->callbacks.onNonInteractiveTurn = [](IEngine& engine) -> void
		{
			updateField(engine);
			std::cout << Config::Language::getTranslation("game.computers_turn") << std::endl;
			pause();
		};
	try
	{
		engine->sendSettings(*engineConfig);
	}
	catch (std::exception& e)
	{
		printError(e.what());
		return;
	}
	engine->launchGame();

	updateField(*engine, true);
	pause();
}

std::string printWinningMessage(IEngine& engine)
{
	GameStatus gameStatus = engine.getStatus();

	if (gameStatus.isUnfinished())
	{
		throw std::exception("Game was unfinished");
	}
	else if (gameStatus.isDraw())
	{
		return Config::Language::getTranslation("game.draw");
	}
	else if (gameStatus.hasWinner())
	{
		Symbol enumSymbol = engine.getSymbol(gameStatus);
		std::string symbol;
		if (enumSymbol == Symbol::X)
		{
			symbol = Config::Language::getTranslation("symbol.X");
		}
		else
		{
			symbol = Config::Language::getTranslation("symbol.O");
		}
		return std::vformat(Config::Language::getTranslation("game.win"), std::make_format_args(symbol));
	}
	else
	{
		throw std::exception("Enum could not be compared");
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

		std::string plAmount = ((config.getGameType() == GameType::AIvAI) ? Config::Language::getTranslation("settings.player_amount.computer_only") : (config.getGameType() == GameType::PvAI) ? Config::Language::getTranslation("settings.player_amount.computer_and_player") : Config::Language::getTranslation("settings.player_amount.player_only"));
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

		std::string fstPlayer = ((config.getFirstPlayer() == PlayerID::PLAYER1) ? Config::Language::getTranslation("settings.yes") : Config::Language::getTranslation("settings.no"));
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

		std::string dffclty = ((config.getDifficulty() == Difficulty::EASY)
			? Config::Language::getTranslation("settings.difficulty.easy") : (config.getDifficulty() == Difficulty::MIDDLE)
			? Config::Language::getTranslation("settings.difficulty.medium") : Config::Language::getTranslation("settings.difficulty.hard"));
		std::string difficulty = std::vformat(Config::Language::getTranslation("settings.difficulty"),
			std::make_format_args(dffclty));
		std::cout
			<< difficulty
			<< std::endl;

		if (!areTempSettings)
		{
			std::cout << Config::Language::getTranslation("settings.language") << std::endl;
		}

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
			config.setGameType((config.getGameType() == GameType::AIvAI) 
				? GameType::PvAI : (config.getGameType() == GameType::PvAI) 
				? GameType::PvP : (config.getGameType() == GameType::PvP) 
				? GameType::AIvAI : GameType::AIvAI);
			break;
		case 3:
			config.setIfSymbolEnforced(!config.isSymbolEnforced());
			break;
		case 4:
			config.setFirstPlayer((config.getFirstPlayer() == PlayerID::PLAYER1) ? PlayerID::PLAYER2 : PlayerID::PLAYER1);
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
			config.setDifficulty((config.getDifficulty() == Difficulty::EASY) 
				? Difficulty::MIDDLE : (config.getDifficulty() == Difficulty::MIDDLE) 
				? Difficulty::HARD : (config.getDifficulty() == Difficulty::HARD) 
				? Difficulty::EASY : Difficulty::HARD);
			break;
		case 7:
			if(!areTempSettings)
				languageSettings(config);
			title = Config::Language::getTranslation("settings.settings");
			break;
		case 9:
			if (areTempSettings)
			{
				throw std::exception("Start aborted");
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

std::string table(IEngine& engine)
{
	std::vector<std::string> xandosString;
	std::string output;
	std::array<PlayerID, 9> field = engine.getField();
	for (int i = 0; i < 9; i++)
	{
		if (field.at(i) == PlayerID::NONE)
		{
			if (engine.getStatus().isUnfinished())
				xandosString.push_back(std::to_string(keymap.at(i)));
			else
				xandosString.push_back(" ");
		}
		else if (field.at(i) == PlayerID::PLAYER1)
		{
			xandosString.push_back(Config::Language::getTranslation(engine.getSymbol(PlayerID::PLAYER1) == Symbol::X ? "symbol.X" : "symbol.O"));
		}
		else if (field.at(i) == PlayerID::PLAYER2)
		{
			xandosString.push_back(Config::Language::getTranslation(engine.getSymbol(PlayerID::PLAYER2) == Symbol::X ? "symbol.X" : "symbol.O"));
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
		if (xandosString.at(i) == Config::Language::getTranslation("symbol.X"))
			fg = rang::fg::cyan;
		else if (xandosString.at(i) == Config::Language::getTranslation("symbol.O"))
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
	std::cout << Config::Language::getTranslation("misc.enter") << std::endl;
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