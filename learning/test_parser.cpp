
/**
 * @file /home/ryan/uml/robotics/checkers/learning/test_parser.cpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Apr 22, 2014
 */

#include <iostream>
#include <fstream>

#include <grammar/utility.hpp>

#include "./ParsePDN.hpp"

using namespace std;
using namespace grammar;

int main() {
  Board b;
  Parser parse = parse_pdn(b);
  {
    std::ifstream input("./OCA/sample.pdn");
    foreach_line(input, parse);
  }

  cout << "Done." << endl;
  
  return 0;
}
