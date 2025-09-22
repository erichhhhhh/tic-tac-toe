#define NOMINMAX

#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <algorithm>
#include <random>

#include "Player.h"
#include "FieldAnalyser.h"
#include "Field.h"
#include "UserInteraction.h"


int8_t Player::minimax(Field& origField, Player& player)
{
	Field field = origField;

	int best = player.minimaxRole == MinimaxRole::MIN ? 1000 : -1000;
	int8_t move8 = -1;


	if (stopWin(field, player.playerFieldType, player.getMinimaxRole(), player.winningDetection) != -1)
	{
		move8 = stopWin(field, player.playerFieldType, player.getMinimaxRole(), player.winningDetection);
		best = 100;
	}
	else if (stopWin(field, !player.getPlayerFieldType(), player.getMinimaxRole(), player.winningDetection) != -1)
	{
		move8 = stopWin(field, !player.getPlayerFieldType(), player.getMinimaxRole(), player.winningDetection);
		best = 100;
	}

	for (int i = 0; i < 9; i++)
	{
		if (field.empty(i))
		{
			field.setPlayAt(i, player.getPlayerFieldType());

			int move = minimax(field, player.getPlayerFieldType(), player.getPlayerFieldType(), player.getMinimaxRole(), 0);

			field.unsetPlayAt(i);

			if (player.getMinimaxRole() == MinimaxRole::MAX)
			{
				if (move > best)
				{
					move8 = i;
					best = move;
				}
			}
			else
			{
				if (move < best)
				{
					move8 = i;
					best = move;
				}
			}
		}
	}

	if (field.fullness() == 0)
	{
		if (player.winningDetection == WinningDetection::DISABLED)
		{
			return std::rand() % 9;
		}

		std::vector<int8_t> places = { 1, 3, 7, 9 };

		std::shuffle(places.begin(), places.end(), std::default_random_engine());
		
		for (int8_t place : places)
		{
			if (field.empty(place))
			{
				return place;
				break;
			}
		}
	}
	else if (field.fullness() == 1)
	{
		if (field.empty(4) && player.winningDetection != WinningDetection::DISABLED)
		{
			return 4;
		}
	}

	return move8;
}

int Player::minimax(Field& field, FieldType origFieldType, FieldType plFieldType, MinimaxRole minimaxRole, const int& depth)
{

	WinningType winner = detectWinner(field);

	if (winner == toWinningType(origFieldType))
	{
		return 10 - depth;
	}
	else if (winner == toWinningType(!origFieldType))
	{
		return -10;
	}
	else if (winner == WinningType::DRAW)
	{
		return 0 - depth;
	}

	if (minimaxRole == MinimaxRole::MAX)
	{
		int best = -1000;

		for (int i = 0; i < 9; i++)
		{
			if (field.empty(i))
			{
				field.setPlayAt(i, plFieldType);

				best = std::max(best, minimax(field, origFieldType, !plFieldType, !minimaxRole, depth + 1));

				field.unsetPlayAt(i);
			}
		}
		return best;
	}
	else if (minimaxRole == MinimaxRole::MIN)
	{
		int best = 1000;

		for (int i = 0; i < 9; i++)
		{
			if (field.empty(i))
			{
				field.setPlayAt(i, plFieldType);
				
				best = std::min(best, minimax(field, origFieldType, !plFieldType, !minimaxRole, depth + 1));

				field.unsetPlayAt(i);
			}
		}
		return best;
	}
	return 0;
}

int Player::stopWin(Field& origField, FieldType plFieldType, MinimaxRole minimaxRole, WinningDetection winningDetection)
{
	if (winningDetection == WinningDetection::DISABLED)
	{
		return -1;
	}

	Field field = origField;

	for (int i = 0; i < 9; i++)
	{
		if (field.empty(i))
		{
			if (minimaxRole == MinimaxRole::MIN)
			{
				field.setPlayAt(i, !plFieldType);

				WinningType winner = detectWinner(field);

				field.unsetPlayAt(i);

				if (winner == toWinningType(!plFieldType))
				{
					return i;
				}
			}
			else if (minimaxRole == MinimaxRole::MAX)
			{
				field.setPlayAt(i, plFieldType);

				WinningType winner = detectWinner(field);

				field.unsetPlayAt(i);

				if (winner == toWinningType(plFieldType))
				{
					return i;
				}
			}
		}
	}

	return -1;
}

bool Player::isComputer()
{
	return Player::type == PlayerType::COMPUTER ? true : false;
}

enum class Symbol Player::getSymbol()
{
	return Player::symbol;
}

enum FieldType Player::getPlayerFieldType()
{
	return Player::playerFieldType;
}

std::string Player::getSymbolString()
{
	switch (Player::symbol)
	{
	case Symbol::X:
		return "symbol.X";
	case Symbol::O:
		return "symbol.O";
	default:
		throw std::exception();
	}
}

enum class MinimaxRole Player::getMinimaxRole()
{
	return minimaxRole;
}

WinningDetection Player::getWinningDetection()
{
	return winningDetection;
}

void Player::addPlayer(enum FieldType fieldType, Player& player)
{
	playerlist.insert(std::pair<enum FieldType, Player>(fieldType, player));
}

void Player::flushPlayerlist()
{
	playerlist.clear();
}

Player& Player::getPlayer(enum FieldType fieldType)
{
	return playerlist.at(fieldType);
}

void Player::play(Field& field)
{
	if (this->isComputer())
	{
		field.setPlayAt(
			minimax(field, *this),
			this->playerFieldType
			);
	} 
	else 
	{
		playerInput(*this, field);
	}
}

void Player::playerInput(Player& player, Field& field)
{
	std::map<int8_t, int8_t> keymap = { {7, 0}, {8, 1}, {9, 2}, {4, 3}, {5, 4}, {6, 5}, {1, 6}, {2, 7}, {3, 8} };
	bool inputSuccessful = false;

	while (!inputSuccessful)
	{
		int i_input = numericInput<int>();
		try
		{
			field.setPlayAt(keymap.at(i_input), player.getPlayerFieldType());
			inputSuccessful = true;
		}
		catch (std::exception e)
		{
			std::cout << Config::Language::getTranslation("misc.wrong_input") << std::endl;
		}
	}
}

Player::Player(enum FieldType playerFieldType, enum PlayerType type, enum Symbol symbol, enum class MinimaxRole minimaxRole, enum class WinningDetection winningDetection)
{
	if ((playerFieldType != FieldType::PLAYER1) && (playerFieldType != FieldType::PLAYER2))
	{
		throw std::exception("Player cannot be EMPTY!");
	}
	else
	{
		this->playerFieldType = playerFieldType;
		this->type = type;
		this->symbol = symbol;
		this->minimaxRole = minimaxRole;
		this->winningDetection = winningDetection;
		Player::addPlayer(this->playerFieldType, *this);
	}
}

MinimaxRole operator!(const MinimaxRole& other)
{
	switch (other)
	{
		case MinimaxRole::MIN:
			return MinimaxRole::MAX;
			break;
		case MinimaxRole::MAX:
			return MinimaxRole::MIN;
			break;
		default:
			return other;
			break;
	}
}
