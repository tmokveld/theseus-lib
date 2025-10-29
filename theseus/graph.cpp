#include <vector>
#include <string>
#include "graph.h"
#include "gfa_graph.h"

namespace theseus {
    Graph::Graph(std::istream &gfa_stream) {
        GfaGraph gfa_graph(gfa_stream);

        // Add nodes
        _vertices.reserve(gfa_graph.gfa_nodes.size());
        for (int i = 0; i < gfa_graph.gfa_nodes.size(); ++i) {
            vertex v;
            v.name = gfa_graph.gfa_nodes[i].name;
            v.value = gfa_graph.gfa_nodes[i].seq;
            _vertices.push_back(v);
        }

        // Add edges
        for (int i = 0; i < gfa_graph.gfa_edges.size(); ++i) {
            edge e;
            e.from_vertex = gfa_graph.gfa_edges[i].from_node;
            e.to_vertex = gfa_graph.gfa_edges[i].to_node;
            e.overlap = gfa_graph.gfa_edges[i].overlap;
            _vertices[e.from_vertex].out_edges.push_back(e);
            _vertices[e.to_vertex].in_edges.push_back(e);
        }

        // Create name to id mapping
        for (int i = 0; i < _vertices.size(); ++i) {
            name_to_id_[_vertices[i].name] = i;
        }
    }
}