#ifndef OBJPARSER_4620
#define OBJPARSER_4620

#include <vector> 
#include <iostream>
#include "Triangle.hpp"

std::vector<Triangle> object_parser(const std::string& file_name);

std::ostream &operator<<(std::ostream &os, const Triangle &v);

#endif 