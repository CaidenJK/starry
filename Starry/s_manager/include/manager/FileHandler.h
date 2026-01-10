#pragma once

#include "StarryAsset.h"
#include "Resource.h"

#include <fstream>
#include <string>
#include <unordered_map>

#include "external/stb_image.h"
#include "external/tiny_obj_loader.h"

#define FILE_Request 1
#define FILETYPE Manager::RawFile*

namespace Manager
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

    struct ModelFile : RawFile
    {
            ModelFile(std::string& path) : RawFile(path) {}
            ~ModelFile() { close(); }
            bool open(size_t args) override;
            void close() override {}

            tinyobj::attrib_t attrib;
            std::vector<tinyobj::shape_t> shapes;
            std::vector<tinyobj::material_t> materials;

            std::string warn, err;
	};

    class FileHandler : public StarryAsset
    {
        public:
            FileHandler();
            ~FileHandler();

            void createDirectory(std::string dir);

            enum FileFlags {
                READ = std::ios::in,
                WRITE = std::ios::out,
                BINARY = std::ios::binary,
                CHAR = 0,
                IMAGE = 256U,
                MODEL = 512U,
                APPEND_START = std::ios::ate,
                APPEND_EACH = std::ios::app,
                OVERRITE = std::ios::trunc,
                CREATE_DIR = 128U
            };

            void askCallback(std::shared_ptr<ResourceAsk>& ask) override;

            ASSET_NAME("File Handler")
        private:
            void removeStale();

            RawFile* createFile(std::string path);
            bool openFile(RawFile*& file, std::vector<size_t> args);

            std::vector<RawFile*> openFiles;
    };

    using FileFlags = FileHandler::FileFlags;
}