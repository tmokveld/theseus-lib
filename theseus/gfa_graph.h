#pragma once

#include <istream>
#include <vector>
#include <string>
#include <cassert>
#include <unordered_map>

/**
 * Gfa Graph representation and loading utilities. More information on the GFA
 * format in: https://gfa-spec.github.io/GFA-spec/GFA1.html
 *
 */

namespace theseus
{
    class GfaGraph
    {
    public:
        /**
         * @brief An edge in the GFA graph.
         *
         */
        struct GfaEdge
        {
            int from_node;  // Source node
            int to_node;    // Target node
            size_t overlap; // Overlap between the two nodes (Currently accept 0)
        };

        /**
         * @brief A node (or segment) in the GFA graph.
         *
         */
        struct GfaNode
        {
            std::string seq;  // DNA sequence of the segment
            std::string name; // Name of the segment
        };

        // The graph can be thought of as two vectors: one containing the node
        // information and another containing the edges connnecting its nodes.
        std::vector<GfaNode> gfa_nodes;
        std::vector<GfaEdge> gfa_edges;


        /**
         * @brief Loads a GFA graph from a stream. More information on the GFA
         * format in: https://gfa-spec.github.io/GFA-spec/GFA1.html
         *
         * Disclaimer: Currently, only segments (S) and links (L) are supported.
         *
         * @param gfa_stream Input stream containing the graph in GFA format
         */
        GfaGraph(std::istream &gfa_stream);

        /**
         * Get the name of a node (or segment) given its id.
         *
         * @param id
         */
        std::string_view id_to_node_name(int id) const
        {
            assert(id < gfa_nodes.size());
            return gfa_nodes[id].name;
        }

        /**
         * Get (or create) the id of a given vertex (or segment).
         *
         * @param name              Name of the segment to check or add.
         * @return size_t           Id of the vertex (or segment)
         */
        size_t node_name_to_id(const std::string &name);

    private:
        /**
         * Load the GFA graph from a file stream.
         *
         * @param gfa_stream
         */
        void load_from_stream(std::istream &gfa_stream);

        // Mapping from node names to their ids.
        std::unordered_map<std::string, size_t> name_to_id_;
    };

} // namespace theseus