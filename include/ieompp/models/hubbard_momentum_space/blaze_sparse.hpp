#ifndef IEOMPP_MODELS_HUBBARD_MOMENTUM_SPACE_BLAZE_SPARSE_HPP_
#define IEOMPP_MODELS_HUBBARD_MOMENTUM_SPACE_BLAZE_SPARSE_HPP_

#include "ieompp/models/hubbard/operator_traits.hpp"
#include "ieompp/models/hubbard_momentum_space/basis.hpp"
#include "ieompp/models/hubbard_momentum_space/liouvillian.hpp"

#include <cstdint>

namespace ieompp
{
    namespace models
    {
        namespace hubbard_momentum_space
        {
            template <typename Monomial>
            uint64_t number_of_matrix_elements(const Basis3Operator<Monomial>& basis)
            {
                return basis.N * (1 + 3 * basis.N);
            }

            template <typename Liouvillian, typename Matrix, typename Monomial, typename Lattice>
            void init_matrix(const Liouvillian& liouvillian, Matrix& matrix,
                             const Basis3Operator<Monomial>& basis, const Lattice& lattice)
            {
                static_assert(hubbard::IsHubbardOperator<typename Monomial::Operator>::value,
                              "Operator-type in Monomial-type must be a Hubbard like operator!");

                using Index = typename types::IndexType<Matrix>::Type;

                const auto interaction1 = liouvillian.U / 2.,
                           interaction2 = liouvillian.U / (2. * lattice.size());

                matrix.resize(basis.size(), basis.size(), false);
                matrix.reset();
                matrix.reserve(number_of_matrix_elements(basis));

                const auto& op = basis[0].front();

                // add diagonal element
                matrix.append(0, 0, liouvillian.dispersion(op.index1) + interaction1);

                // add off-diagonal elements
                for(Index i = 1; i < basis.size(); ++i) {
                    matrix.append(0, i, interaction2);
                }
                matrix.finalize(0);

                for(Index row = 1; row < basis.size(); ++row) {
                    const auto& monomial = basis[row];

                    // add off-diagonal element
                    matrix.append(row, 0, interaction2);

                    // add diagonal element
                    matrix.append(row, row, liouvillian.dispersion(monomial[0].index1)
                                                + liouvillian.dispersion(monomial[1].index1)
                                                - liouvillian.dispersion(monomial[2].index1)
                                                + interaction1);
                    matrix.finalize(row);
                }
            }
        } // namespace hubbard_momentum_space
    }     // namespace models
} // namespace ieompp

#endif
