#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace module
{

class Filesystem
{
public:
	static bool readFile( const std::string& path, std::string& outContent );
	static bool writeFile( const std::string& path, const std::string& content );
	static bool readJsonFile( const std::string& path, nlohmann::json& outJson );
	static bool writeJsonFile( const std::string& path, const nlohmann::json& json );
	static bool createDirectory( const std::string& path );
	static bool exists( const std::string& path );
	static std::string getExecutablePath();
	static std::string getCurrentWorkingDirectory();
	static std::string combinePaths( const std::string& path1, const std::string& path2 );

private:
	static bool ensureDirectoryExists( const std::string& path );
};

}  // namespace module