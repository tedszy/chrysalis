#ifndef SHEET_H
#define SHEET_H

#include <ios>
#include "table.h"

enum class Sort_type {
  text,
  numeric_asc,
  numeric_desc
};

enum class Format {
  text_left,
    text_right
    };

class Sheet {

 protected:
  Row<Format> alignment{};
  Row<unsigned int> width{};
  Table<std::string> data{{}};

  void compute_widths(void);
  void extend_horizontal(const int);
  void extend_vertical(const int);
  void append_horizontal(const Sheet&);
  void append_vertical(const Sheet&);
  void stack_horizontal(const Sheet&);
  void stack_vertical(const Sheet&);

 public:
  Sheet(void) : data{{}} {};
  Sheet(const Row<std::string>&);
  Sheet(const Row<std::string>&, Format);
  Sheet(unsigned int, unsigned int, std::string);
  Sheet(unsigned int, unsigned int, std::string, Format);
  Sheet(const Table<std::string>&);
  Sheet(const Table<std::string>&, Format);
  unsigned int n_rows(void) const;
  unsigned int n_cols(void) const;

  friend Sheet operator+ (const Sheet&, const Sheet&);
  friend Sheet operator* (const Sheet&, const Sheet&);
  friend bool operator== (const Sheet&, const Sheet&);

  bool is_empty(void) const;
  std::string to_string(void);
  void display(void);
  void print(void);
  void sort_by_column(unsigned int, Sort_type);
};


#endif
