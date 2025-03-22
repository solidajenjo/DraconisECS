#include "config.h"
#include "../core/modules/filesystem.h"
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace core
{
bool Config::loadFromFile(const std::string &filename)
{
    std::cout << "Loading config from: " << filename << std::endl;

    if (!module::Filesystem::exists(filename))
    {
        std::cerr << "Config file does not exist: " << filename << std::endl;
        return false;
    }

    json j;
    if (!module::Filesystem::readJsonFile(filename, j))
    {
        std::cerr << "Failed to read or parse config file: " << filename << std::endl;
        return false;
    }

    // Load window config
    if (j.contains("window"))
    {
        const auto &window = j["window"];
        if (window.contains("title"))
            windowConfig.title = window["title"].get<std::string>();
        if (window.contains("width"))
            windowConfig.width = window["width"].get<int>();
        if (window.contains("height"))
            windowConfig.height = window["height"].get<int>();
        if (window.contains("fullscreen"))
            windowConfig.fullscreen = window["fullscreen"].get<bool>();
        if (window.contains("vsync"))
            windowConfig.vsync = window["vsync"].get<bool>();
    }

    // Load graphics config
    if (j.contains("graphics"))
    {
        const auto &graphics = j["graphics"];
        if (graphics.contains("glMajorVersion"))
            graphicsConfig.glMajorVersion = graphics["glMajorVersion"].get<int>();
        if (graphics.contains("glMinorVersion"))
            graphicsConfig.glMinorVersion = graphics["glMinorVersion"].get<int>();
        if (graphics.contains("msaaSamples"))
            graphicsConfig.msaaSamples = graphics["msaaSamples"].get<int>();
    }

    std::cout << "Successfully loaded config from: " << filename << std::endl;
    return true;
}

bool Config::saveToFile(const std::string &filename) const
{
    std::cout << "Saving config to: " << filename << std::endl;

    json j;

    // Save window config
    j["window"] = {{"title", windowConfig.title},
                   {"width", windowConfig.width},
                   {"height", windowConfig.height},
                   {"fullscreen", windowConfig.fullscreen},
                   {"vsync", windowConfig.vsync}};

    // Save graphics config
    j["graphics"] = {{"glMajorVersion", graphicsConfig.glMajorVersion},
                     {"glMinorVersion", graphicsConfig.glMinorVersion},
                     {"msaaSamples", graphicsConfig.msaaSamples}};

    if (!module::Filesystem::writeJsonFile(filename, j))
    {
        std::cerr << "Failed to write config file: " << filename << std::endl;
        return false;
    }

    std::cout << "Successfully saved config to: " << filename << std::endl;
    return true;
}
} // namespace core 