#include "config.h"
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>
#include "core/modules/filesystem.h"

using json = nlohmann::json;

namespace core
{
bool Config::loadFromFile( const std::string& filename )
{
	std::cout << "Loading config from: " << filename << std::endl;

	if( !module::Filesystem::exists( filename ) )
	{
		std::cerr << "Config file does not exist: " << filename << std::endl;
		return false;
	}

	json j;
	if( !module::Filesystem::readJsonFile( filename, j ) )
	{
		std::cerr << "Failed to read or parse config file: " << filename << std::endl;
		return false;
	}

	// Load window config
	if( j.contains( "window" ) )
	{
		const auto& window		  = j["window"];
		m_WindowConfig.title	  = window.value( "title", m_WindowConfig.title );
		m_WindowConfig.width	  = window.value( "width", m_WindowConfig.width );
		m_WindowConfig.height	  = window.value( "height", m_WindowConfig.height );
		m_WindowConfig.fullscreen = window.value( "fullscreen", m_WindowConfig.fullscreen );
		m_WindowConfig.vsync	  = window.value( "vsync", m_WindowConfig.vsync );
	}

	// Load graphics config
	if( j.contains( "graphics" ) )
	{
		const auto& graphics			= j["graphics"];
		m_GraphicsConfig.glMajorVersion = graphics.value( "glMajorVersion", m_GraphicsConfig.glMajorVersion );
		m_GraphicsConfig.glMinorVersion = graphics.value( "glMinorVersion", m_GraphicsConfig.glMinorVersion );
		m_GraphicsConfig.msaaSamples	= graphics.value( "msaaSamples", m_GraphicsConfig.msaaSamples );
	}

	std::cout << "Successfully loaded config from: " << filename << std::endl;
	return true;
}

bool Config::saveToFile( const std::string& filename ) const
{
	std::cout << "Saving config to: " << filename << std::endl;

	json j;

	// Save window config
	j["window"]["title"]	  = m_WindowConfig.title;
	j["window"]["width"]	  = m_WindowConfig.width;
	j["window"]["height"]	  = m_WindowConfig.height;
	j["window"]["fullscreen"] = m_WindowConfig.fullscreen;
	j["window"]["vsync"]	  = m_WindowConfig.vsync;

	// Save graphics config
	j["graphics"]["glMajorVersion"] = m_GraphicsConfig.glMajorVersion;
	j["graphics"]["glMinorVersion"] = m_GraphicsConfig.glMinorVersion;
	j["graphics"]["msaaSamples"]	= m_GraphicsConfig.msaaSamples;

	if( !module::Filesystem::writeJsonFile( filename, j ) )
	{
		std::cerr << "Failed to write config file: " << filename << std::endl;
		return false;
	}

	std::cout << "Successfully saved config to: " << filename << std::endl;
	return true;
}

bool Config::save() const
{
	std::filesystem::path configPath = std::filesystem::current_path() / "config.json";
	return saveToFile( configPath.string() );
}
}  // namespace core