#include <array>
#include "Field.h"

void Field::setPlayAt(int pos, PlayerID playerID)
{
	if (pos > 8 || pos < 0) 
	{
		throw FieldIOException(pos, playerID);
	}

	PlayerID playAt = this->getPlayAt(pos);

	if (playAt != PlayerID::NONE)
	{
		throw FieldIOException(pos, playerID);
	}

	Field::field.at(pos) = playerID;
}

void Field::unsetPlayAt(int pos)
{
	if (pos > 8 || pos < 0)
	{
		throw FieldIOException(pos, PlayerID::NONE);
	}

	Field::field.at(pos) = PlayerID::NONE;
}

PlayerID Field::getPlayAt(int pos)
{
	if (pos < 9 && pos >= 0) 
	{
		return Field::field.at(pos);
	}
	else
	{
		throw FieldIOException(pos, PlayerID::NONE);
	}
}

Field::Field()
{
	this->reset();
}

bool Field::empty(int pos)
{
	if (this->getPlayAt(pos) == PlayerID::NONE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Field::reset()
{
	for(int i = 0; i < Field::field.size(); i++)
	{
		Field::field.at(i) = PlayerID::NONE;
	}
	return true;
}

int8_t Field::fullness()
{
	int8_t fullness = 0;

	for (PlayerID playerID : field)
	{
		if (playerID != PlayerID::NONE)
		{
			fullness++;
		}
	}

	return fullness;
}

Field& Field::operator=(const Field& field)
{
	this->field = field.field;

	return *this;
}

const char* FieldIOException::what() const throw()
{
	return msg.c_str();
}
