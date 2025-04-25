#pragma once

#include "growing_mem_pool.h"

/**
 * STL allocator (wrapper) for memory pools that only can allocate memory and
 * never free it.
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
    using propagate_on_container_move_assignment = std::true_type;

    /**
     * Construct an allocator that uses the given memory pool.
     *
     * @param mem_pool A GrowingMemPool.
     */
    GrowingAllocator(GrowingMemPool *mem_pool) noexcept : _mem_pool(mem_pool){};

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
     * We do not do anything, since a GrowingAllocator just grows.
     *
     * @param p Unused.
     */
    void deallocate([[maybe_unused]] value_type *p, [[maybe_unused]] std::size_t) {
        // We don't do anything, we just grow.
    }

    /**
     * true only if the storage allocated by this can be deallocated
     * through the other allocator. We do not really deallocate, so
     * always true.
     *
     * @tparam U The type of the other allocator.
     * @return bool True.
     */
    template <class U>
    bool operator==(const GrowingAllocator<U> &) const {
        return true;
    }

    /**
     * The inverse to == operator.
     *
     * @tparam U The type of the other allocator.
     * @return bool False.
     */
    template <class U>
    bool operator!=(const GrowingAllocator<U> &) const {
        return false;
    }

private:
    GrowingMemPool *_mem_pool;

    // Add a friend declaration for the rebind constructor
    template <class U>
    friend class GrowingAllocator;
};

} // namespace theseus