#include "../doctest.h"

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "../../theseus/graph.h"
#include "../../include/theseus/alignment.h"
#include "../../include/theseus/penalties.h"
#include "../../include/theseus/theseus_aligner.h"


TEST_CASE("Check sequence-to-graph aligner") {
    SUBCASE("Correct alignment of sequences against a graph with a cycle") {
        // Reference graph
        std::istringstream gfa_stream(
            "S\t1\tACTTAG\n"
            "S\t2\tACA\n"
            "S\t3\tT\n"
            "S\t4\tGTACTT\n"
            "L\t1\t+\t2\t+\t0M\n"
            "L\t1\t+\t3\t+\t0M\n"
            "L\t2\t+\t4\t+\t0M\n"
            "L\t3\t+\t4\t+\t0M\n"
            "L\t4\t+\t1\t+\t0M\n"
        );

        // Sequences for alignment
        std::vector<std::string> sequences = {
            "TAGACAGTACT",   // Perfect match
            "TAGACAGGACT",   // One mismatch
            "ACAGTACTTACT",  // Perfect match with a cycle
            "AACAGTACTTACT", // An deletion with a cycle
            "ACAGTATTACT"    // A insertion with a cycle
        };

        // Starting offsets and vertices
        std::vector<int> start_offsets = {3, 3, 0, 0, 0};
        std::vector<std::string> start_vertices = {"1", "1", "2", "2", "2"};

        // Expected CIGARs
        std::vector<std::vector<char>> expected_cigars = {
            {'M','M','M','M','M','M','M','M','M','M','M'},
            {'M','M','M','M','M','M','M','X','M','M','M'},
            {'M','M','M','M','M','M','M','M','M','M','M','M'},
            {'M','D','M','M','M','M','M','M','M','M','M','M','M'},
            {'M','M','M','M','M','M','I','M','M','M','M','M'}
        };

        // Expected paths TODO: Should have names of vertices instead of internal indices
        std::vector<std::vector<int>> expected_paths = {
            {0, 1, 3},
            {0, 1, 3},
            {1, 3, 0},
            {1, 3, 0},
            {1, 3, 0},
        };

        // Expected scores
        std::vector<int> expected_scores = {0, 2, 0, 4, 4};

        // Set aligner's parameters
        theseus::Penalties penalties(0, 2, 3, 1);            // Create penalties object
        theseus::TheseusAligner aligner(penalties, gfa_stream); // Create aligner

        // Align sequences and check results
        for (int i = 0; i < start_offsets.size(); ++i) {
            theseus::Alignment alignment = aligner.align(
                sequences[i], start_vertices[i], start_offsets[i]
            );

            // Check if alignment was successful
            CHECK(alignment.compute_affine_gap_score(penalties) == expected_scores[i]); // Check score
            CHECK(alignment.edit_op == expected_cigars[i]);        // Check CIGAR
            CHECK(alignment.path == expected_paths[i]); // Check path
        }

        theseus::Graph graph(gfa_stream);
        std::ofstream outstream("graph_cycle.dot");
        graph.print_as_gfa(outstream);
        graph.print_code_graphviz(outstream);
    }
}