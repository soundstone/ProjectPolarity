/*************************************
* Class definitions for Logger class *
**************************************/

#include <stdexcept>
#include "logger.h"

using namespace std;

const string Logger::logLevelDebug = "DEBUG";
const string Logger::logLevelInfo = "INFO";
const string Logger::logLevelError = "ERROR";
const char* const Logger::logFileName = "log.txt";

bool Logger::sInitialized = false;
ofstream Logger::outStream;

void Logger::log(const string& message, const string& logLevel)
{
	if(!sInitialized)
		Initialize();

	//Print message to log file and flush stream with new line
	outStream << logLevel << ": " << message << endl;
}

void Logger::log(const vector<string>& messages, const string& logLevel)
{
	for(size_t i = 0; i < messages.size(); i++)
	{
		log(messages, logLevel);
	}
}

void Logger::closeLog()
{
	if(sInitialized)
	{
		outStream.close();
		sInitialized = false;
	}
}

void Logger::Initialize()
{
	if(!sInitialized)
	{
		outStream.open(logFileName, ios_base::app);
		if(!outStream.good())
		{
			throw runtime_error("Unable to initialize the game logger");
		}
		sInitialized = true;
	}
}

