#pragma once
#include <fstream>
#include <stdarg.h>
#include <sstream>
#include "DL_assert.h"
#include <mutex>

#ifndef _RETAIL
#define DL_ASSERT(_STATEMENT, _MESSAGE, ...) if (!(_STATEMENT)) { DL_Debug::Debug::GetInstance()->Assert(_STATEMENT, DL_META_INFO_CSTR, _MESSAGE, __VA_ARGS__); }

// Prints formatted. Specifiers: %d/%i = int. %u = unsigned int. %f = float. %c = char. %s string.
#define DL_LOG(_PRINTMESSAGE, ...)         (DL_Debug::Debug::GetInstance()->LogFormat(DL_Debug::Debug::LogType::Info, DL_META_INFO_CSTR, _PRINTMESSAGE, __VA_ARGS__))
// Prints formatted. Specifiers: %d/%i = int. %u = unsigned int. %f = float. %c = char. %s string.
#define DL_LOG_ERROR(_PRINTMESSAGE, ...)   (DL_Debug::Debug::GetInstance()->LogFormat(DL_Debug::Debug::LogType::Error, DL_META_INFO_CSTR, _PRINTMESSAGE, __VA_ARGS__))
// Prints formatted. Specifiers: %d/%i = int. %u = unsigned int. %f = float. %c = char. %s string.
#define DL_LOG_WARNING(_PRINTMESSAGE, ...) (DL_Debug::Debug::GetInstance()->LogFormat(DL_Debug::Debug::LogType::Warning, DL_META_INFO_CSTR, _PRINTMESSAGE, __VA_ARGS__))
#define DL_LOG_NOFORMAT(_PRINTMESSAGE) (DL_Debug::Debug::GetInstance()->Log(_PRINTMESSAGE, DL_META_INFO_CSTR))

#define DL_META_INFO_CSTR (std::string(__FUNCTION__) + "() (at " + __FILE__ + ":" + std::to_string(__LINE__) + ")").c_str()
#define LOG_TIME_FORMAT "%H:%M:%S"
#else
#define DL_ASSERT(_STATEMENT, _MESSAGE, ...)
#define DL_LOG(_PRINTMESSAGE, ...)  
#define DL_LOG_ERROR(_PRINTMESSAGE, ...)
#define DL_LOG_WARNING(_PRINTMESSAGE, ...)
#define DL_LOG_NOFORMAT(_PRINTMESSAGE)
#endif

#define DL_CONSOLE_COLOR_FG_BLACK 0
#define DL_CONSOLE_COLOR_FG_GREY 8
#define DL_CONSOLE_COLOR_FG_BLUE 9
#define DL_CONSOLE_COLOR_FG_GREEN 10
#define DL_CONSOLE_COLOR_FG_CYAN 11
#define DL_CONSOLE_COLOR_FG_RED 12
#define DL_CONSOLE_COLOR_FG_PURPLE 13
#define DL_CONSOLE_COLOR_FG_YELLOW 14
#define DL_CONSOLE_COLOR_FG_WHITE 15

namespace DL_Debug
{
	// TODO: Make thread safe

	class Debug
	{
	public:
		enum class LogType
		{
			Info,
			Warning,
			Error,
			Assert,
		};

		static Debug*		GetInstance();
		static void			Create(bool aOutputToFile, bool myVerboseEnabled);
		static void			Destroy();

		void				Assert(bool aStatement, const char* aDLMetaInfo, const char* aMessage, ...);
		void				LogFormat(LogType aLogType, const char* aDLMetaInfo, const char* aMessage, ...);
		void				Log(const char* aString, const char* aDLMetaInfo, WORD aColor = DL_CONSOLE_COLOR_FG_WHITE);
		void				SetConsoleAttributes(WORD aAttribute);
		void				LogImmediateFormat(const char* aMessage, WORD aColor, ...);
	private:
		void				CreateLogFile();
		void				PrintTimeStamp();
		void				LogFormatInternal(const char* aMessage, va_list aArgsList);
		void				LogFormatInternalVerbose(const char* aDLMetaInfo,const char* aMessage, va_list aArgsList);

		void				FlushStream();
		Debug();
		~Debug() = default;

		static Debug*		ourInstance;
		static WORD			ourDefaultConsoleAttribute;

		std::ofstream		myFile;
		std::ostringstream	myFileStreamOutput;
		std::ostringstream	myConsoleStreamOutput;
		WORD				myCurrentConsoleAttribute;
		bool				myVerboseEnabled;
		std::mutex			myMutex;
	};
}
