#include "Config.h"

#include <filesystem>
#include <fstream>
#include <tuple>
#include <ShlObj.h>
#include <json/json.h>

#define VERSION 2

namespace Config
{

    Path::Path(std::string directory, std::string filename)
    {
        this->directory = directory;
        this->filename = filename;
    }

    Path::Path(ConfigFiles type, std::string fileSpecifier)
    {
        if (fileSpecifier == "")
        {
            Path requestedPath = unspecified_paths.at(type);
            this->directory = requestedPath.directory;
            this->filename = requestedPath.filename;
        }
        else
        {
            std::pair<Path, std::string> path_and_extension = specified_paths.at(type);
            this->directory = path_and_extension.first.directory;
            this->filename = fileSpecifier + path_and_extension.second;
        }
    }

    std::string Path::getPath()
    {
        return directory + filename;
    }

#ifdef _WIN32
    std::map<enum ConfigFiles, Path> Path::unspecified_paths =
    {
        std::pair<ConfigFiles, Path>(ConfigFiles::ABSTRACT, Path("", "")),
        std::pair <enum ConfigFiles, Path>(ConfigFiles::GameConfig, Path(getDir(CSIDL_APPDATA), "config.json")),
        std::pair <enum ConfigFiles, Path> (ConfigFiles::Serverlist, Path(getDir(CSIDL_APPDATA), "serverlist.json")),
        std::pair<ConfigFiles, Path>(ConfigFiles::LanguageFile, Path(getDir(CSIDL_PROGRAM_FILESX86) + "language\\", ""))
    };

    std::map<ConfigFiles, std::pair<Path, std::string>> Path::specified_paths =
    {
        std::pair<ConfigFiles, std::pair<Path, std::string>>
        (
            ConfigFiles::LanguageFile, 
            std::pair<Path, std::string>(Path(getDir(CSIDL_PROGRAM_FILESX86) + "language\\", ""), ".json")
        )
    };

#else
    std::map<enum ConfigFiles, AbstractConfig::Path> AbstractConfig::Path::unspecified_paths =
    {
        std::pair <enum ConfigFiles, AbstractConfig::Path>(ConfigFiles::GeneralConfig, AbstractConfig::Path("", "/config.json")),
        std::pair<enum ConfigFiles, AbstractConfig::Path>(ConfigFiles::Serverlist, AbstractConfig::Path("", "/serverlist.json"))
    };

    std::map<ConfigFiles, std::pair<AbstractConfig::Path, std::string>> AbstractConfig::Path::specified_paths =
    {
        std::pair<ConfigFiles, std::pair<AbstractConfig::Path, std::string>>
        (
            ConfigFiles::LanguageFile,
            std::pair<AbstractConfig::Path, std::string>(AbstractConfig::Path("" + "language\\", ""), ".json")
        )
    };

#endif

#ifdef _WIN32
    std::string Path::getDir(int id)
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
        object["language"] = language;
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
                //std::cout << e.what() << std::endl;
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

        if (object["language"].isNull())
        {
            language = "en-US";
        }
        else
        {
            language = object["language"].asString();
        }

