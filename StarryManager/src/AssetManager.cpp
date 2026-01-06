#include "AssetManager.h"

#include <vector>
#include <format>

namespace StarryManager
{
    std::atomic<bool> AssetManager::isDead = false;
    std::shared_ptr<AssetManager> AssetManager::globalPointer = nullptr;

    void AssetManager::InitManager(const std::string& package)
    {
        if (globalPointer) return;

        globalPointer.reset(new AssetManager(package));
	}

    Logger::AssetCall AssetManager::getVersionAlert()
    {
        Logger::AssetCall call;
        call.callerName = "Program";
        call.callerUUID = 0;
        call.severity = BANNER;
        call.message = std::string("Package: ") + packageName + std::string(", Version " VERSION " dev");
        call.callTime = std::chrono::system_clock::now();
		return call;
    }

    AssetManager::AssetManager(const std::string& name) : StarryAsset(false), packageName(name)
    {
        registerAsset(this);
        logger = new Logger();
        registerAsset(logger);

        fileHandler = new FileHandler();
        registerAsset(fileHandler);

        assetThread = std::thread(&AssetManager::worker, this);

#ifdef VERSION
		Logger::AssetCall versionCall = getVersionAlert();
        logger->enqueueAlert(versionCall);
#else
#error "VERSION not defined"
#endif
    }

    AssetManager::~AssetManager()
    {
        delete logger;
        logger = nullptr;
        isDead = true;

        delete fileHandler;
        fileHandler = nullptr;
        
        hasFatal = true;
        resourceCV.notify_all();
        assetThread.join();
    }

    std::weak_ptr<AssetManager> AssetManager::get() 
    {
        if (isDead) return {};
		if (!globalPointer) {
            InitManager("starry-internal");
		}
		return std::weak_ptr<AssetManager>(globalPointer);
    }

    void AssetManager::registerAsset(StarryAsset* asset)
	{
		if (asset == nullptr) {
			return;
		}
		registeryMutex.lock();
		registeredAssets.insert({ asset->getUUID(), asset });
		registeryMutex.unlock();
	}

	void AssetManager::unregisterAsset(uint64_t uuid) 
	{
		if (logger == nullptr || Logger::isLoggerDead()) return;
		
        logger->flushQueueBlock();
        std::scoped_lock lock(registeryMutex);
		registeredAssets.erase(uuid);

        // Mark as dead
        for (auto it = closedRequests.begin(); it != closedRequests.end(); ++it) {
            std::scoped_lock assetLock((*it)->mutex);
            if ((*it)->senderUUID == uuid) {
                (*it)->resourceState = ResourceRequest::DEAD;
            }
        }
        // remove
        std::erase_if(closedRequests, [](std::shared_ptr<ResourceRequest>& request) { return request->resourceState == ResourceRequest::DEAD;});
	}

	void AssetManager::updateAssetPointer(uint64_t uuid, StarryAsset* newPtr) 
    {
		if (uuid == 0 || newPtr == nullptr) {
			registerAlert("Asset Manager received an update pointer request to a NULL object", CRITICAL);
            return;
		}

		auto it = registeredAssets.find(uuid);
		if (it != registeredAssets.end()) {
			it->second = newPtr;
		}
		else {
			registerAlert("Asset Manager received an update pointer request to a NULL object", CRITICAL);
            return;
		}
	}

    Logger::AssetCall AssetManager::getFatalAlert()
    {
        Logger::AssetCall call;
        call.callerName = getAssetName();
        call.callerUUID = getUUID();
        call.severity = CRITICAL;
        call.message = "A fatal alert was registered. Exiting program as permitted by Manager rights.";
        call.callTime = std::chrono::system_clock::now();

        return call;
    }
    
    void AssetManager::registerAssetAlert(uint64_t uuid)
    {
        Logger::AssetCall call;
        {
            registeryMutex.lock();
        
            auto asset = registeredAssets.find(uuid);
            if (asset == registeredAssets.end()) return;

            call.callerName = asset->second->getAssetName();
            call.callerUUID = asset->second->getUUID();
            call.severity = asset->second->getAlertSeverity();
            call.message = asset->second->getAlertMessage();
            call.callTime = std::chrono::system_clock::now();

            if (call.severity != FATAL) {
                asset->second->resetAlert();
            }
        }
        registeryMutex.unlock();

        logger->enqueueAlert(call);

        if (hasExitRights.load() && call.severity == FATAL) {
            call = getFatalAlert();
            logger->enqueueAlert(call);
			delete logger;
            logger = nullptr;
			std::exit(EXIT_FAILURE);
		}
    }

    void AssetManager::worker() 
    {
        while (!hasFatal.load()) {
            std::unique_lock resourceLock(resourceMutex);
			resourceCV.wait(resourceLock, [this]() { return !resourceRequests.empty() || hasFatal.load(); });
            if (hasFatal.load()) return;
            while (!resourceRequests.empty()) {
                {
                    std::scoped_lock requestLock(resourceRequests.front()->mutex);
                    findResources(resourceRequests.front());
                }
                if (resourceRequests.front()->resourceState == ResourceRequest::YES) {
                    closedRequests.push_back(resourceRequests.front());
                }
                resourceRequests.pop();
            }
        }
    }

    void AssetManager::findResources(std::shared_ptr<ResourceRequest>& request)
    {
        std::scoped_lock lock(registeryMutex);
        auto asset = registeredAssets.find(request->senderUUID);
        if (!asset->second) { // should be redundant
            request->resourceState = ResourceRequest::DEAD;
            return;
        }

        // remove STALE
        std::erase_if(closedRequests, [](std::shared_ptr<ResourceRequest>& request) { return request->resourceState == ResourceRequest::STALE; });

        for (auto it = closedRequests.begin(); it != closedRequests.end(); ++it) {
            if ((*it)->senderUUID == request->senderUUID && 
                (*it)->resourceID == request->resourceID &&
                (*it)->resourceArgs == request->resourceArgs) {
                request->resourceState = ResourceRequest::YES;
                request->resource = (*it)->resource;
                return;
            }
        }

        std::optional<void*> result = asset->second->getResource(request->resourceID, request->resourceArgs);
        if (result.has_value()) {
            request->resource = result.value();
            request->resourceState = ResourceRequest::YES;
        }
        else {
            request->resourceState = ResourceRequest::DEAD;
        }
    }

	void AssetManager::dumpRegisteredAssets(bool names)
	{
		registeryMutex.lock();
		size_t registeredAssetsSize = registeredAssets.size();
		std::vector<std::string> pointerArray;
		std::vector<std::string> idArray;
		std::vector<std::string> nameArray;
		for (const auto& asset : registeredAssets) {
            pointerArray.push_back(std::format("{:#016x}", (size_t)asset.second));
			idArray.push_back(std::format("{:#016x}", asset.first));
			nameArray.push_back(std::string(asset.second->getAssetName()));
		}
		registeryMutex.unlock();
		
		registerAlert("Logger: Asset Dump - Total = " + std::to_string(registeredAssetsSize) + "\n", INFO);
		for (int i = 0; i < pointerArray.size(); i++) {
			registerAlert(std::string("  Id = ") + idArray[i] + ", Address = " + pointerArray[i] + ", Name = " + nameArray[i] + "\n", INFO);
		}
		logger->toFlushCalls();
	}
}

// TODO, move registry to worker thread
