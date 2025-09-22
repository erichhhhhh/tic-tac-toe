#include <vector>
#include <format>
#include "FieldAnalyser.h"
#include "Player.h"
#include "Config.h"

WinningType detectWinner(Field& field)
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

        FieldType scanningItem = i == 0 ? FieldType::PLAYER1 : FieldType::PLAYER2;

        for (int h = 0; h < 8; h++)
        {
            if (field.getPlayAt(testingValues[h][0]) == scanningItem && field.getPlayAt(testingValues[h][1]) == scanningItem && field.getPlayAt(testingValues[h][2]) == scanningItem)
            {
                if (scanningItem == FieldType::PLAYER1)
                {
                    return WinningType::PLAYER1;
                }
                else if (scanningItem == FieldType::PLAYER2)
                {
                    return WinningType::PLAYER2;
                }
            }
        }
    }
    if (
        field.getPlayAt(0) != FieldType::EMPTY &&
        field.getPlayAt(1) != FieldType::EMPTY &&
        field.getPlayAt(2) != FieldType::EMPTY &&
        field.getPlayAt(3) != FieldType::EMPTY &&
        field.getPlayAt(4) != FieldType::EMPTY &&
        field.getPlayAt(5) != FieldType::EMPTY &&
        field.getPlayAt(6) != FieldType::EMPTY &&
        field.getPlayAt(7) != FieldType::EMPTY &&
        field.getPlayAt(8) != FieldType::EMPTY
        )
    {
        return WinningType::DRAW;
    }

    return WinningType::UNFINISHED;
}

std::string printWinningMessage(WinningType winner)
{
    if (winner == WinningType::UNFINISHED)
    {
        throw NotWonException();
    }
    else if (winner == WinningType::DRAW)
    {
        return Config::Language::getTranslation("game.draw");
    }
    else if (winner == WinningType::PLAYER1 || winner == WinningType::PLAYER2)
    {
        Player player = winner == WinningType::PLAYER1 ? Player::getPlayer(FieldType::PLAYER1) : Player::getPlayer(FieldType::PLAYER2);
        std::string symbol = Config::Language::getTranslation(player.getSymbolString());
        return std::vformat(Config::Language::getTranslation("game.win"), std::make_format_args(symbol));
    }
    else
    {
        throw std::exception("Enum could not be compared");
    }
}

std::string printWinningMessage(Field& field)
{
    WinningType winningType = detectWinner(field);
    return printWinningMessage(winningType);
}

bool isGameWon(Field& field)
{
    WinningType winningType = detectWinner(field);

    if (winningType == WinningType::UNFINISHED)
    {
        return false;
    }
    else
    {
        return true;
    }
}

FieldType toFieldType(WinningType wtype)
{
    switch (wtype)
    {
    case WinningType::PLAYER1:
        return FieldType::PLAYER1;
        break;
    case WinningType::PLAYER2:
        return FieldType::PLAYER2;
        break;
    default:
        return FieldType::EMPTY;
        break;
    }
}

WinningType toWinningType(FieldType ftype)
{
    switch (ftype)
    {
    case FieldType::PLAYER1:
        return WinningType::PLAYER1;
        break;
    case FieldType::PLAYER2:
        return WinningType::PLAYER2;
        break;
    default:
        return WinningType::UNFINISHED;
    }
}

const char* NotWonException::what() const throw()
{
    return "Game was not finished!";
}
