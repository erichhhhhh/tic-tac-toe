#pragma once
#include "Enums.h"
#include <array>
#include <string>
#include <exception>

class Field
{
public:
	void setPlayAt(int pos, PlayerID playerID);
	void unsetPlayAt(int pos);
	PlayerID getPlayAt(int pos);
	Field();

	bool empty(int pos);
	bool reset();
	int8_t fullness();
	Field& operator=(const Field& field);

private:
	std::array<PlayerID, 9> field;
};

class FieldIOException : public std::exception
{
	std::string msg;
public:
	FieldIOException(const int pos, PlayerID playerID) : msg(std::to_string(pos) + std::to_string(static_cast<int>(playerID))) {};
	const char* what() const throw();
};