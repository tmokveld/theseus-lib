#pragma once

#include <istream>
#include <vector>
#include <string>
#include <cassert>
#include <unordered_map>

/**
 * [description]
 *
 */

namespace theseus
{
    class GfaGraph
    {
    public:
        struct GfaEdge
        {
            int from_node;
            int to_node;
            size_t overlap;
        };

        struct GfaNode
        {
            std::string seq;
            std::string name;
        };

        std::vector<GfaNode> gfa_nodes;
        std::vector<GfaEdge> gfa_edges;

        
        /**
         * @brief Loads a GFA graph from a stream. TODO: explain the expected format.
         *
         * @param gfa_stream TODO:
         */
        GfaGraph(std::istream &gfa_stream);

        /**
         * TODO:
         *
         * @param id
         */
        std::string_view id_to_node_name(int id) const
        {
            assert(id < gfa_nodes.size());
            return gfa_nodes[id].name;
        }

        /**
         * Get the id of a given vertex (or segment).
         *
         * @param name              Name of the segment to check or add.
         * @return size_t           Id of the vertex (or segment)
         */
        size_t node_name_to_id(const std::string &name);

    private:
        /**
         * TODO:
         *
         * @param gfa_stream
         */
        void load_from_stream(std::istream &gfa_stream);

        std::unordered_map<std::string, size_t> name_to_id_;
    };

} // namespace theseus