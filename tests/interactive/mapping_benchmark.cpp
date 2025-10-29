#include <getopt.h>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


#include "../dp_aligner.h"
#include "theseus/alignment.h"
#include "theseus/penalties.h"
#include "theseus/theseus_aligner.h"
#include "../../theseus/msa.h"
#include "../../theseus/graph.h"

#include <vector>

// Control of the output.
#define AVOID_DP 0
#define AVOID_THESEUS 0
#define PRINT_ALIGNMENTS 0

struct CMDArgs {
    int match = 0;
    int mismatch = 2;
    int gapo = 3;
    int gape = 1;
    std::string data_file;
};


// Input data parser. It receives a pointer to a file with sets of graph, read and
// starting position, returns the sequence and position data and saves the graph in
// a temporary file so that the TheseusAligner can process it later. Recall that
// this is a benchmark program, not a production one.
//
// Example of input data:
// H       VN:Z:1.1
// S       10867   GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCAAGATGGGCGGATCACGAGGTCAGGAGATCGAGACCATCTTGGCTAACACCGCGAAACCCCGTCTCTACTAAAAATACAAAAAAATCAGCCGGGCGTAGTGGCGGGCGCCTATAGTGCCAGCTACGCCGGAGGCTGAGGCAGGAGAGTGGCGTGAACCCGGGAGGCGGCGCTTGCAGTGAGCTGAGATTGCGCCACTGCACTCCAGCC
// S       10868   TGGGCGACAGAGCGAGACTCCGCCTCAAAAAAAAAAAAAAAAAA
// L       10867   +       10868   +       0M

// s GCCGGAGGCTGAGGCAGGAGAGTGGCGTGAACCCGGGAGGCGGCGCTTGCAGTGAGCTGAGATTGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGA
// p 10867 173
// ---
void read_graph_data(
    std::ifstream &data_file,
    std::string &sequence,
    std::string &start_node,
    int &start_offset)
{
    std::string line;

    // Open a temporary file to store the graph in GFA format
    std::ofstream temp_gfa_file("temp_graph.tmp");

    while (data_file.good() && data_file.peek() != EOF)
    {
        getline(data_file, line);
        if (line.empty())
            continue;

        // Graph data
        if (line[0] == 'H' || line[0] == 'S' || line[0] == 'L')
        {
            // Store the line in the temporary GFA file
            temp_gfa_file << line << std::endl;
        }

        if (line[0] == 's')
        {
            std::stringstream sstr{line};
            std::string type;
            sstr >> type >> sequence;
        }
        if (line[0] == 'p')
        {
            std::stringstream sstr{line};
            std::string start_node_str, start_offset_str, type;
            sstr >> type >> start_node_str >> start_offset_str;

            // Convert the strings to integers
            start_node = start_node_str;
            start_offset = std::stoi(start_offset_str);
        }

        if (line == "---")
            break;
    }
    temp_gfa_file.close();
}


/**
 * @brief Print the help message.
 */
void help() {
    std::cout << "Usage: benchmark [OPTIONS]\n"
                 "Options:\n"
                 "  -m, --match <int>       The match penalty [default=0]\n"
                 "  -x, --mismatch <int>    The mismatch penalty [default=2]\n"
                 "  -o, --gapo <int>        The gap open penalty [default=3]\n"
                 "  -e, --gape <int>        The gap extension penalty [default=1]\n"
                 "  -d, --data_file <file>  Dataset file\n";
}

CMDArgs parse_args(int argc, char *const *argv) {
    static const option long_options[] = {{"match", required_argument, 0, 'm'},
                                          {"mismatch", required_argument, 0, 'x'},
                                          {"gapo", required_argument, 0, 'o'},
                                          {"gape", required_argument, 0, 'e'},
                                          {"data_file", required_argument, 0, 'd'},
                                          {0, 0, 0, 0}};

    CMDArgs args;

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "m:x:o:e:d:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'o':
                args.gapo = std::stoi(optarg);
                break;
            case 'e':
                args.gape = std::stoi(optarg);
                break;
            case 'm':
                args.match = std::stoi(optarg);
                break;
            case 'x':
                args.mismatch = std::stoi(optarg);
                break;
            case 'd':
                args.data_file = optarg;
                break;
            default:
                std::cerr << "Invalid option" << std::endl;
                exit(1);
        }
    }

    return args;
}


int main(int argc, char *const *argv) {
    // Parsing
    CMDArgs args = parse_args(argc, argv);

    if (args.data_file.empty()) {
        std::cerr << "Missing required arguments\n";
        help();
        return 1;
    }

    theseus::Penalties penalties(args.match, args.mismatch, args.gapo, args.gape);

    // Main while loop
    std::ifstream data_file(args.data_file);

    if (!data_file.is_open()) {
        std::cerr << "Could not open dataset file\n";
        return 0;
    }

    std::string sequence, line, start_node; // Value and metadata of the sequence
    int num = 0, start_offset;
    while (data_file.good() && data_file.peek() != EOF)
    {
        // Read graph, sequence and starting position
        read_graph_data(data_file, sequence, start_node, start_offset);
        std::ifstream temp_gfa_file("temp_graph.tmp");
        ++num;

        // Prepare the data
        theseus::Alignment alignment;
        theseus::TheseusAligner aligner(penalties, temp_gfa_file);

        // Perform alignment
        std::cout << "Seq " << num << std::endl;
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        alignment = aligner.align(sequence, start_node, start_offset);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << std::endl;
        std::cout << "Alignment score: " << alignment.compute_affine_gap_score(penalties) << std::endl;
        for (int l = 0; l < alignment.edit_op.size(); ++l)
        {
            std::cout << alignment.edit_op[l] << " ";
        }
        std::cout << std::endl;
    }

    data_file.close();

    return 0;
}