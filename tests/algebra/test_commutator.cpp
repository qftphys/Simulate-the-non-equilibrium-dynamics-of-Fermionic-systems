#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "hubbard/algebra/commutator.hpp"
#include "hubbard/algebra/operator.hpp"
#include <iostream>

using namespace hubbard;
using namespace algebra;

TEST_CASE("anticommutates", "[algebra]")
{
    SECTION("creator & creator")
    {
        auto ret = anticommutates(
            make_creator(0, true),
            make_creator(0, true)
        );
        REQUIRE(ret == true);

        ret = anticommutates(
            make_creator(0, false),
            make_creator(0, true)
        );
        REQUIRE(ret == true);
        
        ret = anticommutates(
            make_creator(1, true),
            make_creator(0, true)
        );
        REQUIRE(ret == true);
    }

    SECTION("annihilator & creator")
    {
        auto ret = anticommutates(
            make_annihilator(0, true),
            make_creator(0, true)
        );
        REQUIRE(ret == false);

        ret = anticommutates(
            make_annihilator(0, false),
            make_creator(0, true)
        );
        REQUIRE(ret == true);
        
        ret = anticommutates(
            make_annihilator(1, true),
            make_creator(0, true)
        );
        REQUIRE(ret == true);
    }

    SECTION("annihilator & annihilator")
    {
        auto ret = anticommutates(
            make_annihilator(0, true),
            make_annihilator(0, true)
        );
        REQUIRE(ret == true);

        ret = anticommutates(
            make_annihilator(0, false),
            make_annihilator(0, true)
        );
        REQUIRE(ret == true);
        
        ret = anticommutates(
            make_annihilator(1, true),
            make_annihilator(0, true)
        );
        REQUIRE(ret == true);
    }
}

TEST_CASE("commutator", "[algebra]")
{
    using Operator = Operator<int, bool>;
    using Term = Term<Operator>;

    SECTION("[cc,c]")
    {
        Term term1 =
            make_term(Complex{1., 0.}, {make_creator(-1, true), make_annihilator(0, true)});
        Term term2  =
            make_term(Complex{1., 0.}, {make_creator(0, true)});

        auto result = commutate(term1, term2);
        REQUIRE(result.size() == 1);
        REQUIRE(result.front() == make_term(Complex{1., 0.}, {make_creator(-1, true)}));
    }

    SECTION("[c,cc]")
    {
        Term term1 =
            make_term(Complex{1., 0.}, {make_creator(0, true)});
        Term term2 =
            make_term(Complex{1., 0.}, {make_creator(-1, true), make_annihilator(0, true)});

        auto result = commutate(term1, term2);
        REQUIRE(result.size() == 1);
        REQUIRE(result.front() == make_term(Complex{-1., 0.}, {make_creator(-1, true)}));
    }

    SECTION("[cc,cc]")
    {
        Term term1 =
            make_term(Complex{1., 0.}, {make_creator(0, true), make_annihilator(1, true)});
        Term term2 =
            make_term(Complex{1., 0.}, {make_creator(1, true), make_annihilator(0, true)});

        auto result = commutate(term1, term2);
        REQUIRE(result[0]
                == make_term(Complex{-1., 0.}, {make_creator(1, true), make_annihilator(1, true)}));
        REQUIRE(result[1]
                == make_term(Complex{1., 0.}, {make_creator(0, true), make_annihilator(0, true)}));
    }
}