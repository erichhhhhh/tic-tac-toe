#include <vector>
#include "FieldAnalyser.h"
#include "Player.h"

enum WinningType detectWinner(Field& field)
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

    return WinningType::NOT_WON;
}

std::string printWinningMessage(enum WinningType winner)
{
    if (winner == WinningType::NOT_WON)
    {
        throw NotWonException();
    }
    else if (winner == WinningType::DRAW)
    {
        return "Das Spiel ging unentschieden aus";
    }
    else if (winner == WinningType::PLAYER1)
    {
        return Player::getPlayer(FieldType::PLAYER1).getSymbolString() + " hat gewonnen";
    }
    else if (winner == WinningType::PLAYER2)
    {
        return Player::getPlayer(FieldType::PLAYER2).getSymbolString() + " hat gewonnen";
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

    if (winningType == WinningType::NOT_WON)
    {
        return false;
    }
    else
    {
        return true;
    }
}

enum FieldType toFieldType(enum WinningType wtype)
{
    int iWType = static_cast<int>(wtype);
    return static_cast<FieldType>(iWType);
}

enum WinningType toWinningType(enum FieldType ftype)
{
    int iFType = static_cast<int>(ftype);
    
    if (iFType == 1 || iFType == 2)
    {
        return static_cast<WinningType>(iFType);
    }
    else
    {
        return WinningType::NOT_WON;
    }
}

const char* NotWonException::what() const throw()
{
    return "Game was not finished!";
}
