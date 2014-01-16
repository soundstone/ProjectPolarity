/************************************************************************************************************
* Logger.h																									*
* Define the singleton logger class. This class will generate messages to an external text file for review. *
* The logger will generate debug info, generalized info and error messages									*
*************************************************************************************************************/

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

	//Logs a single message at the given log level
	static void log(const std::string& inMessage, 
		const std::string& inLogLevel);

	//Logs a vector of messages at the given log level
	static void log(const std::vector<std::string>& inMessages,
		const std::string& inLogLevel);

	//Closes the file after logging information
	static void closeLog();
protected:
	//Initialize the static logger
	static void Initialize();
	
	//constant for the filename
	static const char* const logFileName;

	//is logger already initialized?
	static bool sInitialized;
	//data member for output stream
	static std::ofstream outStream;

private:
	Logger() {};
	
};

