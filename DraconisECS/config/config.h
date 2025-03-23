#pragma once
#include <string>

namespace core
{
struct WindowConfig
{
	std::string title = "DraconisECS";
	int width		  = 1280;
	int height		  = 720;
	bool fullscreen	  = false;
	bool vsync		  = true;
};

struct GraphicsConfig
{
	int glMajorVersion = 3;
	int glMinorVersion = 0;
	int msaaSamples	   = 4;
};

class Config
{
public:
	static Config& get()
	{
		static Config instance;
		return instance;
	}

	bool loadFromFile( const std::string& filename );
	bool saveToFile( const std::string& filename ) const;
	bool save() const;

	WindowConfig& getWindowConfig()
	{
		return m_WindowConfig;
	}
	GraphicsConfig& getGraphicsConfig()
	{
		return m_GraphicsConfig;
	}

	void setWindowConfig( const WindowConfig& config )
	{
		m_WindowConfig = config;
	}

	void setGraphicsConfig( const GraphicsConfig& config )
	{
		m_GraphicsConfig = config;
	}

private:
	Config()						   = default;
	~Config()						   = default;
	Config( const Config& )			   = delete;
	Config& operator=( const Config& ) = delete;

	WindowConfig m_WindowConfig;
	GraphicsConfig m_GraphicsConfig;
};
}  // namespace core