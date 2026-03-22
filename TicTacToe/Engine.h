#pragma once
#include <map>
#include <functional>
#include "Field.h"
#include "Player.h"
#include "Enums.h"

enum class Connectivity
{
	LOCAL,
	REMOTE
};

struct GameStatus {
	enum class GameResult
	{
		UNFINISHED = 0,
		PLAYER1 = 1,
		PLAYER2 = 2,
		DRAW = 3,
	};
	GameStatus() : result(GameResult::UNFINISHED) {}
	GameStatus(GameResult r) : result(r) {}

	operator FieldType() const;
	operator PlayerID() const;

	bool hasWinner() const;
	bool isDraw() const;
	bool isUnfinished() const;

private:
	GameResult result;
};

class IEngine;

struct EngineConfig
{
	struct PlayerSymbol
	{
		PlayerID x = PlayerID::PLAYER1;
		PlayerID o = PlayerID::PLAYER2;
	} playerSymbols;

	struct CallbackFunctions
	{
		std::function<void(IEngine&)> onUpdate;
		std::function<int8_t(IEngine&, bool)> onInteractiveTurn;
		std::function<void(IEngine&) > onNonInteractiveTurn;
	} callbacks;
	virtual ~EngineConfig() = default;
protected:
	Connectivity connectivity;
};

struct LocalEngineConfig : EngineConfig
{
	Difficulty difficulty = Difficulty::EASY;
	GameType gameType = GameType::PvP;

	LocalEngineConfig()
	{
		connectivity = Connectivity::LOCAL;
	}
};

class IEngine 
{
public:
	virtual ~IEngine() = default; // If unset only parent class gets deleted
	virtual bool launchGame() = 0;
	virtual bool sendSettings(const EngineConfig& engineConfig) = 0;
	virtual Symbol getSymbol(const PlayerID playerID) = 0;
	virtual std::array<PlayerID, 9> getField() = 0;
	virtual GameStatus getStatus() const = 0;
	virtual PlayerID getTurn() const = 0;
};

class LocalEngine : public IEngine 
{
	std::map<PlayerID, Player> playerlist;
	Field field;
	GameStatus gameStatus;
	std::map<PlayerID, Symbol> symbolMap;
	LocalEngineConfig::CallbackFunctions callbacks;
	Difficulty difficulty = Difficulty::EASY;
	GameType gameType = GameType::PvP;
	PlayerID turn = PlayerID::NONE;

	void addPlayer(PlayerID playerID, Player&& player);
	void flushPlayerlist();
	Player& getPlayer(PlayerID playerID);
	void updateGameStatus();
	bool checkSettings(const LocalEngineConfig& engineConfig);


public:
	bool launchGame() override;
	bool sendSettings(const EngineConfig& engineConfig) override;
	Symbol getSymbol(const PlayerID playerID) override;
	std::array<PlayerID, 9> getField() override;
	GameStatus getStatus() const override;
	PlayerID getTurn() const override;

	static GameStatus detectWinner(Field& field);
};

class RemoteEngine : public IEngine 
{
public:
	bool launchGame() override;
	bool sendSettings(const EngineConfig& engineConfig) override;
};