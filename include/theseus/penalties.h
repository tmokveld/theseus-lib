#pragma once

#include <span>
#include <string>
#include <vector>

#include "theseus/alignment.h"

/**
 * A class containing the penalties for the alignment algorithm. The objective
 * function is to minimize the score.
 *
 */

namespace theseus {

class Penalties {
public:
    using penalty_t = int;

    enum Type {
        Linear,
        Affine,
        DualAffine
    };

    /**
     * Create a Gap-Linear penalty object.
     *
     * @param match The match score.
     * @param mismatch The mismatch score.
     * @param gape The gap extension penalty.
     */
    Penalties(penalty_t match,
              penalty_t mismatch,
              penalty_t gape);

    /**
     * Create a Gap-Affine penalty object.
     *
     * @param match The match score.
     * @param mismatch The mismatch score.
     * @param gapo The gap open penalty.
     * @param gape The gap extension penalty.
     */
    Penalties(penalty_t match,
              penalty_t mismatch,
              penalty_t gapo,
              penalty_t gape);

    /**
     * Create a Dual Gap-Affine penalty object.
     *
     * @param match The match score.
     * @param mismatch The mismatch score.
     * @param gapo The first gap open penalty.
     * @param gape The first gap extension penalty.
     * @param gapo2 The second gap open penalty.
     * @param gape2 The second gap extension penalty.
     */
    Penalties(penalty_t match,
              penalty_t mismatch,
              penalty_t gapo,
              penalty_t gape,
              penalty_t gapo2,
              penalty_t gape2);

    /**
     * Get the gap type.
     *
     * @return The gap type.
     */
    Type type() const { return type_; }

    /**
     * Get the match score.
     *
     * @return The match score.
     */
    penalty_t match() const { return match_; }

    /**
     * Get the mismatch score.
     *
     * @return The mismatch score.
     */
    penalty_t mism() const { return mismatch_; }

    /**
     * Get the gap open penalty if the gap type is affine or dual affine.
     * Otherwise, return 0.
     *
     * @return The gap open penalty if the gap type is affine or dual affine.
     * Otherwise, return 0.
     */
    penalty_t gapo() const { return gapo_; }

    /**
     * Get the gap extension penalty.
     *
     * @return The gap extension penalty.
     */
    penalty_t gape() const { return gape_; }

    /**
     * Get the second gap open penalty if the gap type is dual affine.
     * Otherwise, return 0.
     *
     * @return The gap open penalty if the gap type is dual affine. Otherwise,
     * return 0.
     */
    penalty_t gapo2() const { return gapo2_; }

    /**
     * Get the second gap extension penalty if the gap type is dual affine.
     * Otherwise, return 0.
     *
     * @return The gap extension penalty if the gap type is dual affine.
     * Otherwise, return 0.
     */
    penalty_t gape2() const { return gape_; }

    // Compute the affine gap score of the CIGAR
    int compute_affine_gap_score(const Alignment::Cigar &cigar) {
        int score = 0;
        bool insertion_open = false, deletion_open = false;
        for (const auto &op : cigar.edit_op) {
            if (op == 'X') {
                insertion_open = false;
                deletion_open = false;
                score += mism(); // Mismatch score
            }
            else if (op == 'I') {
                deletion_open = false;
                if (!insertion_open) {
                    insertion_open = true;
                    score += gapo_ + gape_; // Gap open penalty for insertion
                }
                else {
                    score += gape_; // Gap extend penalty for insertion
                }
            }
            else if (op == 'D') {
                insertion_open = false;
                if (!deletion_open) {
                    deletion_open = true;
                    score += gapo_ + gape_; // Gap open penalty for deletion
                }
                else {
                    score += gape_; // Gap extend penalty for deletion
                }
            }
            else if (op == 'M') {
                insertion_open = false;
                deletion_open = false;
                score += match_; // Match score
            }
        }
        return score;
    }

protected:
    Type type_;

    penalty_t match_;
    penalty_t mismatch_;

    penalty_t gapo_; // Gap open.
    penalty_t gape_; // Gap extension.

    penalty_t gapo2_; // Gap open for dual gap-affine.
    penalty_t gape2_; // Gap extension for dual gap-affine.
};

} // namespace theseus
