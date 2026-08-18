#include "Config.h"
#include <Language.h>

#include <filesystem>
#include <fstream>
#include <tuple>
#include <json/json.h>
#include <iostream>

#define VERSION 2

namespace Config
{
    const std::unordered_map<ConfigFiles, std::filesystem::path> paths{
	{ConfigFiles::ABSTRACT,		{}},
        {ConfigFiles::GameConfig,	"./config.json"},
        {ConfigFiles::Serverlist,	"./serverlist.json"},
	{ConfigFiles::LanguageFile,	"./language/"}
    };

    std::filesystem::path getPath(ConfigFiles configFiles, std::filesystem::path filename)
    {
	if(paths.at(configFiles).empty())
	    return {};

	const char* home = std::getenv("HOME");
	std::filesystem::path homeDir{};
	if(home == nullptr)
		throw std::invalid_argument("$HOME variable is not defined.");
	homeDir = home;
	homeDir /= ".TicTacToe";

	std::filesystem::path path = homeDir / paths.at(configFiles);
	if(path.string().back() == '/')
	{
		return path / filename;
	}
	return path;
    }

    bool AbstractConfig::serialize()
    {
	Json::StreamWriterBuilder factory;
	Json::Value object = toJSON();
	if(object.empty())
		return false;

	if (!std::filesystem::exists(path.parent_path()))
        {
		std::filesystem::create_directories(path.parent_path());
        }

        std::ofstream output(this->path);
        output << Json::writeString(factory, object);
        output.close();

        return true;
    }

    bool AbstractConfig::deserialize()
    {
	std::ifstream input(path);

        if (!input)
        {
		return false;
	}

        std::string jsonString;
	std::ostringstream stream;
	stream << input.rdbuf();
	jsonString = stream.str();

	if(path.extension() != ".json")
		return false;

        Json::Value object;
        if(!Json::Reader().parse(jsonString, object))
		return false;
	return toConfig(object);
    }
    
    Json::Value Config::toJSON()
    {
        Json::StreamWriterBuilder factory;
        Json::Value object;

        object["preferedSymbol"] = static_cast<int>(preferedSymbol);
        object["gameType"] = static_cast<int>(gameType);
        object["enforceSymbol"] = enforceSymbol;
        object["firstPlayer"] = static_cast<int>(firstPlayer);
        object["showSettingsBeforeGame"] = showSettingsBeforeGame;
        object["difficulty"] = static_cast<int>(difficulty);
        object["language"] = language;
        object["version"] = version;

	return object;
    }

    bool Config::toConfig(Json::Value& input)
    {
        if (input["version"].asInt() != VERSION)
        {
            if (input["version"].asInt() == 1)
            {
                input["difficulty"] = static_cast<int>(Difficulty::HARD);
            } 
            else if (input["version"].asInt() == 2)
            {
                input["gameType"] = input["playerAmount"];
            }
        }

        if (input["language"].isNull())
        {
            language = "en-US";
        }
        else
        {
            language = input["language"].asString();
        }

        preferedSymbol = static_cast<enum Symbol>(input["preferedSymbol"].asInt());
        gameType = static_cast<GameType>(input["gameType"].asInt());
        enforceSymbol = input["enforceSymbol"].asBool();
        firstPlayer = static_cast<enum PlayerID>(input["firstPlayer"].asInt());
        showSettingsBeforeGame = input["showSettingsBeforeGame"].asBool();
        difficulty = static_cast<enum Difficulty>(input["difficulty"].asInt());

        return true;
    }

    bool Config::setLanguage(std::string lang)
    {
            bool success = Language::loadLanguage(lang);
            language = lang;
            return success;
    }

    bool Config::operator==(const Config& config) const
    {
        return std::tie(preferedSymbol, gameType, enforceSymbol, firstPlayer, showSettingsBeforeGame, difficulty, language)
            == std::tie(config.preferedSymbol, config.gameType, config.enforceSymbol, config.firstPlayer, config.showSettingsBeforeGame, config.difficulty, config.language);
    }

    Config& Config::operator=(const Config& config)
    {
        this->preferedSymbol = config.preferedSymbol;
        this->gameType = config.gameType;
        this->enforceSymbol = config.enforceSymbol;
        this->firstPlayer = config.firstPlayer;
        this->showSettingsBeforeGame = config.showSettingsBeforeGame;
        this->difficulty = config.difficulty;
        this->language = config.language;

        return *this;
        
    }
}
