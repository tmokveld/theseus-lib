#pragma once

#include <manual_capacity_vector.h>
#include <cell.h>

/**
 * TODO:
 *
 */

// TODO: Create cpp.

class Scope {
public:
    // TODO: Prefer this?
    // using Wavefront = ManualCapacityVector<Cell>;
    // using Jumps = ManualCapacityVector<Cell>;
    // using JumpsPos = ManualCapacityVector<int32_t>;

    // TODO:
    Scope(int nscores)
        : nscores(nscores),
          front_idx(0),
          front_score(0),
          _sdata(nscores) {

        // for (int i = 0; i < nscores; i++) {
        //     _iwfs[i].reserve(1000);
        //     _dwfs[i].reserve(1000);
        //     _i2wfs[i].reserve(1000);
        //     _d2wfs[i].reserve(1000);
        //     _mjumps[i].reserve(1000);
        //     _mjumps_pos[i].reserve(1000);
        //     _ijumps_pos[i].reserve(1000);
        //     _i2jumps_pos[i].reserve(1000);
        // }
    }

    // TODO:
    void new_alignment() {
        front_idx = 0;
        front_score = 0;
    }

    // TODO:
    void new_score() {
        front_idx = (front_idx + 1) % nscores;
        front_score++;
    }

    // TODO:
    ManualCapacityVector<Cell> &i_wf(int score) {
        return _sdata[score_to_idx(score)]._i_wf;
    }

private:
    // TODO:
    int score_to_idx(int score) {
        return (front_idx + score - front_score) % nscores;
    }

    int nscores;
    int front_idx;
    int front_score;

    struct ScoreData {
        ManualCapacityVector<Cell> _i_wf;
        ManualCapacityVector<Cell> _d_wf;

        ManualCapacityVector<Cell> _i2_wf;
        ManualCapacityVector<Cell> _d2_wf;

        ManualCapacityVector<Cell> _m_jumps;

        ManualCapacityVector<int32_t> _m_jumps_pos;
        ManualCapacityVector<int32_t> _i_jumps_pos;
        ManualCapacityVector<int32_t> _i2_jumps_pos;
    };

    ManualCapacityVector<ScoreData> _sdata;
};
