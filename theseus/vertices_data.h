#pragma once

/**
 * TODO:
 *
 */

class VerticesData { // TODO: Other name?
public:
    struct Segment {
        int32_t start_d;
        int32_t end_d;
    };

    struct InvalidData {
        Segment seg;        // Invalid diagonals
        int32_t rem_up;     // Remaining scores to grow the segment one diagonal up
        int32_t rem_down;   // Remaining scores to grow the segment one diagonal down
    };

    // TODO: Compact the invalid vectors.
    void compact();

    // TODO:
    bool is_active(Cell::vertex_id v) const {
        // return _active_vertices[v] != -1;
    }

    // TODO:
    void activate(Cell::vertex_id v) {
        // _active_vertices.push_back(v);
    }

private:
    ManualCapacityVector<Cell::vertex_id> _active_vertices;

    ManualCapacityVector<InvalidData> _minvalid;
    ManualCapacityVector<InvalidData> _iinvalid;
    ManualCapacityVector<InvalidData> _dinvalid;
};