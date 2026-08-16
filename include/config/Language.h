#pragma once
#include <Config.h>

namespace Config
{
	class Language : public AbstractConfig
	{
	private:
		std::string filename;
		std::string displayName;
		std::string region;
		bool functional = false;

		std::map<std::string, std::string> translations;
		std::map<std::string, char> maskedCharacter;

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
		std::string getFilename();
		std::string getDisplayName();
		std::string getRegion();
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
