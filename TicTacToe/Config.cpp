#include "Config.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <tuple>

#include <json/json.h>


#define VERSION 2

namespace Config
{

    AbstractConfig::Path::Path(std::string directory, std::string filename)
    {
        this->directory = directory;
        this->filename = filename;
    }

    std::string AbstractConfig::Path::getPath()
    {
        return directory + filename;
    }

#ifdef _WIN32
    std::map<enum ConfigFiles, AbstractConfig::Path> AbstractConfig::paths =
    {
        std::pair <enum ConfigFiles, AbstractConfig::Path> (ConfigFiles::GeneralConfig, AbstractConfig::Path(getDir(CSIDL_APPDATA), "config.json")),
        std::pair<enum ConfigFiles, AbstractConfig::Path>(ConfigFiles::Serverlist, AbstractConfig::Path(getDir(CSIDL_APPDATA), "serverlist.json")),
        std::pair<enum ConfigFiles, AbstractConfig::Path>(ConfigFiles::LanguageFile, AbstractConfig::Path(getDir(CSIDL_PROGRAM_FILESX86) + "\\language\\", ""))
    };

#else
    std::map<enum ConfigFiles, AbstractConfig::Path> AbstractConfig::paths =
    {
        std::pair <enum ConfigFiles, AbstractConfig::Path>(ConfigFiles::GeneralConfig, AbstractConfig::Path("", "/config.json")),
        std::pair<enum ConfigFiles, AbstractConfig::Path>(ConfigFiles::Serverlist, AbstractConfig::Path("", "/serverlist.json")),
        std::pair<enum ConfigFiles, AbstractConfig::Path>(ConfigFiles::LanguageFile, AbstractConfig::Path("" + "/language/", ""))
    };
#endif

#ifdef _WIN32
    std::string AbstractConfig::getDir(int id)
    {
        LPWSTR strPath = new WCHAR[2048];
        SHGetSpecialFolderPath(0, strPath, id, FALSE);
        std::wstring ws_temp(strPath);
        std::string dir(ws_temp.begin(), ws_temp.end());
        delete[] strPath;

        if (id == CSIDL_PROGRAM_FILESX86 || id == CSIDL_PROGRAM_FILES)
            dir.append("\\egerk");

        return dir.append("\\TicTacToe\\");
#else

        std::string homeDir = std::getenv("HOME");
        return homeDir.append("/.TicTacToe/");
#endif
    }
    
    bool Config::serialize()
    {
        Json::StreamWriterBuilder factory;
        Json::Value object;

        object["preferedSymbol"] = static_cast<int>(preferedSymbol);
        object["playerAmount"] = static_cast<int>(playerAmount);
        object["enforceSymbol"] = enforceSymbol;
        object["firstPlayer"] = static_cast<int>(firstPlayer);
        object["showSettingsBeforeGame"] = showSettingsBeforeGame;
        object["difficulty"] = static_cast<int>(difficulty);
        object["version"] = version;

        std::filesystem::path path(this->path.directory);
        if (!std::filesystem::exists(path))
        {
            try
            {
                std::filesystem::create_directory(path);
            }
            catch (std::exception e)
            {
                std::cout << e.what() << std::endl;
            }
        }

        std::ofstream output(this->path.getPath());
        output << Json::writeString(factory, object);
        output.close();

        return true;
    }

    bool Config::deserialize()
    {
        std::ifstream input(path.getPath());

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
        }

        preferedSymbol = static_cast<enum Symbol>(object["preferedSymbol"].asInt());
        playerAmount = static_cast<enum PlayerAmount>(object["playerAmount"].asInt());
        enforceSymbol = object["enforceSymbol"].asBool();
        firstPlayer = static_cast<enum FieldType>(object["firstPlayer"].asInt());
        showSettingsBeforeGame = object["showSettingsBeforeGame"].asBool();
        difficulty = static_cast<enum Difficulty>(object["difficulty"].asInt());

        return true;
    }

    bool Config::operator==(const Config& config) const
    {
        return std::tie(preferedSymbol, playerAmount, enforceSymbol, firstPlayer, showSettingsBeforeGame, difficulty)
            == std::tie(config.preferedSymbol, config.playerAmount, config.enforceSymbol, config.firstPlayer, config.showSettingsBeforeGame, config.difficulty);
    }

    Config& Config::operator=(const Config& config)
    {
        Config newConfig;
        newConfig.preferedSymbol = config.preferedSymbol;
        newConfig.playerAmount = config.playerAmount;
        newConfig.enforceSymbol = config.enforceSymbol;
        newConfig.firstPlayer = config.firstPlayer;
        newConfig.showSettingsBeforeGame = config.showSettingsBeforeGame;
        newConfig.difficulty = config.difficulty;

        return newConfig;

        
    }
    bool Language::deserialize()
    {
        std::ifstream input(path.getPath());

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

        lang = object["lang"].asString();
        name = object["name"].asString();
        region = object["region"].asString();
        const Json::Value& translationsJSON = object["translations"];
        for (int i = 0; i < translationsJSON.size(); i++)
        {
            std::stringstream complete;
            complete.str(translationsJSON[i].asString());
            std::string part;
            std::vector<std::string> devided;

            while (std::getline(complete, part, ':'))
            {
                devided.push_back(part);
            }

            translations.insert(std::pair<std::string, std::string>(devided.at(0), devided.at(1)));
        }
        return true;
    }
}