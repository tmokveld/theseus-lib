#pragma once

// Check/rewrite this after the last changes to the allocators.

#if 0

/**
 * This class implements a memory pool that allocates chunks of memory of a
 * size given at construction time. The allocated chunk will be used to supply
 * memory when allocate() is called. If the chunk is not large enough to satisfy
 * the allocation request, a new chunk will be allocated and the previous chunk
 * will never be revisited again, even if there was memory left to use. In case
 * the requested size is larger than the chunk size given at construction time,
 * the requested size will be used to allocate the new chunk.
 *
 * The memory pool does not allow deallocation of memory, it only allows to
 * fully clear the memory pool so that the allocated chunks can be reused.
 *
 */

namespace theseus {

class ArenaMemPool : public GrowingMemPool {
public:
    /**
     * Construct an arena with one chunk of memory.
     *
     * @param min_chunk_bytes The minimum size of each chunk of memory
     * allocated.
     */
    ArenaMemPool(std::size_t min_chunk_bytes) noexcept
        : _min_chunk_bytes(min_chunk_bytes) {

        _first_chunk = new Chunk{min_chunk_bytes,
                                 0,
                                 static_cast<std::byte *>(::operator new(_min_chunk_bytes)),
                                 nullptr};
        _curr_chunk = _first_chunk;
    };

    /**
     * Deallocate all the memory.
     *
     */
    ~ArenaMemPool() noexcept {
        Chunk *chunk = _first_chunk;
        while (chunk != nullptr) {
            Chunk *next = chunk->next;

            ::operator delete(static_cast<void *>(chunk->data));
            delete chunk;

            chunk = next;
        }
    };

    /**
     * Clear the pool without deallocating the memory. The old memory will be
     * used for new allocations if possible.
     *
     */
    void clear() override {
        _curr_chunk = _first_chunk;
        _curr_chunk->used_bytes = 0;
    }

    // Do not allow copy.
    ArenaMemPool(const ArenaMemPool &) = delete;
    ArenaMemPool &operator=(const ArenaMemPool &) = delete;

    /**
     * Allocate memory for @p nbytes bytes.
     * Use previously allocated unused bytes if possible.
     *
     * @param nbytes The number of bytes to allocate.
     * @return void* A pointer to the allocated memory.
     */
    void *allocate(std::size_t nbytes) override {
        void *ret = nullptr;

        bool need_new_chunk = _curr_chunk->total_bytes - _curr_chunk->used_bytes <
                              nbytes;
        if (need_new_chunk) {
            // Try to find a chunk that has enough space.
            // Here we are not accessing the used_bytes but the
            // total_bytes, since the used_bytes are stale.
            while (_curr_chunk->next != nullptr) {
                _curr_chunk = _curr_chunk->next;
                if (_curr_chunk->total_bytes >= nbytes) {
                    _curr_chunk->used_bytes = 0;
                    need_new_chunk = false;
                    break;
                }
            }
        }

        // We need a new chunk.
        if (need_new_chunk) {
            std::size_t alloc_bytes = std::max(nbytes, _min_chunk_bytes);

            Chunk *new_chunk = new Chunk{alloc_bytes,
                                         0,
                                         static_cast<std::byte *>(::operator new(alloc_bytes)),
                                         nullptr};

            assert(_curr_chunk->next == nullptr);

            // Insert the chunk after curr_chunk.
            _curr_chunk->next = new_chunk;
            _curr_chunk = new_chunk;
        }

        ret = static_cast<void *>(_curr_chunk->data + _curr_chunk->used_bytes);
        _curr_chunk->used_bytes += nbytes;

        return ret;
    }

private:
    struct Chunk {
        std::size_t total_bytes;
        std::size_t used_bytes;
        std::byte *data;

        Chunk *next;
    };

    Chunk *_first_chunk;
    Chunk *_curr_chunk;

    const std::size_t _min_chunk_bytes;
};

} // namespace theseus

#endif