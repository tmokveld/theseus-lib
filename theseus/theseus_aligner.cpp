#include "theseus/theseus_aligner.h"

#include "theseus_aligner_impl.h"

namespace theseus {

TheseusAligner::TheseusAligner(const Penalties &penalties,
                               std::istream &gfa_stream)
{
    Graph graph(gfa_stream);
    aligner_impl_ = std::make_unique<TheseusAlignerImpl>(penalties, std::move(graph), false);
}


TheseusAligner::~TheseusAligner() {}

/**
 * @brief Main alignment function for the Theseus aligner.
 *
 * @param seq
 * @param start_node
 * @param start_offset
 * @return Alignment
 */
Alignment TheseusAligner::align(
    std::string_view seq,
    std::string &start_node,
    int start_offset) {

    return aligner_impl_->align(seq, start_node, start_offset);
}

} // namespace theseus