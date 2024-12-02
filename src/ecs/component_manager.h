#pragma once

#include "component_usage_types.h"
#include "entity.h"

#include "../logging/logging.h"

#include <typeinfo>
#include <iterator>
#include <cstddef>

namespace ECS {

class ComponentIteratorStrategyBase {

};

class ComponentManagerBase {
  public:
    virtual ~ComponentManagerBase() {};
    virtual void RemoveComponent(Entity entity) = 0;
};

template <typename ComponentT>
class ComponentManager;

template <typename ComponentT>
class ComponentManagerView {
  private:
    std::vector<Entity> entities;
    ComponentManager<ComponentT> *componentManager = nullptr;
    typedef std::tuple<ComponentT*, unsigned int> (ComponentManager<ComponentT>::*GetComponentFnT)(Entity entity, const unsigned int pos);

  public:
    ComponentManagerView() {}

    ComponentManagerView(const std::vector<Entity> &entities, ComponentManager<ComponentT> *componentManager) :
        entities(entities), componentManager(componentManager) {}

    ~ComponentManagerView() {
        if (componentManager) {
            componentManager->SignalViewFinished();
        }
    }

    // ITERATOR
    class ComponentIterator {
        friend class ComponentManagerView<ComponentT>;
      public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = ComponentT;
        using pointer           = ComponentT*;
        using reference         = ComponentT&;

      private:
        std::vector<Entity> *entities;
        unsigned int entityPos;
        ComponentManager<ComponentT> *componentManager;
        pointer m_ptr = nullptr;
        unsigned int componentPos = 0;
        GetComponentFnT getComponentFn;

      public:
        ComponentIterator& operator=(const ComponentIterator& other) {
            entities = other.entities;
            entityPos = other.entityPos;
            componentManager = other.componentManager;
            m_ptr = other.m_ptr;
            componentPos = other.componentPos;
            getComponentFn = other.getComponentFn;
            return *this;
        }

        ComponentIterator() {}

        ComponentIterator(std::vector<Entity> *entities, ComponentManager<ComponentT> *componentManager, GetComponentFnT getComponentFn) :
            componentManager(componentManager), getComponentFn(getComponentFn) {
            this->entities = (entities);
            entityPos = 0;

            if (entities->size() == 0) {
                return;
            }

            unsigned int newPos;
            std::tie(m_ptr, newPos) = (componentManager->*getComponentFn)((*entities)[entityPos], 0);

            if (m_ptr == nullptr) {
                componentPos--;
            } else {
                componentPos = newPos;
            }
        }

        ComponentIterator(const ComponentIterator &other) {
            entities = other.entities;
            entityPos = other.entityPos;
            componentManager = other.componentManager;
            m_ptr = other.m_ptr;
            componentPos = other.componentPos;
            getComponentFn = other.getComponentFn;
        }

        reference operator*() const {
            return *m_ptr;
        }


        reference get() const {
            return *m_ptr;
        }

        pointer operator->() const {
            return m_ptr;
        }

        // Prefix increment
        ComponentIterator &operator++() {
            entityPos++;
            componentPos++;

            //printf("\n%d\n", entityPos);
            if (entityPos >= entities->size()) {
                m_ptr = nullptr;
                return *this;
            }

            unsigned int newPos;
            std::tie(m_ptr, newPos) = (componentManager->*getComponentFn)((*entities)[entityPos], componentPos);

            if (m_ptr == nullptr) {
                componentPos--;
            } else {
                componentPos = newPos;
            }

            return *this;
        }

        // Postfix increment
        ComponentIterator operator++(int) {
            ComponentIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const ComponentIterator& a, const ComponentIterator& b) {
            return a.m_ptr == b.m_ptr;
        };

        friend bool operator!=(const ComponentIterator& a, const ComponentIterator& b) {
            return a.m_ptr != b.m_ptr;
        };

        bool finished() {
            return entityPos >= entities->size();
        }
    };

    ComponentIterator begin() {
        if (entities.size() < componentManager->size() / 30) {
            return ComponentIterator(&entities, componentManager, &ComponentManager<ComponentT>::GetComponentBinarySearch);
        } else {
            return ComponentIterator(&entities, componentManager, &ComponentManager<ComponentT>::GetComponentLinearSearch);
        }
    }

