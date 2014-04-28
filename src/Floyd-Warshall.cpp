#ifndef FLOYD_WARSHALL_CPP
#define FLOYD_WARSHALL_CPP

#include "Floyd-Warshall.hpp"

using namespace std;

FloydWarshall::FloydWarshall(int n)
{
	numberOfNodes = n;
	for(int i=0; i<numberOfNodes; ++i)
	{
		vector < double > temp( numberOfNodes );
		allPairShortestPath.push_back( temp );
	}
}

int
FloydWarshall::run( vector < vector < double > > tempMatrix ) // O(n^3) /SadFace
{
	if(tempMatrix.size() != numberOfNodes)
	{
		cerr<< " FloydWarshall : I seem to have been created using the default constructor. I don't like that. Please initialize all my member variables and then we can talk!";
		exit( 0 );
	}
	allPairShortestPath = tempMatrix;
	int i = 0, j = 0, k = 0;
	for( k = 0; k<numberOfNodes; ++k)
		for( i = 0; i<numberOfNodes; ++i)
			for( j = 0; j<numberOfNodes; ++j)
				allPairShortestPath[ i ][ j ] = min( allPairShortestPath[ i ][ j ], allPairShortestPath[ i ][ k ] + allPairShortestPath[ k ][ j ] );
	for( i=0; i<numberOfNodes; ++i)
		allPairShortestPath[i][i] = 0;
	return( 0 );
}

#endif