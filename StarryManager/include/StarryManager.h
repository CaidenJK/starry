#pragma once

#include <starrymanager/AssetManager.h>
#include <starrymanager/FileHandler.h>

using StarryAsset = StarryManager::StarryAsset;
using CallSeverity = StarryManager::StarryAsset::CallSeverity;
using AssetManager = StarryManager::AssetManager;
using ResourceState = StarryManager::ResourceRequest::ResourceState;
using Flags = StarryManager::FileHandler::Flags;
using File = StarryManager::File;

template<typename T>
using ResourceHandle = StarryManager::ResourceHandle<T>;