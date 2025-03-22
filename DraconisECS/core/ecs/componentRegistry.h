#pragma once
#include <unordered_map>
#include <typeindex>
#include <bitset>
#include "ecsGlobals.h"

namespace ecs {
    // Component registry for runtime size lookup
    class ComponentRegistry {
    public:
        static ComponentRegistry& getInstance() {
            static ComponentRegistry instance;
            return instance;
        }

        template<typename T>
        void registerComponent() {
            size_t id = typeId<T>;
            sizes[id] = sizeof(T);
            names[id] = typeid(T).name();
        }

        size_t getComponentSize(size_t typeId) const {
            auto it = sizes.find(typeId);
            if (it != sizes.end()) {
                return it->second;
            }
            return sizeof(void*); // Fallback to pointer size
        }

        const char* getComponentName(size_t typeId) const {
            auto it = names.find(typeId);
            if (it != names.end()) {
                return it->second;
            }
            return "Unknown";
        }

        size_t getTotalComponentSize(const std::bitset<MAX_COMPONENTS>& signature) const {
            size_t totalSize = 0;
            for (size_t i = 0; i < MAX_COMPONENTS; ++i) {
                if (signature.test(i)) {
                    totalSize += getComponentSize(i);
                }
            }
            return totalSize;
        }

    private:
        ComponentRegistry() = default;
        std::unordered_map<size_t, size_t> sizes;
        std::unordered_map<size_t, const char*> names;
    };
} // namespace ecs 