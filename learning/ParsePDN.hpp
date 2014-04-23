#ifndef PARSEPDN_HPP
#define PARSEPDN_HPP
/**
 * @file /home/ryan/uml/robotics/checkers/learning/ParsePDN.hpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Apr 22, 2014
 */
#include <grammar/grammar.hpp>
#include <string>

grammar::Parser&& parse_pdn(Board &b) {
  using namespace std;
  using namespace grammar;

  Parser p;

  RunVerbose<Branch>::run_verbose();

  DefineGrammar pdn;
  pdn = pdn.label("top")
    .branch( re("[^[]*\\[")
	     .branch(re("Result \"1-0\"]").thunk([&]() { cout << "Play black" << endl; }).go("top")
	    
		     , re("Result \"0-1\"]").thunk([&]() { cout << "Play white" << endl; })
		     
		     , re("Result \"1/2-1/2\"").thunk([&] { cout << "Skip game" << endl; })

		     , re("\\[.*\\]").go("top")

		     , re(".*").go("top")
		     ).go("top")

	     , re("\\d*\\. (\\d*)[-x](\\d*)").on_match([&](boost::smatch& match) {
		 cout << stoi(match[1]) << "-" << stoi(match[2]) << endl;
	       }).go("top")

	     , re(".*").go("top")
	     ).go("top");

	     
  pdn.print();

  p.sink(pdn);
  return move(p);
}

#endif
