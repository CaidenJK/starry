#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

#include "FileHandler.h"

#include <filesystem>

namespace StarryManager
{   
    bool RawFile::open(size_t args)
    {
        file.open(path, args);
        return (bool)file;
    }
    void RawFile::close()
    {
        if (file) file.close();
    }

    bool ImageFile::open(size_t args)
    {
        pixels = stbi_load(path.c_str(), &width, &height, &channels, (int)args);
        return (bool)pixels;
    }

    void ImageFile::close()
    {
        if (pixels) stbi_image_free(pixels);
    }

    FileHandler::FileHandler() : StarryAsset(false)
    {
        
    }

    FileHandler::~FileHandler()
    {
        for (auto file : openFiles) {
            file.second->file.close();
        }
    }

    RawFile* FileHandler::createFile(std::string path)
    {
        RawFile* file = new RawFile(path);
        return file;
    }

    bool FileHandler::openFile(RawFile*& file, std::vector<size_t> args)
    {
        size_t flags = CHAR;
        for (auto arg : args) flags |= arg;

        if (flags & CREATE_DIR) {
            std::filesystem::create_directories(file->path);
            return true;
        }
        if (flags & IMAGE && flags & READ) {
            auto path = file->path;
            delete file;
            file = new ImageFile(path);
        }
        else {
            if (!file->open(flags)) {
                registerAlert(std::string("Could not open file: ") + file->path, WARNING);
                return false;
            }
        }

        return true;
    }

    std::optional<void*> FileHandler::getResource(size_t resourceID, std::vector<size_t> resourceArgs)
    {
        auto file = openFiles.find(resourceID);
        if (file == openFiles.end()) {
            return {};
        }

        //std::erase_if(openFiles, [](std::shared_ptr<RawFile>& file) { return file->dead(); }); // Need the request to signal back to sender

        if (!openFile(file->second, resourceArgs)) {
            return nullptr;
        }
        return (void*)&(file->second);
    }

    size_t FileHandler::getResourceIDFromString(std::string resourceString)
    {
        size_t fileID = generateUUID();
        openFiles.emplace(fileID, createFile(resourceString));

        return fileID;
    }
}