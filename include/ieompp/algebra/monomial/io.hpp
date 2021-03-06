#ifndef IEOMPP_ALGEBRA_MONOMIAL_IO_HPP_
#define IEOMPP_ALGEBRA_MONOMIAL_IO_HPP_

#include "ieompp/algebra/monomial/monomial.hpp"

#include <iterator>
#include <ostream>

namespace ieompp
{
    namespace algebra
    {
        template <typename Operator>
        std::ostream& operator<<(std::ostream& strm, const Monomial<Operator>& monomial)
        {
            std::copy(monomial.begin(), --monomial.end(),
                      std::ostream_iterator<Operator>(strm, " "));
            strm << monomial.back();
            return strm;
        }
    } // namespace algebra
} // namespace ieompp

#endif
