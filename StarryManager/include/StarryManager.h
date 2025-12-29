#pragma once

#include <starrymanager/StarryAsset.h>
#include <starrymanager/AssetManager.h>

using StarryAsset = StarryManager::StarryAsset;
using CallSeverity = StarryManager::StarryAsset::CallSeverity;
using AssetManager = StarryManager::AssetManager;
using ResourceState = StarryManager::ResourceRequest::ResourceState;

template<typename T>
using ResourceHandle = StarryManager::ResourceHandle<T>;