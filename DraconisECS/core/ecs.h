#pragma once
#include <iostream>
#include <bitset>

namespace ecs
{
    inline size_t type_id_seq = 0;
    template< typename T > inline const size_t type_id = type_id_seq++;

    template <typename ... T>
    struct Entity {

        Entity() {
            std::cout << "Creating entity with components: " << std::endl;
            ((signature.set(type_id<T>)), ...);
        }

        std::bitset<32> signature;
    };

    struct Chunk {
        std::bitset<32> signature;
    };

    class ChunkManager {
    public:
        ChunkManager() {
            std::cout << "Creating chunk manager" << std::endl;
        }

        template <typename ... T>
        void createEntity(T... components) {
            Entity<T...> entity;
            std::cout << "Creating entity with signature: " << entity.signature << std::endl;
        }
    };
} // namespace ecs