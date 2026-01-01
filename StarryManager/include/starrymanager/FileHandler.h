#pragma once

#include "StarryAsset.h"

#include <fstream>
#include <string>

#include "stb_image.h"

#define FILE_REQUEST 1
#define FILETYPE RawFile*

namespace StarryManager
{
    struct RawFile
    {
            RawFile(std::string& path) : path(path) {}
            ~RawFile() { close(); }
            virtual bool open(size_t args);
            virtual void close();

            std::fstream file = {};
            std::string path = {};
    };

    struct ImageFile : RawFile
    {
            ImageFile(std::string& path) : RawFile(path) {}
            ~ImageFile() { close(); }
            bool open(size_t args) override;
            void close() override;

            stbi_uc* pixels;
            int height, width, channels;
    };

    class FileHandler : public StarryAsset
    {
        public:
            FileHandler();
            ~FileHandler();

            void createDirectory(std::string dir);

            enum Flags {
                READ = std::ios::in,
                WRITE = std::ios::out,
                BINARY = std::ios::binary,
                CHAR = 0,
                IMAGE = 256U,
                APPEND_START = std::ios::ate,
                APPEND_EACH = std::ios::app,
                OVERRITE = std::ios::trunc,
                CREATE_DIR = 128U
            };

            GET_RESOURCE;
            GET_RESOURCE_FROM_STRING;

            ASSET_NAME("File Handler")
        private:
            void removeStale();

            RawFile* createFile(std::string path);
            bool openFile(RawFile* file, std::vector<size_t> args);

            std::unordered_map<size_t, RawFile*> openFiles;
    };
}