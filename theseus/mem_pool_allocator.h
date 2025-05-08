#pragma once

#include "mem_pool.h"

/**
 * STL allocator (wrapper) for memory pools.
 *
 * An STL allocator must have no state, so we cannot store the memory
 * pool in the allocator. Instead, we store a pointer to the memory
 * pool.
 */

namespace theseus {

template <class T>
class MemPoolAllocator {
public:
    using value_type = T;
    using propagate_on_container_move_assignment =
          MemPool::propagate_on_container_move_assignment;
    // using implements_reallocate = std::true_type;

    /**
     * Construct an allocator that uses the given memory pool.
     *
     * @param mem_pool A MemPool.
     */
    MemPoolAllocator(MemPool *mem_pool) noexcept : _mem_pool(mem_pool){};

    /**
     * Copy constructor.
     *
     * @param other Other allocator.
     */
    MemPoolAllocator(const MemPoolAllocator &other) noexcept = default;

    /**
     * Copy operator.
     *
     * @param other The other allocator.
     */
    MemPoolAllocator &operator=(const MemPoolAllocator &other) noexcept {
        if (this == &other) {
            return *this;
        }

        _mem_pool = other._mem_pool;
        return *this;
    };

    /**
     * Create an allocator of type U that uses the same memory pool.
     *
     * @param other The other allocator.
     */
    template <class U>
    MemPoolAllocator(const MemPoolAllocator<U> &other) noexcept
        : _mem_pool(other._mem_pool){}

    /**
     * Move constructor.
     *
     * @param other Other allocator.
     */
    MemPoolAllocator(MemPoolAllocator &&other) noexcept = default;

    /**
     * Move operator.
     *
     * @param other The other allocator.
     */
    MemPoolAllocator &operator=(MemPoolAllocator &&other) noexcept = default;

    /**
     * Move operator from an allocator of type U.
     *
     * @param other The other allocator.
     */
    template <class U>
    MemPoolAllocator(MemPoolAllocator<U> &&other) noexcept
        : _mem_pool(std::move(other._mem_pool)) {
        other._mem_pool = nullptr;
    }

    /**
     * Allocate memory for num elements of type value_type.
     *
     * @param num The number of elements to allocate.
     * @return value_type* A pointer to the allocated memory.
     */
    value_type *allocate(std::size_t num) {
        return static_cast<value_type *>(_mem_pool->allocate(num * sizeof(value_type)));
    }

    /**
     * Deallocate memory for num elements of type value_type.
     *
     * @param p The pointer to the memory to deallocate.
     */
    void deallocate(value_type *p, std::size_t num) {
        _mem_pool->deallocate(p, num * sizeof(value_type));
    }

    // value_type *reallocate(void *p, std::size_t old_bytes, std::size_t new_bytes) {
    //     return _mem_pool->reallocate(p, old_bytes, new_bytes);
    // }

    /**
     * True only if the storage allocated by this can be deallocated
     * through the other allocator. Ask the memory pool.
     *
     * @tparam U The type of the other allocator.
     * @return bool True if the storage can be deallocated through the other
     * allocator.
     */
    template <class U>
    bool operator==(const MemPoolAllocator<U> &other) const {
        return (*_mem_pool) == other->mem_pool;
    }

    /**
     * The inverse to == operator.
     *
     * @tparam U The type of the other allocator.
     * @return bool True if the storage cannot be deallocated through the other
     * allocator.
     */
    template <class U>
    bool operator!=(const MemPoolAllocator<U> &other) const {
        return (*_mem_pool) != other->mem_pool;
    }

private:
    MemPool *_mem_pool;

    // Add a friend declaration for the rebind constructor
    template <class U>
    friend class MemPoolAllocator;
};

} // namespace theseus