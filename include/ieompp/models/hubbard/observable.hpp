#ifndef IEOMPP_HUBBARD_OBSERVABLE_HPP_
#define IEOMPP_HUBBARD_OBSERVABLE_HPP_

#include <functional>
#include <numeric>
#include <vector>

#include <omp.h>

#include "ieompp/models/hubbard/basis.hpp"
#include "ieompp/types/matrix.hpp"
#include "ieompp/types/number.hpp"

namespace ieompp
{
    namespace hubbard
    {
        namespace real_space
        {
            template <typename Basis>
            struct ParticleNumber {
            };

            template <typename Term>
            struct ParticleNumber<Basis1Operator<Term>> {
                using Basis    = Basis1Operator<Term>;
                using Float    = typename types::RealType<typename Term::Prefactor>::type;
                using Operator = typename Term::Operator;
                using ExpectationValueFunction =
                    std::function<Float(const Operator&, const Operator&)>;

                ExpectationValueFunction expectation_value;

                template <typename Vector>
                typename types::ScalarType<Vector>::type operator()(const Basis& basis,
                                                                    const Vector& vector) const
                {
                    using ResultType = typename types::ScalarType<Vector>::type;

                    const auto size = basis.size();
                    std::vector<ResultType> results(omp_get_max_threads(), ResultType(0));

#pragma omp parallel for
                    for(auto i = 0ul; i < size; ++i) {
                        for(auto j = 0ul; j < size; ++j) {
                            results[omp_get_thread_num()] +=
                                expectation_value(basis[i].operators.front(),
                                                  basis[j].operators.front())
                                * (vector[i] * std::conj(vector[j]));
                            // TODO(fkoehler): implement a * b^* without creating a new complex
                            // number by conjugating
                        }
                    }
                    return std::accumulate(results.begin(), results.end(), ResultType(0.));
                }
            };

            template <typename Term>
            struct ParticleNumber<Basis3Operator<Term>> {
                using Basis    = Basis3Operator<Term>;
                using Float    = typename types::RealType<typename Term::Prefactor>::type;
                using Operator = typename Term::Operator;
                using ExpectationValueFunction =
                    std::function<Float(const Operator&, const Operator&)>;

                ExpectationValueFunction expectation_value;

                template <typename Vector>
                typename types::ScalarType<Vector>::type operator()(const Basis& basis,
                                                                    const Vector& vector) const
                {
                    using ResultType = typename types::ScalarType<Vector>::type;

                    const auto N          = basis.N;
                    const auto basis_size = basis.size();
                    std::vector<ResultType> results(omp_get_max_threads(), ResultType(0));

#pragma omp parallel for
                    for(auto i = 0ul; i < N; ++i) {
                        const auto thread = omp_get_thread_num();
                        for(auto j = 0ul; j < N; ++j) {
                            results[thread] += expectation_value(basis[i].operators.front(),
                                                                 basis[j].operators.front())
                                               * (vector[i] * std::conj(vector[j]));
                            // TODO(fkoehler): implement a * b^* without creating a new complex
                            // number by conjugating
                        }
                    }

#pragma omp parallel for
                    for(auto i = 0ul; i < N; ++i) {
                        const auto thread = omp_get_thread_num();
                        const auto& ops_i = basis[i].operators;
                        for(auto j = N; j < basis_size; ++j) {
                            const auto& ops_j = basis[j].operators;
                            results[thread] += expectation_value(ops_i.front(), ops_j.front())
                                               * expectation_value(ops_i[1], ops_j[2])
                                               * (vector[i] * std::conj(vector[j]));
                            // TODO(fkoehler): implement a * b^* without creating a new complex
                            // number by conjugating
                        }
                    }

#pragma omp parallel for
                    for(auto i = N; i < basis_size; ++i) {
                        const auto thread = omp_get_thread_num();
                        const auto& ops_i = basis[i].operators;
                        for(auto j = 0ul; j < N; ++j) {
                            const auto& ops_j = basis[j].operators;
                            results[thread] += expectation_value(ops_i.front(), ops_i[1])
                                               * expectation_value(ops_i.back(), ops_j.front())
                                               * (vector[i] * std::conj(vector[j]));
                            // TODO(fkoehler): implement a * b^* without creating a new complex
                            // number by conjugating
                        }
                    }

#pragma omp parallel for
                    for(auto i = N; i < basis_size; ++i) {
                        const auto thread = omp_get_thread_num();
                        const auto& ops_i = basis[i].operators;
                        for(auto j = N; j < basis_size; ++j) {
                            const auto& ops_j = basis[j].operators;
                            results[thread] +=
                                expectation_value(ops_i.front(), ops_j.front())
                                * (expectation_value(ops_i[1], ops_i.back())
                                       * expectation_value(ops_j[1], ops_j.back())
                                   + expectation_value(ops_i[1], ops_j.back())
                                         * ((ops_j[1].same_indices(ops_i.back()) ? 1. : 0.)
                                            - expectation_value(ops_j[1], ops_i.back())))
                                * (vector[i] * std::conj(vector[j]));
                            // TODO(fkoehler): implement a * b^* without creating a new complex
                            // number by conjugating
                        }
                    }
                    return std::accumulate(results.begin(), results.end(), ResultType(0.));
                }
            };
        } // namespace real_space
    }     // namespace hubbard
} // namespace ieompp

#endif