#ifndef IEOMPP_MODELS_HUBBARD_MOMENTUM_SPACE_EXCITED_FERMI_SEA_HPP_
#define IEOMPP_MODELS_HUBBARD_MOMENTUM_SPACE_EXCITED_FERMI_SEA_HPP_

#include "ieompp/algebra/operator/get_index.hpp"

#include <algorithm>
#include <list>
#include <utility>
#include <vector>

namespace ieompp
{
    namespace models
    {
        namespace hubbard_momentum_space
        {
            template <typename MonomialT>
            struct ExcitedFermiSea {
                using Monomial = MonomialT;
                using Operator = typename Monomial::Operator;
                using Indices  = typename Operator::Indices;

                bool vanishes = false;
                std::list<Indices> created_particles;
                std::list<Indices> annihilated_particles;

                ExcitedFermiSea() = default;

                template <typename Monomial, typename Dispersion>
                bool apply_monomial(const Monomial& monomial, const Dispersion& dispersion,
                                    const typename Dispersion::Float& fermi_energy = 0.)
                {
                    if(vanishes) {
                        return false;
                    }
                    for(auto it = monomial.crbegin(); it != monomial.crend(); ++it) {
                        if(it->creator) {
                            create_particle(algebra::get_indices(*it), dispersion, fermi_energy);
                        } else {
                            annihilate_particle(algebra::get_indices(*it), dispersion,
                                                fermi_energy);
                        }
                        if(vanishes) {
                            break;
                        }
                    }

                    if(!vanishes) {
                        created_particles.sort();
                        annihilated_particles.sort();
                    }

                    return vanishes;
                }

                template <typename Operator, typename Dispersion>
                bool apply_operator(const Operator& op, const Dispersion& dispersion,
                                    const typename Dispersion::Float& fermi_energy = 0.)
                {
                    if(vanishes) {
                        return false;
                    }
                    if(op.creator) {
                        create_particle(algebra::get_indices(op), dispersion, fermi_energy);
                    } else {
                        annihilate_particle(algebra::get_indices(op), dispersion, fermi_energy);
                    }

                    if(!vanishes) {
                        created_particles.sort();
                        annihilated_particles.sort();
                    }
                    return vanishes;
                }

                template <typename Indices, typename Dispersion>
                static bool is_initially_occupied(const Indices& indices,
                                                  const Dispersion& dispersion,
                                                  const typename Dispersion::Float& fermi_energy)
                {
                    return dispersion(std::get<0>(indices)) < fermi_energy;
                }

                template <typename Indices, typename Dispersion>
                bool create_particle(const Indices& indices, const Dispersion& dispersion,
                                     const typename Dispersion::Float& fermi_energy)
                {
                    if(vanishes) {
                        return false;
                    }
                    if(is_initially_occupied(indices, dispersion, fermi_energy)) {
                        // particle is initially present -> check for annihilation
                        auto pos = std::find(annihilated_particles.begin(),
                                             annihilated_particles.end(), indices);
                        if(pos == annihilated_particles.end()) {
                            // particle is still present -> creation not possible
                            vanishes = true;
                            return false;
                        }
                        // particle has been annihilated -> reverse annihilation
                        annihilated_particles.erase(pos);
                        return true;
                    }
                    // particle is initially absent -> check for creation
                    auto pos =
                        std::find(created_particles.begin(), created_particles.end(), indices);
                    if(pos == created_particles.end()) {
                        // particle has not been created yet -> create it
                        created_particles.push_back(indices);
                        return true;
                    }
                    // particle has alread been created -> creation is not possible
                    vanishes = true;
                    return false;
                }

                template <typename Indices, typename Dispersion>
                bool annihilate_particle(const Indices& indices, const Dispersion& dispersion,
                                         const typename Dispersion::Float& fermi_energy)
                {
                    if(vanishes) {
                        return false;
                    }
                    if(is_initially_occupied(indices, dispersion, fermi_energy)) {
                        // particle is initially present -> check for annihilation
                        auto pos = std::find(annihilated_particles.begin(),
                                             annihilated_particles.end(), indices);
                        if(pos == annihilated_particles.end()) {
                            // particle is still present -> annihilate it
                            annihilated_particles.push_back(indices);
                            return true;
                        }
                        // particle has been annihilated -> annihilation is not possible
                        vanishes = true;
                        return false;
                    }
                    // particle is initially absent -> check for creation
                    auto pos =
                        std::find(created_particles.begin(), created_particles.end(), indices);
                    if(pos == created_particles.end()) {
                        // particle has not been created yet -> annihilation is not possible
                        vanishes = true;
                        return false;
                    }
                    // particle has alread been created -> reverse creation
                    created_particles.erase(pos);
                    return true;
                }

                bool is_initial_fermi_sea() const
                {
                    return !vanishes && created_particles.empty() && annihilated_particles.empty();
                }
            };
        } // namespace hubbard_momentum_space
    }     // namespace models
} // namespace ieompp

#endif
