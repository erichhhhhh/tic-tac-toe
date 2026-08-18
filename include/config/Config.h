#pragma once
#include <unordered_map>
#include <string>
#include <filesystem>
#include <json/json.h>
#include "Enums.h"

namespace Config
{


	enum class ConfigFiles
	{
		ABSTRACT,
		GameConfig,
		Serverlist,
		LanguageFile
	};

	extern const std::unordered_map<ConfigFiles,std::filesystem::path> paths;

	std::filesystem::path getPath(ConfigFiles configFiles, std::filesystem::path filename = {});

	class AbstractConfig
	{
	private:
		virtual Json::Value toJSON() = 0;
		virtual bool toConfig(Json::Value& input) = 0;
	public:
		bool serialize();
		bool deserialize();

	protected:

		enum ConfigFiles type = ConfigFiles::ABSTRACT;
		std::filesystem::path path = getPath(ConfigFiles::ABSTRACT);
		int8_t version = 0;

	};


	class Config : public AbstractConfig
	{

		/*
		* How to properly add a parameter:
		* 1. private Attribute
		* 2. Define getter and setter
		* 3. Add to operator==
		* 4. Add to operator=
		*/

	public:

		Config()
		{
			type = ConfigFiles::GameConfig;
			path = getPath(ConfigFiles::GameConfig);
			version = 3;
		}

		Json::Value toJSON() override;
		bool toConfig(Json::Value& input) override;

		Symbol getPreferedSymbol() const{ return preferedSymbol; }
		void setPreferedSymbol(const Symbol& symbol) { preferedSymbol = symbol; }

		GameType getGameType() const { return gameType; }
		void setGameType(const GameType& type) { gameType = type; }

		bool isSymbolEnforced() const { return enforceSymbol; }
		void setIfSymbolEnforced(const bool& param) { enforceSymbol = param; }

		PlayerID getFirstPlayer() const { return firstPlayer; }
		void setFirstPlayer(const PlayerID& symbol)
		{ 
			if(symbol != PlayerID::NONE)
				firstPlayer = symbol; 
		}

		bool areSettingsBeforeGameShown() const { return showSettingsBeforeGame; }
		void setIfSettingsAreShownBeforeGame(const bool& param) { showSettingsBeforeGame = param; }

		Difficulty getDifficulty() const { return difficulty; }
		void setDifficulty(const Difficulty& diff) { difficulty = diff; }

		std::string getLanguage() const { return language; }
		bool setLanguage(std::string language);

		bool operator==(const Config& config) const;

		Config& operator=(const Config& config);

	private:
		Symbol preferedSymbol = Symbol::X;
		GameType gameType = GameType::PvAI;
		bool enforceSymbol = false;

		PlayerID firstPlayer = PlayerID::PLAYER1;
		bool showSettingsBeforeGame = true;
		Difficulty difficulty = Difficulty::HARD;
		std::string language = "en-US";

	};

	class Serverlist : public AbstractConfig
	{

	};

}
