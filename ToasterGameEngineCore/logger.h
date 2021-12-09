#pragma once
// ToastEngine
#include "types/primitive.h"
#include "types/io.h"
#include "types/string.h"
#include "types/time.h"

#include "platform/platform.h"
#include "macros.h"

// logger specific macros
#define TLOG_WARN
#define TLOG_INFO
#define TLOG_DEBUG
#define TLOG_TRACE
#define TLOG_USER_DEBUG

// I know this probably isn't the best
// but I think it's neater
#ifdef GAME_RELEASE
#undef TLOG_USER_DEBUG
#endif

#ifdef TOAST_RELEASE
#undef TLOG_DEBUG
#undef TLOG_TRACE
#endif

namespace toast
{
	/// <summary>
	/// Specifically for usage in the logger class.
	/// </summary>
	enum class logLevel 
	{
		TFATAL = 0,
		TERROR = 1,
		TWARN = 2,
		TINFO = 3,
		TDEBUG = 4,
		TTRACE = 5,
		TUSER_DEBUG = 6
	};

	/// <summary>
	/// A nice quick function for getting a time stamp
	/// </summary>
	/// <returns></returns>
	TINLINE static const tm getTimeStamp()
	{
		const time_t tt = sysClock::to_time_t(sysClock::now());
		tm local_tm;
#ifdef TWIN32
		errno_t err = localtime_s(&local_tm, &tt);
#else
		localtime_r(&tt, &local_tm);
#endif
		return local_tm;
	}

	/// <summary>
	/// The standard logging class. One of a few class that uses the 'char' type
	/// at the core of its string and io systems. This is for simplicity of implementation.
	/// </summary>
	class Logger
	{
	private:
		fstream<cv> * logFile;
	public:
		/// <summary>
		/// Initialise the logger.
		/// </summary>
		/// <returns>an instance of the logger</returns>
		Logger();
		~Logger();

		/// <summary>
		/// Open the loggers file stream (must be called if Logger() wasn't called to call log()).
		/// </summary>
		void open();

		void clearCachedLogs();

		/// <summary>
		/// Close the loggers file stream (can no longer call log() after doing this).
		/// </summary>
		TINLINE void close();

		/// <summary>
		/// Check if the file stream is open.
		/// </summary>
		/// <returns>true if the file stream is open</returns>
		TINLINE bool isOpen();
		/// <summary>
		/// Logs a message in the file and, depending on the logLevel, to stderr.
		/// </summary>
		/// <param name="message: ">the message to log</param>
		template<logLevel ll = logLevel::TINFO>
		TINLINE void log(const str<cv> &message) const;

		template<logLevel ll = logLevel::TINFO>
		TINLINE static void staticLog(const str<cv>& message);
	};

	TINLINE void Logger::close() 
	{
		logFile->close();
	}

	TINLINE bool Logger::isOpen() { return logFile->is_open(); }

	template<> TINLINE void Logger::log<logLevel::TFATAL>(const str<cv> &message) const
	{
		*logFile << "[FATAL]: " << message << "\n";
		Platform::consoleWriteError("[FATAL]: " + message + "\n", color::TRED_BKG);
	}

	template<> TINLINE void Logger::staticLog<logLevel::TFATAL>(const str<cv>& message)
	{
		Platform::consoleWriteError("[FATAL]: " + message + "\n", color::TRED_BKG);
	}

	template<> TINLINE void Logger::log<logLevel::TERROR>(const str<cv> &message) const
	{
		*logFile << "[ERROR]: " << message << "\n";
		Platform::consoleWriteError("[ERROR]: " + message + "\n", color::TRED);
	}

	template<> TINLINE void Logger::staticLog<logLevel::TERROR>(const str<cv>& message)
	{
		Platform::consoleWriteError("[ERROR]: " + message + "\n", color::TRED);
	}

#ifdef TLOG_WARN
	template<> TINLINE void Logger::log<logLevel::TWARN>(const str<cv> &message) const
	{
		*logFile << "[WARNING]: " << message << "\n";
		Platform::consoleWrite("[WARNING]: " + message + "\n", color::TYELLOW);
	}

	template<> TINLINE void Logger::staticLog<logLevel::TWARN>(const str<cv>& message)
	{
		Platform::consoleWriteError("[WARNING]: " + message + "\n", color::TYELLOW);
	}
#endif
#ifdef TLOG_INFO
	template<> TINLINE void Logger::log<logLevel::TINFO>(const str<cv> &message) const
	{
		*logFile << "[INFO]: " << message << "\n";
		Platform::consoleWrite("[INFO]: " + message + "\n", color::TBLUE);
	}

	template<> TINLINE void Logger::staticLog<logLevel::TINFO>(const str<cv>& message)
	{
		Platform::consoleWriteError("[INFO]: " + message + "\n", color::TBLUE);
	}
#endif
#ifdef TLOG_DEBUG
	template<> TINLINE void Logger::log<logLevel::TDEBUG>(const str<cv> &message) const
	{
		*logFile << "[DEBUG]: " << message << "\n";
		Platform::consoleWrite("[DEBUG]: " + message + "\n", color::TGREEN);
	}

	template<> TINLINE void Logger::staticLog<logLevel::TDEBUG>(const str<cv>& message)
	{
		Platform::consoleWriteError("[DEBUG]: " + message + "\n", color::TGREEN);
	}
#endif
#ifdef TLOG_TRACE
	template<> TINLINE void Logger::log<logLevel::TTRACE>(const str<cv> &message) const
	{
		*logFile << "[TRACE]: " << message << "\n";
		Platform::consoleWrite("[TRACE]: " + message + "\n", color::TGREY);
	}

	template<> TINLINE void Logger::staticLog<logLevel::TTRACE>(const str<cv>& message)
	{
		Platform::consoleWriteError("[TRACE]: " + message + "\n", color::TGREY);
	}
#endif
#ifdef TLOG_USER_DEBUG
	template<> TINLINE void Logger::log<logLevel::TUSER_DEBUG>(const str<cv>& message) const
	{
		*logFile << "[USER_DEBUG]: " << message << "\n";
		Platform::consoleWrite("[USER_DEBUG]: " + message + "\n", color::TGREY);
	}

	template<> TINLINE void Logger::staticLog<logLevel::TUSER_DEBUG>(const str<cv>& message)
	{
		Platform::consoleWriteError("[USER_DEBUG]: " + message + "\n", color::TGREY);
	}
#endif

	template<logLevel ll> TINLINE void Logger::log(const str<cv> &message) const
	{}

	template<logLevel ll> TINLINE void Logger::staticLog(const str<cv>& message)
	{}
}