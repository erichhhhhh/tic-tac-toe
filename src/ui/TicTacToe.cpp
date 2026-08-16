#include "UserInteraction.h"
#include "Config.h"
#include <Language.h>

int main()
{
    Config::Config config;
    bool result = config.deserialize();

    try
    {
        Config::Language::loadLanguage(config.getLanguage());
        mainMenu(!result, config);
    }
    catch (Config::LanguageNotReadableException e)
    {
        try
        {
            Config::Language::loadLanguage("en-US");
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
    }
    return 0;
}
