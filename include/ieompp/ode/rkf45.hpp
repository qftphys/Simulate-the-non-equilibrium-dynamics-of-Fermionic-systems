#ifndef IEOMPP_ODE_RKF45_HPP_
#define IEOMPP_ODE_RKF45_HPP_

#include <cassert>

#include <ieompp/description.hpp>
#include <ieompp/ode/step_size_controller.hpp>
#include <ieompp/types/eigen/check.hpp>

namespace ieompp
{
    namespace ode
    {
        template <typename RealT>
        class RKF45
        {
            using Real = RealT;

        private:
            const std::size_t _dimension;
            Real _step_size;
            Real _last_error;
            StepSizeController<Real> _controller;

        public:
            RKF45(std::size_t dimension, const Real& step_size)
                : _dimension(dimension), _step_size(step_size), _last_error(0.)
            {
            }

            const Real& step_size() const { return _step_size; }
            Real& step_size() { return _step_size; }

            std::size_t dimension() const { return _dimension; }
            const Real& last_error() const { return _last_error; }

            const Real& controller() const { return _controller; }
            Real& controller() { return _controller; }

            template <typename Matrix, typename Vector>
            void step(const Matrix& m, Vector& u)
            {
                assert(types::is_quadratic(m));
                assert(m.rows() == _dimension);
                assert(u.cols() == 1);
                assert(u.rows() == _dimension);

                static const Real a_2_1 = Real(1) / Real(4);
                static const Real a_3_1 = Real(3) / Real(32), a_3_2 = Real(9) / Real(32);
                static const Real a_4_1 = Real(1932) / Real(2197), a_4_2 = Real(-7200) / Real(2197),
                                  a_4_3 = Real(7296) / Real(2197);
                static const Real a_5_1 = Real(439) / Real(216), a_5_2 = Real(-8),
                                  a_5_3 = Real(3680) / Real(513), a_5_4 = Real(-845) / Real(4104);
                static const Real a_6_1 = Real(-8) / Real(27), a_6_2 = Real(2),
                                  a_6_3 = Real(-3544) / Real(2565), a_6_4 = Real(2197) / Real(4104),
                                  a_6_5 = Real(-11) / Real(40);
                static const Real b4_1 = Real(25) / Real(216), b4_2 = Real(0),
                                  b4_3 = Real(1408) / Real(2565), b4_4 = Real(2197) / Real(4104),
                                  b4_5 = Real(-1) / Real(5), b4_6 = Real(0);
                static const Real b5_1 = Real(16) / Real(135), b5_2 = Real(0),
                                  b5_3 = Real(6656) / Real(12825), b5_4 = Real(28561) / Real(56430),
                                  b5_5 = Real(-9) / Real(50), b5_6 = Real(2) / Real(55);

                static Vector k_1, k_2, k_3, k_4, k_5, k_6;
                static Vector u4, u5;

                k_1 = m * u;
                do {
                    k_2 = m * (u + _step_size * a_2_1 * k_1);
                    k_3 = m * (u + _step_size * (a_3_1 * k_1 + a_3_2 * k_2));
                    k_4 = m * (u + _step_size * (a_4_1 * k_1 + a_4_2 * k_2 + a_4_3 * k_3));
                    k_5 = m * (u
                               + _step_size
                                     * (a_5_1 * k_1 + a_5_2 * k_2 + a_5_3 * k_3 + a_5_4 * k_4));
                    k_6 = m * (u
                               + _step_size * (a_6_1 * k_1 + a_6_2 * k_2 + a_6_3 * k_3 + a_6_4 * k_4
                                               + a_6_5 * k_5));

                    u4 = u + _step_size * (b4_1 * k_1 + b4_3 * k_3 + b4_4 * k_4 + b4_5 * k_5);
                    u5 = u
                         + _step_size
                               * (b5_1 * k_1 + b5_3 * k_3 + b5_4 * k_4 + b5_5 * k_5 + b5_6 * k_6);

                    _last_error = (u4 - u5).norm();
                    if(!_controller(_step_size, _last_error)) {
                        u = u5;
                        break;
                    }
                } while(true);
            }
        };
    }
}

#endif