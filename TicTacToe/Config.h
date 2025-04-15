#pragma once
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <exception>
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
		ABSTRACT,
		GeneralConfig,
		Serverlist,
		LanguageFile
	};

	struct Path
	{
	public:
		std::string directory;
		std::string filename;

		Path(ConfigFiles type, std::string fileSpecifier = "");
		std::string getPath();
	protected:
		Path(std::string directory, std::string filename);
	private:
		static std::string getDir(int id);
		static std::map <enum ConfigFiles, Path> unspecified_paths;
		static std::map <ConfigFiles, std::pair<Path, std::string>> specified_paths;
	};

	class PathNotRetrievableException : std::exception
	{
		std::string path;
	public:
		PathNotRetrievableException(std::string path) : path(path) {};
		const char* what() const throw();
	};

	class AbstractConfig
	{
	public:
		virtual bool serialize() = 0;
		virtual bool deserialize() = 0;

	protected:

		enum ConfigFiles type = ConfigFiles::ABSTRACT;
		Path path = Path(ConfigFiles::ABSTRACT);
		int8_t version = 0;

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
			path = Path(ConfigFiles::GeneralConfig);
			version = 2;
		}

		bool serialize() override;
		bool deserialize() override;

		enum class Symbol getPreferedSymbol() const{ return preferedSymbol; }
		void setPreferedSymbol(const enum class Symbol& symbol) { preferedSymbol = symbol; }

		enum class PlayerAmount getPlayerAmount() const { return playerAmount; }
		void setPlayerAmount(const enum class PlayerAmount& amount) { playerAmount = amount; }

		bool isSymbolEnforced() const { return enforceSymbol; }
		void setIfSymbolEnforced(const bool& param) { enforceSymbol = param; }

		enum class FieldType getFirstPlayer() const { return firstPlayer; }
		void setFirstPlayer(const enum class FieldType& symbol)
		{ 
			if(symbol != FieldType::EMPTY)
				firstPlayer = symbol; 
		}

		bool areSettingsBeforeGameShown() const { return showSettingsBeforeGame; }
		void setIfSettingsAreShownBeforeGame(const bool& param) { showSettingsBeforeGame = param; }

		enum class Difficulty getDifficulty() const { return difficulty; }
		void setDifficulty(const enum class Difficulty& diff) { difficulty = diff; }

		std::string getLanguage() const { return language; }
		bool setLanguage(std::string language);

		bool operator==(const Config& config) const;

		Config& operator=(const Config& config);

	private:
		enum Symbol preferedSymbol = Symbol::X;
		enum PlayerAmount playerAmount = PlayerAmount::COMPUTER_PLAYER;
		bool enforceSymbol = false;

		enum FieldType firstPlayer = FieldType::PLAYER1;
		bool showSettingsBeforeGame = true;
		enum class Difficulty difficulty = Difficulty::HARD;
		std::string language = "en-US";

	};

	class Serverlist : public AbstractConfig
	{

	};

	class Language : public AbstractConfig
	{
	private:
		std::string filename;
		std::string displayName;
		std::string region;
		bool functional = false;

		std::map<std::string, std::string> translations;

		static inline std::unique_ptr<Language> loadedLanguage = nullptr;
		static inline std::vector<Language> languageList;

		bool deserialize() override;
		bool serialize() override { return false; }

		Language(std::string name);

		static void loadLanguageList();

	public:

		static std::vector<Language> getLanguageList(const bool reload = false);
		static bool loadLanguage(std::string name);
		static std::string getTranslation(std::string key);
	};

	class LanguageNotReadableException : public std::exception
	{
	private:
		std::string languageName;
	public:
		LanguageNotReadableException(std::string langName = "") { languageName = langName; }
		const char* what() const throw();
	};
}