    ComponentIterator end() {
        if (entities.size() == 0) {
            return begin();
        }

        ComponentIterator it;

        if (entities.size() < componentManager->size() / 30) {
            it = ComponentIterator(&entities, componentManager, &ComponentManager<ComponentT>::GetComponentBinarySearch);
        } else {
            it = ComponentIterator(&entities, componentManager, &ComponentManager<ComponentT>::GetComponentLinearSearch);
        }

        it.entityPos = it.entities->size() - 1;
        it++;
        it.componentPos = it.componentManager->size();
        return it;
    }
};


template <typename ComponentT>
class ComponentManager : public ComponentManagerBase {
    friend class BaseComponentGroupView;
  private:
    struct Operation {
        virtual ~Operation() {}
        virtual void Process() = 0;
    };

    struct AddOperation : public Operation {
      private:
        ComponentManager<ComponentT> *componentManager;
        Entity entity;
        ComponentT component;

      public:
        AddOperation(ComponentManager<ComponentT> *componentManager, Entity entity, const ComponentT &component) :
            componentManager(componentManager), entity(entity), component(component) {}

        virtual void Process() override {
            componentManager->AddComponent(entity, component);
        }
    };

    struct RemoveOperation : public Operation {
      private:
        ComponentManager<ComponentT> *componentManager;
        Entity entity;

      public:
        RemoveOperation(ComponentManager<ComponentT> *componentManager, Entity entity) :
            componentManager(componentManager), entity(entity) {}

        virtual void Process() override {
            componentManager->RemoveComponent(entity);
        }
    };

  private:
    std::vector<std::pair<Entity, ComponentT>> components;
    std::vector<std::unique_ptr<Operation>> uncommitedOperations;
    unsigned int viewsInUse = 0;
    const bool optimization = true;

    void Commit() {
        if (uncommitedOperations.empty()) {
            return;
        }

        for (auto &operation : uncommitedOperations) {
            operation->Process();
        }

        uncommitedOperations.clear();
        Optimize();
    }

    void Optimize() {
        if (!optimization) {
            return;
        }

        std::sort(components.begin(), components.end(),
        [](const std::pair<Entity, ComponentT> &c1, const std::pair<Entity, ComponentT> &c2) {
            return c1.first < c2.first;
        });

        if (components.size() >= components.capacity() / 2) {
            return;
        }

        components.shrink_to_fit();
    }

  protected:
    ComponentManagerView<ComponentT> *GetViewInternal(std::vector<Entity> &entities) {
        viewsInUse++;
        auto p = new ComponentManagerView<ComponentT>(entities, this);
        return p;
    }

  public:
    void SignalViewFinished() {
        viewsInUse--;

        if (viewsInUse == 0) {
            Commit();
        }
    }

    size_t size() {
        return components.size();
    }

    ComponentManagerView<ComponentT> GetView(std::vector<Entity> &entities) {
        viewsInUse++;
        return ComponentManagerView<ComponentT>(entities, this);
    }

    void AddComponent(const Entity entity, const ComponentT &component) {
        if (viewsInUse) {
            uncommitedOperations.emplace_back(std::make_unique<AddOperation>(this, entity, component));
            return;
        }

        components.push_back(std::make_pair(entity, ComponentT(component)));
        Optimize();
    }

    auto find(const Entity entity, unsigned int pos = 0) {
        auto it = std::lower_bound(components.begin() + pos, components.end(), entity,
        [](const std::pair<Entity, ComponentT> &elem, const Entity & key) {
            return elem.first < key;
        });

        if (it == components.end() || it->first != entity) {
            return components.end();
        }

        return it;
    }

    virtual void RemoveComponent(Entity entity) override {
        if (viewsInUse) {
            uncommitedOperations.emplace_back(std::make_unique<RemoveOperation>(this, entity));
            return;
        }

        auto it = find(entity);

        if (it == components.end()) {
            return;
        }

        int pos = it - components.begin();
        int last = components.size() - 1;

        std::swap(components[pos], components[last]);
        components.pop_back();

        Optimize();
    }

