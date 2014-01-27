//Logger.cpp
//Definitions for class Logger

#include <stdexcept>
#include "Logger.h"

using namespace std;

const string Logger::logLevelDebug = "DEBUG";
const string Logger::logLevelInfo = "INFO";
const string Logger::logLevelError = "ERROR";
const char* const Logger::logFileName = "log.txt";
bool Logger::sInitialized = false;
ofstream Logger::sOutputStream;

void Logger::Log(const string& message, const string& logLevel)
{
	if (!sInitialized)
		InitializeLogger();

	sOutputStream << logLevel << ": " << message << endl;
}

void Logger::Log(const vector<string>& messages, const string& logLevel)
{
	for (size_t i = 0; i < messages.size(); i++)
	{
		Log(messages[i], logLevel);
	}
}

void Logger::ShutdownLogger()
{
	if (sInitialized)
	{
		sOutputStream.close();
		sInitialized = false;
	}
}

void Logger::InitializeLogger()
{
	if (!sInitialized)
	{
		sOutputStream.open(logFileName, ios_base::app);
		if (!sOutputStream.good() )
		{
			throw runtime_error("Unable to initialize the logger!");
		}

		sInitialized = true;
	}
}