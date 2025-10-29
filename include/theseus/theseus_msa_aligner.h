#pragma once

#include <memory>
#include <istream>

#include "theseus/penalties.h"
#include "theseus/alignment.h"

/**
 * Multiple Sequence Aligner (MSA) based on POA graphs. Internally uses the
 * TheseusAlignerImpl class.
 *
 */

namespace theseus
{

    class TheseusAlignerImpl; // Forward declaration of the implementation class.

    class TheseusMSA
    {
    public:
        /**
         * Initial constructor
         *
         * @param penalties User defined alignment penalties
         * @param seq Sequence to initialize the graph
         */
        TheseusMSA(const Penalties &penalties, std::string_view seq);

        /**
         * Class destructor
         *
         */
        ~TheseusMSA();

        /**
         * Add a new sequence to the POA graph, representing the MSA so far.
         *
         * @param seq
         * @param start_node
         * @param start_offset
         * @return Alignment
         */
        Alignment align(std::string_view seq);

        /**
         * @brief Print the current POA graph as a GFA file.
         *
         */
        void print_as_gfa(std::ofstream &out_stream);


        /**
         * @brief Print the current POA graph in MSA format.
         *
         */
        void print_as_msa(std::ofstream &out_stream);


        /**
         * @brief Return consensus sequence.
         *
         */
        std::string get_consensus_sequence();


        /**
         * @brief Print in graphviz format.
         *
         */
        void print_as_dot(std::ofstream &out_stream);


    private:
        std::unique_ptr<TheseusAlignerImpl> msa_aligner_impl_;
    };

} // namespace theseus
