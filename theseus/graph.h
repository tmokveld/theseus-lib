#pragma once

#include<vector>
#include<string>
#include<iostream>
#include<fstream>

#include"gfa_graph.h"

/**
 * Internal representation of a directed graph.
 *
 */

namespace theseus {

class Graph {
    public:
        struct edge {
            int from_vertex;     // from vertex
            int to_vertex;       // to vertex
            size_t overlap = 0;  // overlap length
        };

        struct vertex
        {
            std::vector<edge> in_edges;     // in-going vertices
            std::vector<edge> out_edges;    // out-going vertices
            std::string value;              // sequence associated to the edge
            std::string name;               // name of the vertex
            int first_poa_vtx;              // starting point in the poa graph
        };

        std::vector<vertex> _vertices;
        std::unordered_map<std::string, size_t> name_to_id_;    // Mapping from node names to their ids.

        // Return the vertices of the graph
        std::vector<vertex> &vertices() { return _vertices; }

        Graph() = default;

        /**
         * @brief Construct a new Graph object from a GfaGraph object.
         *
         * @param gfa_graph
         */
        Graph(std::istream &gfa_stream);

        /**
         * @brief Visualize the graph in Graphviz format.
         *
         * @param G
         */
        void print_code_graphviz(std::ofstream &out_stream)
        {
            out_stream << "digraph G {" << std::endl;
            // Set all sequences to the nodes
            for (int i = 0; i < _vertices.size(); ++i)
            {
                out_stream << i << " [label=\"";
                for (int j = 0; j < _vertices[i].value.size(); ++j)
                {
                    out_stream << _vertices[i].value[j];
                }
                out_stream << "\"]" << std::endl;
            }

            // Set edges
            for (int i = 0; i < _vertices.size(); ++i)
            {
                for (int j = 0; j < _vertices[i].out_edges.size(); ++j)
                {
                    int out_v = _vertices[i].out_edges[j].to_vertex;
                    out_stream << i << "->" << out_v << std::endl;
                }
            }

            out_stream << "}" << std::endl;
        }


        void print_as_gfa(std::ofstream &gfa_output) {
            if (!gfa_output.is_open())
            {
                throw std::runtime_error("Could not open output file");
            }

            // Print all nodes as segments
            for (const auto &vtx : _vertices)
            {
                gfa_output << "S\t" << vtx.name << "\t" << vtx.value << "\n";
            }

            // Print all edges as links
            for (const auto &vtx : _vertices)
            {
                // Go through all incoming vertices (with this you cover all possible edges,
                // since the graph is directed)
                for (const auto &edge : vtx.in_edges)
                {
                    gfa_output << "L\t" << _vertices[edge.from_vertex].name << "\t+\t"
                        << vtx.name << "\t+\t"
                        << edge.overlap << "M\n";
                }
            }

            gfa_output.close();
        }

        // Get the id of a vertex given its name
        size_t get_id(const std::string &name) {
            return name_to_id_.at(name);
        }

    private:
};

} // namespace theseus
