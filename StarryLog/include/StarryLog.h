#pragma once

#include <starrylog/StarryAsset.h>
#include <starrylog/Logger.h>
#include <starrylog/AssetManager.h>

using StarryAsset = StarryLog::StarryAsset;
using CallSeverity = StarryLog::StarryAsset::CallSeverity;
using AssetManager = StarryLog::AssetManager;
using ResourceState = StarryLog::ResourceRequest::ResourceState;

template<typename T>
using ResourceHandle = StarryLog::ResourceHandle<T>;