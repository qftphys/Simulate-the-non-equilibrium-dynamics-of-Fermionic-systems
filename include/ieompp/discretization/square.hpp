#ifndef IEOMPP_DISCRETIZATION_SQUARE_HPP_
#define IEOMPP_DISCRETIZATION_SQUARE_HPP_

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <type_traits>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <Eigen/Dense>
#pragma GCC diagnostic pop

#include <ieompp/constants.hpp>
#include <ieompp/iterators/integer_iterator.hpp>

namespace ieompp
{
    namespace discretization
    {
        template <typename FloatT, typename IndexT = uint64_t>
        class SquareDiscretization
        {
        public:
            using Float              = FloatT;
            using Index              = IndexT;
            using IndexIterator      = iterators::IntegerIterator<Index, false>;
            using ConstIndexIterator = iterators::IntegerIterator<Index, true>;
            using Vector             = Eigen::Matrix<Float, 2, 1>;

            static constexpr uint64_t coordination_number = 4;

        private:
            const Index _num_x, _num_y, _num;
            const Index _first, _last;
            const Float _x_min, _x_max;
            const Float _x_length, _dx;
            const Float _y_min, _y_max;
            const Float _y_length, _dy;
            const std::array<Vector, 2> _lattice_vectors;

        public:
            SquareDiscretization(const Index& num_x, const Index& num_y);
            SquareDiscretization(const Index& num_x, const Index& num_y, const Float& dx,
                                 const Float& dy);

            Index index(const Index& i, const Index& j) const;

            std::array<Index, 4> neighbors(const Index& idx) const;
            std::array<Index, 2> unique_neighbors(const Index& idx) const;
            Index closest(Vector v) const;

            const Index& num() const;
            const Index& first() const;
            const Index& last() const;
            const Float& x_min() const;
            const Float& x_max() const;
            const Float& x_length() const;
            const Float& dx() const;
            const Float& y_min() const;
            const Float& y_max() const;
            const Float& y_length() const;
            const Float& dy() const;
            const std::array<Vector, 2>& lattice_vectors() const;

            bool neighboring(const Index a, const Index b) const;

            ConstIndexIterator begin() const;
            ConstIndexIterator end() const;
            IndexIterator begin();
            IndexIterator end();
            ConstIndexIterator cbegin() const;
            ConstIndexIterator cend() const;

            template <typename IndexT_>
            typename std::enable_if<std::is_same<Index, IndexT_>::value, Vector>::type
            operator[](const IndexT_& i) const;

            Index operator()(Vector v) const;
        };


        template <typename Float, typename Index>
        SquareDiscretization<Float, Index>::SquareDiscretization(const Index& num_x,
                                                                 const Index& num_y)
            : _num_x(num_x), _num_y(num_y), _num(num_x * num_y), _first(0), _last(_num - 1),
              _x_min(-Pi<Float>::value), _x_max(Pi<Float>::value), _x_length(TwoPi<Float>::value),
              _dx(TwoPi<Float>::value / num_x), _y_min(-Pi<Float>::value), _y_max(Pi<Float>::value),
              _y_length(TwoPi<Float>::value), _dy(TwoPi<Float>::value / num_y),
              _lattice_vectors{{Vector(_dx, 0.), Vector(0., _dy)}}
        {
            assert(_num_x > 0);
            assert(_num_y > 0);
            assert(_num > 0);
        }

        template <typename Float, typename Index>
        SquareDiscretization<Float, Index>::SquareDiscretization(const Index& num_x,
                                                                 const Index& num_y,
                                                                 const Float& dx, const Float& dy)
            : _num_x(num_x), _num_y(num_y), _num(num_x * num_y), _first(0), _last(_num - 1),
              _x_min(0.), _x_max((num_x - 1) * dx), _x_length(_num_x * dx), _dx(dx), _y_min(0.),
              _y_max((num_y - 1) * dy), _y_length(_num_y * dy), _dy(dy),
              _lattice_vectors{{Vector(_dx, 0.), Vector(0., _dy)}}
        {
            assert(_num_x > 0);
            assert(_num_y > 0);
            assert(_num > 0);
        }

        template <typename Float, typename Index>
        typename SquareDiscretization<Float, Index>::Index
        SquareDiscretization<Float, Index>::index(const Index& i, const Index& j) const
        {
            return i * _num_x + j;
        }

        template <typename Float, typename Index>
        std::array<typename SquareDiscretization<Float, Index>::Index, 4>
        SquareDiscretization<Float, Index>::neighbors(const Index& idx) const
        {
            const auto i = idx / _num_y;
            const auto j = idx % _num_x;
            return std::array<Index, 4>{
                {index((i == 0) ? _num_x - 1 : i - 1, j), index(i, (j == 0) ? _num_y - 1 : j - 1),
                 index((i == _num_x - 1) ? 0 : i + 1, j), index(i, (j == _num_y - 1) ? 0 : j + 1)}};
        }

        template <typename Float, typename Index>
        std::array<typename SquareDiscretization<Float, Index>::Index, 2>
        SquareDiscretization<Float, Index>::unique_neighbors(const Index& idx) const
        {
            const auto i = idx / _num_y;
            const auto j = idx % _num_x;
            return std::array<Index, 2>{
                {index((i == _num_x - 1) ? 0 : i + 1, j), index(i, (j == _num_y - 1) ? 0 : j + 1)}};
        }

