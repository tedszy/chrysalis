/*

Discover all anagrams in a corpus of English text.

The text must be organized by lines. We don't consider
hyphenated words across lines. 

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <array>
#include <cstdint> //unit8_t

/*

Rules for words. a, b, c, d are alpha caseless characters.
Hyphen and apostrophe can be part of words.

abcd
abcd-efgh
abcd'efgh

But if - or ' are followed by ' - or non-alpha then the word ends.

A key is a 28 element uint8 array.

For example, the word FROGGIE is associated with the key

a b c d e f g h i j k l m n o p q r s t u v w x y z - '
0 0 0 0 1 1 2 0 1 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0

Any other word associated with this key is an anagram of FROGGIE.
It's added to the vector of words associated with an Anagram_key.

*/

// Use std::array because we want to use them as map keys
// and this seems like a better way than plain C arrays.
typedef std::array<uint8_t, 28> Anagram_key;

bool is_alphabetical(const char ch)
{
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

void make_anagram_key(const std::string word, Anagram_key & key)
{
  for (int i = 0; i < 28; ++i)
    key[i] = 0;
  for (auto ch : word)
    if (ch == '-') 
      ++key[26];
    else if (ch == '\'')
      ++key[27];
    else
      ++key[ch-97];
}

int main(int argv, char* argc[]) 
{

  std::map<Anagram_key,std::vector<std::string>> anagram_map;
  std::set<std::string> words;
  std::ifstream in(argc[1]);
  std::string line;

  while (std::getline(in, line)) {

    bool in_word{false};			
    std::string::iterator it = line.begin();
    std::string word;

    // There are many blank lines in the corpus. Skip them.
    if (line.length()==0) continue;

    while (it != line.end()) {
      
      if (!in_word && is_alphabetical(*it)) {
	in_word = true;
	word.clear();
	word += std::tolower(*it);
	++it;
	continue;
      }
      
      if (in_word && is_alphabetical(*it)) {
	word += std::tolower(*it);
	++it;
	continue;
      }
      
      // Current char is - or ' and we are in a word.
      // Peek forward. If peeked char is alphabetical, 
      // then - or ' is part of word.
      if (in_word && (*it == '\'' || *it == '-') && is_alphabetical(*(it+1))) {
	word += std::tolower(*it);
	++it;
	continue;
      }
        
      // We are at the end of the word. 
      // Check if word is in the word set. We don't want to add 
      // duplicate words to the vectors in the anagram map.
      // If not, make the anagram key. 
      // If key is not in map, then add it with new word vector.
      // If key is in map, add word to word vector.
      if (words.find(word) == words.end()) {
	words.insert(word);
	Anagram_key key;
	make_anagram_key(word, key);	
	if (anagram_map.count(key) == 0)
	  anagram_map[key] = std::vector<std::string>{word};
	else  
	  anagram_map[key].push_back(word);
      }
      in_word = false;
      ++it;     
    } 
  }
   
  // Ragged-right formatted display of words.    
  int max_line_width = 80;
  int line_width = 0;
  for (const auto & key_val_pair : anagram_map) {
    if (key_val_pair.second.size() > 1)
      for (auto word : key_val_pair.second) {
	if (word.length() + line_width + 1 > max_line_width) {
	  std::cout << std::endl << word << " ";
	  line_width = word.length() + 1;
	} else {
	  std::cout << word << " ";
	  line_width += word.length() + 1;
	}
      }
  }
  std::cout << std::endl;

}
