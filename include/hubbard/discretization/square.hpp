#ifndef HUBBARD_DISCRETIZATION_SQUARE_HPP_
#define HUBBARD_DISCRETIZATION_SQUARE_HPP_

#include <array>
#include <tuple>
#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <Eigen/Dense>
#pragma GCC diagnostic pop

#include "hubbard/constants.hpp"

namespace hubbard
{
    namespace discretization
    {
        template <typename Real>
        struct SquareDiscretization {
            using RealType   = Real;
            using IndexType  = std::tuple<std::size_t, std::size_t>;
            using VectorType = Eigen::Matrix<Real, 2, 1>;

            std::vector<IndexType> indices;
            std::vector<std::vector<VectorType>> sites;
            const std::size_t num_x, num_y;
            const Real dx, dy;
            const Real x_min, y_min, x_max, y_max;

            // init in real space
            SquareDiscretization(const std::size_t nx, const std::size_t ny, const Real& delta_x,
                                 const Real& delta_y);

            // init in momentum space
            SquareDiscretization(const std::size_t nx, const std::size_t ny);

            IndexType closest(const VectorType& v) const;
            std::array<IndexType, 4> neighbours(const IndexType& idx) const;
            std::array<IndexType, 2> unique_neighbours(const IndexType& idx) const;

            inline const VectorType& operator()(const IndexType& i) const;
            inline VectorType& operator()(const IndexType& i);
        };
    }
}

#include "detail/square_impl.hpp"

#endif