#pragma once

#include <memory>
#include <istream>

#include "theseus/penalties.h"
#include "theseus/alignment.h"

/**
 * @file theseus_aligner.h
 * @brief Header file for the TheseusAligner class. This class provides an interface
 * for aligning sequences to a graph given a starting position using the Theseus
 * algorithm.
 *
 *
 */

namespace theseus
{

    class TheseusAlignerImpl; // Forward declaration of the implementation class.

    class TheseusAligner
    {
    public:
        /**
         * Constructor
         *
         * @param penalties User defined alignment penalties
         * @param gfa_stream Input stream containing the graph in GFA format
         */
        TheseusAligner(const Penalties &penalties, std::istream &gfa_stream);

        /**
         * Class destructor
         *
         */
        ~TheseusAligner();

        /**
         * Main alignment function. Aligns the given sequence to the graph starting
         * from the specified node and offset.
         *
         * @param seq Sequence to be aligned
         * @param start_node Starting node in the graph
         * @param start_offset Starting offset within the starting node
         * @return Alignment
         */
        Alignment align(std::string_view seq,
                        std::string &start_node,
                        int start_offset = 0);

    private:
        std::unique_ptr<TheseusAlignerImpl> aligner_impl_;
    };

} // namespace theseus
