//Logger.h
//Class declaration for static Logger

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

protected:
	static void InitializeLogger();
	static const char* const logFileName;
	static bool sInitialized;
	static std::ofstream sOutputStream;

private:
	Logger()
	{
	}

};