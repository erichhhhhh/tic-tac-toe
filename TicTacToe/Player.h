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
		PlayerID playerID;
		MinimaxRole minimaxRole;
		WinningDetection winningDetection;

	public:


		bool isComputer();
		PlayerID getPlayerID();
		MinimaxRole getMinimaxRole();
		WinningDetection getWinningDetection();

		Player(PlayerID playerID, PlayerType type, MinimaxRole minimaxRole, WinningDetection winningDetection);
};

namespace PlayerAI
{
	int8_t minimax(Field& field, Player& player);
	int minimax(Field& origField, PlayerID origPlayerID, PlayerID plPlayerID, MinimaxRole minimaxRole, const int& depth);
	int stopWin(Field& origField, PlayerID plPlayerID, MinimaxRole minimaxRole, WinningDetection winningDetection);

};