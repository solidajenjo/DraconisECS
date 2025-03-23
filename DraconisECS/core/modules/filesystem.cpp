#include "filesystem.h"
#include <SDL.h>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace module
{

bool Filesystem::readFile( const std::string& path, std::string& content )
{
	std::ifstream file( path, std::ios::binary );
	if( !file.is_open() )
	{
		std::cerr << "[Filesystem] Failed to open file: " << path << std::endl;
		return false;
	}

	// Get file size
	file.seekg( 0, std::ios::end );
	std::streamsize size = file.tellg();
	file.seekg( 0, std::ios::beg );

	// Read file content
	content.resize( static_cast<size_t>( size ) );
	if( !file.read( content.data(), size ) )
	{
		std::cerr << "[Filesystem] Failed to read file: " << path << std::endl;
		return false;
	}

	std::cout << "[Filesystem] Successfully read file: " << path << std::endl;
	return true;
}

bool Filesystem::writeFile( const std::string& path, const std::string& content )
{
	std::ofstream file( path, std::ios::binary );
	if( !file.is_open() )
	{
		std::cerr << "[Filesystem] Failed to open file for writing: " << path << std::endl;
		return false;
	}

	if( !file.write( content.data(), content.size() ) )
	{
		std::cerr << "[Filesystem] Failed to write to file: " << path << std::endl;
		return false;
	}

	std::cout << "[Filesystem] Successfully wrote to file: " << path << std::endl;
	return true;
}

bool Filesystem::readJsonFile( const std::string& path, nlohmann::json& outJson )
{
	std::string content;
	if( !readFile( path, content ) )
	{
		return false;
	}

	// Parse JSON without exceptions
	auto result = nlohmann::json::parse( content, nullptr, false );
	if( result.is_discarded() )
	{
		std::cerr << "[Filesystem] Failed to parse JSON content from: " << path << std::endl;
		return false;
	}

	outJson = std::move( result );
	std::cout << "[Filesystem] Successfully parsed JSON from: " << path << std::endl;
	return true;
}

bool Filesystem::writeJsonFile( const std::string& path, const nlohmann::json& json )
{
	std::string content = json.dump( 4 );
	return writeFile( path, content );
}

bool Filesystem::createDirectory( const std::string& path )
{
	std::error_code ec;
	std::filesystem::create_directories( path, ec );
	if( ec )
	{
		std::cerr << "[Filesystem] Failed to create directory: " << path << ": " << ec.message() << std::endl;
		return false;
	}

	std::cout << "[Filesystem] Successfully created directory: " << path << std::endl;
	return true;
}

bool Filesystem::exists( const std::string& path )
{
	bool exists = std::filesystem::exists( path );
	std::cout << "[Filesystem] Checking if exists: " << path << " -> " << ( exists ? "yes" : "no" ) << std::endl;
	return exists;
}

std::string Filesystem::getExecutablePath()
{
	char* basePath = SDL_GetBasePath();
	if( !basePath )
	{
		std::cerr << "[Filesystem] Failed to get executable path" << std::endl;
		return "";
	}
	std::string path( basePath );
	SDL_free( basePath );
	std::cout << "[Filesystem] Executable path: " << path << std::endl;
	return path;
}

std::string Filesystem::getCurrentWorkingDirectory()
{
	std::error_code ec;
	std::string path = std::filesystem::current_path( ec ).string();
	if( ec )
	{
		std::cerr << "[Filesystem] Failed to get current working directory: " << ec.message() << std::endl;
		return "";
	}
	std::cout << "[Filesystem] Current working directory: " << path << std::endl;
	return path;
}

std::string Filesystem::combinePaths( const std::string& path1, const std::string& path2 )
{
	std::filesystem::path p1( path1 );
	std::filesystem::path p2( path2 );
	std::string result = ( p1 / p2 ).string();
	std::cout << "[Filesystem] Combining paths: " << path1 << " + " << path2 << " -> " << result << std::endl;
	return result;
}

bool Filesystem::ensureDirectoryExists( const std::string& path )
{
	std::cout << "[Filesystem] Ensuring directory exists for: " << path << std::endl;

	std::filesystem::path filePath( path );
	auto parentPath = filePath.parent_path();
	if( !parentPath.empty() )
	{
		std::error_code ec;
		bool success = std::filesystem::create_directories( parentPath, ec );
		if( ec )
		{
			std::cerr << "[Filesystem] Failed to create parent directory " << parentPath << ": " << ec.message()
					  << std::endl;
			return false;
		}

		if( success )
		{
			std::cout << "[Filesystem] Successfully created parent directory: " << parentPath << std::endl;
		}
		else
		{
			std::cout << "[Filesystem] Parent directory already exists: " << parentPath << std::endl;
		}
	}
	return true;
}

}  // namespace module