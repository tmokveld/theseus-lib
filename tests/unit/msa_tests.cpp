#include "../doctest.h"

#include <vector>
#include <string>
#include <iostream>
#include "../../include/theseus/alignment.h"
#include "../../include/theseus/penalties.h"
#include "../../include/theseus/theseus_msa_aligner.h"


TEST_CASE("Check MSA aligner") {
    SUBCASE("Correct MSA with a matching sequence") {
        // Equal sequences
        std::string initial_seq = "ACCCGTAAAAGGG";
        std::string new_seq = "ACCCGTAAAAGGG";
        std::vector<char> expected_cigar = {'M','M','M','M','M','M','M','M','M','M','M','M','M'};

        // Set aligner's parameters
        theseus::Penalties penalties(0, 2, 3, 1);            // Create penalties object
        theseus::TheseusMSA aligner(penalties, initial_seq); // Create aligner

        // Align the new sequence
        theseus::Alignment alignment = aligner.align(new_seq);

        // Check if alignment was successful
        CHECK(alignment.compute_affine_gap_score(penalties) == 0); // Check score
        CHECK(alignment.edit_op == expected_cigar);                  // Check CIGAR
        CHECK(alignment.path == std::vector<int>({0, 1, 2}));        // Check path
    }

    SUBCASE("Correct MSA with a sequence with a mismatch") {
        // Equal sequences
        std::string initial_seq = "ACCCGTAAAAGGG";
        std::string new_seq = "ACCCGTCAAAGGG";
        std::vector<char> expected_cigar = {'M','M','M','M','M','M','X','M','M','M','M','M','M'};

        // Set aligner's parameters
        theseus::Penalties penalties(0, 2, 3, 1);            // Create penalties object
        theseus::TheseusMSA aligner(penalties, initial_seq); // Create aligner

        // Align the new sequence
        theseus::Alignment alignment = aligner.align(new_seq);

        // Check if alignment was successful
        CHECK(alignment.compute_affine_gap_score(penalties) == 2); // Check score
        CHECK(alignment.edit_op == expected_cigar);                  // Check CIGAR
        CHECK(alignment.path == std::vector<int>({0, 1, 2}));        // Check path
    }

    SUBCASE("Correct MSA with an deletion at the end") {
        // Equal sequences
        std::string initial_seq = "ACCCGTAAAAGGG";
        std::string new_seq = "ACCCGTAAAAGGGAAA";
        std::vector<char> expected_cigar = {'M','M','M','M','M','M','M','M','M','M','M','M','M', 'D','D','D'};

        // Set aligner's parameters
        theseus::Penalties penalties(0, 2, 3, 1);            // Create penalties object
        theseus::TheseusMSA aligner(penalties, initial_seq); // Create aligner

        // Align the new sequence
        theseus::Alignment alignment = aligner.align(new_seq);

        // Check if alignment was successful
        CHECK(alignment.compute_affine_gap_score(penalties) == 6); // Check score
        CHECK(alignment.edit_op == expected_cigar);                  // Check CIGAR
        CHECK(alignment.path == std::vector<int>({0, 1, 2}));        // Check path
    }

    SUBCASE("Correct MSA with an deletion at the beginning") {
        // Equal sequences
        std::string initial_seq = "ACCCGTAAAAGGG";
        std::string new_seq = "CATACCCGTAAAAGGG";
        std::vector<char> expected_cigar = {'D','D','D','M','M','M','M','M','M','M','M','M','M','M','M','M'};

        // Set aligner's parameters
        theseus::Penalties penalties(0, 2, 3, 1);            // Create penalties object
        theseus::TheseusMSA aligner(penalties, initial_seq); // Create aligner

        // Align the new sequence
        theseus::Alignment alignment = aligner.align(new_seq);

        // Check if alignment was successful
        CHECK(alignment.compute_affine_gap_score(penalties) == 6); // Check score
        CHECK(alignment.edit_op == expected_cigar);                  // Check CIGAR
        CHECK(alignment.path == std::vector<int>({0, 1, 2}));        // Check path
    }

    SUBCASE("Correct MSA with an insertion in the middle") {
        // Equal sequences
        std::string initial_seq = "ACCCGTAAAAGGG";
        std::string new_seq = "ACCCGAAGGG";
        std::vector<char> expected_cigar = {'M','M','M','M','M','I','I','I','M','M','M','M','M'};

        // Set aligner's parameters
        theseus::Penalties penalties(0, 2, 3, 1);            // Create penalties object
        theseus::TheseusMSA aligner(penalties, initial_seq); // Create aligner

        // Align the new sequence
        theseus::Alignment alignment = aligner.align(new_seq);

        // Check if alignment was successful
        CHECK(alignment.compute_affine_gap_score(penalties) == 6); // Check score
        CHECK(alignment.edit_op == expected_cigar);                  // Check CIGAR
        CHECK(alignment.path == std::vector<int>({0, 1, 2}));        // Check path
    }

    SUBCASE("Correct MSA with diverging sequence") {
        // Equal sequences
        std::string initial_seq = "ACCCGTAAAAGGG";
        std::string new_seq = "ACCCCCATAAGAGGG";

        // Set aligner's parameters
        theseus::Penalties penalties(0, 2, 3, 1);            // Create penalties object
        theseus::TheseusMSA aligner(penalties, initial_seq); // Create aligner

        // Align the new sequence
        theseus::Alignment alignment = aligner.align(new_seq);

        // Check if alignment was successful
        CHECK(alignment.compute_affine_gap_score(penalties) == 9); // Check score
        CHECK(alignment.path == std::vector<int>({0, 1, 2}));        // Check path
    }

    SUBCASE("Correct MSA with several sequences") {
        // Equal sequences
        std::string initial_seq = "ACCCGTAAAAGGG";
        std::string seq_1 = "ACCCGTCAAAGGG";
        std::string seq_2 = "ACCCGAAGGG";
        std::string seq_3 = "ACCCGTCAAAGGG";
        std::string seq_4 = "ACCCCCATAAGAGGG";

        // Set aligner's parameters
        theseus::Penalties penalties(0, 2, 3, 1);            // Create penalties object
        theseus::TheseusMSA aligner(penalties, initial_seq); // Create aligner

        // Align and check sequence 1
        theseus::Alignment alignment = aligner.align(seq_1);
        CHECK(alignment.compute_affine_gap_score(penalties) == 2); // Check score

        // Align and check sequence 2
        alignment = aligner.align(seq_2);
        CHECK(alignment.compute_affine_gap_score(penalties) == 6); // Check score

        // Align and check sequence 3
        alignment = aligner.align(seq_3);
        CHECK(alignment.compute_affine_gap_score(penalties) == 0); // Check score

        // Align and check sequence 4
        alignment = aligner.align(seq_4);
        CHECK(alignment.compute_affine_gap_score(penalties) == 9); // Check score
    }
}