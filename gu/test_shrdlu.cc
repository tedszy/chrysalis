#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Shrdlu

#include <boost/test/unit_test.hpp>
#include <sstream>
#include <fstream>
#include "shrdlu.h"


BOOST_AUTO_TEST_SUITE(Comma_delimited)

BOOST_AUTO_TEST_CASE(Plain_unquoted_fields)
{
  std::stringstream ss{"aaa,bbb,ccc"};
  Field_parser fp;
  ss >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "aaa");
  BOOST_CHECK(!fp.is_last());
  ss >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "bbb");
  BOOST_CHECK(!fp.is_last());
  ss >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "ccc");
  BOOST_CHECK(fp.is_last());
}

BOOST_AUTO_TEST_CASE(Empty_fields)
{
  std::stringstream s1{""};
  std::stringstream s2{"\n"};
  std::stringstream s3{"\n\n"};
  Field_parser fp;
  s1 >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "");
  BOOST_CHECK(fp.is_last());
  s2 >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "");
  BOOST_CHECK(fp.is_last());
  s3 >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "");
  BOOST_CHECK(fp.is_last());
}

BOOST_AUTO_TEST_CASE(Empty_fields2)
{
  std::stringstream s{",,"};
  Field_parser fp;
  s >> fp;
  BOOST_CHECK(!fp.is_last());
  s >> fp;
  BOOST_CHECK(!fp.is_last());
  s >> fp;
  BOOST_CHECK(fp.is_last());
}

BOOST_AUTO_TEST_CASE(Embedded)
{
  std::stringstream s{"\"ab\ncd\",\"ab,cd\""};
  Field_parser fp;
  s >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "ab\ncd");
  BOOST_CHECK(!fp.is_last());
  s >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "ab,cd");
  BOOST_CHECK(fp.is_last());
}

BOOST_AUTO_TEST_CASE(What_should_fail)
{
  Field_parser fp;
  std::stringstream s{"ab\"cd"};
  s >> fp;
  BOOST_CHECK(s.fail());

  s.str("abcd\","); s.clear();
  s >> fp;
  BOOST_CHECK(s.fail());

  s.str("abcd\"\n"); s.clear();
  s >> fp;
  BOOST_CHECK(s.fail());
  
  s.str("ab\"\"cd"); s.clear();
  s >> fp;
  BOOST_CHECK(s.fail());
  
  s.str("\"abcd"); s.clear();
  s >> fp;
  BOOST_CHECK(s.fail());
  
  s.str("abcd\""); s.clear();
  s >> fp;
  BOOST_CHECK(s.fail());

  s.str("ab\"cd"); s.clear();
  s >> fp;
  BOOST_CHECK(s.fail());

  s.str("\"abcd\"e"); s.clear();
  s >> fp;
  BOOST_CHECK(s.fail());
}

BOOST_AUTO_TEST_CASE(Quote_quote)
{
  std::stringstream s1{"\"\"\"abcd\""};
  std::stringstream s2{"\"ab\"\"cd\""};
  std::stringstream s3{"\"abcd\"\"\""};
  Field_parser fp;
  s1 >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "\"abcd");
  s2 >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "ab\"cd");
  s3 >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "abcd\"");
}

BOOST_AUTO_TEST_CASE(Quote_quote2)
{
  std::stringstream s{"\"\"\"abcd\",\"ab\"\"cd\",\"abcd\"\"\""};
  Field_parser fp;
  s >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "\"abcd");
  BOOST_CHECK(!fp.is_last());
  s >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "ab\"cd");
  BOOST_CHECK(!fp.is_last());
  s >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "abcd\"");
  BOOST_CHECK(fp.is_last());
}

BOOST_AUTO_TEST_CASE(Records)
{
  std::stringstream ss{"aaa,bbb,ccc\nddd,eee,fff\n\"gg\"\"g\",\"h,hh\",\"ii\ni\""};
  std::vector<std::string> r1{"aaa","bbb","ccc"};
  std::vector<std::string> r2{"ddd","eee","fff"};
  std::vector<std::string> r3{"gg\"g","h,hh", "ii\ni"};
  std::vector<std::string> result;
  Record_parser rp;
  ss >> rp;
  // If I move rp.get_record() into the boost macro, it doesn't work.
  result = rp.get_record();
  BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(),
                                result.end(),
                                r1.begin(),
                                r1.end());
  ss >> rp;
  result = rp.get_record();
  BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(),
                                result.end(),
                                r2.begin(),
                                r2.end());
  ss >> rp;
  result = rp.get_record();
  BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(),
                                result.end(),
                                r3.begin(),
                                r3.end());
  BOOST_CHECK(rp.is_last());
}

BOOST_AUTO_TEST_CASE(Documents)
{
  std::ifstream myfile;
  Document_parser dp;
  myfile.open("p46-engsci-2014-term1.csv");
  myfile >> dp;
  myfile.close();
  BOOST_CHECK_EQUAL(dp.get_document().front().size(), 8);
  BOOST_CHECK_EQUAL(dp.get_document().size(), 17);
}
  
BOOST_AUTO_TEST_SUITE_END();
  
