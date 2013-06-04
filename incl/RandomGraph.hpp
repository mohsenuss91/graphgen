#ifndef RANDOMGRAPH_HPP
#define RANDOMGRAPH_HPP

#include <iostream>
#include <boost/lexical_cast.hpp>
#include "GraphIR.hpp"
#include <iomanip>


using namespace std;

class RandomGraph
{
	public:
		int64_t numberOfNodes, numberOfLevels, inDegree, outDegree;
		double alpha, beta, gamma, CCR;
		GraphIR randomGraph;

		RandomGraph();
		RandomGraph(int64_t n, int64_t i, int64_t o, double c, double a, double b, double g);		
		int run();
};

#endif
