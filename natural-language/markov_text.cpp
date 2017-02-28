/*

Markov nonsense generating utility.

Version 2.

*/

#include <iostream>
#include <fstream>
#include <array>
#include <sstream>
#include <deque>
#include <assert.h>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

namespace bf = boost::filesystem;
namespace bpo = boost::program_options;

std::random_device ran_dev;
std::mt19937 random_generator(ran_dev());

/*
Set up character set. 

             abcdefghijklmnopqrstuvwxyz . - , ' space

We record frequencies of  a to z, with space, hyphen, 
apostrophe, comma and period.
*/

const int character_set_size = 31;
const std::array<char,character_set_size> character_set =
  {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
   'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
   'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
   'y', 'z', '.', '-', ',', '\'', ' '};

/*
Trie node:

    frequency
    [array of pointers to other nodes indexed by a ... space]\
*/

struct Node
{
  Node() {
    for (auto & u : next) u = nullptr;
  }
 
  Node *& get_next(char c) {
    assert((c >= 'a' && c <= 'z') || c == '.' || 
           c == '-' || c == ',' || c == '\'' || c == ' '); 
    int index;
    switch (c)
      {
      case '.': index = 26; break;
      case '-': index = 27; break;
      case ',': index = 28; break;
      case '\'': index = 29; break;
      case ' ': index = 30; break;
      default: index = c - 97; break;
      }
    return next[index]; 
  }

  // predict what the next node will be. Return char associated with it.
  char predict() {
    std::array<int, character_set_size> weights;
    for (int k = 0; k < next.size(); ++k) {
      if (next[k] != nullptr) {
        weights[k] = (next[k])->frequency;
      } else {
        weights[k] = 0;
      }
    }

    // Don't continue if all entries are null or 0.
    // int sum = 0;
    //for (auto w : weights) {
    //  sum += w;
    // }
    //assert(sum > 0);

    std::discrete_distribution<> dist(weights.begin(), weights.end());
    return index_to_char(dist(random_generator));
  }

  int frequency{0};
  std::array<Node *, character_set_size> next;
private:
  static char index_to_char(int index)
  {
    assert((index >= 0) && (index <= character_set_size - 1));
    return character_set[index];
  }
};

void delete_trie (Node *root) {
  std::cout << "----------------------" << std::endl;
  std::cout << "cleaning up memory..." << std::endl;
  unsigned long node_counter = 0;
  unsigned long node_size = sizeof(*root);
  std::stack<Node *> cleanup_stack;
  cleanup_stack.push(root);
  while (!cleanup_stack.empty()) {
    Node *top = cleanup_stack.top();
    cleanup_stack.pop();
    for (auto it = top->next.begin(); it != top->next.end(); ++it) { 
      if (*it != nullptr){
        cleanup_stack.push(*it);
      }      
    }
    delete top;
    ++node_counter;
  }
  std::cout.imbue(std::locale(""));
  std::cout << "Trie size:  " << node_counter << " nodes"
            <<  "  " << node_counter*node_size << " bytes" 
            << std::endl;
}
  
// For scanning text files.
bool is_alpha(const char c) 
{ 
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); 
}

/*

  Builds trie by making new nodes and connecting them.

*/

struct Trie_builder {
  Trie_builder(Node *root, int N) : root(root), current(root), N(N) {}
  void operator()(char c) {
    key.push_back(c);
    if (key.size() == N) {
      current = root;
      for (auto u : key) {
        if (current->get_next(u) == nullptr) {
          Node * new_node = new Node;
          ++(new_node->frequency);
          current->get_next(u) = new_node;
          current = new_node;
        } else {
          ++((current->get_next(u))->frequency);
          current = current->get_next(u);
        }
      }
      key.pop_front();
    }
  }

private:
  Node *current;
  Node *root;
  std::deque<char> key{};
  int inserted{0};
  int N;
};

/*
  
Text generator. Give it the root of a trie and the
history size and it will merrily generate text forever.

*/

struct Text_generator {
  Text_generator(Node *root, int history) : root(root) {
    // Initialize the key. We will always start text generation 
    // with a space, since that's what precedes a word. Gives us 
    // the best chance of starting  with something sensible. 
    // But we wont push it on to the key since we don't actually 
    // want that space in our prose. We push 'history' number of 
    // characters onto the key. History <= ngram_size - 1.
    if (history > 0) {
      char p = root->get_next(' ')->predict();
      key.push_back(p);
      current = root->get_next(p);
      while (key.size() < history) {
        char choice = current->predict();
        current = current->get_next(choice);
        key.push_back(choice);
      }
    }
  }
  char operator()() {
    current = root;
    for (auto c : key) {
      current = current->get_next(c);
    }
    char predicted = current->predict();
    key.push_back(predicted);
    key.pop_front();
    return predicted;
  }
  std::deque<char> key{};
  Node *root;
  Node *current;
};

std::stringstream prettify(std::string &, const int); 

/*

App struct handles command line options, help etc.

*/

struct App {
  App(int, char**);
  int ngram_size;
  int text_length;
  static const std::string app_title;
  static const std::string usage_comment;
  static const std::string help_message;
  std::vector<std::string> infiles;
  bpo::options_description description;
  bpo::variables_map vm;
  std::string help();
};

/*

Main.

*/

