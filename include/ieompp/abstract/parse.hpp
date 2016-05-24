#ifndef IEOMPP_ABSTRACT_PARSE_HPP_
#define IEOMPP_ABSTRACT_PARSE_HPP_

#include <regex>
#include <tuple>

#include "ieompp/abstract/term.hpp"

namespace ieompp
{
    namespace abstract
    {
        namespace parse_new
        {
            namespace components
            {
                static const std::string operator_symbol = u8"c";

                //! range of the lowercase latin letters
                static const std::string letters         = u8"a-z";

                //! range of the uppercase latin letters
                static const std::string letters_capital = u8"A-Z";

                //! range of the arabic numbers
                static const std::string numbers         = u8"0-9";

                //! range of the lowercase greek letters
                static const std::string greek           = u8"αβγδεζηθικλμνξοπρστνφχψω";

                //! range of the upeprcase greek letters
                static const std::string greek_capital   = u8"ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩ";

                //! range of arrows
                static const std::string arrows          = u8"←↑↓→";

                //! special symbols
                static const std::string symbols         = u8"\\-\\+_'";

                //! allowed characters in a spatial index
                static const std::string index =
                    letters + letters_capital + numbers + greek + greek_capital + symbols;

                //! allowed characters in a spin
                static const std::string spin =
                    letters + letters_capital + numbers + greek + greek_capital + arrows + symbols;
            }

            namespace regexes
            {
                static const std::string str_exponent  = u8"(†|\\\\dagger)";
                static const std::string str_index     = "([" + components::index + "]+)";
                static const std::string str_spin      = "([" + components::spin + "]+)";
                // TODO: whitespaces?
                static const std::string str_subscript = "(?:\\{" + str_index + "," + str_spin + "\\})";
                static const std::string str_creator =
                    "(" + components::operator_symbol + ")(?:" + "\\_" + str_subscript + "\\^"
                    + str_exponent + "|" + "\\^" + str_exponent + "\\_" + str_subscript + ")";
                static const std::string str_annihilator =
                    "(" + components::operator_symbol + ")\\_" + str_subscript;
                static const std::string str_commutator = "\\[(.*);(.*)\\]";

                static const std::regex exponent(str_exponent);
                static const std::regex index(str_index);
                static const std::regex spin(str_spin);
                static const std::regex subscript(str_subscript);
                static const std::regex creator(str_creator);
                static const std::regex annihilator(str_annihilator);
                static const std::regex commutator(str_commutator);
            }

            std::tuple<bool, std::string, std::string> parse_commutator(const std::string& expr);
            AbstractTerm parse_term(std::string expr);
        }

        namespace parse
        {
            namespace components {
                //! string to use for operators
                static const std::string operator_ = u8"c";

                //! range of the lowercase latin letters
                static const std::string letters = u8"a-z";

                //! range of the uppercase latin letters
                static const std::string letters_capital = u8"A-Z";

                //! range of the arabic numbers
                static const std::string numbers = u8"0-9";

                //! range of the lowercase greek letters
                static const std::string greek = u8"αβγδεζηθικλμνξοπρστνφχψω";

                //! range of the upeprcase greek letters
                static const std::string greek_capital = u8"ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩ";

                //! range of arrows
                static const std::string arrows = u8"←↑↓→";


                //! allowed characters for an index
                static const std::string index =
                    letters + letters_capital + numbers + greek + greek_capital + "_'\\\\\\+\\-";

                //! allowed charctaers for a spin index
                static const std::string spin = letters + letters_capital + numbers + greek
                                                + greek_capital + arrows + "_'\\\\\\+\\-";

                //! structure of a term in the commutator
                static const std::string commutator_term     = u8"\\s*(.+)\\s*";

                //! structure of an operator exponent (daggered or not)
                static const std::string operator_exponent   = u8"(?:\\^\\{?(\\\\dagger|†)\\}?)?";

                //! structure of the subscript of a commutator (index, spin)
                static const std::string operator_subscript  = u8"_\\{\\s*([" + index + u8"]+)\\s*\\,\\s*([" + spin + u8"]+)\\s*\\}";

                //! structure of the subscript of a Kronecker symbol
                static const std::string kronecker_subscript = u8"_\\{\\s*([" + spin + u8"]+)\\s*\\,\\s*([" + spin + u8"]+)\\s*\\}";


                //! structure of a commutator [term1;term2]
                static const std::string str_commutator = u8"^\\[" + commutator_term + ";" + commutator_term + "\\]$";

                //! structure of an operator c_subscript(^exponent)
                static const std::string str_operator   = operator_ + operator_subscript + operator_exponent;

                //! structure of a Kronecker delta δ_{index1,index}
                static const std::string str_kronecker  = u8"(?:δ|\\\\delta)" + kronecker_subscript;

                //! structure of a complex number
                static const std::string str_number     = u8"\\(\\s*([\\deE\\.\\+-]+)\\s*\\,\\s*([\\deE\\.\\+-]+)\\s*\\)";
            }


            //! regex for a commutator
            static const std::regex re_commutator(components::str_commutator);

            //! regex for an operator
            static const std::regex re_operator(components::str_operator);

            //! regex for a kronecker symbol
            static const std::regex re_kronecker(components::str_kronecker);

            //! regex for a complex number
            static const std::regex re_number(components::str_number);

            /*!
             * @brief Determine whether a string resembles a commutator
             *
             * @param expression The expression
             * @return Returns a tuple of a bool and two strings. The first element indicates if the
             * expression matches the commutator regex. If true the strings hold the two terms of
             * the commutator. They are empty otherwise.
             */
            std::tuple<bool, std::string, std::string> is_commutator(const std::string& expression);

            /*!
             * @brief Determine whether a string resembles an operator.
             *
             * @param expression The expression
             * @return Returns a tuple of a bool, two strings and and another bool. The first
             * element indicates whether the expression is a operator. If true the other elements
             * contain the index and the spin of the operator while the last one indicates whether
             * it is a creator or an annihilator.
             */
            std::tuple<bool, std::string, std::string, bool> is_operator(const std::string& expression);

            /*!
             * @brief Determine whether a string resembles a Kronecker symbol
             *
             * @param expression The expression
             * @return Returns a tuple of a bool and two strings. The first element indicates if the
             * expression matches the Kronecker regex. If true the strings hold the two indices of
             * the Kronecker.
             */
            std::tuple<bool, std::string, std::string> is_kronecker(const std::string& expression);

            /*!
             * @brief Determine whether a string resembles a complex number
             *
             * @param expression The expression
             * @return Returns a tuple of a bool and two strings. The first element indicates if the
             * expression matches the Kronecker regex. If true the strings hold the
             */
            std::tuple<bool, std::string, std::string> is_number(const std::string& expression);

            /*!
             * @brief Parse the input as an AbstractTerm
             *
             * @param expression The expression to parse.
             *
             * @return The AbstractTerm representing the expression
             */
            AbstractTerm parse_term(const std::string& expression);
        }
    }
}

#include "detail/parse_impl.hpp"

#endif
