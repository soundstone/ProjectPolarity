//Logger.h
//Class declaration for static Logger
#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class Logger
{
public:
	static const std::string logLevelDebug;
	static const std::string logLevelInfo;
	static const std::string logLevelError;

	static void Log(const std::string& message,
					const std::string& logLevel);

	static void Log(const std::vector<std::string>& messages,
					const std::string& logLevel);

	static void ShutdownLogger();
	static std::ofstream sOutputStream;

protected:
	static void InitializeLogger();
	static const char* const logFileName;
	static bool sInitialized;
	

private:
	Logger()
	{
	}

};