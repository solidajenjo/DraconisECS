#pragma once
#include <string>

namespace core
{
struct WindowConfig
{
    std::string title = "DraconisECS";
    int width = 800;
    int height = 600;
    bool fullscreen = false;
    bool vsync = true;
};

struct GraphicsConfig
{
    int glMajorVersion = 3;
    int glMinorVersion = 0;
    int msaaSamples = 0;
    bool enableDebugOutput = true;
};

class Config
{
  public:
    static Config &get()
    {
        static Config instance;
        return instance;
    }

    bool loadFromFile(const std::string &filename);
    bool saveToFile(const std::string &filename) const;

    // Configuration getters
    const WindowConfig &getWindowConfig() const
    {
        return windowConfig;
    }
    const GraphicsConfig &getGraphicsConfig() const
    {
        return graphicsConfig;
    }

    // Configuration setters
    void setWindowConfig(const WindowConfig &config)
    {
        windowConfig = config;
    }
    void setGraphicsConfig(const GraphicsConfig &config)
    {
        graphicsConfig = config;
    }

  private:
    Config() = default; // Private constructor for singleton
    Config(const Config &) = delete;
    Config &operator=(const Config &) = delete;

    WindowConfig windowConfig;
    GraphicsConfig graphicsConfig;
};
} // namespace core