#include <assert.h>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <regex>
#include "gu.h"

Geometry::Geometry(unsigned int sic, unsigned int nhr, unsigned int nri)
 : student_info_columns(sic)
  , num_header_rows(nhr)
  , norm_row_index(nri)
{}

void upcase_string(std::string& s)
{
  for (auto i = 0; i < s.length(); ++i)
    s[i] = toupper(s[i]);
}

std::string make_meta_data_string(std::string str)
{
  std::string group;
  std::string description;
  std::string year;
  unsigned int next_year;
  std::string term;
  std::string token;
  std::string sep{"  "};
  char delimiter{'-'};
  upcase_string(str);
  std::stringstream ss(str);
  std::getline(ss, token, delimiter);
  group = token;
  std::getline(ss, token, delimiter);
  description = token;
  std::getline(ss, token, delimiter);
  year = token;
  next_year = string_to_uint(year) + 1;
  std::getline(ss, token, delimiter);
  term = token;
  ss.str(""); ss.clear();
  ss << group << sep << description << sep
     << year << delimiter << next_year << sep
     << term;
  return ss.str();
}

bool is_integer(const std::string& s)
{
  return !s.empty()
    && std::find_if(s.begin(),
                    s.end(),
                    [](char c)
                    {
                      return !std::isdigit(c);
                    }) == s.end();
}

bool is_na(const std::string& s)
{
  return s == "NA" || s == "na";
}

std::string double_to_string(const double r)
{
  std::stringstream buf;
  buf << std::fixed << std::setprecision(2) << r;
  return buf.str();
}

unsigned int string_to_uint(const std::string& str)
{
  unsigned int res = (unsigned int) stoul(str, 0, 10);
  return res;
}

bool all_scores_NA(const std::vector<std::string>& v, unsigned int pos)
{
  return std::all_of(v.begin() + pos,
                     v.end(),
                     [](std::string str)
                     {
                       return str == "NA";
                     });
}

bool is_valid_filename(const std::string& filename)
{
  std::string regex_str = "([a-zA-Z0-9_]+)-([a-zA-Z0-9_]+)-([0-9]+)-([a-zA-Z0-9_]+)";
  std::regex filename_regex(regex_str);
  return std::regex_match(filename.begin(),
                          filename.end(),
                          filename_regex);
}

std::string make_latex_doc(const std::vector<std::string>& reports)
{
  std::stringstream latex_document;
  latex_document << "\\documentclass[10pt,a4paper]{memoir}" << std::endl;
  latex_document << "\\setlrmarginsandblock{0.5in}{0.5in}{*}" << std::endl;
  latex_document << "\\setulmarginsandblock{0.5in}{0.5in}{8}" << std::endl;
  latex_document << "\\fixthelayout" << std::endl;
  latex_document << "\\begin{document}" << std::endl;

  std::vector<std::string>::const_iterator ir;
  for (ir = reports.begin(); ir != reports.end(); ++ir)
  {
    latex_document << "\\thispagestyle{empty}" << std::endl;
    latex_document << "\\begin{verbatim}" << std::endl;
    latex_document << *ir;
    latex_document << "\\end{verbatim}" << std::endl;
    if (ir + 1 != reports.end())
      latex_document << "\\newpage" << std::endl;
  }
  latex_document << "\\end{document}" << std::endl;
  return latex_document.str();
}

void check_anomalies(const Table<std::string> table, Geometry g)
{

  // Check if exam score is greater than norm value for that exam.


  // Check if ordinal is equal to one of the exam scores.


  // Check standard deviation for outlier..


}



