#include "Field.h"
#include "FieldAnalyser.h"
#include "Player.h"
#include "Config.h"
#include "UserInteraction.h"

int main()
{
    Config::Config config;

    bool result = config.deserialize();

    mainMenu(!result, config);

    return 0;
}
