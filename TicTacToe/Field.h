#pragma once
#include <array>
#include <string>
#include <exception>

class Field
{
public:
	void setPlayAt(int pos, enum class FieldType fieldType);
	void unsetPlayAt(int pos);
	enum class FieldType getPlayAt(int pos);
	Field();

	bool empty(int pos);
	int8_t fullness();
	Field& operator=(const Field& field);

private:
	std::array<enum class FieldType, 9> field;
};

class FieldIOException : public std::exception
{
	std::string msg;
public:
	FieldIOException(const int pos, enum class FieldType field) : msg(std::to_string(pos) + std::to_string(static_cast<int>(field))) {};
	const char* what() const throw();
};

enum class FieldType 
{
	EMPTY,
	PLAYER1,
	PLAYER2
};

FieldType operator!(const FieldType& fieldType);