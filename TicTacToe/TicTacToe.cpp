#include "Field.h"
#include "FieldAnalyser.h"
#include "Player.h"
#include "Config.h"
#include "UserInteraction.h"

int main()
{
    Config::Config config;

    bool result = config.deserialize();
    try
    {
        Config::Language::loadLanguage(config.getLanguage());
    }
    catch (Config::LanguageNotReadableException e)
    {
        printError(e.what());
        return 121;
    }
    
    try
    {
        mainMenu(!result, config);
    }
    catch (Config::LanguageNotReadableException e)
    {
        printError(e.what());
        return 121;
    }
    catch (std::exception e)
    {
        printError(e.what());
        return 1;
    }

    return 0;
}
