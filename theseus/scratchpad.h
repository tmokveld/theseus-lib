#pragma once

#include <span>

#include "manual_capacity_vector.h"
#include "wavefront.h"

// TODO:

namespace theseus {

class ScratchPad {
public:
    // TODO: Define types elsewhere?
    using diag_type = int;
    using offset_type = int;
    using prev_type = int;
    using size_type = ptrdiff_t;

    // TODO: Define cell elsewhere?
    struct Cell {
        offset_type offset;
        prev_type prev;
    };

    // TODO:
    ScratchPad(diag_type min_diag, diag_type max_diag) :
        _wf(min_diag, max_diag, Cell{-1, -1}) {

        _diags.realloc(_wf.size());
    }

    // TODO:
    Cell& access_alloc(diag_type diag) {
        // If the diagonal was not yet in the wavefront (offset is -1), add the
        // diagonal to _diags.
        auto size = _diags.size();

        _diags[size] = diag;

        size += _wf[diag].offset == -1;

        _diags.resize_unsafe(size);

        return _wf[diag];
    }

    // TODO:
    Cell& operator[](diag_type diag) { return _wf[diag]; }
    const Cell& operator[](diag_type diag) const { return _wf[diag]; }

    // TODO:
    size_type nactive_diags() {
        return _diags.size();
    }

    // TODO:
    std::span<const diag_type> active_diags() {
        return std::span<diag_type>(_diags.data(), _diags.data() + _diags.size());
    }

    // TODO:
    void reset() {
        for (const auto diag : _diags) {
            _wf[diag].offset = -1;
        }
        _diags.resize_unsafe(0);
    }

private:
    Wavefront<Cell> _wf;
    ManualCapacityVector<diag_type> _diags;
};

} // namespace theseus