int main(int argc, char* argv[])
{
  App app(argc, argv);    
  
  // Called with no args or with help.
  if (argc < 2 || app.vm.count("help")) {
    std::cout << app.help();
    return 0;
  }
  
  // Infiles should exist.
  for (auto f : app.infiles) {
    bf::path p(f);
    if (!bf::exists(p)) {
      std::cout << "No such file: " << p << std::endl;
      return 1;
    }
  }

  Node *root = new Node;
  Trie_builder build = Trie_builder(root, app.ngram_size);
 
  for (const auto f : app.infiles) {
    std::ifstream in(f);
    bool in_word{false}; // Are we in a word?
    char c;
    while (in.get(c)) {
      
      // Whitespaces until alpha char are replaced with single space.
      if (std::isspace(c)) {
        build(' ');
        in_word = false;
        // Eat characters until alpha is encountered.
        while (in.get(c)) {
          if (is_alpha(c)) 
            break;
        }
      }
      
      // Sentence-ending punctuation with anything following up
      // to next alpha char is replace by one period and one space.
      if (c == '.' || c == '!' || c == '?') {
        build('.');
        build(' ');
        in_word = false;
        // Eat characters until alpha is encountered.
        while (in.get(c)) {
          if (is_alpha(c)) 
            break;
        }
      }
        
      // Internal punctuation + anything up to next alpha char
      // is replaced with one comma and one space.
      if (c == ',' || c == ':' || c == ';') {
        build(',');
        build(' ');
        in_word = false;
        while (in.get(c)) {
          if (is_alpha(c)) 
            break;
        }
      }

      // c must be an alpha char if we got to here.
      // We still check that this is so.
      if (is_alpha(c)) {
        in_word = true;
        c = std::tolower(c);
        build(c);
      }

      // Process in-word ' and -
      if ((c == '\'' || c == '-') && in_word) {
        char q;
        in.get(q);
        if (is_alpha(q)) {
          build(c);
        }
        in.unget();
      }

    }
  }
  
  // Generate text, prettify, cleanup.

  Text_generator generate = Text_generator(root, app.ngram_size-1);
  std::string prose;
  while (prose.length() < app.text_length) { 
    prose += generate();
  }
  
  std::cout << std::endl << prettify(prose, 70).str() << std::endl;

  delete_trie(root);
  std::cout << "ngram size: " << app.ngram_size << std::endl;
  return 0;

} // end main.

App::App(int argc, char* argv[])
{
  description.add_options()
    ("help,h",
     "show help message")

    ("ngram_size,n",
     bpo::value<int>(&ngram_size)->default_value(4),
     "ngram size > 0")

    ("text_length,l",
     bpo::value<int>(&text_length)->default_value(1000),
     "text length")
    
    ("input",
     bpo::value<std::vector<std::string>>(&infiles),
     "input files");
  
  bpo::positional_options_description p;
  p.add("input", -1);
  bpo::store(bpo::command_line_parser(argc, argv)
             .options(description)
             .positional(p)
             .run(), vm);
  bpo::notify(vm);
}

std::string App::help()
{
  return app_title + '\n' + usage_comment + '\n' + help_message + '\n';
}

const std::string App::app_title = "\n           MARKOV NONSENSE UTILITY v2";
const std::string App::usage_comment{R"(
Usage: markov -n<ngram-size> -l<text-length>  <file1.txt> <file2.txt> ...
)"};
const std::string App::help_message{R"(You can process any number of text files:

   $ markov -n 6 -l 3000 lovecraft.txt ayn_rand.txt marcel_proust.txt

-n<ngram_size> The smallest ngram size is 1, which means no history. 
               Default is 4.

-l<text_length> How much text do you want to generate? 
                Defaults to 1000 characters.

The first word of the final prose is discarded, and if the text does 
not end in a complete sentence (it probably won't), an ellipsis is added.
)"};

/*
Text Prettifier.
The prettifier eats the original text.
*/

std::stringstream prettify(std::string & text, const int line_width) 
{ 
  // Tokenize text and applying some rules for capitalization.
  std::stringstream prettified_prose;
  size_t position = 0;
  std::string token;
  char delimiter = ' ';
  bool in_sentence = false;
  int line_length = 0;
  
  // Throw out the first token.
  position = text.find(delimiter);
  text.erase(0, position + 1);

  // Scan rest of prose.
  while ((position = text.find(delimiter)) != std::string::npos) {
    token = text.substr(0, position);
    
    // Handle special cases of I pronoun.
    if (token == "i") {
      token = "I";
    } else if (token == "i,") {
      token = "I,";
    } else if (token == "i.") {
      token = "I.";
    }

    // Capitalize first word in sentence.
    if (!in_sentence) {
      token[0] = std::toupper(token[0]);
    }

    // Format sentences to fit in screen width.
    if (token[token.size()-1] == '.') {
      if (line_length + token.length() + 1 >= line_width) {
	prettified_prose << std::endl << token << std::endl << std::endl;
	in_sentence = false;
	line_length = 0;
      } else {
	prettified_prose << token << std::endl << std::endl;
	line_length = 0;
	in_sentence = false;
      }
    } else {
      if (line_length + token.length() + 1 >= line_width) {
	prettified_prose << std::endl << token << " ";
	line_length = token.length() + 1;
	in_sentence = true;
      } else {
	prettified_prose << token << " ";
	line_length += token.length() + 1;
	in_sentence = true;
      }
    }
    // . and , are treated as part of words, so the delimiter
    // is strictly only ' '. Therefore we add 1 to position.
    text.erase(0, position + 1);
  }
   
  // If last sentence is incomplete, add an ellipsis.
  if (in_sentence) { 
    prettified_prose << "..." << std::endl;
  }
  
  return prettified_prose;
}