// ------------------ test other delimiter -------------

BOOST_AUTO_TEST_SUITE(Pipe_delimited)

BOOST_AUTO_TEST_CASE(Plain_unquoted_fields)
{
  std::stringstream ss{"aaa|bbb|ccc"};
  Field_parser fp('|');
  ss >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "aaa");
  BOOST_CHECK(!fp.is_last());
  ss >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "bbb");
  BOOST_CHECK(!fp.is_last());
  ss >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "ccc");
  BOOST_CHECK(fp.is_last());
}

BOOST_AUTO_TEST_CASE(Empty_fields)
{
  std::stringstream s1{""};
  std::stringstream s2{"\n"};
  std::stringstream s3{"\n\n"};
  Field_parser fp('|');
  s1 >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "");
  BOOST_CHECK(fp.is_last());
  s2 >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "");
  BOOST_CHECK(fp.is_last());
  s3 >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "");
  BOOST_CHECK(fp.is_last());
}

BOOST_AUTO_TEST_CASE(Empty_fields2)
{
  std::stringstream s{"||"};
  Field_parser fp('|');
  s >> fp;
  BOOST_CHECK(!fp.is_last());
  s >> fp;
  BOOST_CHECK(!fp.is_last());
  s >> fp;
  BOOST_CHECK(fp.is_last());
}

BOOST_AUTO_TEST_CASE(Embedded)
{
  std::stringstream s{"\"ab\ncd\"|\"ab|cd\""};
  Field_parser fp('|');
  s >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "ab\ncd");
  BOOST_CHECK(!fp.is_last());
  s >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "ab|cd");
  BOOST_CHECK(fp.is_last());
}

BOOST_AUTO_TEST_CASE(What_should_fail)
{
  Field_parser fp('|');
 
  std::stringstream s{"ab\"cd"};
  s >> fp;
  BOOST_CHECK(s.fail());

  s.str("abcd\"|"); s.clear();
  s >> fp;
  BOOST_CHECK(s.fail());

  s.str("abcd\"\n"); s.clear();
  s >> fp;
  BOOST_CHECK(s.fail());
  
  s.str("ab\"\"cd"); s.clear();
  s >> fp;
  BOOST_CHECK(s.fail());
  
  s.str("\"abcd"); s.clear();
  s >> fp;
  BOOST_CHECK(s.fail());
  
  s.str("abcd\""); s.clear();
  s >> fp;
  BOOST_CHECK(s.fail());

  s.str("ab\"cd"); s.clear();
  s >> fp;
  BOOST_CHECK(s.fail());

  s.str("\"abcd\"e"); s.clear();
  s >> fp;
  BOOST_CHECK(s.fail());
}

BOOST_AUTO_TEST_CASE(Quote_quote)
{
  std::stringstream s1{"\"\"\"abcd\""};
  std::stringstream s2{"\"ab\"\"cd\""};
  std::stringstream s3{"\"abcd\"\"\""};
  Field_parser fp('|');
  s1 >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "\"abcd");
  s2 >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "ab\"cd");
  s3 >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "abcd\"");
}

BOOST_AUTO_TEST_CASE(Quote_quote2)
{
  std::stringstream s{"\"\"\"abcd\"|\"ab\"\"cd\"|\"abcd\"\"\""};
  Field_parser fp('|');
  s >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "\"abcd");
  BOOST_CHECK(!fp.is_last());
  s >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "ab\"cd");
  BOOST_CHECK(!fp.is_last());
  s >> fp;
  BOOST_CHECK_EQUAL(fp.get_field(), "abcd\"");
  BOOST_CHECK(fp.is_last());
}

BOOST_AUTO_TEST_CASE(Records)
{
  std::stringstream ss{"aaa|bbb|ccc\nddd|eee|fff\n\"gg\"\"g\"|\"h|hh\"|\"ii\ni\""};
  std::vector<std::string> r1{"aaa","bbb","ccc"};
  std::vector<std::string> r2{"ddd","eee","fff"};
  std::vector<std::string> r3{"gg\"g","h|hh", "ii\ni"};
  std::vector<std::string> result;
  Record_parser rp('|');
  ss >> rp;
  // If I move rp.get_record() into the boost macro, it doesn't work.
  result = rp.get_record();
  BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(),
                                result.end(),
                                r1.begin(),
                                r1.end());
  ss >> rp;
  result = rp.get_record();
  BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(),
                                result.end(),
                                r2.begin(),
                                r2.end());
  ss >> rp;
  result = rp.get_record();
  BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(),
                                result.end(),
                                r3.begin(),
                                r3.end());
  BOOST_CHECK(rp.is_last());
}

BOOST_AUTO_TEST_CASE(Documents)
{
  std::ifstream myfile;
  Document_parser dp('|');
  myfile.open("pipe-delimited.psv");
  myfile >> dp;
  myfile.close();
  BOOST_CHECK_EQUAL(dp.get_document().front().size(), 8);
  BOOST_CHECK_EQUAL(dp.get_document().size(), 17);
}
  
BOOST_AUTO_TEST_SUITE_END()
  


  

  
