#pragma once

#include <vector>
#include "theseus/penalties.h"

/**
 * Class containing the alignment information (CIGAR and path) and a custom conversion
 * function to compute the alignment score according to user defined penalties.
 *
 */

namespace theseus {

class Alignment {
    public:
    /**
     * @brief Backtrace objects: similar to the CIGAR in sequence alignment. It consists
     *        of the set of edit operations of the alignment and the path of the alignment
     *        through the reference graph.
     */
      std::vector<char> edit_op; // Edit operations
      std::vector<int> path;     // Path of the alignment


      // Compute the affine gap score of the CIGAR,
      /**
       * @brief Compute the affine gap score of the CIGAR. This is due to the fact
       * that we use equivalent internal penalties during the alignment stage that
       * might differ from user defined penalties.
       *
       * @param user_penalties
       * @return int Alignment score according to user penalties and computed CIGAR
       */
      int compute_affine_gap_score(Penalties &user_penalties) {
          int score = 0;
          bool insertion_open = false, deletion_open = false;
          for (const auto &op : edit_op) {
              if (op == 'X') {
                  insertion_open = false;
                  deletion_open = false;
                  score += user_penalties.mism(); // Mismatch score
              }
              else if (op == 'I') {
                  deletion_open = false;
                  if (!insertion_open) {
                      insertion_open = true;
                      score += user_penalties.gapo() + user_penalties.gape(); // Gap open penalty for insertion
                  }
                  else {
                      score += user_penalties.gape(); // Gap extend penalty for insertion
                  }
              }
              else if (op == 'D') {
                  insertion_open = false;
                  if (!deletion_open) {
                      deletion_open = true;
                      score += user_penalties.gapo() + user_penalties.gape(); // Gap open penalty for deletion
                  }
                  else {
                      score += user_penalties.gape(); // Gap extend penalty for deletion
                  }
              }
              else if (op == 'M') {
                  insertion_open = false;
                  deletion_open = false;
                  score += user_penalties.match(); // Match score
              }
          }
          return score;
      }

    private:
};

} // namespace theseus