#pragma once

/**
 * TODO:
 *
 */

class BeyondScope {
public:
    // TODO: Prefer this?
    // using Wavefront = ManualCapacityVector<Cell>;
    // using Jumps = ManualCapacityVector<Cell>;

    // TODO:
    BeyondScope() {
        constexpr int expected_nscores = 1024;
        _sdata.realloc(expected_nscores);
    }

    // TODO:
    void new_score() {
        if (_sdata.size() == _sdata.capacity()) {
            _sdata.realloc(_sdata.capacity() * 2);
        }
        _sdata.resize(_sdata.size() + 1);
    }

    // TODO:
    ManualCapacityVector<Cell> &m_wf(int score) {
        return _sdata[score]._m_wf;
    }

    // TODO:
    ManualCapacityVector<Cell> &_i_jumps(int score) {
        return _sdata[score]._i_jumps;
    }
private:
    struct ScoreData {
        ManualCapacityVector<Cell> _m_wf;

        ManualCapacityVector<Cell> _i_jumps;
        ManualCapacityVector<Cell> _i2_jumps;
    };

    ManualCapacityVector<ScoreData> _sdata;
};