#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

#define TINYOBJLOADER_IMPLEMENTATION

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

    bool ModelFile::open(size_t args)
    {
        return tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
    }

    FileHandler::FileHandler() : StarryAsset(false)
    {
    }

    FileHandler::~FileHandler()
    {
        for (auto file : openFiles) {
            file->file.close();
        }
    }

    RawFile* FileHandler::createFile(std::string path)
    {
        RawFile* file = new RawFile(path);
        return file;
    }

    bool FileHandler::openFile(RawFile*& file, std::vector<size_t> args)
    {
        if (args.size() < 1) {
            Alert("No arguments passed to file.", WARNING);
            return false;
        }
        size_t flags = args[0];

        if (flags & CREATE_DIR) {
            std::filesystem::create_directories(file->path);
            return true;
        }
        if (flags & IMAGE && flags & READ) {
            auto path = file->path;
            delete file;
            file = new ImageFile(path);
            flags = args[1];
        }
        if (flags & MODEL && flags & READ) {
            auto path = file->path;
            delete file;
            file = new ModelFile(path);
        }

        if (!file->open(flags)) {
            Alert(std::string("Could not open file: ") + file->path, WARNING);
            return false;
        }

        return true;
    }

    void FileHandler::askCallback(std::shared_ptr<ResourceAsk>& ask)
    {
        openFiles.emplace_back(createFile(ask->getID()));

        //std::erase_if(openFiles, [](std::shared_ptr<RawFile>& file) { return file->dead(); }); // Need the Request to signal back to sender

        if (!openFile(openFiles.back(), ask->getArguments())) {
            ask->invalidate();
        }
        else {
            ask->setResource((void*)&(openFiles.back()));
        }
    }
}