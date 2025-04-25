#pragma once

#include <vector>

#include "cell.h"
#include "wavefront_mem_pool.h"
#include "growing_allocator.h"

/**
 * TODO:
 *
 */

namespace theseus {

class BeyondScope {
public:
    /**
     * @brief Construct a new Beyond Scope object
     *
     */
    BeyondScope() {
        constexpr int expected_nscores = 1024;
        _sdata.reserve(expected_nscores);
    }

    /**
     * @brief Reinitialize the beyond the scope object each time that a new
     * alignment is called.
     *
     */
    void new_alignment() {
        _m_wf_mem_pool.clear();
        _m_jumps_mem_pool.clear();
        _i_jumps_mem_pool.clear();
        _i2_jumps_mem_pool.clear();

        _sdata.clear();
    }


    /**
     * @brief Add a new score to the beyond scope object.
     *
     */
    void new_score() {
        if (_sdata.size() == _sdata.capacity()) {
            _sdata.reserve(_sdata.capacity() * 2);
        }

        ScoreData sd(&_m_wf_mem_pool,
                     &_m_jumps_mem_pool,
                     &_i_jumps_mem_pool,
                     &_i2_jumps_mem_pool);

        _sdata.push_back(std::move(sd));
    }

    /**
     * @brief Access the M wavefront associated to score "score".
     *
     * @param score
     * @return Cell::Wavefront&
     */
     Cell::Wavefront &m_wf(int score) {
        return _sdata[score]._m_wf;
    }

    /**
     * @brief Access the M jumps wavefront associated to score "score".
     *
     * @param score
     * @return Cell::Wavefront&
     */
    Cell::Wavefront &m_jumps(int score) {
        return _sdata[score]._m_jumps;
    }

    /**
     * @brief Access the I jumps wavefront associated to score "score".
     *
     * @param score
     * @return Cell::Wavefront&
     */
    Cell::Wavefront &i_jumps(int score) {
        return _sdata[score]._i_jumps;
    }

    /**
     * @brief Access the I2 jumps wavefront associated to score "score".
     *
     * @param score
     * @return Cell::Wavefront&
     */
    Cell::Wavefront &i2_jumps(int score) {
        return _sdata[score]._i2_jumps;
    }

private:
    // Memory pools for faster realloc.
    WavefrontMemPool _m_wf_mem_pool;
    WavefrontMemPool _m_jumps_mem_pool;
    WavefrontMemPool _i_jumps_mem_pool;
    WavefrontMemPool _i2_jumps_mem_pool;

    struct ScoreData {
        Cell::Wavefront _m_wf;
        Cell::Wavefront _m_jumps;
        Cell::Wavefront _i_jumps;
        Cell::Wavefront _i2_jumps;

        ScoreData(WavefrontMemPool *const m_wf_mem_pool,
                  WavefrontMemPool *const m_jumps_mem_pool,
                  WavefrontMemPool *const i_jumps_mem_pool,
                  WavefrontMemPool *const i2_jumps_mem_pool) :
            _m_wf(GrowingAllocator<Cell>{m_wf_mem_pool}),
            _m_jumps(GrowingAllocator<Cell>{m_jumps_mem_pool}),
            _i_jumps(GrowingAllocator<Cell>{i_jumps_mem_pool}),
            _i2_jumps(GrowingAllocator<Cell>{i2_jumps_mem_pool}) {}
    };

    std::vector<ScoreData> _sdata;
};

} // namespace theseus