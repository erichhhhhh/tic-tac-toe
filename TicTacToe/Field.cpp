#include <array>
#include "Field.h"

void Field::setPlayAt(int pos, enum FieldType fieldType)
{
	if (pos > 8 || pos < 0) 
	{
		throw FieldIOException(pos, fieldType);
	}

	FieldType playAt = this->getPlayAt(pos);

	if (playAt != FieldType::EMPTY)
	{
		throw FieldIOException(pos, fieldType);
	}

	Field::field.at(pos) = fieldType;
}

void Field::unsetPlayAt(int pos)
{
	if (pos > 8 || pos < 0)
	{
		throw FieldIOException(pos, FieldType::EMPTY);
	}

	Field::field.at(pos) = FieldType::EMPTY;
}

enum FieldType Field::getPlayAt(int pos)
{
	if (pos < 9 && pos >= 0) 
	{
		return Field::field.at(pos);
	}
	else
	{
		return FieldType::EMPTY;
	}
}

Field::Field()
{
	this->reset();
}

bool Field::empty(int pos)
{
	if (this->getPlayAt(pos) == FieldType::EMPTY)
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
		Field::field.at(i) = FieldType::EMPTY;
	}
	return true;
}

int8_t Field::fullness()
{
	int8_t fullness = 0;

	for (FieldType ftype : field)
	{
		if (ftype != FieldType::EMPTY)
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

FieldType operator!(const FieldType& fieldType)
{
	if (fieldType == FieldType::PLAYER1)
	{
		return FieldType::PLAYER2;
	}
	else if (fieldType == FieldType::PLAYER2)
	{
		return FieldType::PLAYER1;
	}
	return FieldType::EMPTY;
}
