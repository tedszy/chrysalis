#include <algorithm>
#include <assert.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "sheet.h"

void Sheet::compute_widths()
{
  Row<std::string>::const_iterator i;
  Row<unsigned int>::iterator j;
  for (auto row : data)
    for (i = row.begin(), j = width.begin();
         i != row.end() && j != width.end();
         ++i, ++j)
      *j = std::max((unsigned int)((*i).length()), *j);
}

Sheet::Sheet(unsigned int r, unsigned int c, std::string init_str)
  : data(Table<std::string>(r, Row<std::string>(c, init_str)))
  , width(Row<unsigned int>(c, 0))
  , alignment(Row<Format>(c, Format::text_left))
{
  compute_widths();
}

Sheet::Sheet(unsigned int r, unsigned int c, std::string init_str, Format f)
  : data(Table<std::string>(r, Row<std::string>(c, init_str)))
  , width(Row<unsigned int>(c, 0))
  , alignment(Row<Format>(c, f))
{
  compute_widths();
}

Sheet::Sheet(const Row<std::string>& row)
  : data({row})
  , width(Row<unsigned int>(row.size()))
  , alignment(Row<Format>(row.size(), Format::text_left))
{
  compute_widths();
}

Sheet::Sheet(const Row<std::string>& row, Format f)
  : data({row})
  , width(Row<unsigned int>(row.size()))
  , alignment(Row<Format>(row.size(), f))
{
  compute_widths();
}

Sheet::Sheet(const Table<std::string>& table)
  : data(table)
  , width(Row<unsigned int>(table.front().size()))
  , alignment(Row<Format>(table.front().size(), Format::text_left))
{
  compute_widths();
}

Sheet::Sheet(const Table<std::string>& table, Format f)
  : data(table)
  , width(Row<unsigned int>(table.front().size()))
  , alignment(Row<Format>(table.front().size(), f))
{
  compute_widths();
}

unsigned int Sheet::n_rows() const
{
  return data.size();
}

unsigned int Sheet::n_cols() const
{
  return data.front().size();
}

bool Sheet::is_empty() const
{
  return (n_rows() == 1) && (n_cols() == 0);
}

void Sheet::append_horizontal(const Sheet& other)
{
  assert(n_rows() == other.n_rows());
  Table<std::string>::iterator i;
  Table<std::string>::const_iterator j;
  for (i = data.begin(), j = other.data.begin();
       i != data.end() && j != other.data.end();
       ++i, ++j)
    {
      i->insert(i->end(), j->begin(), j->end());
    }
}

void Sheet::append_vertical(const Sheet& other)
{
  assert(n_cols() == other.n_cols());
  data.insert(data.end(), other.data.begin(), other.data.end());
}
void Sheet::extend_horizontal(const int by_n_columns)
{
  Table<std::string>::iterator i;
  for (i = data.begin(); i != data.end(); ++i) {
    Row<std::string> fill(by_n_columns, "");
    i->insert(i->end(), fill.begin(), fill.end());
  }
}

void Sheet::extend_vertical(const int by_n_rows)
{
  Row<std::string> fill(n_cols(), "");
  for (auto i = 0; i < by_n_rows; ++i)
    data.push_back(fill);
}

void Sheet::stack_horizontal(const Sheet& other)
{
  unsigned int ra = n_rows();
  unsigned int rb = other.n_rows();
  if (ra > rb) {
    Sheet shb(other.data);
    shb.extend_vertical(ra - rb);
    append_horizontal(shb);
  } else if (ra < rb) {
    extend_vertical(rb - ra);
    append_horizontal(other);
  } else {
    append_horizontal(other);
  }
}

void Sheet::stack_vertical(const Sheet& other)
{
  unsigned int ca = n_cols();
  unsigned int cb = other.n_cols();
  if (ca > cb) {
    Sheet shb(other.data);
    shb.extend_horizontal(ca - cb);
    append_vertical(shb);
  } else if (ca < cb) {
    extend_horizontal(cb - ca);
    append_vertical(other);
  } else {
    append_vertical(other);
  }
}

