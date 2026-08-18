#include <Language.h>

#include <en-US.h>

#include <fstream>
#include <json/json.h>
namespace Config
{
    Language::Language(std::string name)
    {
        type = ConfigFiles::LanguageFile;
        path = getPath(ConfigFiles::LanguageFile, name + ".json");
        version = 2;
    }

    bool Language::toConfig(Json::Value& input)
    {
        if (input["version"].asInt() == 1 || input["version"].isNull())
        {
            return false;
        }

        filename = input["filename"].asString();
        displayName = input["displayName"].asString();
        region = input["region"].asString();

        const Json::Value& translationsJSON = input["translations"];
        for (Json::Value::ArrayIndex i = 0; i < translationsJSON.size(); i++)
        {
            const Json::Value& translation = translationsJSON[i];
            translations.insert(std::pair<std::string, std::string>(translation[0].asString(), translation[1].asString()));
        }

        return true;
    }

    void Language::provideEmergencyTranslations()
    {
	    const std::string jsonString(emergency_translation);
	    Json::Value object;
	   
	    if(!Json::Reader().parse(jsonString, object))
		    return;

	    const Json::Value& translationsJSON = object["translations"];
	    for (Json::Value::ArrayIndex i = 0; i < translationsJSON.size(); i++)
	    {
		    const Json::Value& translation = translationsJSON[i];
		    emergencyTranslations.insert(std::pair<std::string, std::string>(translation[0].asString(), translation[1].asString()));
	    }
    }

    bool Language::loadLanguageList()
    {
        try
        {
            languageList.clear();
           for (const auto& entry : std::filesystem::directory_iterator(getPath(ConfigFiles::LanguageFile)))
            {
                Language lang = Language(entry.path().stem().string());
                if(lang.deserialize())
	                languageList.push_back(lang);
            }
	   return true;
        }
        catch (std::filesystem::filesystem_error& e)
        {
            languageList.clear();
	    return false;
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
	bool success = lang.deserialize();
        loadedLanguage = std::make_unique<Language>(lang);
	if(loadedLanguage == nullptr || !success)
	        return false;
	return true;
    }

    std::string Language::getTranslation(const std::string& key)
    {
        if (loadedLanguage != nullptr)
        {
            if (loadedLanguage->translations.contains(key))
            	return loadedLanguage->translations.at(key);
        }

	if(emergencyTranslations.empty())
		provideEmergencyTranslations();

	if(emergencyTranslations.contains(key))
	{
		return emergencyTranslations.at(key);
	}
	else
	{
		return key;
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
}
