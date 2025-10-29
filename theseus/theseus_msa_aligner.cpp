#include "theseus/theseus_msa_aligner.h"

#include "theseus_aligner_impl.h"

namespace theseus {

TheseusMSA::TheseusMSA(const Penalties &penalties,
                               std::string_view seq) {

    // Create the initial graph
    theseus::Graph G;
    theseus::Graph::vertex source_v, central_v, sink_v;
    Graph::edge source_edge, central_edge;

    // Source vertex
    source_edge.from_vertex = 0;
    source_edge.to_vertex = 1;
    source_v.out_edges.push_back(source_edge);
    source_v.first_poa_vtx = 0;
    G._vertices.push_back(source_v);

    // Central vertex (initial sequence)
    central_edge.from_vertex = 1;
    central_edge.to_vertex = 2;
    central_v.in_edges.push_back(source_edge);
    central_v.out_edges.push_back(central_edge);
    central_v.first_poa_vtx = 1;
    central_v.value = seq;
    G._vertices.push_back(central_v);

    // Sink vertex
    sink_v.in_edges.push_back(central_edge);
    sink_v.first_poa_vtx = seq.size() + 1;
    G._vertices.push_back(sink_v);

    msa_aligner_impl_ = std::make_unique<TheseusAlignerImpl>(penalties, std::move(G), true);
}

/**
 * @brief Destroy the Theseus Aligner:: Theseus Aligner object
 *
 */
TheseusMSA::~TheseusMSA() {}

/**
 * @brief Main alignment function for the Theseus aligner.
 *
 * @param seq
 * @param start_node
 * @param start_offset
 * @return Alignment
 */
Alignment TheseusMSA::align(
    std::string_view seq) {

    std::string start_node;
    return msa_aligner_impl_->align(seq, start_node, 0);
}

/**
 * @brief Print the current POA graph in MSA format.
 *
 */
void TheseusMSA::print_as_gfa(std::ofstream &out_stream) {
    msa_aligner_impl_->print_as_gfa(out_stream);
}

/**
 * @brief Print the current POA graph in MSA format.
 *
 */
void TheseusMSA::print_as_msa(std::ofstream &out_stream) {
    msa_aligner_impl_->print_as_msa(out_stream);
}

/**
 * @brief Return consensus sequence.
 *
 */
std::string TheseusMSA::get_consensus_sequence() {
    return msa_aligner_impl_->get_consensus_sequence();
}

/**
 * @brief Print in graphviz format.
 *
 */
void TheseusMSA::print_as_dot(std::ofstream &out_stream) {
    msa_aligner_impl_->print_as_dot(out_stream);
}

} // namespace theseus