#include "Config.h"
#include <Language.h>

#include <filesystem>
#include <fstream>
#include <tuple>
#include <json/json.h>

#define VERSION 2

namespace Config
{
    const std::map<ConfigFiles, std::filesystem::path> paths{
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
    
    bool Config::serialize()
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

        if (!std::filesystem::exists(path.parent_path()))
        {
            try
            {
                std::filesystem::create_directory(path.parent_path());
            }
            catch (std::exception e)
            {
                //std::cout << e.what() << std::endl;
            }
        }

        std::ofstream output(this->path);
        output << Json::writeString(factory, object);
        output.close();

        return true;
    }

    bool Config::deserialize()
    {
        std::ifstream input(path);

        if (!input.good())
        {
            return false;
        }

        std::string jsonString;

        if (input)
        {
            std::ostringstream stream;
            stream << input.rdbuf();
            jsonString = stream.str();
        }

        Json::Value object;
        Json::Reader().parse(jsonString, object);

        if (object["version"].asInt() != VERSION)
        {
            if (object["version"].asInt() == 1)
            {
                object["difficulty"] = static_cast<int>(Difficulty::HARD);
            } 
            else if (object["version"].asInt() == 2)
            {
                object["gameType"] = object["playerAmount"];
            }
        }

        if (object["language"].isNull())
        {
            language = "en-US";
        }
        else
        {
            language = object["language"].asString();
        }

        preferedSymbol = static_cast<enum Symbol>(object["preferedSymbol"].asInt());
        gameType = static_cast<GameType>(object["gameType"].asInt());
        enforceSymbol = object["enforceSymbol"].asBool();
        firstPlayer = static_cast<enum PlayerID>(object["firstPlayer"].asInt());
        showSettingsBeforeGame = object["showSettingsBeforeGame"].asBool();
        difficulty = static_cast<enum Difficulty>(object["difficulty"].asInt());

        return true;
    }

    bool Config::setLanguage(std::string lang)
    {
        try
        {
            Language::loadLanguage(lang);
            language = lang;
            return true;
        }
        catch (LanguageNotReadableException e)
        {
            return false;
        }
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
