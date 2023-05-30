#pragma once
#include <string>

struct FieldType
{
	std::string type = 0;
};

struct WinningType
{
	std::string type = 0;
};

struct Player : FieldType, WinningType
{
	std::string type = "PLAYER";
};

struct Computer : FieldType, WinningType
{
	std::string type = "COMPUTER";
};

struct NotDone : WinningType
{
	std::string type = "NOT_DONE";
};

struct Draw : WinningType
{
	std::string type = "DRAW";
};

struct Empty : FieldType
{
	std::string type = "EMPTY";
};