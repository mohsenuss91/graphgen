#ifndef GRAPHIR_HPP
#define GRAPHIR_HPP

#include <cstdlib>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

class GraphIR
{
	public:
	int nvtxs,					// Number of vertices in G(V, E)
	    ncon;					// Number of connections in G(V, E)
	std::vector < int64_t > vwgt; 	// Constraint vector.
	std::vector < int64_t > singleConstraintVwgt; 	// Constraint vector.
	std::vector < std::vector < double > > connectivityMatrix;
	std::vector < int64_t > IDMap;
	GraphIR operator = (GraphIR);

	int parseMETIS(std::string fileName);
	int parseXML(std::string fileName);
	int createMETISFile( int64_t numberOfNodes, int64_t numberOfEdges, int ncon, std::string fileName); 
	
	GraphIR();
	~GraphIR();
};

#endif
