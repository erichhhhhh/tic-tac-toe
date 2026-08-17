#pragma once
enum class PlayerID
{
	NONE,
	PLAYER1,
	PLAYER2
};

inline PlayerID operator!(PlayerID playerID)
{
	switch (playerID)
	{
	case PlayerID::PLAYER1:
		return PlayerID::PLAYER2;
	case PlayerID::PLAYER2:
		return PlayerID::PLAYER1;
	default:
		return PlayerID::NONE;
	}
}

enum class Symbol
{
	X,
	O
};

inline Symbol operator!(Symbol symbol)
{
	switch (symbol)
	{
		case Symbol::X:
			return Symbol::O;
		case Symbol::O:
			return Symbol::X;
	}
	return Symbol::X;
}

enum class Difficulty
{
	EASY,
	MIDDLE,
	HARD
};

enum class GameType
{
	PvP,
	PvAI,
	AIvAI
};
