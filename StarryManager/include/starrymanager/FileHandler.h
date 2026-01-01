#pragma once

#include "StarryAsset.h"

#include <fstream>
#include <string>

#define FILE_REQUEST 1
#define FILETYPE std::shared_ptr<File>

namespace StarryManager
{
    struct File
    {
        std::fstream file = {};
        std::string path = {};

        File(std::string path);
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
                APPEND_START = std::ios::ate,
                APPEND_EACH = std::ios::app,
                OVERRITE = std::ios::trunc,
                CREATE_DIR = 300U
            };

            GET_RESOURCE;
            GET_RESOURCE_FROM_STRING;

            ASSET_NAME("File Handler")
        private:
            void removeStale();

            std::unordered_map<size_t, std::shared_ptr<File>> openFiles;
    };
}