#pragma once
#include <Config.h>
#include <Enums.h>

#include <unordered_map>
#include <vector>
namespace Config
{
	class Language : public AbstractConfig
	{
	private:
		std::string filename;
		std::string displayName;
		std::string region;

		std::unordered_map<std::string, std::string> translations;
		static inline std::unordered_map<std::string, std::string> emergencyTranslations;

		static inline std::unique_ptr<Language> loadedLanguage = nullptr;
		static inline std::vector<Language> languageList;

		bool toConfig(Json::Value& input) override;
		Json::Value toJSON() override { return ""; }

		Language(std::string name);

		static bool loadLanguageList();
		static void provideEmergencyTranslations();
	public:

		static std::vector<Language> getLanguageList(const bool reload = false);
		static bool loadLanguage(std::string name);
		static std::string getTranslation(const std::string& key);
		std::string getFilename();
		std::string getDisplayName();
		std::string getRegion();
		template<typename T>
		static std::string getEnumTranslation(T value)
		{
			if constexpr (std::is_same_v<T, Symbol>)	
			{
				switch (value)
				{
					case Symbol::X:
						return getTranslation("symbol.X");
					case Symbol::O:
						return getTranslation("symbol.O");
				}
			}
			else if constexpr (std::is_same_v<T, Difficulty>)
			{
				switch (value)
				{
					case Difficulty::EASY:
						return getTranslation("settings.difficulty.easy");
					case Difficulty::MIDDLE:
						return getTranslation("settings.difficulty.middle");
					case Difficulty::HARD:
						return getTranslation("settings.difficulty.hard");
				}
			}
			else if constexpr (std::is_same_v<T, GameType>)
			{
				switch (value)
				{
					case GameType::PvP:
						return getTranslation("settings.game_type.p_v_p");
					case GameType::PvAI:
						return getTranslation("settings.game_type.p_v_ai");
					case GameType::AIvAI:
						return getTranslation("settings.game_type.ai_v_ai");
				}
			}
			return "";
		}
	};
}
