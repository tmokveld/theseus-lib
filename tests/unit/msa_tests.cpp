/*
 *                             The MIT License
 *
 * Copyright (c) 2024 by Albert Jimenez-Blanco
 *
 * This file is part of #################### Theseus Library ####################.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */


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

    SUBCASE("Align only does not update MSA graph") {
        std::string initial_seq = "ACCCGTAAAAGGG";
        std::string seq_a = "ACCCGTCAAAGGG";
        std::string seq_b = "ACCCGAAGGG";

        theseus::Penalties penalties(0, 2, 3, 1);
        theseus::TheseusMSA aligner(penalties, initial_seq);

        // Repeated align_only for seq_a should be stable
        theseus::Alignment a_probe_1 = aligner.align_only(seq_a);
        CHECK(a_probe_1.compute_affine_gap_score(penalties) == 2);
        theseus::Alignment a_probe_2 = aligner.align_only(seq_a);
        CHECK(a_probe_2.compute_affine_gap_score(penalties) == 2);

        // Repeated align_only for seq_b should be stable
        theseus::Alignment b_probe_1 = aligner.align_only(seq_b);
        CHECK(b_probe_1.compute_affine_gap_score(penalties) == 6);
        theseus::Alignment b_probe_2 = aligner.align_only(seq_b);
        CHECK(b_probe_2.compute_affine_gap_score(penalties) == 6);

        // Add seq_a to the graph and verify that only then it becomes a perfect match
        theseus::Alignment a_added = aligner.align(seq_a);
        CHECK(a_added.compute_affine_gap_score(penalties) == 2);
        theseus::Alignment a_after_add_1 = aligner.align_only(seq_a);
        CHECK(a_after_add_1.compute_affine_gap_score(penalties) == 0);
        theseus::Alignment a_after_add_2 = aligner.align_only(seq_a);
        CHECK(a_after_add_2.compute_affine_gap_score(penalties) == 0);

        // seq_b still behaves like align_only until it is explicitly added.
        theseus::Alignment b_before_add_1 = aligner.align_only(seq_b);
        CHECK(b_before_add_1.compute_affine_gap_score(penalties) == 6);
        theseus::Alignment b_before_add_2 = aligner.align_only(seq_b);
        CHECK(b_before_add_2.compute_affine_gap_score(penalties) == 6);

        aligner.align(seq_b);
        theseus::Alignment b_after_add = aligner.align_only(seq_b);
        CHECK(b_after_add.compute_affine_gap_score(penalties) == 0);

        // Adding seq_b should not regress seq_a
        theseus::Alignment a_after_b_add = aligner.align_only(seq_a);
        CHECK(a_after_b_add.compute_affine_gap_score(penalties) == 0);
    }
}
