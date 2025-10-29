#include "theseus/penalties.h"

// User defined alignment penalties
namespace theseus {

// Linear gap penalties constructor
Penalties::Penalties(penalty_t match, penalty_t mismatch, penalty_t gape)
    : type_(Type::Linear),
      match_(match),
      mismatch_(mismatch),
      gapo_(0),
      gape_(gape),
      gapo2_(0),
      gape2_(0) {}

// Affine gap penalties constructor
Penalties::Penalties(penalty_t match, penalty_t mismatch, penalty_t gapo, penalty_t gape)
    : type_(Type::Affine),
      match_(match),
      mismatch_(mismatch),
      gapo_(gapo),
      gape_(gape),
      gapo2_(0),
      gape2_(0) {}

// Dual affine gap penalties constructor
Penalties::Penalties(penalty_t match,
                     penalty_t mismatch,
                     penalty_t gapo,
                     penalty_t gape,
                     penalty_t gapo2,
                     penalty_t gape2)
    : type_(Type::DualAffine),
      match_(match),
      mismatch_(mismatch),
      gapo_(gapo),
      gape_(gape),
      gapo2_(gapo2),
      gape2_(gape2) {}

}   // namespace theseus