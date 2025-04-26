#pragma once
#include <map>
#include <string>
#include "Field.h"

class Player 
{
	private:
		enum class Symbol symbol;
		enum class PlayerType type;
		enum class FieldType playerFieldType;
		enum class MinimaxRole minimaxRole;
		enum class WinningDetection winningDetection;

		static inline std::map<enum FieldType, Player> playerlist;

	public:
		static int8_t minimax(Field& field, Player& player);
		static int minimax(Field& origField, FieldType origFieldType, FieldType plFieldType, MinimaxRole minimaxRole, const int& depth);
		static int stopWin(Field& origField, FieldType plFieldType, MinimaxRole minimaxRole, WinningDetection winningDetection);

		bool isComputer();
		enum class Symbol getSymbol();
		enum class FieldType getPlayerFieldType();
		std::string getSymbolString();
		enum class MinimaxRole getMinimaxRole();
		enum class WinningDetection getWinningDetection();

		static Player& getPlayer(enum class FieldType fieldType);

		static void addPlayer(enum class FieldType fieldType, Player& player);
		static void flushPlayerlist();
		void playerInput(Player& player, Field& field);
		void play(Field& field);
		Player(enum class FieldType playerFieldType, enum class PlayerType type, enum class Symbol symbol, enum class MinimaxRole minimaxRole, enum class WinningDetection winningDetection);
};

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

enum class Symbol
{
	X,
	O
};

enum class MinimaxRole
{
	NOPART,
	MIN,
	MAX
};

MinimaxRole operator!(const MinimaxRole& other);