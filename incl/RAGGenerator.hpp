#ifndef _RANDOMGRAPH_HPP
#define _RANDOMGRAPH_HPP

#include <iostream>
#include <boost/lexical_cast.hpp>
#include <iomanip>

#include "GraphIR.hpp"
#include "Commons.hpp"

using namespace std;

class RAGGenerator
{
	public:
		int64_t numberOfNodes, numberOfLevels, inDegree, outDegree, level, height, avgWidth;
		double alpha, beta, gamma, CCR;
		GraphIR randomGraph;
		string filename;
		vector < vector < int64_t > > nodesInList;

		RAGGenerator();
		RAGGenerator(int64_t n, int64_t i, int64_t o, double c, double a, double b, double g);		
		int printDOTFile();
		int run();
};

#endif