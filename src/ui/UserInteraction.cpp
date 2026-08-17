#include "UserInteraction.h"

#include <iostream>
#include <numeric>
#include <ranges>
#include <stdexcept>
#include <unistd.h>
#include <vector>
#include <string>
#include <format>
#include <sstream>

#include <rang.hpp>

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>

#include "Config.h"
#include <Language.h>
#include "Menu.h"


std::string buildOutput(std::string_view str, auto&&... args)
{
	return std::vformat(str, std::make_format_args(args...));
}

std::vector<std::string> refreshSettingsEntry(std::vector<SettingsMenuEntry>& menuEntries, Config::Config& config, bool areTempSettings)
{
	menuEntries.clear();
	std::string isSymbolEnforced = (config.isSymbolEnforced() ?
			Config::Language::getTranslation("settings.yes") :
			Config::Language::getTranslation("settings.no"));
	std::string firstPlayer = ((config.getFirstPlayer() == PlayerID::PLAYER1) ?
			Config::Language::getTranslation("settings.yes") :
			Config::Language::getTranslation("settings.no"));

	menuEntries = {
		{Settings::Symbol, 
			buildOutput(Config::Language::getTranslation("settings.symbol"),
					Config::Language::getEnumTranslation(config.getPreferedSymbol()),
					Config::Language::getEnumTranslation(!config.getPreferedSymbol()))},
		{Settings::GameType, 
			buildOutput(Config::Language::getTranslation("settings.game_type"), 
					Config::Language::getEnumTranslation(config.getGameType()))},
		{Settings::EnforceSymbol, 
			buildOutput(Config::Language::getTranslation("settings.is_symbol_enforced"), isSymbolEnforced)},
		{Settings::FirstPlayer, 
			buildOutput(Config::Language::getTranslation("settings.first_player"), firstPlayer)},
		{Settings::Difficulty, 
			buildOutput(Config::Language::getTranslation("settings.difficulty"),
				Config::Language::getEnumTranslation(config.getDifficulty()))}
	};

	if(!areTempSettings)
	{
		std::string settingsBeforeGame = (config.areSettingsBeforeGameShown() ?
				Config::Language::getTranslation("settings.yes") :
				Config::Language::getTranslation("settings.no"));
		menuEntries.push_back({Settings::SettingsBeforeGame, buildOutput(Config::Language::getTranslation("settings.settings_before_game"), settingsBeforeGame)});
		menuEntries.push_back({Settings::Language, buildOutput(Config::Language::getTranslation("settings.language"))});
		menuEntries.push_back({Settings::SaveSettings, buildOutput(Config::Language::getTranslation("settings.save_settings"))});
	}
	else 
	{
		menuEntries.push_back({Settings::AbortGame, buildOutput(Config::Language::getTranslation("settings.abort_game"))});
		menuEntries.push_back({Settings::StartGame, buildOutput(Config::Language::getTranslation("settings.start_game"))});
	}

	std::vector<std::string> textMenuEntries;
	for(SettingsMenuEntry entry : menuEntries)
	{
		textMenuEntries.push_back(entry.text);
	}
	return textMenuEntries;
}

void languageSettings(Config::Config& config)
{
	clear();
	auto screen = ftxui::ScreenInteractive::TerminalOutput();
	std::vector<Config::Language> langlist = Config::Language::getLanguageList();
	std::vector<std::string> menuEntries;

	int selected = 0;

	for(Config::Language language : langlist)
	{
		menuEntries.push_back(buildOutput(Config::Language::getTranslation("settings.language.entry"),  
					language.getDisplayName(),
					language.getRegion()));
	}

	ftxui::MenuOption option;
	option.on_enter = screen.ExitLoopClosure();
	auto menu = ftxui::Menu(&menuEntries, &selected, option);

	auto renderer = ftxui::Renderer(menu, [&] {
		return ftxui::vbox({ftxui::text(Config::Language::getTranslation("settings.language.title")), menu->Render()});
	});

	screen.Loop(renderer);

	config.setLanguage(langlist.at(selected).getFilename());
	return;
}

