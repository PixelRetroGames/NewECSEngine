#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <algorithm>

namespace Engine {

#define LoadFnT_TYPE(name) ResourceT *(*name)(LoadArgsT)
#define DeleteFnT_TYPE(name) void (*name)(ResourceT *)

#define DeleteFnT_DEFINITION typedef void (*DeleteFnT)(ResourceT *);
#define LoadFnT_DEFINITION typedef ResourceT *(*LoadFnT)(LoadArgsT);

typedef unsigned int ID;

class TypeCounter {
  public:
    static ID GetTypeId() {
        static ID counter = 0u;
        return (++counter);
    }
};

template <typename T>
class TypeCounted {
  public:
    TypeCounted() {
        GetTypeId();
    }

    static ID GetTypeId() {
        static ID id = TypeCounter::GetTypeId();
        return id;
    }
};

template <typename ResourceT>
class Resource : public TypeCounted<ResourceT> {
  protected:
    DeleteFnT_DEFINITION

    ResourceT *resource;
    DeleteFnT deleteFn;

  public:
    ResourceT *get() {
        return resource;
    }

    Resource(ResourceT *resource, DeleteFnT deleteFn) : resource(resource), deleteFn(deleteFn) {}

    ~Resource() {
        deleteFn(resource);
    }
};

template <typename ResourceT, typename ManagerT>
class SharedResourceInternal : public Resource<ResourceT> {
  private:
    DeleteFnT_DEFINITION
    ManagerT *manager;

  public:
    SharedResourceInternal(ResourceT *resource, DeleteFnT deleteFn, ManagerT *manager)
        : Resource<ResourceT>(resource, deleteFn), manager(manager) {}

    ~SharedResourceInternal() {
        manager->DeleteUnusedResources();
    }
};

template <typename ResourceT>
class UniqueResource {
  private:
    mutable std::unique_ptr<Resource<ResourceT>> res;

  public:
    bool operator!=(const UniqueResource &other) const {
        return !operator==(other);
    }

    bool operator==(const UniqueResource &other) const {
        return res == other.res;
    }

    UniqueResource() : res(nullptr) {}
    UniqueResource(std::unique_ptr<Resource<ResourceT>> res) : res(std::move(res)) {}
    UniqueResource(const UniqueResource<ResourceT> &other) : res(std::move(other.res)) {}

    UniqueResource<ResourceT> &operator=(UniqueResource<ResourceT> &&other) {
        res = std::move(other.res);
        return *this;
    }

    ResourceT *operator->() const {
        return res.get()->get();
    }

    ResourceT *get() {
        return res.get()->get();
    }
};


template <typename ResourceT>
class SharedResource {
  private:
    std::shared_ptr<Resource<ResourceT>> res;

  public:
    bool operator!=(const SharedResource &other) const {
        return !operator==(other);
    }

    bool operator==(const SharedResource &other) const {
        return res == other.res;
    }

    SharedResource() : res(nullptr) {}
    SharedResource(std::shared_ptr<Resource<ResourceT>> res) : res(res) {}

    //SharedResource(SharedResource &other) : res(other.res) {}

    ResourceT *operator->() const {
        return res.get()->get();
    }

    ResourceT *get() {
        return res.get()->get();
    }
};

class BaseResourceManager {
};

template <typename ResourceT, typename LoadArgsT>
class SharedResourceManager : public BaseResourceManager {
  private:
    SharedResourceManager<ResourceT, LoadArgsT> get() {
        return this;
    }

    typedef SharedResourceManager<ResourceT, LoadArgsT> ManagerT;
    typedef SharedResourceInternal<ResourceT, ManagerT> ManagedResourceT;
    typedef SharedResource<ResourceT> OutputResourceT;
    // Mapping from id to resource
    std::unordered_map<LoadArgsT, std::weak_ptr<ManagedResourceT>> resources;

    LoadFnT_DEFINITION
    DeleteFnT_DEFINITION

    LoadFnT loadFn;
    DeleteFnT deleteFn;

  public:
    SharedResourceManager(LoadFnT loadFn, DeleteFnT deleteFn) :
        loadFn(loadFn), deleteFn(deleteFn) {}

    template <typename... T>
    OutputResourceT Load(T... args) {
        return Load(LoadArgsT(args...));
    }

    OutputResourceT Load(LoadArgsT args) {
        std::shared_ptr<ManagedResourceT> res;

        if (resources.count(args) == 0) {
            res = std::shared_ptr<ManagedResourceT>(new ManagedResourceT(loadFn(args), deleteFn, this));
            resources[args] = res;
            return OutputResourceT(std::static_pointer_cast<Resource<ResourceT>>(res));
        }

        return OutputResourceT(std::static_pointer_cast<Resource<ResourceT>>(resources[args].lock()));
    }

    void DeleteUnusedResources() {
        for (auto it = resources.begin(); it != resources.end();) {
            if (it->second.expired()) {
                it = resources.erase(it);
            } else {
                it++;
            }
        }
    }

