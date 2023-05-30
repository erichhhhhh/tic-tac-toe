#pragma once
#include <map>
#include <string>
#include "Field.h"

class Player 
{
	private:
		enum class Symbol symbol;
		enum class PlayerType type;
		enum FieldType playerFieldType;
		std::string symbolString;
		enum class MinimaxRole minimaxRole;
		enum class WinningDetection winningDetection;

		static inline std::map<enum FieldType, Player> playerlist;
		Player(enum class FieldType playerFieldType, enum class PlayerType type, enum class Symbol symbol, std::string symbolString, enum class MinimaxRole minimaxRole, enum class WinningDetection winningDetection) : playerFieldType(playerFieldType), type(type), symbol(symbol), symbolString(symbolString), minimaxRole(minimaxRole), winningDetection(winningDetection) {}

	public:
		static int8_t minimax(Field& field, Player& player);
		static int minimax(Field& origField, FieldType origFieldType, FieldType plFieldType, MinimaxRole minimaxRole, const int& depth);
		static int stopWin(Field& origField, FieldType plFieldType, MinimaxRole minimaxRole, WinningDetection winningDetection);

		bool isComputer();
		enum class Symbol getSymbol();
		enum FieldType getPlayerFieldType();
		std::string getSymbolString();
		enum class MinimaxRole getMinimaxRole();
		enum class WinningDetection getWinningDetection();

		static Player& getPlayer(enum FieldType fieldType);

		static void addPlayer(enum FieldType fieldType, Player& player);
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