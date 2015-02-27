#include <iostream>
#include <conio.h>

#define BOOST_TEST_MODULE Simple testcases

#include "boost/test/included/unit_test.hpp"


BOOST_AUTO_TEST_CASE(test1)
{
    BOOST_CHECK(1 == 2);
}

