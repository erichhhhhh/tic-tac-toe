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
	catch (std::exception e)
        {
            printError(e.what());
            return 1;
        }
    return 0;
}
