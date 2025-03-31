#pragma once

#include <cstdint>
#include <type_traits>
#include <typeinfo>

/**
 * TODO:
 *
 */

namespace theseus {

// WARNING: We want Cell to be a simple struct so it is standard layout and
// trivial. This way, resizes of ManualCapacityVector<Cell> are free.
struct Cell {
    using vertex_t = int32_t; // TODO: This should be here?
    using offset_t = int32_t;
    using diag_t = int32_t;

    Cell *prev;
    vertex_t vertex_id;
    offset_t offset;
    diag_t diag;
};

}   // namespace theseus
