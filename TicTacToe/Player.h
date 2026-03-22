#pragma once
#include <map>
#include <string>
#include "Field.h"

enum class WinningDetection
{
	NONE,
	DISABLED,
	ENABLED
};

enum class PlayerType
{
	PLAYER,
	COMPUTER
};

enum class MinimaxRole
{
	NOPART,
	MIN,
	MAX
};

MinimaxRole operator!(const MinimaxRole& other);

class Player 
{
	private:
		PlayerType type;
		FieldType playerFieldType;
		MinimaxRole minimaxRole;
		WinningDetection winningDetection;

	public:


		bool isComputer();
		FieldType getPlayerFieldType();
		MinimaxRole getMinimaxRole();
		WinningDetection getWinningDetection();

		Player(FieldType playerFieldType, PlayerType type, MinimaxRole minimaxRole, WinningDetection winningDetection);
};

namespace PlayerAI
{
	int8_t minimax(Field& field, Player& player);
	int minimax(Field& origField, FieldType origFieldType, FieldType plFieldType, MinimaxRole minimaxRole, const int& depth);
	int stopWin(Field& origField, FieldType plFieldType, MinimaxRole minimaxRole, WinningDetection winningDetection);

};