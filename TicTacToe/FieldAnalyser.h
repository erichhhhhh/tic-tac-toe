#pragma once
#include <exception>
#include <array>
#include <string>
#include "Field.h"

enum class WinningType
{
	NOT_WON = 0,
	PLAYER1 = 1,
	PLAYER2 = 2,
	DRAW = 3
};

enum WinningType detectWinner(Field& field);
std::string printWinningMessage(enum WinningType winner);
std::string printWinningMessage(Field& field);
bool isGameWon(Field& field);
enum FieldType toFieldType(enum WinningType wtype);
enum WinningType toWinningType(enum FieldType ftype);

class NotWonException : public std::exception
{
public:
	const char* what() const throw ();
};
