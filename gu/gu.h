#ifndef GU_H
#define GU_H

#include "table.h"

struct Geometry {
  unsigned int student_info_columns; // {3};
  unsigned int num_header_rows; // {2};
  unsigned int norm_row_index; // {1};
  Geometry(unsigned int, unsigned int, unsigned int);
};

bool is_integer(const std::string&);

bool is_na(const std::string&);

unsigned int string_to_uint(const std::string&);

std::string double_to_string(const double);

bool is_valid_filename(const std::string&);

void upcase_string(std::string&);

std::string make_meta_data_string(std::string);

bool all_scores_NA(const std::vector<std::string>&, unsigned int);

std::string make_latex_doc(const std::vector<std::string>&);

void check_anomalies(const Table<std::string>&);

#endif

