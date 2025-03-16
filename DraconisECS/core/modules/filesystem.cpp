#include "filesystem.h"
#include <SDL.h>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace module
{

bool Filesystem::readFile(const std::string &path, std::string &outContent)
{
    std::cout << "[Filesystem] Reading file: " << path << std::endl;

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "[Filesystem] Failed to open file for reading: " << path << std::endl;
        return false;
    }

    file.seekg(0, std::ios::end);
    outContent.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(outContent.data(), outContent.size());

    std::cout << "[Filesystem] Successfully read " << outContent.size() << " bytes from: " << path << std::endl;
    return true;
}

bool Filesystem::writeFile(const std::string &path, const std::string &content)
{
    std::cout << "[Filesystem] Writing file: " << path << std::endl;

    if (!ensureDirectoryExists(path))
    {
        std::cerr << "[Filesystem] Failed to create directory for: " << path << std::endl;
        return false;
    }

    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "[Filesystem] Failed to open file for writing: " << path << std::endl;
        return false;
    }

    file.write(content.data(), content.size());
    bool success = file.good();

    if (success)
    {
        std::cout << "[Filesystem] Successfully wrote " << content.size() << " bytes to: " << path << std::endl;
    }
    else
    {
        std::cerr << "[Filesystem] Failed to write to file: " << path << std::endl;
    }

    return success;
}

bool Filesystem::readJsonFile(const std::string &path, nlohmann::json &outJson)
{
    std::cout << "[Filesystem] Reading JSON file: " << path << std::endl;

    std::string content;
    if (!readFile(path, content))
    {
        std::cerr << "[Filesystem] Failed to read JSON file: " << path << std::endl;
        return false;
    }

    // Parse JSON without exceptions
    auto result = nlohmann::json::parse(content, nullptr, false);
    if (result.is_discarded())
    {
        std::cerr << "[Filesystem] Failed to parse JSON content from: " << path << std::endl;
        return false;
    }
    outJson = std::move(result);
    std::cout << "[Filesystem] Successfully parsed JSON from: " << path << std::endl;
    return true;
}

bool Filesystem::writeJsonFile(const std::string &path, const nlohmann::json &json)
{
    std::cout << "[Filesystem] Writing JSON file: " << path << std::endl;

    try
    {
        std::string content = json.dump(4);
        if (writeFile(path, content))
        {
            std::cout << "[Filesystem] Successfully wrote JSON to: " << path << std::endl;
            return true;
        }
        return false;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[Filesystem] Exception while writing JSON to " << path << ": " << e.what() << std::endl;
        return false;
    }
}

bool Filesystem::createDirectory(const std::string &path)
{
    std::cout << "[Filesystem] Creating directory: " << path << std::endl;

    try
    {
        bool success = std::filesystem::create_directories(path);
        if (success)
        {
            std::cout << "[Filesystem] Successfully created directory: " << path << std::endl;
        }
        else
        {
            std::cerr << "[Filesystem] Failed to create directory: " << path << std::endl;
        }
        return success;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[Filesystem] Exception while creating directory " << path << ": " << e.what() << std::endl;
        return false;
    }
}

bool Filesystem::exists(const std::string &path)
{
    bool exists = std::filesystem::exists(path);
    std::cout << "[Filesystem] Checking if exists: " << path << " -> " << (exists ? "yes" : "no") << std::endl;
    return exists;
}

std::string Filesystem::getExecutablePath()
{
    char *basePath = SDL_GetBasePath();
    if (!basePath)
    {
        std::cerr << "[Filesystem] Failed to get executable path" << std::endl;
        return "";
    }
    std::string path(basePath);
    SDL_free(basePath);
    std::cout << "[Filesystem] Executable path: " << path << std::endl;
    return path;
}

std::string Filesystem::getCurrentWorkingDirectory()
{
    std::string path = std::filesystem::current_path().string();
    std::cout << "[Filesystem] Current working directory: " << path << std::endl;
    return path;
}

std::string Filesystem::combinePaths(const std::string &path1, const std::string &path2)
{
    std::filesystem::path p1(path1);
    std::filesystem::path p2(path2);
    std::string result = (p1 / p2).string();
    std::cout << "[Filesystem] Combining paths: " << path1 << " + " << path2 << " -> " << result << std::endl;
    return result;
}

bool Filesystem::ensureDirectoryExists(const std::string &path)
{
    std::cout << "[Filesystem] Ensuring directory exists for: " << path << std::endl;

    std::filesystem::path filePath(path);
    auto parentPath = filePath.parent_path();
    if (!parentPath.empty())
    {
        try
        {
            bool success = std::filesystem::create_directories(parentPath);
            if (success)
            {
                std::cout << "[Filesystem] Successfully created parent directory: " << parentPath << std::endl;
            }
            else
            {
                std::cout << "[Filesystem] Parent directory already exists: " << parentPath << std::endl;
            }
            return true;
        }
        catch (const std::exception &e)
        {
            std::cerr << "[Filesystem] Exception while creating parent directory " << parentPath << ": " << e.what()
                      << std::endl;
            return false;
        }
    }
    return true;
}

} // namespace module