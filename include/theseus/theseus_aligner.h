#pragma once

#include <memory>
#include <istream>

#include "theseus/penalties.h"
#include "theseus/alignment.h"

/**
 * TODO:
 *
 */

namespace theseus
{

    class TheseusAlignerImpl; // Forward declaration of the implementation class.

    class TheseusAligner
    {
    public:
        /**
         * TODO:
         *
         * @param penalties
         * @param seq
         */
        TheseusAligner(const Penalties &penalties, std::string_view seq);

        /**
         * [description]
         *
         * @param penalties
         * @param gfa_stream
         */
        TheseusAligner(const Penalties &penalties, std::istream &gfa_stream);

        /**
         * [description]
         *
         */
        ~TheseusAligner();

        /**
         * [description]
         *
         * @param seq
         * @param start_node
         * @param start_offset
         * @return Alignment
         */
        Alignment align(std::string seq, int start_node = 0, int start_offset = 0);

        /**
         * [description]
         *
         * @param output_file
         */
        void output_msa_as_fasta(const std::string &output_file);

        /**
         * [description]
         *
         * @param output_file
         */
        void output_as_gfa(const std::string &output_file);

    private:
        std::unique_ptr<TheseusAlignerImpl> aligner_impl_;
    };

} // namespace theseus
