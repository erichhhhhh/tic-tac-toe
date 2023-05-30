#pragma once
#include <map>
#include <string>
#include <ShlObj.h>

#include "Player.h"

namespace Config
{

	enum class Difficulty
	{
		EASY,
		MIDDLE,
		HARD
	};

	enum class PlayerAmount
	{
		COMPUTER_ONLY,
		COMPUTER_PLAYER,
		PLAYER_ONLY
	};

	enum class ConfigFiles
	{
		GeneralConfig,
		Serverlist,
		LanguageFile
	};

	class AbstractConfig
	{
	protected:

		enum ConfigFiles type;
		std::string path;
		int8_t version = 0;

		static std::string getDir(int id);
		static std::map<enum ConfigFiles, const std::string> paths;

	public:
		virtual bool serialize() = 0;
		virtual bool deserialize() = 0;
	};


	class Config : public AbstractConfig
	{

		/*
		* How to properly add a param:
		* 1. private Attribute
		* 2. Define getter and setter
		* 3. Add to operator==
		* 4. Add to operator=
		*/

	public:

		Config()
		{
			type = ConfigFiles::GeneralConfig;
			path = paths.at(type);
			version = 2;
		}

		bool serialize() override;
		bool deserialize() override;

		enum Symbol getPreferedSymbol() const{ return preferedSymbol; }
		void setPreferedSymbol(const enum Symbol& symbol) { preferedSymbol = symbol; }

		enum PlayerAmount getPlayerAmount() const { return playerAmount; }
		void setPlayerAmount(const enum PlayerAmount& amount) { playerAmount = amount; }

		bool isSymbolEnforced() const { return enforceSymbol; }
		void setIfSymbolEnforced(const bool& param) { enforceSymbol = param; }

		enum FieldType getFirstPlayer() const { return firstPlayer; }
		void setFirstPlayer(const enum FieldType& symbol)
		{ 
			if(symbol != FieldType::EMPTY)
				firstPlayer = symbol; 
		}

		bool areSettingsBeforeGameShown() const { return showSettingsBeforeGame; }
		void setIfSettingsAreShownBeforeGame(const bool& param) { showSettingsBeforeGame = param; }

		enum Difficulty getDifficulty() const { return difficulty; }
		void setDifficulty(const enum class Difficulty& diff) { difficulty = diff; }

		bool operator==(const Config& config) const;

		Config& operator=(const Config& config);

	private:
		enum Symbol preferedSymbol = Symbol::X;
		enum PlayerAmount playerAmount = PlayerAmount::COMPUTER_PLAYER;
		bool enforceSymbol = false;

		enum FieldType firstPlayer = FieldType::PLAYER1;
		bool showSettingsBeforeGame = true;
		enum class Difficulty difficulty = Difficulty::HARD;

	};

	class Serverlist : public AbstractConfig
	{

	};

	class Language : public AbstractConfig
	{

	public:
		Language(std::string name)
		{
			type = ConfigFiles::LanguageFile;
			path = paths.at(type) + name;
			version = 1;
		}

	private:
		std::string lang;
		std::string name;
		std::string region;

		std::map<std::string, std::string> translations;
	};
}