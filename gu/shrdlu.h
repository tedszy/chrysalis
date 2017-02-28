#ifndef SHRDLU_H
#define SHRDLU_H

#include <iostream>
#include <vector>

const char quote = '"';
const char newline = '\n';

enum class State {
  start,
    unquoted,
    quoted
    };

class  Field_parser {
 protected:
  char delimiter;  
  State state{State::start};
  std::string field{};
  bool last{false};

 public:
  Field_parser(void);
  Field_parser(const char);
  bool read_field (std::istream&);
  std::string get_field(void);
  bool is_last(void);
};

std::istream& operator>>(std::istream&, Field_parser&);
std::ostream& operator<<(std::ostream&, Field_parser&);

// -----

class Record_parser : public Field_parser
{
 protected:
  std::vector<std::string> record{};

 public:
  bool is_blank(void);
  Record_parser(void);
  Record_parser(const char);
  bool read_record (std::istream&);
  std::vector<std::string> get_record(void);
};

std::istream& operator>>(std::istream&, Record_parser&);
std::ostream& operator<<(std::ostream&, Record_parser&);

// -----

class Document_parser : public Record_parser
{
  std::vector<std::vector<std::string>> document{};

 public:
  Document_parser(void);
  Document_parser(const char);
  bool read_document (std::istream&);
  std::vector<std::vector<std::string>> get_document(void);
};

std::istream& operator>>(std::istream&, Document_parser&);
std::ostream& operator<<(std::ostream&, Document_parser&);


#endif
