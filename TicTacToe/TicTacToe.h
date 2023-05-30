#pragma once
#include <vector>
#include <string>
#include <array>

std::string divisionLine(int length);
std::string table(std::vector<int> xandos, bool playerIsX);
void computer_sTurn(std::vector<int>& xandos);
std::vector<int> fieldToXandos(std::array<FieldType, 9> field);
void applyXandos(std::vector<int>& xandos, Field& field);
int main();