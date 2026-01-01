#include "FileHandler.h"

#include <fstream>
#include <filesystem>

namespace StarryManager
{
    File::File(std::string path) : path(path) {}

    FileHandler::FileHandler() : StarryAsset(false)
    {
        
    }

    FileHandler::~FileHandler()
    {
        for (auto file : openFiles) {
            file.second->file.close();
        }
    }

    std::optional<void*> FileHandler::getResource(size_t resourceID, std::vector<size_t> resourceArgs)
    {
        auto file = openFiles.find(resourceID);
        if (file == openFiles.end()) {
            return {};
        }

        if (resourceArgs.size() == 1 && resourceArgs[0] == CREATE_DIR) {
            std::filesystem::create_directories(file->second->path);
            return {};
        }

        size_t flags = CHAR;
        for (auto arg : resourceArgs) flags |= arg;
        
        file->second->file.open(file->second->path, flags);
        if (!file->second->file) {
            registerAlert(std::string("Could not open file: ") + file->second->path, WARNING);
            return {};
        }

        return (void*)&(file->second);
    }

    size_t FileHandler::getResourceIDFromString(std::string resourceString)
    {
        size_t fileID = generateUUID();
        std::shared_ptr<File> file = std::make_shared<File>(resourceString);
        openFiles.emplace(fileID, file);

        return fileID;
    }
}