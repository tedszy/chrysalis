#include <ios>
#include "shrdlu.h"

Field_parser::Field_parser() : delimiter(',') 
{}

Field_parser::Field_parser(const char d) : delimiter(d) 
{}

bool Field_parser::is_last()
{
  return last;
}

std::string Field_parser::get_field(void)
{
  return field;
}

bool Field_parser::read_field (std::istream& ss)
{
  last = false;
  state = State::start;
  field.clear();
  while (true) {
    char c = ss.get();
    switch (state) {
    case State::start:
      if (ss.eof()) {
        last = true;
        return true;
      }
      if (c == quote)
        state = State::quoted;
      else if (c == delimiter)
        return true;
      else if (c == newline) {
        last = true;
        return true;
      } else {
        state = State::unquoted;
        field.push_back(c);
      }
      break; 
    case State::unquoted:
      if (ss.eof()) {
        last = true;
        return true;
      }
      if (c == quote)
        return false;
      else if (c == delimiter)
        return true;
      else if (c == newline) {
        last = true;
        return true;
      } else {
        field.push_back(c);
      }
      break;
    case State::quoted:
      if (ss.eof()) return false;
      if (c == quote) {
        char cnext = ss.get();
        if (ss.eof()) {
          last = true;
          return true;
        } else if (cnext == delimiter) {
          return true;
        } else if (cnext == newline) {
          last = true;
          return true;
        } else if (cnext == quote) {
          field.push_back(quote);
        } else {
          ss.unget();
          return false;
        }
      } else {
        field.push_back(c);
      }
      break;        
    }
  }
}

std::istream& operator>>(std::istream& is, Field_parser& f)
{
  if (f.read_field(is))
    return is;
  else {
    is.setstate(std::ios::failbit);
    return is;
  } 
}

std::ostream& operator<<(std::ostream& os, Field_parser& f)
{
  os << f.get_field();
  return os;
}

// -----

Record_parser::Record_parser()
{
  delimiter = ',';
}

Record_parser::Record_parser(const char sep)
{
  delimiter = sep;
}

std::vector<std::string> Record_parser::get_record()
{
  return record;
}

bool Record_parser::read_record(std::istream& is)
{
  record.clear();
  last = false;
  while (!last && !is.fail()) {
    bool ok = read_field(is);
    record.push_back(field);
  }
}

bool Record_parser::is_blank()
{
  return record.size() == 1 && record.front() == "";
}

std::istream& operator>>(std::istream& is, Record_parser& r)
{
  r.read_record(is);
  return is;
}

std::ostream& operator<<(std::ostream& os, Record_parser& r)
{
  for (auto f : r.get_record())
    os << f << " | ";
  return os;
}

// -----

Document_parser::Document_parser()
{
    delimiter = ',';
}

Document_parser::Document_parser(const char sep)
{
    delimiter = sep;
}

std::vector<std::vector<std::string>> Document_parser::get_document()
{
  return document;
}

bool Document_parser::read_document(std::istream& is)
{
  document.clear();
  last = false;
  while (!is.eof() && !is.fail()) {
    bool ok = read_record(is);
    if (!is_blank())
      document.push_back(record);
  }
}

std::istream& operator>>(std::istream& is, Document_parser& d)
{
  d.read_document(is);
  return is;
}

std::ostream& operator<<(std::ostream& os, Document_parser& d)
{
  for (auto r: d.get_document()) {
    for (auto f: r)
      std::cout << f << " ";
    std::cout << std::endl;
  }
  return os;
}

