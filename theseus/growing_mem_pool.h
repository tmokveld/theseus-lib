#pragma once

/**
 * A memory pull that only creates new chunks, it does not allow deallocation.
 *
 */

namespace theseus {

class GrowingMemPool {
public:
    /**
     * Allocate nbytes of memory.
     * 
     * @param nbytes The number of bytes to allocate.
     * @return A pointer to the allocated memory.
     */
    virtual void *allocate(std::size_t nbytes) = 0;

    /**
     * Clear the memory pool so that all the allocated data can be reused.
     *
     */
    virtual void clear() = 0;

    /**
     * Virtual class destructor.
     *
     */
    virtual ~GrowingMemPool() = default;
};

} // namespace theseus