    ComponentT *GetComponent(int pos) {
        if (pos >= components.size()) {
            return nullptr;
        }

        return &(components[pos].second);
    }

    ComponentT *GetComponent(Entity entity) {
        auto it = find(entity, 0);

        if (it == components.end()) {
            return nullptr;
        }

        return &(it->second);
    }

    std::tuple<ComponentT*, unsigned int> GetComponent(Entity entity, const unsigned int pos) {
        auto it = find(entity, pos);

        if (it == components.end()) {
            return std::make_tuple(nullptr, components.size());
        }

        return std::make_tuple(&(it->second), it - components.begin());
    }

    std::tuple<ComponentT*, unsigned int> GetComponentBinarySearch(Entity entity, const unsigned int pos) {
        auto it = find(entity, pos);

        if (it == components.end()) {
            return std::make_tuple(nullptr, components.size());
        }

        return std::make_tuple(&(it->second), it - components.begin());
    }

    std::tuple<ComponentT*, unsigned int> GetComponentLinearSearch(Entity entity, const unsigned int pos) {
        auto it = components.begin();

        if (it == components.end() || pos > components.size()) {
            return std::make_tuple(nullptr, components.size());
        }

        it += pos;

        while (it != components.end() && it->first != entity) {
            it++;
        }

        if (it == components.end()) {
            return std::make_tuple(nullptr, 0);
        }

        return std::make_tuple(&(it->second), it - components.begin());
    }

    const std::vector<std::pair<Entity, ComponentT>> *GetComponentEntityVector() {
        return &components;
    }
};

class BaseComponentGroupView {
  public:
    template <typename T>
    ComponentManagerView<T> *GetViewInternal(ComponentManager<T>* cm, std::vector<Entity> &entities) {
        return cm->GetViewInternal(entities);
    }
};

template <typename... T>
class ComponentGroupView : public BaseComponentGroupView {
};

template <typename... T>
class ComponentGroupView<std::tuple<T...>> : public BaseComponentGroupView {
  private:
    std::tuple<ComponentManagerView<T>*...> views;

  public:
    ~ComponentGroupView() {
        std::apply([](auto const & ...x) {
            (delete x, ...);
        }, views);
    }

    ComponentGroupView(int) {}

    ComponentGroupView(ComponentManagerView<T>*... views) {
        this->views = std::make_tuple(views...);
    }

    ComponentGroupView(std::vector<Entity> entities, ComponentManager<T>*... componentManagers) {
        this->views = std::make_tuple(GetViewInternal(componentManagers, entities)...);
    }

    const auto GetViews() {
        return views;
    }

    // ITERATOR
    class ComponentGroupIterator {
        //friend class ComponentGroupView<T...>;
      public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = std::tuple<T...>;
        using pointer           = std::tuple<T*...>;
        using reference         = std::tuple<T&...>;

      private:
        std::tuple<typename ComponentManagerView<T>::ComponentIterator...> tupleIter;
      public:
        ComponentGroupIterator(typename ComponentManagerView<T>::ComponentIterator... it) {
            tupleIter = std::make_tuple(it...);
        }

        ComponentGroupIterator(const ComponentGroupIterator &other) {
            tupleIter = other.tupleIter;
        }

        reference operator*() const {
            return std::apply([](auto const & ...x) {
                return std::tie((x.get())...);
            }, tupleIter);
            ;
        }

        pointer operator->() const {
            return
            std::apply([](auto const & ...x) {
                return std::make_tuple(x.operator->()...);
            }, tupleIter);
        }

        // Prefix increment
        ComponentGroupIterator &operator++() {
            tupleIter = std::apply([](auto ...x) {
                return std::make_tuple(++x...);
            }, tupleIter);
            return *this;
        }

        // Postfix increment
        ComponentGroupIterator operator++(int) {
            ComponentGroupIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const ComponentGroupIterator& a, const ComponentGroupIterator& b) {
            return (a.tupleIter == b.tupleIter);
        };

        friend bool operator!=(const ComponentGroupIterator& a, const ComponentGroupIterator& b) {
            return (a.tupleIter != b.tupleIter);
        };
    };

    ComponentGroupIterator begin() {
        return std::make_from_tuple<ComponentGroupIterator>(std::apply([](auto ...x) {
            return std::make_tuple(x->begin()...);
        }, views));
    }

