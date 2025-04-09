#pragma once

#include <memory>
#include <string_view>

#include "alignment.h"
#include "beyond_scope.h"
#include "graph.h"
#include "internal_penalties.h"
#include "penalties.h"
#include "scope.h"
#include "scratchpad.h"
#include "vertices_data.h"
#include "cell.h"

namespace theseus {

class TheseusAlignerImpl {
public:
    TheseusAlignerImpl(Penalties penalties, Graph &graph, bool msa, bool score_only);

    void process_vertex(Graph::vertex* curr_v, int v);

    // TODO:
    void add_to_graph(std::string_view seq);

    // TODO:
    Alignment align(std::string_view seq);

private:
    // TODO:
    // void extend();

    // TODO:
    // template <Penalties::Type gap_type>
    // void next();

    // TODO:
    // template <Penalties::Type gap_type>
    void backtrace(Alignment& alignment);

    int32_t _score = 0;

    Penalties _orig_penalties;
    InternalPenalties _penalties;

    Graph _graph;   // TODO:

    bool _is_msa;
    bool _is_score_only;
    bool _end = false;
    int _end_vertex;
    Cell _start_pos;

    std::unique_ptr<ScratchPad> _scratchpad;   // TODO: Scratchpad inside scope?

    std::unique_ptr<Scope> _scope;
    std::unique_ptr<BeyondScope> _beyond_scope;

    std::unique_ptr<VerticesData> _vertices_data;

    std::string_view _seq;
};

}   // namespace theseus