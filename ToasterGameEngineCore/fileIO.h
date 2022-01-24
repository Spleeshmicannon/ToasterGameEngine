#include <string>

#include "macros.h"
#include "types/string.h"
#include "logger.h"

namespace toast
{
	TINLINE str<char> readFile(const str<char> filename)
	{
		str<char> data;

		std::getline(std::ifstream(filename), data, '\0');

		if (data.size() == 0)
		{
			Logger::staticLog<logLevel::TERROR>("No File Found: " + filename);
		}

		return data;
	}
}