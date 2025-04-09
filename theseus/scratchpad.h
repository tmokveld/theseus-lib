#pragma once

#include <span>

#include "manual_capacity_vector.h"
#include "wavefront.h"
#include "cell.h"

// TODO:

namespace theseus {

class ScratchPad {
public:
    using size_type = ptrdiff_t;
    using diag_type = Cell::idx2d_t;

    // TODO:
    ScratchPad(diag_type min_diag, diag_type max_diag) :
        _wf(min_diag, max_diag, Cell{-1, -1, -1, -1, -1, Cell::Matrix::None}) {

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
    diag_type min_diag() const {
        return _wf.min_diag();
    }

    // TODO:
    diag_type max_diag() const {
        return _wf.max_diag();
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