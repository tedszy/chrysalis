#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "gu.h"
#include "shrdlu.h"
#include "sheet.h"

using namespace std;
using namespace boost::filesystem;
namespace po = boost::program_options;

const string app_title = "Gu grading utility v0.2";
const string usage_comment{R"(Usage: gu [options] <file1.csv> <file2.csv> ...
   gu --check <file1.csv>)"};
const string help_message{R"(Filenames must have the following hyphen-delimited form:

   <group>-<desc>-<year>-<term>.csv

where year is numeric and the other fields are alphanumeric
with optional underscore.

'gu --check' works on one file at a time.)"};

struct App {

  string latex_outfile;
  string check_file;
  vector<string> infiles;
  vector<path> csvfiles; // they have been checked.
  vector<string> text_documents;
  po::options_description desc;
  po::variables_map vm;
  string help();
  App(int, char**);
};

App::App(int argc, char* argv[])
  : desc{"Allowed options"}
{
    desc.add_options()
      ("help,h", "show help message")
      ("check,c", po::value<string>(&check_file), "check for data anomalies.")
      ("sort-ord,s", "sort by ordinal")
      ("sort-final,S", "sort by final score")
      ("drop-first,d", "drop first column")
      ("output,o", po::value<string>(&latex_outfile)->default_value("report.tex"), "latex output filename")
      ("latex,l", "generate latex")
      ("input", po::value<vector<string>>(&infiles), "input file(s)")
    ;
    po::positional_options_description p;
    p.add("input", -1);
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);
}

string App::help()
{
  stringstream temp;
  temp << endl << app_title << endl
       << endl << usage_comment << endl
       << endl << desc << endl << help_message << endl << endl;
  return temp.str();
}

int main(int argc, char* argv[])
{
  try {
    Geometry g(3, 2, 1);
    App app(argc, argv);

    if (argc < 2 || app.vm.count("help")) {
      cout << app.help();
      return 1;
    }

    for (auto u : app.infiles) {
      path p(u);
      if (p.extension().string() != ".csv") {
        cout << "wanted extension .csv, got: " << p << endl;
        return 1;
      } else if (!is_valid_filename(p.stem().string())) {
        cout << "malformed csv file name: " << p << endl;
        return 1;
      } else if (!exists(p)) {
        cout << "No such file: " << p << endl;
        return 1;
      }
    app.csvfiles.push_back(p);
    }

    for (auto itp = app.csvfiles.begin(); itp != app.csvfiles.end(); ++itp)
    {

      stringstream report;

      ifstream fs;
      Document_parser dp;
      fs.open((*itp).string());
      fs >> dp;
      fs.close();
      Table<std::string> table{dp.get_document()};
      if (app.vm.count("drop-first"))
        for (auto& u : table) {
          assert(!u.empty());
          u.erase(u.begin());
      }

      Row<std::string> header(table.front());
      for (auto& h : header)
        if (h == "ordinal")
          h = "#";
        else
          upcase_string(h);
      header.push_back("TOTAL");
      header.push_back("SCORE");

      Row<std::string> norm(table.at(g.norm_row_index).begin()
                              + g.student_info_columns,
                            table.at(g.norm_row_index).end());

      Row<std::string> subheader(g.student_info_columns, " ");
      for (auto u : norm)
        subheader.push_back("/" + u);
      subheader.push_back(" ");
      subheader.push_back("/100");

      Table<std::string> student_record(table.begin()
                                          + g.num_header_rows, table.end());

      Row<unsigned int> test_score_acc(table.front().size(), 0);
      double final_score_acc{0.0};
      Row<unsigned int> test_score_sample_size_acc(table.front().size(), 0);
      unsigned int final_score_sample_size{0};
      Table<std::string> student_result;
      for (auto u : student_record)
      {
        unsigned int score_total{0};
        unsigned int norm_total{0};
        Row<std::string>::const_iterator n;
        Row<std::string>::const_iterator s;
        Row<unsigned int>::iterator tsa; // test_score_acc iterator;
        Row<unsigned int>::iterator tssa; // test score sample size iterator
        for (n = norm.begin(),
             s = u.begin() + g.student_info_columns,
             tsa = test_score_acc.begin(),
             tssa = test_score_sample_size_acc.begin();
             n != norm.end()
               && s != u.end()
               && tsa != test_score_acc.end()
               && tssa != test_score_sample_size_acc.end();
             ++n, ++s, ++tsa, ++tssa)
        {
          if (!is_na(*s)) {
            if (is_integer(*s)) {
              norm_total += string_to_uint(*n);
              score_total += string_to_uint(*s);
              *tsa += string_to_uint(*s);
              ++(*tssa);
            } else {
              cout << "error: non-integer score cell." << endl;
              return 1;
            }
          }
        }

        if (all_scores_NA(u, g.student_info_columns)) {
          student_result.push_back(Row<std::string>({"NA", "NA"}));
        } else {
          double final = 100.0 * (double)score_total / (double)norm_total;
          final_score_acc += final;
          ++final_score_sample_size;
          std::string rational_res = std::to_string(score_total)
                                     + "/"
                                     + std::to_string(norm_total);
          std::string real_res = double_to_string(final);

          if (final < 50.0)
            student_result.push_back(Row<std::string>({rational_res, real_res, "*"}));
          else
            student_result.push_back(Row<std::string>({rational_res, real_res, " "}));
        }
      }

      Sheet body = Sheet(student_record) + Sheet(student_result);
      if (app.vm.count("sort-ord"))
        body.sort_by_column(0, Sort_type::numeric_asc);
      else if (app.vm.count("sort-final"))
        body.sort_by_column(body.n_cols() - 2, Sort_type::numeric_desc);

      Sheet top_blank = Sheet(1, 1, " ", Format::text_right)
                        + Sheet(1,
                                g.student_info_columns - 1,
                                " ",
                                Format::text_left)
                        + Sheet(1,
                                body.n_cols() - g.student_info_columns,
                                " ",
                                Format::text_right);

      Row<std::string> stats(g.student_info_columns, " ");
      Row<std::string>::const_iterator n;
      Row<unsigned int>::const_iterator tsa; // test_score_acc iterator;
      Row<unsigned int>::const_iterator tssa; // test score sample size iterator
      for (tsa = test_score_acc.begin(),
           tssa = test_score_sample_size_acc.begin(),
           n = norm.begin();
           tsa != test_score_acc.end()
               && tssa != test_score_sample_size_acc.end()
               && n != norm.end();
           ++tsa, ++tssa, ++n)
      {
        double avg = 100.0 * (double)(*tsa) / (double)(*tssa * string_to_uint(*n));
        stats.push_back(double_to_string(avg));
      }
      stats.push_back(" ");
      double final = final_score_acc / (double)final_score_sample_size;
      stats.push_back(double_to_string(final));

      report << make_meta_data_string((*itp).stem().string()) << endl;
      report << (top_blank * Sheet(header)
                           * Sheet(subheader)
                           * top_blank
                           * body
                           * top_blank
                           * Sheet(stats)
                           ).to_string() << endl;

      app.text_documents.push_back(report.str());
    }

    if (app.vm.count("latex")) {
      ofstream os;
      os.open(app.latex_outfile, ios_base::out);
      os << make_latex_doc(app.text_documents);
      os.close();
    } else {
      for (auto u : app.text_documents)
        cout << u << endl;
    }

  } catch(exception& e) {
    cerr << "error: " << e.what() << endl;
    return 1;
  }

  return 0;
}

