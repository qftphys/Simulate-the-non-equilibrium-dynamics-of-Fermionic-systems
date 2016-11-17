#ifndef IEOMPP_MODELS_HUBBARD_MOMENTUM_SPACE_BASIS_HPP_
#define IEOMPP_MODELS_HUBBARD_MOMENTUM_SPACE_BASIS_HPP_

#include "ieompp/models/hubbard_common/operator_traits.hpp"

#include <vector>

namespace ieompp
{
    namespace models
    {
        namespace hubbard_momentum_space
        {
            template <typename TermT>
            struct Basis3Operator : public std::vector<TermT> {
                using Term  = TermT;
                using Index = typename std::vector<Term>::size_type;

                const std::size_t N;

                template <typename MomentumSpace>
                Basis3Operator(Index q_idx, const MomentumSpace& momentum_space)
                    : N(momentum_space.num())
                {
                    static_assert(hubbard_common::IsHubbardOperator<typename Term::Operator>::value,
                                  "Operator must be of Hubbard type");

                    this->reserve(N * N + 1);

                    this->push_back(Term{1, {{true, q_idx, true}}});

                    const auto q = momentum_space[q_idx];
                    for(auto i1 : momentum_space) {
                        const auto k1 = momentum_space[i1];
                        for(auto i2 : momentum_space) {
                            auto i3 = momentum_space(k1 + momentum_space[i2] - q);
                            this->push_back(
                                Term{1, {{true, i1, true}, {true, i2, false}, {false, i3, false}}});
                        }
                    }
                }

                Index get_3op_index(Index i1, Index i2) const { return 1 + i1 * N + i2; }
            };

            template <typename Basis>
            struct IsOneOperatorBasis {
                static constexpr bool value = false;
            };

            template <typename Basis>
            struct IsThreeOperatorBasis {
                static constexpr bool value = false;
            };

            template <typename Term>
            struct IsThreeOperatorBasis<Basis3Operator<Term>> {
                static constexpr bool value = true;
            };
        } // namespace hubbard_momentum_space
    }     // namespace models
} // namespace ieompp

#endif
