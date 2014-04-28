#ifndef _FLOYD_WARSHALL_HPP
#define _FLOYD_WARSHALL_HPP

#include <iostream>
#include <vector>
#include <cstdlib>

class FloydWarshall
{
	public :
	int numberOfNodes;
	std::vector < std::vector < double > > allPairShortestPath; // Initially this is the weighted connectivity matrix

	FloydWarshall(int n);
	int run( std::vector < std::vector < double > > tempMatrix ); // O(n^3) /SadFace
};

#endif
