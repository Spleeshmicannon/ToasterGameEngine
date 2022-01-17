#pragma once
#include "symbols.h"
#include <string>
#include <vector>

__forceinline constexpr bool compareCharArr(const size_t size, const char* str0, const char* str1)
{
	for (int i = 0; i < size; ++i)
	{
		if(str0[i] != str1[i]) return false;
	}

	return true;
}

bool compareString(const std::string& str0, const std::string& str1)
{
	if (str0.length() != str1.length()) return false;
	return compareCharArr(str0.length(), str0.c_str(), str1.c_str());
}

std::vector<symbol> lex(const std::string& file)
{
	std::vector<symbol> symList;

	std::string data = "";

	bool string = false;

	for (int i = 0; i < file.length(); ++i)
	{
		data += file[i];

		if (!string)
		{
			if (compareString(data, SYM_SET))
			{
				symList.push_back(symbol(symbolType::SET, data));
				data = "";
			}
			else if (compareString(data, SYM_PAREN_OPEN))
			{
				symList.push_back(symbol(symbolType::PAREN_OPEN, data));
				data = "";
			}
			else if (compareString(data, SYM_PAREN_CLOSE))
			{
				symList.push_back(symbol(symbolType::PAREN_CLOSE, data));
				data = "";
			}
			else if (compareString(data, SYM_DEBUG_MESSAGE))
			{
				symList.push_back(symbol(symbolType::DEBUG_MESSAGE, data));
				data = "";
			}
			else if (compareString(data, SYM_STR0) || compareString(data, SYM_STR1))
			{
				data = "";
				string = true;
			}
			else if (compareString(data, SYM_NEWLINE))
			{
				symList.push_back(symbol(symbolType::NEWLINE, data));
				data = "";
			}
			else if (compareString(data, SYM_WHITESPACE))
			{
				data = "";
			}
		}
		else
		{
			if ((i + 1) < file.length())
			{
				if ((file[i + 1] == SYM_STR0[0]) || (file[i + 1] == SYM_STR1[0]))
				{
					symList.push_back(symbol(symbolType::STR_DATA, data));
					data = "";
					string = false;
					++i;
				}
			}
		}
	}

	return symList;
}