bool operator==(const Sheet& sha, const Sheet& shb)
{
  return sha.data == shb.data  && sha.width == shb.width
    && sha.alignment == shb.alignment;
}

Sheet operator+(const Sheet& sha, const Sheet& shb)
{
  if (sha.is_empty()) {
    Sheet sh = shb;
    return sh;
  } else if (shb.is_empty()) {
    Sheet sh = sha;
    return sh;
  }
  Sheet sh = sha;
  sh.stack_horizontal(shb);
  sh.alignment.insert(sh.alignment.end(),
                      shb.alignment.begin(),
                      shb.alignment.end());
  sh.width.insert(sh.width.end(),
                  shb.width.begin(),
                  shb.width.end());
  return sh;
}

// Keep the alignments of the first sheet. If second sheet
// is wider, use the excess alignments. Recompute widths.
Sheet operator*(const Sheet& sha, const Sheet& shb)
{
  if (sha.is_empty()) {
    Sheet sh = shb;
    return sh;
  } else if (shb.is_empty()) {
    Sheet sh = sha;
    return sh;
  }
  Sheet sh = sha;
  sh.stack_vertical(shb);
  unsigned int ca = sha.n_cols();
  unsigned int cb = shb.n_cols();
  if (ca < cb) {
    sh.alignment.insert(sh.alignment.end(),
                        shb.alignment.begin() + ca,
                        shb.alignment.end());
  }
  sh.width = Row<unsigned int>(sh.n_cols(), 0);
  Row<std::string>::const_iterator i;
  Row<unsigned int>::iterator j;
  for (auto row : sh.data)
    for (i = row.begin(), j = sh.width.begin();
         i != row.end() && j != sh.width.end();
         ++i, ++j)
      *j = std::max((unsigned int)((*i).length()), *j);

  return sh;
}

void Sheet::print(void)
{
  for (auto i : data) {
    for (auto j : i)
      std::cout << j << " | ";
    std::cout << std::endl;
  }
}

std::string Sheet::to_string()
{
  std::stringstream ss;
  const std::string sep{"  "};
  Row<std::string>::const_iterator i;
  Row<unsigned int>::const_iterator j;
  Row<Format>::const_iterator k;
  for (auto row : data) {
    for (i = row.begin(), j = width.begin(), k = alignment.begin();
         i != row.end() && j != width.end() && k != alignment.end();
         ++i, ++j, ++k)
      {
        ss << std::setw(*j);
        switch (*k) {
        case Format::text_left :
          ss << std::left;
          break;
        case Format::text_right :
          ss << std::right;
          break;
        default :
          std::cerr << "Option not supported yet." << std::endl;
        }
        ss << *i << sep;
      }
    ss << std::endl;
  }
  return ss.str();
}

void Sheet::display(void)
{
  std::cout << to_string();
}


void Sheet::sort_by_column(unsigned int col, Sort_type st)
{

  auto text_compare = [col](Row<std::string> s, Row<std::string> t) {
                        return s.at(col) < t.at(col);
                      };
  // Default behaviour of atof is good enough for NA handling.
  auto numeric_compare_asc = [col](Row<std::string> s, Row<std::string> t) {
                           return atof(s.at(col).c_str()) < atof(t.at(col).c_str());
                         };

  auto numeric_compare_desc = [col](Row<std::string> s, Row<std::string> t) {
                           return atof(s.at(col).c_str()) > atof(t.at(col).c_str());
                         };

  if (st == Sort_type::text)
    std::sort(data.begin(), data.end(), text_compare);
  else if (st == Sort_type::numeric_asc)
    std::sort(data.begin(), data.end(), numeric_compare_asc);
  else if (st == Sort_type::numeric_desc)
    std::sort(data.begin(), data.end(), numeric_compare_desc);
  else
    std::cerr << "Unsupported sort comparison type." << std::endl;
}
