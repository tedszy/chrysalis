#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Gu
#include <boost/test/unit_test.hpp>
#include <fstream>
#include "shrdlu.h"
#include "gu.h"

struct Set2
{
  std::string int_string;
  std::string na_string;
  std::string NA_string;
  std::string non_int_string1;
  std::string non_int_string2;
  std::string blank_string;
  Set2(void);
};

Set2::Set2() : int_string("12345")
             , na_string("na")
             , NA_string("NA")
             , non_int_string1("zarf")
             , non_int_string2("123.45")
             , blank_string("")
{}

BOOST_FIXTURE_TEST_SUITE(Cell_values, Set2)

BOOST_AUTO_TEST_CASE(Integer)
{
  BOOST_CHECK(is_integer(int_string));
}

BOOST_AUTO_TEST_CASE(Non_integers)
{
  BOOST_CHECK(!is_integer(NA_string));
  BOOST_CHECK(!is_integer(non_int_string1));
  BOOST_CHECK(!is_integer(non_int_string2));
  BOOST_CHECK(!is_integer(blank_string));
}

BOOST_AUTO_TEST_CASE(NA)
{
  BOOST_CHECK(is_na(na_string));
  BOOST_CHECK(is_na(NA_string));
  BOOST_CHECK(!is_na(blank_string));
  BOOST_CHECK(!is_na(int_string));
}

BOOST_AUTO_TEST_SUITE_END();

struct Set3 {
  Row<std::string> norms1;
  Row<std::string> scores1;
  Row<std::string> scores2;
  Row<std::string> scores3;
  Row<std::string> scores4;
  Set3() : norms1({"50","50","50","50"})
         , scores1({"40","40","40","40"})
         , scores2({"NA","40","NA","40"})
         , scores3({"NA","NA","NA","NA"})
         , scores4({"0","0","0","0",})
  {};
};

BOOST_FIXTURE_TEST_SUITE(Computations, Set3)

BOOST_AUTO_TEST_CASE(Calc1)
{
  Computation f(norms1);
  f(scores1);
  BOOST_CHECK_EQUAL(f.get_frac_total(), "160/200");
  BOOST_CHECK_EQUAL(f.get_real_total(), "80.00");
}

BOOST_AUTO_TEST_CASE(Calc2)
{
  Computation f(norms1);
  f(scores2);
  BOOST_CHECK_EQUAL(f.get_frac_total(), "80/100");
  BOOST_CHECK_EQUAL(f.get_real_total(), "80.00");
}

BOOST_AUTO_TEST_CASE(Calc3)
{
  Computation f(norms1);
  f(scores3);
  BOOST_CHECK_EQUAL(f.get_frac_total(), "NA");
  BOOST_CHECK_EQUAL(f.get_real_total(), "NA");
}

BOOST_AUTO_TEST_CASE(Calc4)
{
  Computation f(norms1);
  f(scores4);
  BOOST_CHECK_EQUAL(f.get_frac_total(), "0/200");
  BOOST_CHECK_EQUAL(f.get_real_total(), "0.00");
}

BOOST_AUTO_TEST_CASE(Result_accumulation)
{
  Computation f(norms1);
  f(scores1);
  f(scores2);
  f(scores3);
  f(scores4);
  f.display();
  std::cout << std::endl;
}

// If final score is NA, don't count it and don't
// increment the sample size.
BOOST_AUTO_TEST_CASE(Final_score_accumulation)
{
  Computation f(norms1);
  f(scores1);
  f(scores2);
  f(scores3);
  f(scores4);
  BOOST_CHECK_EQUAL(f.get_final_score_average(), "53.33");
}

BOOST_AUTO_TEST_CASE(Statistics)
{
  Computation f(norms1);
  f(scores1);
  f(scores2);
  f(scores3);
  f(scores4);
  Sheet savg({f.get_test_averages()});
  Sheet favg({{f.get_final_score_average()}});
  (savg + favg).display();
}

BOOST_AUTO_TEST_SUITE_END();

struct Set4 {
  std::string filename1;
  std::string valid_filename;
  std::string invalid_filename1;
  std::string invalid_filename2;
  Set4(void);
};

Set4::Set4() : filename1("p45-engmath-2011-term2")
             , valid_filename("p55-engsci-2013-term1")
             , invalid_filename1("foo_goo_soo")
             , invalid_filename2("foo-p44-engmath-2000-term2")
{}

BOOST_FIXTURE_TEST_SUITE(Metadata, Set4);

BOOST_AUTO_TEST_CASE(Meta)
{
  BOOST_CHECK_EQUAL(make_meta_data_string(filename1),
                    "P45  ENGMATH  2011-2012  TERM2");
}

BOOST_AUTO_TEST_CASE(Valid_filenames)
{
  BOOST_CHECK(is_valid_filename(valid_filename));
  BOOST_CHECK(!is_valid_filename(invalid_filename1));
  BOOST_CHECK(!is_valid_filename(invalid_filename2));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Utilities)

BOOST_AUTO_TEST_CASE(NA)
{
  Row<std::string> allNA{"NA","NA","NA","NA"};
  Row<std::string> not_allNA{"NA","NA","50","NA"};
  BOOST_CHECK(all_na(allNA));
  BOOST_CHECK(!all_na(not_allNA));
}

BOOST_AUTO_TEST_CASE(Upcase)
{
  std::string z{"zarf1999"};
  upcase_string(z);
  BOOST_CHECK_EQUAL(z, "ZARF1999");
}

BOOST_AUTO_TEST_CASE(Double_to_string)
{
  double z{15.55};
  BOOST_CHECK_EQUAL(double_to_string(z), "15.55");
}

BOOST_AUTO_TEST_CASE(String_to_uint)
{
  std::string z{"123"};
  BOOST_CHECK_EQUAL(string_to_uint(z), 123);
}

BOOST_AUTO_TEST_SUITE_END()





