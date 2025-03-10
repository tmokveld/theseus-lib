#pragma once

#include "theseus/penalties.h"

/**
 * A class derived from Penalties that contains the actual penalties used
 * in the alignment algorithm.
 *
 */

namespace theseus {

class InternalPenalties : public Penalties {
public:
    InternalPenalties(Penalties penalties);

    // Substitute gape() for ins() and del().
    penalty_t gape() = delete;
    penalty_t gapo2() = delete;

private:
    // We no longer have Ins=Del penalties.
    penalty_t _gape_alt;
    penalty_t _gape2_alt;
};

} // namespace theseus