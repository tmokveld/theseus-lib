#include "theseus/penalties.h"

namespace theseus {

Penalties::Penalties(penalty_t match, penalty_t mismatch, penalty_t gape)
    : _type(Type::Linear),
      _match(match),
      _mismatch(mismatch),
      _gapo(0),
      _gape(gape),
      _gapo2(0),
      _gape2(0) {}

Penalties::Penalties(penalty_t match, penalty_t mismatch, penalty_t gapo, penalty_t gape)
    : _type(Type::Affine),
      _match(match),
      _mismatch(mismatch),
      _gapo(gapo),
      _gape(gape),
      _gapo2(0),
      _gape2(0) {}

Penalties::Penalties(penalty_t match,
                     penalty_t mismatch,
                     penalty_t gapo,
                     penalty_t gape,
                     penalty_t gapo2,
                     penalty_t gape2)
    : _type(Type::DualAffine),
      _match(match),
      _mismatch(mismatch),
      _gapo(gapo),
      _gape(gape),
      _gapo2(gapo2),
      _gape2(gape2) {}

}   // namespace theseus