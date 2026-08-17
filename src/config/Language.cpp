#include <Language.h>

#include <fstream>
#include <json/json.h>
namespace Config
{
    Language::Language(std::string name)
    {
        type = ConfigFiles::LanguageFile;
        path = getPath(ConfigFiles::LanguageFile, name + ".json");
        version = 2;
        bool functional = deserialize();

    }

    bool Language::deserialize()
    {
        std::ifstream input(path);

        if (!input.good())
        {
            throw LanguageNotReadableException("Failed to load language file: " + path.stem().string());
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

        filename = object["filename"].asString();
        displayName = object["displayName"].asString();
        region = object["region"].asString();

        const Json::Value& translationsJSON = object["translations"];
        for (Json::Value::ArrayIndex i = 0; i < translationsJSON.size(); i++)
        {
            const Json::Value& translation = translationsJSON[i];
            translations.insert(std::pair<std::string, std::string>(translation[0].asString(), translation[1].asString()));
        }

        return true;
    }

    void Language::loadLanguageList()
    {
        try
        {
            languageList.clear();
           for (const auto& entry : std::filesystem::directory_iterator(getPath(ConfigFiles::LanguageFile)))
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

    std::string Language::getTranslation(const std::string& key)
    {
        if (loadedLanguage != nullptr)
        {
            if (loadedLanguage->translations.contains(key))
            {
                return loadedLanguage->translations.at(key);
            }
            return key;
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
}
