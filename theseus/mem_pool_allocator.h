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
class GrowingAllocator {
public:
    using value_type = T;
    using propagate_on_container_move_assignment =
          MemPool::propagate_on_container_move_assignment;
    using realloc_possible = std::true_type;

    /**
     * Construct an allocator that uses the given memory pool.
     *
     * @param mem_pool A MemPool.
     */
    GrowingAllocator(MemPool *mem_pool) noexcept : _mem_pool(mem_pool){};

    /**
     * Copy constructor.
     *
     * @param other Other allocator.
     */
    GrowingAllocator(const GrowingAllocator &other) noexcept = default;

    /**
     * Copy operator.
     *
     * @param other The other allocator.
     */
    GrowingAllocator &operator=(const GrowingAllocator &other) noexcept {
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
    GrowingAllocator(const GrowingAllocator<U> &other) noexcept
        : _mem_pool(other._mem_pool){}

    /**
     * Move constructor.
     *
     * @param other Other allocator.
     */
    GrowingAllocator(GrowingAllocator &&other) noexcept = default;

    /**
     * Move operator.
     *
     * @param other The other allocator.
     */
    GrowingAllocator &operator=(GrowingAllocator &&other) noexcept = default;

    /**
     * Move operator from an allocator of type U.
     *
     * @param other The other allocator.
     */
    template <class U>
    GrowingAllocator(GrowingAllocator<U> &&other) noexcept
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

    /**
     * Reallocate memory from @p p to hold at least @p num_elements of type
     * value_type.
     *
     * @param p The pointer to the memory to reallocate.
     * @param num The number of elements to reallocate.
     * @return value_type* A pointer to the reallocated memory.
     */
    // value_type *reallocate(value_type* p, std::size_t num) {
    //     return static_cast<value_type*>(_mem_pool->reallocate(p, num * sizeof(value_type)));
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
    bool operator==(const GrowingAllocator<U> &other) const {
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
    bool operator!=(const GrowingAllocator<U> &other) const {
        return (*_mem_pool) != other->mem_pool;
    }

private:
    MemPool *_mem_pool;

    // Add a friend declaration for the rebind constructor
    template <class U>
    friend class GrowingAllocator;
};

} // namespace theseus