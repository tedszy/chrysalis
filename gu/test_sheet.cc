#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Sheet
#include <boost/test/unit_test.hpp>
#include "sheet.h"

struct Set1
{
  Sheet sh_empty;
  Sheet a1;
  Sheet a2;
  Sheet a1_hstack_a2;
  Sheet a2_hstack_a1;
  Sheet a1_vstack_a2;
  Sheet a2_vstack_a1;
  Sheet b1;
  Sheet b2;
  Sheet b2x;
  Set1(void);
};

Set1::Set1() :
  sh_empty{}
    
  , a1({{"a","b","c"}})
    
  , a2({{"a"}
      , {"b"}
      , {"c"}})
    
  , a1_hstack_a2({{"a","b","c","a"}
      , {"","","","b"}
      , {"","","","c"}})
    
  , a2_hstack_a1({{"a","a","b","c"}
      , {"b","","",""}
      , {"c","","",""}})
    
  , a1_vstack_a2({{"a","b","c"}
      , {"a","",""}
      , {"b","",""}
      , {"c","",""}})
    
  , a2_vstack_a1({{"a","",""}
      , {"b","",""}
      , {"c","",""}
      , {"a","b","c"}})
    
  , b1({{"a ","aaaa", "aaaaaa"}
      , {"aaaaaa","aa","aaaa"}
      , {"aaaa","aaaaaa","aa"}})

  , b2({{"bbbbbbb","bbbbbbb","bbbbbbb","bbbbbbb","bbbbbbb"}
      , {"bbb","bbb","bbb","bbb","bbb"}
      , {"bbb","bbb","bbb","bbb","bbb"}})

  , b2x({{"bbbbbbb","bbbbbbb","bbbbbbb","bbbbbbb","bbbbbbb"}
      , {"bbb","bbb","bbb","bbb","bbb"}
      , {"bbb","bbb","bbb","bbb","bbb"}}, Format::text_right)   
{
};

BOOST_FIXTURE_TEST_SUITE(Sheets, Set1)

BOOST_AUTO_TEST_CASE(Empty)
{
  BOOST_CHECK(sh_empty.is_empty());
}

BOOST_AUTO_TEST_CASE(Rows_and_cols)
{
  BOOST_CHECK(a1.n_rows() == 1 && a1.n_cols() == 3);
  BOOST_CHECK(a2.n_rows() == 3 && a2.n_cols() == 1);
}

BOOST_AUTO_TEST_CASE(Horizontal_add1)
{
  BOOST_CHECK(a1 + a2 == a1_hstack_a2);
}

BOOST_AUTO_TEST_CASE(Horizontal_add2)
{
  BOOST_CHECK(a2 + a1 == a2_hstack_a1);
}

BOOST_AUTO_TEST_CASE(Vertical_multiply1)
{
  BOOST_CHECK(a1 * a2 == a1_vstack_a2);
}

BOOST_AUTO_TEST_CASE(Vertical_multiply2)
{
  BOOST_CHECK(a2 * a1 == a2_vstack_a1);
}
                   
BOOST_AUTO_TEST_CASE(Appearance1)
{
  std::cout << std::endl;
  (b1 * b2).display();
}

BOOST_AUTO_TEST_CASE(Appearance2)
{
  std::cout << std::endl;
  (b1 * b2x).display();
}

BOOST_AUTO_TEST_CASE(Appearance3)
{
  std::cout << std::endl;
  (b2x * b1).display();
}

BOOST_AUTO_TEST_SUITE_END()
