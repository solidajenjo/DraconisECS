#include "Config.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace core
{
bool Config::loadFromFile(const std::string &filename)
{
    std::cout << "Loading config from: " << filename << std::endl;

    if (!std::filesystem::exists(filename))
    {
        std::cerr << "Config file does not exist: " << filename << std::endl;
        return false;
    }

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open config file: " << filename << std::endl;
        return false;
    }

    std::string jsonStr((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    if (jsonStr.empty())
    {
        std::cerr << "Config file is empty: " << filename << std::endl;
        return false;
    }

    std::cout << "Read JSON content: " << jsonStr << std::endl;

    auto j = json::parse(jsonStr, nullptr, false);
    if (j.is_discarded())
    {
        std::cerr << "Failed to parse config file: " << filename << std::endl;
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

    // Create parent directory if it doesn't exist
    std::filesystem::path filePath(filename);
    if (!std::filesystem::exists(filePath.parent_path()))
    {
        std::cout << "Creating directory: " << filePath.parent_path() << std::endl;
        if (!std::filesystem::create_directories(filePath.parent_path()))
        {
            std::cerr << "Failed to create directory: " << filePath.parent_path() << std::endl;
            return false;
        }
    }

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

    std::string jsonStr = j.dump(4); // Pretty print with 4 spaces indentation
    std::cout << "Generated JSON content: " << jsonStr << std::endl;

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open config file for writing: " << filename << std::endl;
        return false;
    }

    file.write(jsonStr.c_str(), jsonStr.size());
    file.close();

    if (file.fail())
    {
        std::cerr << "Failed to write config file: " << filename << std::endl;
        return false;
    }

    std::cout << "Successfully saved config to: " << filename << std::endl;
    return true;
}
} // namespace core