        preferedSymbol = static_cast<enum Symbol>(object["preferedSymbol"].asInt());
        playerAmount = static_cast<enum PlayerAmount>(object["playerAmount"].asInt());
        enforceSymbol = object["enforceSymbol"].asBool();
        firstPlayer = static_cast<enum FieldType>(object["firstPlayer"].asInt());
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
        return std::tie(preferedSymbol, playerAmount, enforceSymbol, firstPlayer, showSettingsBeforeGame, difficulty, language)
            == std::tie(config.preferedSymbol, config.playerAmount, config.enforceSymbol, config.firstPlayer, config.showSettingsBeforeGame, config.difficulty, config.language);
    }

    Config& Config::operator=(const Config& config)
    {
        this->preferedSymbol = config.preferedSymbol;
        this->playerAmount = config.playerAmount;
        this->enforceSymbol = config.enforceSymbol;
        this->firstPlayer = config.firstPlayer;
        this->showSettingsBeforeGame = config.showSettingsBeforeGame;
        this->difficulty = config.difficulty;
        this->language = config.language;

        return *this;
        
    }

    Language::Language(std::string name)
    {
        type = ConfigFiles::LanguageFile;
        path = Path(ConfigFiles::LanguageFile, name);
        path.filename = name;
        version = 2;
        bool functional = deserialize();

    }

    std::string Language::maskPhrases(std::string phrase)
    {
        for (int i = 0; i < static_cast<int>(phrase.size()) - 1; i++)
        {
            std::string codepoint = phrase.substr(i, 2);

            if (maskedCharacter.find(codepoint) != maskedCharacter.end())
            {
                phrase = phrase.substr(0, i) + maskedCharacter.at(codepoint) + phrase.substr(i + 2);
            }
        }
        
        return phrase;
    }

    bool Language::deserialize()
    {
        std::ifstream input(path.getPath() + ".json");

        if (!input.good())
        {
            throw LanguageNotReadableException("Failed to load language file: " + path.filename);
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

        if (object["version"].asInt() == 1 || object["version"].isNull())
        {
            return false;
        }

        filename = maskPhrases(object["filename"].asString());
        displayName = maskPhrases(object["displayName"].asString());
        region = maskPhrases(object["region"].asString());

        const Json::Value& maskedCharsJSON = object["maskedCharacters"];
        for (Json::Value::ArrayIndex i = 0; i < maskedCharsJSON.size(); i++)
        {
            const Json::Value& maskedChar = maskedCharsJSON[i];
            maskedCharacter.insert(std::pair<std::string, char> (maskedChar[0].asString(), static_cast<char>((maskedChar[1].asInt()))));
        }

        const Json::Value& translationsJSON = object["translations"];
        for (Json::Value::ArrayIndex i = 0; i < translationsJSON.size(); i++)
        {
            const Json::Value& translation = translationsJSON[i];
            translations.insert(std::pair<std::string, std::string>(translation[0].asString(), maskPhrases(translation[1].asString())));
        }

        return true;
    }

    void Language::loadLanguageList()
    {
        try
        {
            languageList.clear();
           for (const auto& entry : std::filesystem::directory_iterator(Path(ConfigFiles::LanguageFile).getPath()))
            {
                Language lang = Language(entry.path().stem().string());
                lang.deserialize();
                languageList.push_back(lang);
            }
        }
        catch (std::exception e)
        {
            throw LanguageNotReadableException();
        }
    }

    std::vector<Language> Language::getLanguageList(const bool reload)
    {
        if (reload || languageList.empty())
        {
            Language::loadLanguageList();
        }
        return languageList;
    }

    bool Language::loadLanguage(std::string name)
    {
        Language lang = Language(name);
        loadedLanguage = std::make_unique<Language>(lang);
        return true;
    }

    std::string Language::getTranslation(std::string key)
    {
        if (loadedLanguage != nullptr)
        {
            if (loadedLanguage->translations.contains(key))
            {
                return loadedLanguage->translations.at(key);
            }
#ifndef _DEBUG
            throw LanguageNotReadableException();
#else
            return key;
#endif
        }
        else
        {
            throw LanguageNotReadableException();
        }
    }

    std::string Language::getFilename()
    {
        return filename;
    }

    std::string Language::getDisplayName()
    {
        return displayName;
    }

    std::string Language::getRegion()
    {
        return region;
    }

    const char* LanguageNotReadableException::what() const throw()
    {
        if (languageName == "")
        {
            return "Attempted to access or failed to load unloaded language resource";
        }

        return languageName.c_str();
    }
    const char* PathNotRetrievableException::what() const throw()
    {
        if (path == "")
        {
            return "Unknown path could not be retrieved";
        }

        return path.c_str();
    }
}