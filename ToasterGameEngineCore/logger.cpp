#include "Logger.h"
#include "memory.h"

#define FILE_PATH "logs.txt"

namespace toast
{
	Logger::Logger() : logFile(tnew<fstream<cv>>(1))
	{}

	Logger::~Logger()
	{
		*logFile << "\n-------------------- session closed --------------------\n\n";

		if (logFile->is_open())
		{
			logFile->close();
		}

		tdelete<fstream<cv>>(logFile);
	}

	void Logger::clearCachedLogs()
	{
		if (logFile->is_open())
		{
			logFile->close();

			logFile->open(FILE_PATH, std::ios::out | std::ios::trunc);
			logFile->close();

			open();
		}
		else
		{
			logFile->open(FILE_PATH, std::ios::out | std::ios::trunc);
			logFile->close();
		}
	}

	void Logger::open()
	{
		const tm local_tm = getTimeStamp();

		// figure out how to change this to str<char> at some point
		// maybe write your own to_string function

		logFile->open(FILE_PATH, std::ios::app);

		*logFile << "--------------------session opened--------------------\n\n"
				<< "Session Begin TimeStamp: "
				<< std::to_string(local_tm.tm_mday) <<
			" " << std::to_string(local_tm.tm_mon) <<
			" " << std::to_string(local_tm.tm_year + 1900) <<
			" | " << std::to_string(local_tm.tm_hour) <<
			":" << std::to_string(local_tm.tm_min) <<
			":" << std::to_string(local_tm.tm_sec) << "\n\n";
	}
}