void mainMenu(bool playDisabled, Config::Config& config)
{
	clear();
	if(isatty(STDIN_FILENO))
	{
		rang::setControlMode(rang::control::Force);
	}
	else
	{
		rang::setControlMode(rang::control::Off);
	}

	bool abort = false;
	while (!abort)
	{
		auto screen = ftxui::ScreenInteractive::TerminalOutput();
		int selected = 0;


		auto buildEntries = [&]() -> std::vector<MainMenuEntry>
		{
			if(!playDisabled)
			{
				return {
					{MainMenu::SinglePlayer, Config::Language::getTranslation("title.play")},
					{MainMenu::MultiPlayer, Config::Language::getTranslation("title.multiplayer")},
					{MainMenu::Settings, Config::Language::getTranslation("title.settings")},
					{MainMenu::Exit, Config::Language::getTranslation("title.exit")}
				};
			}
			else 
			{
				return {
					{MainMenu::Settings, Config::Language::getTranslation("title.settings")},
					{MainMenu::Exit, Config::Language::getTranslation("title.exit")}
				};
			}		
		};

		auto menuEntries = buildEntries();

		std::vector<std::string> textMenuEntries;

		for (const MainMenuEntry& menuEntry : menuEntries)
		{
			textMenuEntries.push_back(menuEntry.text);
		
		}
		
		MainMenu selectedMainMenu = MainMenu::Exit;
		ftxui::MenuOption option;
		option.on_enter = [&] {
			selectedMainMenu = menuEntries.at(selected).mainMenu;
			screen.Exit();

		};
		auto menu = ftxui::Menu(&textMenuEntries, &selected, option);

		auto renderer = ftxui::Renderer(menu, [&] {
			return ftxui::vbox({ftxui::text(Config::Language::getTranslation("title.presentation")), ftxui::separator(), menu->Render()});
		});

		screen.Loop(renderer);
		
		if(playDisabled)
		{
			if (config.deserialize())
			{
				playDisabled = false;
			}
		}

		switch(selectedMainMenu)
		{	
			case MainMenu::SinglePlayer:
				if (config.areSettingsBeforeGameShown())
				{
					Config::Config tmpConfig = config;
					if(settings(tmpConfig, true))
						launchGame(tmpConfig, Connectivity::LOCAL);
				}
				else
				{
					launchGame(config, Connectivity::LOCAL);
				}
				break;
			case MainMenu::MultiPlayer:
				break;
			case MainMenu::Settings:
				settings(config, false);
				break;
			case MainMenu::Exit:
				abort = true;
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
			UI_pause();
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
	UI_pause();
}

std::string printWinningMessage(IEngine& engine)
{
	GameStatus gameStatus = engine.getStatus();

	if (gameStatus.isUnfinished())
	{
		throw std::invalid_argument("Game was unfinished");
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
		throw std::invalid_argument("Enum could not be compared");
	}

}

bool settings(Config::Config& config, const bool& areTempSettings)
{
	clear();
	auto screen = ftxui::ScreenInteractive::TerminalOutput();
	std::string title = areTempSettings ? Config::Language::getTranslation("settings.temp_settings") : Config::Language::getTranslation("settings.settings"); // Spielparameter ; Standardeinstellungen
																				  //
	int selected = 0;
	bool returnValue = false;
	std::vector<SettingsMenuEntry> menuEntries;
	std::vector<std::string> textMenuEntries = refreshSettingsEntry(menuEntries, config, areTempSettings);
	ftxui::MenuOption option;
	option.on_enter = [&] {
	Settings setting = menuEntries.at(selected).setting;

	switch (setting)
	{
		case Settings::Symbol:
			config.setPreferedSymbol((config.getPreferedSymbol() == Symbol::X) ? Symbol::O : Symbol::X);
			break;
		case Settings::GameType:
			config.setGameType((config.getGameType() == GameType::AIvAI) 
				? GameType::PvAI : (config.getGameType() == GameType::PvAI) 
				? GameType::PvP : (config.getGameType() == GameType::PvP) 
				? GameType::AIvAI : GameType::AIvAI); 
			break;
		case Settings::EnforceSymbol:
			config.setIfSymbolEnforced(!config.isSymbolEnforced());
			break;
		case Settings::FirstPlayer:
			config.setFirstPlayer((config.getFirstPlayer() == PlayerID::PLAYER1) ? PlayerID::PLAYER2 : PlayerID::PLAYER1);
			break;
		case Settings::SettingsBeforeGame:
			config.setIfSettingsAreShownBeforeGame(!config.areSettingsBeforeGameShown());
			break;
		case Settings::Difficulty:
			config.setDifficulty((config.getDifficulty() == Difficulty::EASY) 
				? Difficulty::MIDDLE : (config.getDifficulty() == Difficulty::MIDDLE) 
				? Difficulty::HARD : (config.getDifficulty() == Difficulty::HARD) 
				? Difficulty::EASY : Difficulty::HARD);
			break;
		case Settings::Language:
			languageSettings(config);
			title = Config::Language::getTranslation("settings.settings"); // refreshSettingsEntry does not refresh this automatically
			break;
		case Settings::AbortGame:
			screen.Exit();
			returnValue = false;
			break;
		case Settings::StartGame:
			screen.Exit();
			returnValue = true;
			break;
		case Settings::SaveSettings:
			returnValue = config.serialize();
			screen.Exit();
			break;
		default:
			returnValue = false;
			break;
	}
	textMenuEntries = refreshSettingsEntry(menuEntries, config, areTempSettings);
	};
	auto menu = ftxui::Menu(&textMenuEntries, &selected, option);

	auto renderer = ftxui::Renderer(menu, [&] {
		return ftxui::vbox({ftxui::text(title), menu->Render()});
	});

	screen.Loop(renderer);
	return returnValue;
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

	std::cout <<	"\u250C\u2500\u2500\u2500"
					"\u252C\u2500\u2500\u2500"
					"\u252C\u2500\u2500\u2500\u2510\n";

	for (int i = 0; i < xandosString.size(); i++)
	{
		auto fg = rang::fg::reset;
		if (xandosString.at(i) == Config::Language::getTranslation("symbol.X"))
			fg = rang::fg::cyan;
		else if (xandosString.at(i) == Config::Language::getTranslation("symbol.O"))
			fg =  rang::fg::yellow;

		if (i % 3 == 2 && i != 0)
		{
			std::cout << " " << fg << xandosString.at(i) << rang::fg::reset << " \u2502";
			if (i != 8)
			{
				std::cout <<	"\n\u251C\u2500\u2500\u2500"
								"\u253C\u2500\u2500\u2500"
								"\u253C\u2500\u2500\u2500\u2524\n";
			}
		}
		else if (i % 3 == 0)
		{
			std::cout << "\u2502 " << fg << xandosString.at(i) << rang::fg::reset << " \u2502";
		}
		else
		{
			std::cout << " " << fg << xandosString.at(i) << rang::fg::reset << " \u2502";
		}
	}

	std::cout <<	"\n\u2514\u2500\u2500\u2500"
					"\u2534\u2500\u2500\u2500"
					"\u2534\u2500\u2500\u2500\u2518\n";

	std::cout.rdbuf(backup);

	return ostream.str();
}

void printError(std::string exception)
{
	clear();
	std::cout << rang::fg::red << rang::style::bold << tictactoe_title << rang::style::reset << std::endl;
	std::cout << "There was an exception whilst running the program" << std::endl << std::endl;
	std::cout << rang::style::bold << exception << rang::style::reset << std::endl;
	UI_pause();
}

void UI_pause()
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
