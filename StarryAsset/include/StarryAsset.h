#pragma once

#include <starryasset/StarryAsset.h>
#include <starryasset/AssetManager.h>

using StarryAsset = StarryAssets::StarryAsset;
using CallSeverity = StarryAssets::StarryAsset::CallSeverity;
using AssetManager = StarryAssets::AssetManager;
using ResourceState = StarryAssets::ResourceRequest::ResourceState;

template<typename T>
using ResourceHandle = StarryAssets::ResourceHandle<T>;