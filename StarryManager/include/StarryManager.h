#pragma once

#include <starrymanager/AssetManager.h>
#include <starrymanager/FileHandler.h>
#include <external/stb_image.h>
#include <external/tiny_obj_loader.h>

using StarryAsset = StarryManager::StarryAsset;
using CallSeverity = StarryManager::StarryAsset::CallSeverity;
using AssetManager = StarryManager::AssetManager;
using ResourceState = StarryManager::ResourceRequest::ResourceState;
using Flags = StarryManager::FileHandler::Flags;
using RawFile = StarryManager::RawFile;
using ImageFile = StarryManager::ImageFile;
using ModelFile = StarryManager::ModelFile;

template<typename T>
using ResourceHandle = StarryManager::ResourceHandle<T>;