    size_t size() {
        return resources.size();
    }
};

template <typename ResourceT, typename LoadArgsT>
SharedResourceManager<ResourceT, LoadArgsT> *MakeSharedResourceManager(
    LoadFnT_TYPE(loadFn),
    DeleteFnT_TYPE(deleteFn)) {
    return (new SharedResourceManager<ResourceT, LoadArgsT>(loadFn, deleteFn));
}

template <typename ResourceT, typename LoadArgsT>
class UniqueResourceManager : public BaseResourceManager {
  private:
    LoadFnT_DEFINITION
    DeleteFnT_DEFINITION

    LoadFnT loadFn;
    DeleteFnT deleteFn;

  public:
    UniqueResourceManager(LoadFnT loadFn, DeleteFnT deleteFn) :
        loadFn(loadFn), deleteFn(deleteFn) {}

    template <typename... T>
    UniqueResource<ResourceT> Load(T... args) {
        return Load(LoadArgsT(args...));
    }

    UniqueResource<ResourceT> Load(LoadArgsT args) {
        return UniqueResource(std::move(std::unique_ptr<Resource<ResourceT>>(new Resource<ResourceT>(loadFn(args), deleteFn))));
    }
};

template <typename ResourceT, typename LoadArgsT>
UniqueResourceManager<ResourceT, LoadArgsT> *MakeUniqueResourceManager(
    LoadFnT_TYPE(loadFn),
    DeleteFnT_TYPE(deleteFn)) {
    return (new UniqueResourceManager<ResourceT, LoadArgsT>(loadFn, deleteFn));
}

/*template <typename... T>
using UniqueResManagerGroup = std::tuple<T...>;

template <typename... T>
using SharedResManagerGroup = std::tuple<T...>;*/


template <typename... T>
class UniqueResManagerGroup : public std::tuple<T...> {
  public:
    UniqueResManagerGroup(T... args) : std::tuple<T...>(args...) {}
};


template <typename... T>
class SharedResManagerGroup : public std::tuple<T...> {
  public:
    SharedResManagerGroup(T... args) : std::tuple<T...>(args...) {}
};



template <typename... T>
class ResourceManager {
};

template <typename... UniqueResourceT, typename... UniqueLoadArgsT, typename... SharedResourceT, typename... SharedLoadArgsT>
class ResourceManager<std::tuple<UniqueResourceManager<UniqueResourceT, UniqueLoadArgsT>...>, std::tuple<SharedResourceManager<SharedResourceT, SharedLoadArgsT>...>> {
    std::tuple<UniqueResourceManager<UniqueResourceT, UniqueLoadArgsT>*...> uniqueResourceManagers;
    std::tuple<SharedResourceManager<SharedResourceT, SharedLoadArgsT>*...> sharedResourceManagers;

  public:

    ResourceManager(UniqueResourceManager<UniqueResourceT, UniqueLoadArgsT>*... _uniqueResourceManagers,
                    SharedResourceManager<SharedResourceT, SharedLoadArgsT>*... _sharedResourceManagers) :
        uniqueResourceManagers(_uniqueResourceManagers...), sharedResourceManagers(_sharedResourceManagers...) {}

    template <typename ResourceT, typename LoadArgsT>
    SharedResource<ResourceT> LoadShared(LoadArgsT args) {
        auto resManager = std::get<SharedResourceManager<ResourceT, LoadArgsT>*>(sharedResourceManagers);
        return resManager->Load(args);
    }

    template <typename ResourceT, typename LoadArgsT>
    UniqueResource<ResourceT> LoadUnique(LoadArgsT args) {
        auto resManager = std::get<UniqueResourceManager<ResourceT, LoadArgsT>*>(uniqueResourceManagers);
        return resManager->Load(args);
    }

    template <typename ResourceT, typename LoadArgsT>
    SharedResourceManager<ResourceT, LoadArgsT> *GetSharedManager() {
        return std::get<SharedResourceManager<ResourceT, LoadArgsT>*>(sharedResourceManagers);
    }

    template <typename ResourceT, typename LoadArgsT>
    UniqueResourceManager<ResourceT, LoadArgsT> *GetUniqueManager() {
        return std::get<UniqueResourceManager<ResourceT, LoadArgsT>*>(uniqueResourceManagers);
    }
};

template<typename... T1, typename... T2>
auto CreateResourceManager(Engine::UniqueResManagerGroup<T1*...> uniqueGroup, Engine::SharedResManagerGroup<T2*...> sharedGroup) {
    const auto &t1 = (std::tuple<T1*...>)uniqueGroup;
    const auto &t2 = (std::tuple<T2*...>)sharedGroup;

    auto merged = std::tuple_cat(t1, t2);

    return std::apply([](const auto... args) {
        return Engine::ResourceManager<std::tuple<T1...>,
               std::tuple<T2...>>(args...);
    }, merged);
}

/*template <typename ResourceT>
using UniqueResource = std::unique_ptr<Resource<ResourceT>>;

template <typename ResourceT>
using SharedResource = std::shared_ptr<Resource<ResourceT>>;*/

}
