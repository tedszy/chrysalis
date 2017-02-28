#ifndef TABLE_H
#define TABLE_H

#include <vector>

template<typename T>
using Row = std::vector<T>;

template<typename T>
using Table = std::vector<Row<T>>;


#endif
