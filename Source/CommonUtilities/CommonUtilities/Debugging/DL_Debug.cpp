#include "DL_Debug.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <filesystem>

namespace DL_Debug
{
	WORD Debug::ourDefaultConsoleAttribute = DL_CONSOLE_COLOR_FG_WHITE;
	Debug* Debug::ourInstance = nullptr;
	Debug* Debug::GetInstance()
	{
		return ourInstance;
	}

	Debug::Debug() :
		myConsoleStreamOutput(std::ios_base::app | std::ios_base::out),
		myVerboseEnabled(false),
		myCurrentConsoleAttribute(DL_CONSOLE_COLOR_FG_WHITE)
	{
		CONSOLE_SCREEN_BUFFER_INFO info;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
		Debug::ourDefaultConsoleAttribute = info.wAttributes;
	}

	void Debug::Create(bool aOutputToFile, bool myVerboseEnabled)
	{
		assert(ourInstance == nullptr && "There is already an instance of Debug.");
		ourInstance = new Debug();
		ourInstance->myVerboseEnabled = myVerboseEnabled;

		if (aOutputToFile)
		{
			ourInstance->CreateLogFile();
		}
	}

	void Debug::CreateLogFile()
	{
		std::time_t now = std::time(0);
		struct tm timeinfo;
		localtime_s(&timeinfo, &now);
		std::ostringstream stream;
		stream << "Logs\\" << 1900 + timeinfo.tm_year << "-" << timeinfo.tm_mon << "-" << timeinfo.tm_mday << "_" << timeinfo.tm_hour << "-" << timeinfo.tm_min << "-" << timeinfo.tm_sec << ".txt";

		/// TODO: This should output to a "legal" folder since we are not allowed to create files in install directories. 
		std::filesystem::create_directory(std::filesystem::path("Logs"));
		
		ourInstance->myFile.open(stream.str());

		assert(ourInstance->myFile.is_open() && "Failed to open log file.");

		// Header
		ourInstance->myFile
			<< "Log Created: " << __DATE__ << " - " << __TIME__
			<< "\n-----------------------------------------------"
			<< std::endl;
	}

	void Debug::Destroy()
	{
		assert(ourInstance != nullptr && "No DL_DEBUG instance has been created!");
		if (ourInstance)
		{
			ourInstance->myFile.close();
		}

		delete ourInstance;
		ourInstance = nullptr;
	}

	void Debug::Assert(bool aStatement, const char* aDLMetaInfo, const char* aMessage, ...)
	{
		PrintTimeStamp();
		
		va_list args;
		va_start(args, aMessage);
		LogFormatInternal((std::string("Assert: ") + aMessage + "\n" + aDLMetaInfo).c_str(), args);
		va_end(args);
		
		FlushStream();
		assert(aStatement && aMessage);
	}

	void Debug::LogFormat(LogType aLogType, const char* aDLMetaInfo, const char* aMessage, ...)
	{		
		PrintTimeStamp();

		std::string messageType;
		WORD color = DL_CONSOLE_COLOR_FG_WHITE;

		switch (aLogType)
		{
		case DL_Debug::Debug::LogType::Info: break;
		case DL_Debug::Debug::LogType::Warning:
			messageType = "Warning: ";
			color = DL_CONSOLE_COLOR_FG_YELLOW;
			break;
		case DL_Debug::Debug::LogType::Error:
			messageType = "Error: ";
			color = DL_CONSOLE_COLOR_FG_RED;
			break;
		}
		SetConsoleAttributes(color);

		va_list args;
		va_start(args, aMessage);
		
		myVerboseEnabled || aLogType == LogType::Error || aLogType == LogType::Warning ?
			LogFormatInternalVerbose(aDLMetaInfo, messageType.append(aMessage).c_str(), args) : 
			LogFormatInternal(messageType.append(aMessage).c_str(), args);

		va_end(args);

		FlushStream();
	}

	void Debug::Log(const char* aString, const char* aDLMetaInfo, WORD aColor)
	{
		SetConsoleAttributes(aColor);

		myFileStreamOutput    << '\n' << aString;
		myConsoleStreamOutput << '\n' << aString;
		if (myVerboseEnabled)
		{
			myFileStreamOutput    << "\n" << aDLMetaInfo;
			myConsoleStreamOutput << "\n" << aDLMetaInfo;
		}
		myFileStreamOutput << '\n';
		myConsoleStreamOutput << '\n';

		FlushStream();
	}

	void Debug::SetConsoleAttributes(WORD aAttribute)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, aAttribute);
	}

	void Debug::LogImmediateFormat(const char* aMessage, WORD aColor, ...)
	{
		va_list args;
		va_start(args, aMessage);
		LogFormatInternal(aMessage, args);
		va_end(args);

		SetConsoleAttributes(aColor);
		FlushStream();
	}

	void Debug::PrintTimeStamp()
	{
		const auto now = std::chrono::system_clock::now();
		const auto time_T = std::chrono::system_clock::to_time_t(now);
		char timeBuffer[26];
		ctime_s(timeBuffer, size_t(26), &time_T);
		std::string str(std::begin(timeBuffer) + 11, std::end(timeBuffer) - 7);

		myFileStreamOutput    << "\n\n[" << str << "] ";
		myConsoleStreamOutput << "\n\n[" << str << "] ";
	}

	void Debug::LogFormatInternal(const char* aMessage, va_list aArgsList)
	{
		char buffer[256];
		std::vsnprintf(buffer, sizeof(buffer), aMessage, aArgsList);

		myFileStreamOutput    << buffer;
		myConsoleStreamOutput << buffer;
	}

	void Debug::LogFormatInternalVerbose(const char* aDLMetaInfo, const char* aMessage, va_list aArgsList)
	{
		LogFormatInternal(aMessage, aArgsList);
		myFileStreamOutput << "\n" << aDLMetaInfo;
		myConsoleStreamOutput << "\n" << aDLMetaInfo;
	}

	void Debug::FlushStream()
	{
		std::cout << myConsoleStreamOutput.str();
		SetConsoleAttributes(ourDefaultConsoleAttribute);
		myConsoleStreamOutput.str(std::string());

		if (myFile.is_open())
		{
			myFile << myFileStreamOutput.str();
		}
		myFileStreamOutput.str(std::string());
	}
}