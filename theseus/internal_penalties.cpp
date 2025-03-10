#include "internal_penalties.h"

namespace theseus {

InternalPenalties::InternalPenalties(Penalties penalties)
    : Penalties(penalties) {

    _gape_alt = _gape;
    _gape2_alt = _gape2;

    // TODO: Transform all penalties to >= 0.


    // TODO: Check that all penalties make sense. Otherwise, throw an exception.
}

} // namespace theseus