    ComponentGroupIterator end() {
        return std::make_from_tuple<ComponentGroupIterator>(std::apply([](auto ...x) {
            return std::make_tuple(x->end()...);
        }, views));
    }
};

template <typename... T, typename... OptionalsT>
class ComponentGroupView<std::tuple<T...>, std::tuple<OptionalsT...>> : public BaseComponentGroupView {
  private:
    std::tuple<ComponentManagerView<T>*..., ComponentManagerView<OptionalsT>*...> views;

  public:
    ~ComponentGroupView() {
        std::apply([](auto const & ...x) {
            (delete x, ...);
        }, views);
    }

    ComponentGroupView(int) {}

    ComponentGroupView(ComponentManagerView<T>*... views, ComponentManagerView<OptionalsT>*... optViews) {
        this->views = std::tuple_cat(
                          std::make_tuple(views...),
                          std::make_tuple(optViews...));
    }

    ComponentGroupView(std::vector<Entity> entities, ComponentManager<T>*... componentManagers, ComponentManager<OptionalsT>*... optComponentManagers) {
        this->views = std::tuple_cat(
                          std::make_tuple(GetViewInternal(componentManagers, entities)...),
                          std::make_tuple(GetViewInternal(optComponentManagers, entities)...));
    }

    const auto GetViews() {
        return views;
    }

    // ITERATOR
    class ComponentGroupIterator {
        //friend class ComponentGroupView<T...>;
      public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = std::tuple<T..., OptionalsT*...>;
        using pointer           = std::tuple<T*..., OptionalsT*...>;
        using reference         = std::tuple<T&..., OptionalsT*...>;

      private:
        std::tuple<typename ComponentManagerView<T>::ComponentIterator...> tupleIter;
        std::tuple<typename ComponentManagerView<OptionalsT>::ComponentIterator...> optTupleIter;
      public:
        ComponentGroupIterator(typename ComponentManagerView<T>::ComponentIterator... it, typename ComponentManagerView<OptionalsT>::ComponentIterator... itOpt) {
            tupleIter = std::make_tuple(it...);
            optTupleIter = std::make_tuple(itOpt...);
        }

        ComponentGroupIterator(const ComponentGroupIterator &other) {
            tupleIter = other.tupleIter;
            optTupleIter = other.optTupleIter;
        }

        reference operator*() const {
            return std::tuple_cat(std::apply([](auto const & ...x) {
                return std::tie((x.get())...);
            }, tupleIter),

            std::apply([](auto const & ...x) {
                return std::make_tuple((x.operator->())...);
            }, optTupleIter))
            ;
        }

        pointer operator->() const {
            return std::tuple_cat(
            std::apply([](auto const & ...x) {
                return std::make_tuple(x.operator->()...);
            }, tupleIter),

            std::apply([](auto const & ...x) {
                return std::make_tuple((x.operator->())...);
            }, optTupleIter));
        }

        // Prefix increment
        ComponentGroupIterator &operator++() {
            tupleIter = std::apply([](auto ...x) {
                return std::make_tuple(++x...);
            }, tupleIter);

            optTupleIter = std::apply([](auto ...x) {
                return std::make_tuple(++x...);
            }, optTupleIter);
            return *this;
        }

        // Postfix increment
        ComponentGroupIterator operator++(int) {
            ComponentGroupIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const ComponentGroupIterator& a, const ComponentGroupIterator& b) {
            return (a.tupleIter == b.tupleIter && a.optTupleIter == b.optTupleIter);
        };

        friend bool operator!=(const ComponentGroupIterator& a, const ComponentGroupIterator& b) {
            return (a.tupleIter != b.tupleIter || a.optTupleIter != b.optTupleIter);
        };
    };

    ComponentGroupIterator begin() {
        return std::make_from_tuple<ComponentGroupIterator>(std::apply([](auto ...x) {
            return std::make_tuple(x->begin()...);
        }, views));
    }

    ComponentGroupIterator end() {
        return std::make_from_tuple<ComponentGroupIterator>(std::apply([](auto ...x) {
            return std::make_tuple(x->end()...);
        }, views));
    }
};

}
