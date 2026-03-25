#include "Engine.h"
#include <exception>
bool LocalEngine::checkSettings(const LocalEngineConfig& engineConfig)
{
    if ((engineConfig.playerSymbols.x != PlayerID::PLAYER1 && engineConfig.playerSymbols.x != PlayerID::PLAYER2)
        || (engineConfig.playerSymbols.o != PlayerID::PLAYER1 && engineConfig.playerSymbols.o != PlayerID::PLAYER2))
    {
        return false;
    }
    if (engineConfig.playerSymbols.x == engineConfig.playerSymbols.o)
    {
        return false;
    }

    if (!(engineConfig.callbacks.onUpdate && engineConfig.callbacks.onNonInteractiveTurn && engineConfig.callbacks.onInteractiveTurn))
    {
        return false;
    }
    return true;
}

bool LocalEngine::launchGame()
{

    field.reset();

    callbacks.onUpdate(*this);

    turn = turnBeginValue; /* Player order is here changeable */
    while (getStatus().isUnfinished())
    {
        if (getPlayer(turn).isComputer())
        {
            int8_t targetField = PlayerAI::minimax(this->field, getPlayer(turn));
            callbacks.onNonInteractiveTurn(*this);
            this->field.setPlayAt(targetField, getPlayer(turn).getPlayerID());
        }
        else
        {
            bool inputValid = false;
            bool firstTry = true;
            int8_t input = 0;
            while (!inputValid)
            {
                input = callbacks.onInteractiveTurn(*this, firstTry);
                if (input >= 0 && input < 9 && this->field.empty(input))
                    inputValid = true;
                firstTry = false;
            }
            this->field.setPlayAt(input, getPlayer(turn).getPlayerID());
        }
        callbacks.onUpdate(*this);
        turn = !turn;
        updateGameStatus();
    }

    turn = PlayerID::NONE;
    return true;
}

bool LocalEngine::sendSettings(const EngineConfig& engineConfig)
{
    /* Check if is instance of LocalEngineConfig */
    const LocalEngineConfig* localEngineConfig = dynamic_cast<const LocalEngineConfig*>(&engineConfig);

    if (!localEngineConfig || !checkSettings(*localEngineConfig))
    {
        throw std::exception("Settings are invalid");
        return false;
    }

    this->callbacks = localEngineConfig->callbacks;

    this->symbolMap.emplace(localEngineConfig->playerSymbols.x, Symbol::X);
    this->symbolMap.emplace(localEngineConfig->playerSymbols.o, Symbol::O);
    this->difficulty = localEngineConfig->difficulty;
    this->gameType = localEngineConfig->gameType;
    if(localEngineConfig->firstPlayer != PlayerID::NONE)
        this->turnBeginValue = localEngineConfig->firstPlayer;

    /* Populate playerlist */
    flushPlayerlist();
    MinimaxRole role = MinimaxRole::NOPART;
    WinningDetection wd = WinningDetection::NONE;
    switch (this->difficulty)
    {
    case Difficulty::EASY:
        role = MinimaxRole::MIN;
        wd = WinningDetection::DISABLED;
        break;
    case Difficulty::MIDDLE:
        role = MinimaxRole::MAX;
        wd = WinningDetection::DISABLED;
        break;
    case Difficulty::HARD:
        role = MinimaxRole::MAX;
        wd = WinningDetection::ENABLED;
        break;
    default:
        break;
    }
    switch (this->gameType)
    {
    case GameType::PvP:
        addPlayer(PlayerID::PLAYER1, Player(PlayerID::PLAYER1, PlayerType::PLAYER, MinimaxRole::NOPART, WinningDetection::NONE));
        addPlayer(PlayerID::PLAYER2, Player(PlayerID::PLAYER2, PlayerType::PLAYER, MinimaxRole::NOPART, WinningDetection::NONE));
        break;
    case GameType::PvAI:
        addPlayer(PlayerID::PLAYER1, Player(PlayerID::PLAYER1, PlayerType::PLAYER, MinimaxRole::NOPART, WinningDetection::NONE));
        addPlayer(PlayerID::PLAYER2, Player(PlayerID::PLAYER2, PlayerType::COMPUTER, role, wd));
        break;
    case GameType::AIvAI:
        addPlayer(PlayerID::PLAYER1, Player(PlayerID::PLAYER1, PlayerType::COMPUTER, role, wd));
        addPlayer(PlayerID::PLAYER2, Player(PlayerID::PLAYER2, PlayerType::COMPUTER, role, wd));
        break;
    default:
        break;
    }

    return true;
}