        template <typename Float, typename Index>
        typename SquareDiscretization<Float, Index>::Index
        SquareDiscretization<Float, Index>::closest(Vector v) const
        {
            const auto dx2 = _dx / 2;
            const auto dy2 = _dy / 2;
            while(v(0) < _x_min - dx2) v(0) += _x_length;
            while(v(0) > _x_max + dx2) v(0) -= _x_length;
            while(v(1) < _y_min - dy2) v(1) += _y_length;
            while(v(1) > _y_max + dy2) v(1) -= _y_length;
            Float min_dist = (v - (*this)[Index(0)]).dot(v - (*this)[Index(0)]), dist;
            Index min      = 0;
            for(auto idx : *this) {
                dist = (v - (*this)[idx]).dot(v - (*this)[idx]);
                if(dist < min_dist) {
                    std::swap(min_dist, dist);
                    min = idx;
                }
            }
            return min;
        }

        template <typename Float, typename Index>
        const typename SquareDiscretization<Float, Index>::Index&
        SquareDiscretization<Float, Index>::num() const
        {
            return _num;
        }

        template <typename Float, typename Index>
        const typename SquareDiscretization<Float, Index>::Index&
        SquareDiscretization<Float, Index>::first() const
        {
            return _first;
        }

        template <typename Float, typename Index>
        const typename SquareDiscretization<Float, Index>::Index&
        SquareDiscretization<Float, Index>::last() const
        {
            return _last;
        }

        template <typename Float, typename Index>
        const Float& SquareDiscretization<Float, Index>::x_min() const
        {
            return _x_min;
        }

        template <typename Float, typename Index>
        const Float& SquareDiscretization<Float, Index>::x_max() const
        {
            return _x_max;
        }

        template <typename Float, typename Index>
        const Float& SquareDiscretization<Float, Index>::x_length() const
        {
            return _x_length;
        }

        template <typename Float, typename Index>
        const Float& SquareDiscretization<Float, Index>::dx() const
        {
            return _dx;
        }

        template <typename Float, typename Index>
        const Float& SquareDiscretization<Float, Index>::y_min() const
        {
            return _y_min;
        }

        template <typename Float, typename Index>
        const Float& SquareDiscretization<Float, Index>::y_max() const
        {
            return _y_max;
        }

        template <typename Float, typename Index>
        const Float& SquareDiscretization<Float, Index>::y_length() const
        {
            return _y_length;
        }

        template <typename Float, typename Index>
        const Float& SquareDiscretization<Float, Index>::dy() const
        {
            return _dy;
        }

        template <typename Float, typename Index>
        const std::array<typename SquareDiscretization<Float, Index>::Vector, 2>&
        SquareDiscretization<Float, Index>::lattice_vectors() const
        {
            return _lattice_vectors;
        }

        template <typename Float, typename Index>
        bool SquareDiscretization<Float, Index>::neighboring(const Index a, const Index b) const
        {
            const auto x_a = a / _num_y;
            const auto y_a = a % _num_x;
            const auto x_b = b / _num_y;
            const auto y_b = b % _num_x;

            if(x_a == x_b) {
                return ((y_a + 1) % _num_y == y_b) || ((y_b + 1) % _num_y == y_a);
            }

            if(y_a == y_b) {
                return ((x_a + 1) % _num_x == x_b) || ((x_b + 1) % _num_x == x_a);
            }

            return false;
        }

        template <typename Float, typename Index>
        typename SquareDiscretization<Float, Index>::ConstIndexIterator
        SquareDiscretization<Float, Index>::begin() const
        {
            return ConstIndexIterator(_first);
        }

        template <typename Float, typename Index>
        typename SquareDiscretization<Float, Index>::ConstIndexIterator
        SquareDiscretization<Float, Index>::end() const
        {
            return ConstIndexIterator(_last + 1);
        }

        template <typename Float, typename Index>
        typename SquareDiscretization<Float, Index>::IndexIterator
        SquareDiscretization<Float, Index>::begin()
        {
            return IndexIterator(_first);
        }

        template <typename Float, typename Index>
        typename SquareDiscretization<Float, Index>::IndexIterator
        SquareDiscretization<Float, Index>::end()
        {
            return IndexIterator(_last + 1);
        }

        template <typename Float, typename Index>
        typename SquareDiscretization<Float, Index>::ConstIndexIterator
        SquareDiscretization<Float, Index>::cbegin() const
        {
            return ConstIndexIterator(_first);
        }

        template <typename Float, typename Index>
        typename SquareDiscretization<Float, Index>::ConstIndexIterator
        SquareDiscretization<Float, Index>::cend() const
        {
            return ConstIndexIterator(_last + 1);
        }

        template <typename Float, typename Index>
        template <typename IndexT>
        typename std::enable_if<std::is_same<Index, IndexT>::value,
                                typename SquareDiscretization<Float, Index>::Vector>::type
            SquareDiscretization<Float, Index>::operator[](const IndexT& idx) const
        {
            const auto i = idx / _num_x;
            const auto j = idx % _num_x;
            return Vector(_x_min + i * _dx, _y_min + j * _dy);
        }

        template <typename Float, typename Index>
        Index SquareDiscretization<Float, Index>::
        operator()(typename SquareDiscretization<Float, Index>::Vector v) const
        {
            while(v(0) < _x_min) v(0) += _x_length;
            while(v(0) > _x_max) v(0) -= _x_length;
            while(v(1) < _y_min) v(1) += _y_length;
            while(v(1) > _y_max) v(1) -= _y_length;
            const auto i = Index(std::round((v(0) - _x_min) / _dx));
            const auto j = Index(std::round((v(1) - _y_min) / _dy));
            return i * _num_x + j;
        }
    }
}

#endif
