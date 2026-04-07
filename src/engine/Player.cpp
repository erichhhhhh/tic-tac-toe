#include <stdexcept>
#define NOMINMAX

#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <algorithm>
#include <random>

#include "Player.h"
#include "Field.h"
#include "Engine.h"

bool Player::isComputer()
{
	return Player::type == PlayerType::COMPUTER ? true : false;
}

PlayerID Player::getPlayerID()
{
	return playerID;
}

MinimaxRole Player::getMinimaxRole()
{
	return minimaxRole;
}

WinningDetection Player::getWinningDetection()
{
	return winningDetection;
}

Player::Player(PlayerID playerID, PlayerType type, MinimaxRole minimaxRole, WinningDetection winningDetection)
{
	if ((playerID != PlayerID::PLAYER1) && (playerID != PlayerID::PLAYER2))
	{
		throw std::invalid_argument("Player cannot be EMPTY!");
	}
	else
	{
		this->playerID = playerID;
		this->type = type;
		this->minimaxRole = minimaxRole;
		this->winningDetection = winningDetection;
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

namespace PlayerAI
{
	int8_t minimax(Field& origField, Player& player)
	{
		Field field = origField;

		int best = player.getMinimaxRole() == MinimaxRole::MIN ? 1000 : -1000;
		int8_t move8 = -1;


		if (PlayerAI::stopWin(field, player.getPlayerID(), player.getMinimaxRole(), player.getWinningDetection()) != -1)
		{
			move8 = PlayerAI::stopWin(field, player.getPlayerID(), player.getMinimaxRole(), player.getWinningDetection());
			best = 100;
		}
		else if (PlayerAI::stopWin(field, !player.getPlayerID(), player.getMinimaxRole(), player.getWinningDetection()) != -1)
		{
			move8 = PlayerAI::stopWin(field, !player.getPlayerID(), player.getMinimaxRole(), player.getWinningDetection());
			best = 100;
		}

		for (int i = 0; i < 9; i++)
		{
			if (field.empty(i))
			{
				field.setPlayAt(i, player.getPlayerID());

				int move = PlayerAI::minimax(field, player.getPlayerID(), player.getPlayerID(), player.getMinimaxRole(), 0);

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
			if (player.getWinningDetection() == WinningDetection::DISABLED)
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
			if (field.empty(4) && player.getWinningDetection() != WinningDetection::DISABLED)
			{
				return 4;
			}
		}

		return move8;
	}

	int minimax(Field& field, PlayerID origPlayerID, PlayerID plPlayerID, MinimaxRole minimaxRole, const int& depth)
	{

		GameStatus gameStatus = LocalEngine::detectWinner(field);

		if (gameStatus == origPlayerID)
		{
			return 10 - depth;
		}
		else if (gameStatus == !origPlayerID)
		{
			return -10;
		}
		else if (gameStatus.isDraw())
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
					field.setPlayAt(i, plPlayerID);

					best = std::max(best, minimax(field, origPlayerID, !plPlayerID, !minimaxRole, depth + 1));

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
					field.setPlayAt(i, plPlayerID);

					best = std::min(best, PlayerAI::minimax(field, origPlayerID, !plPlayerID, !minimaxRole, depth + 1));

					field.unsetPlayAt(i);
				}
			}
			return best;
		}
		return 0;
	}

	int stopWin(Field& origField, PlayerID plPlayerID, MinimaxRole minimaxRole, WinningDetection winningDetection)
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
					field.setPlayAt(i, !plPlayerID);

					GameStatus gameStatus = LocalEngine::detectWinner(field);

					field.unsetPlayAt(i);

					if (gameStatus == !plPlayerID)
					{
						return i;
					}
				}
				else if (minimaxRole == MinimaxRole::MAX)
				{
					field.setPlayAt(i, plPlayerID);

					GameStatus gameStatus = LocalEngine::detectWinner(field);

					field.unsetPlayAt(i);

					if (gameStatus == plPlayerID)
					{
						return i;
					}
				}
			}
		}

		return -1;
	}
}