Symbol LocalEngine::getSymbol(const PlayerID playerID)
{
    if (playerID == PlayerID::NONE)
        throw std::exception("Illegal SYMBOL retrieve: NONE has no SYMBOL");

    return symbolMap.at(playerID);
}

std::array<PlayerID, 9> LocalEngine::getField()
{
    std::array<PlayerID, 9> fieldArray;
    for (int8_t i = 0; i < 9; i++)
    {
        fieldArray.at(i) = (PlayerID)field.getPlayAt(i);
    }

    return fieldArray;
}

GameStatus LocalEngine::getStatus() const
{
    return gameStatus;
}

PlayerID LocalEngine::getTurn() const
{
    return turn;
}

void LocalEngine::addPlayer(PlayerID playerID, Player&& player)
{
	playerlist.emplace(playerID, std::move(player));
}

void LocalEngine::flushPlayerlist()
{
	playerlist.clear();
}

Player& LocalEngine::getPlayer(PlayerID playerID)
{
	return playerlist.at(playerID);
}

GameStatus LocalEngine::detectWinner(Field& field)
{
    int testingValues[8][3] =
    {
        { 0, 1, 2 },
        { 3, 4, 5 },
        { 6, 7, 8 },
        { 0, 3, 6 },
        { 1, 4, 7 },
        { 2, 5, 8 },
        { 0, 4, 8 },
        { 2, 4, 6 }
    };

    for (int i = 0; i < 2; i++)
    {

        PlayerID scanningItem = i == 0 ? PlayerID::PLAYER1 : PlayerID::PLAYER2;

        for (int h = 0; h < 8; h++)
        {
            if (field.getPlayAt(testingValues[h][0]) == scanningItem && field.getPlayAt(testingValues[h][1]) == scanningItem && field.getPlayAt(testingValues[h][2]) == scanningItem)
            {
                if (scanningItem == PlayerID::PLAYER1)
                {
                    return GameStatus(GameStatus::GameResult::PLAYER1);
                }
                else if (scanningItem == PlayerID::PLAYER2)
                {
                    return GameStatus(GameStatus::GameResult::PLAYER2);
                }
            }
        }
    }
    if (field.fullness() == 9)
    {
        return GameStatus(GameStatus::GameResult::DRAW);
    }

    return GameStatus(GameStatus::GameResult::UNFINISHED);
}

void LocalEngine::updateGameStatus() {
    gameStatus = detectWinner(field);
}

GameStatus::operator PlayerID() const
{
    switch (result)
    {
    case GameStatus::GameResult::PLAYER1:
        return PlayerID::PLAYER1;
    case GameStatus::GameResult::PLAYER2:
        return PlayerID::PLAYER2;
    default:
        return PlayerID::NONE;
    }
}

bool GameStatus::hasWinner() const
{
    return result == GameStatus::GameResult::PLAYER1 ? true 
        : result == GameStatus::GameResult::PLAYER2 ? true 
        : false;
}

bool GameStatus::isDraw() const
{
    return result == GameStatus::GameResult::DRAW ? true : false;
}

bool GameStatus::isUnfinished() const
{
    return result == GameStatus::GameResult::UNFINISHED ? true : false;
}
