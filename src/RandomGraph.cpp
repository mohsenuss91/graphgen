#ifndef RANDOMGRAPH_CPP
#define RANDOMGRAPH_CPP

#include "RandomGraph.hpp"
#define NUM_CONSTRAINTS 2

RandomGraph :: RandomGraph()
{
	cout<<"Improper constructor call. Call with required graph parameters";
	exit( 0 );
}

RandomGraph :: RandomGraph(int64_t n, int64_t i, int64_t o, double c, double a, double b, double g)
{
	numberOfNodes = n;
	inDegree = i;
	outDegree = o;
	CCR = c;
	alpha = a;
	beta = b;
	gamma = g;

	for(int i=0; i<n; ++i)
	{
		vector < double > tempVector( n, 0 );
		randomGraph.connectivityMatrix.push_back( tempVector );
	}
	randomGraph.singleConstraintVwgt.resize( n, 0 );
	randomGraph.vwgt.resize( n * NUM_CONSTRAINTS, 0 );
}

int64_t // Returns random number in range [a,b]
randomInRange( int64_t a, int64_t b )
{
	return( (b-a == 0)? b : a + rand()%(b-a+1) );
}

int
RandomGraph :: run()
{
	srand( time(NULL) );
	int i=0, level=0, j=0;
	int64_t numberOfEdges = 0;
	int64_t nodesAllocated = 0, height, avgWidth;
	double off = 0.3;
	vector < vector < int64_t > > nodesInList;

	cout<<fixed;

	// Calculate height and width of the graph
	height = ceil( sqrt( numberOfNodes )/alpha );
	avgWidth = ceil( sqrt( numberOfNodes ) * alpha );
	for(level=0; ; level++)
	{
		vector < int64_t > buffer( 0 );
		nodesInList.push_back( buffer );
		if( numberOfNodes - nodesAllocated < avgWidth )
		{
			// In the last level, according to height, see how many nodesToBeAllocated
			// If there are more than wat can fit in one level, add one to height and continue
			while(numberOfNodes - nodesAllocated > 0)
			{
				nodesInList[level].push_back(nodesAllocated);
				++nodesAllocated;
			}
			++level;
			height = level;
			break;
		}
		else
		{
			// Assign one node to starting level.
			if( level == 0 )
			{
				nodesInList[level].push_back( 0 );
				++nodesAllocated;
			}
			else
			{
				// Assign between 1 and 2*avgWidth nodes in level i.
				int offset = randomInRange( avgWidth*(1-off), avgWidth*(1+off));
				for(i=0; i<offset; ++i)
				{
					nodesInList[level].push_back(nodesAllocated);
					++nodesAllocated;
					if(nodesAllocated == numberOfNodes)
						goto jumpOut;
				}
			}
		}
	}
	jumpOut:
	cout<<"\nO/P: nodesInList.size="<<nodesInList.size()<<"level="<<level<<" height="<<height<<" width="<<avgWidth<<"nodesAllocated="<<nodesAllocated<<endl;

	cout<<"\nHeight before pruning : "<<height;
	vector < vector < int64_t > >::iterator it1;
	for(it1=nodesInList.begin(); it1!=nodesInList.end(); ++it1)
		if((*it1).size() == 0)
		{
			cout<<"***yoink***";
			nodesInList.erase(it1);
			it1--;
		}
	height = nodesInList.size();
	cout<<"\nHeight after pruning : "<<height;

	for(i=0; i<height; ++i)
	{
		cout<<"Level "<<i+1<<" : ";
		vector < int64_t >::iterator it;
		for(it = nodesInList[i].begin(); it!= nodesInList[i].end(); ++it)
			cout<<(*it)<<"-";
		cout<<"\n";
	}


	// Next loop to form connections between nodes in the different levels.
	// In each iteration, go through all nodes in level i.
	for(i=0; i<height-1; ++i)
	{
		vector < int64_t >::iterator it;
		for(it = nodesInList[i].begin(); it!= nodesInList[i].end(); ++it)
		{
			int64_t numberOfOutDegree = randomInRange( outDegree*(1-off), outDegree*(1+off) );
			// For each such node j, make a connection to k where k's level is >= j's level.
			// Choose 1 to outDegree such k's for each j.
			for(j=0; j<numberOfOutDegree; ++j)
			{
				int64_t t1 = randomInRange(i+1, height-1);
/*
				int64_t t1;
				do
				{
					t1 = randomInRange(i+1, height-1);
				}while(nodesInList[t1].size()<1);
*/
				int64_t t2 = randomInRange(0, nodesInList[t1].size()-1);
				if(randomGraph.connectivityMatrix[(*it)][nodesInList[t1][t2]] != 1 )
				{
					randomGraph.connectivityMatrix[(*it)][nodesInList[t1][t2]] = 1;
//					randomGraph.connectivityMatrix[nodesInList[t1][t2]][(*it)] = 1;
					numberOfEdges++;
				}
			}			
		}
	}
	
	for(i=1; i<numberOfNodes; ++i)
	{
		// Final pass to clean up communication.
		// Iterate once over to figure out if there are any straddlers, i.e. nodes with 0 inDegree.
		// If there are any such nodes called i, make one connection to it from some random node j, where level(j) < level(i);
		int flip = 0;
		for(j=0; j<numberOfNodes;++j)
			if(randomGraph.connectivityMatrix[j][i] != 0)
				flip = 1;
		if( flip == 0 )
		{
			randomGraph.connectivityMatrix[randomInRange(0, i-1)][i] = 1;
			numberOfEdges++;
		}
	}

	pair< double, double > higherBandMIPS, lowerBandMIPS, higherBandVEC, lowerBandVEC, middleBand;
	higherBandVEC = make_pair(1000, 9999);
	lowerBandVEC = make_pair(128, 512);
	higherBandMIPS = make_pair(100, 2400);
	lowerBandMIPS = make_pair(50, 80);
	middleBand = make_pair(((lowerBandMIPS.first+lowerBandMIPS.second)/2+(higherBandMIPS.first+higherBandMIPS.second)/2)/2,((lowerBandVEC.first+lowerBandVEC.second)/2+(higherBandVEC.first+higherBandVEC.second)/2)/2);
	double delta = 0.4; // Radius of uniform constraint draw circle
	// Assign computation values based on the G(r)eeky constants. Get it? G(r)eeky? :-D
	for(i=0; i<numberOfNodes; ++i)
	{
		for(j=0; j<NUM_CONSTRAINTS; ++j)
		{
//			randomGraph.vwgt[ i*NUM_CONSTRAINTS + j ] = 1;
			if( randomInRange(0, 100) > gamma )
			{
				// This node has to be a skewed node : )
				if( randomInRange(0, 100) > beta )
				{
					// This node has to be made a for intensive node
					if( j == 0 )
						randomGraph.vwgt[ i*NUM_CONSTRAINTS + j ] = randomInRange( higherBandMIPS.first, higherBandMIPS.second );
					else if( j == 1 )
						randomGraph.vwgt[ i*NUM_CONSTRAINTS + j ] = randomInRange( lowerBandVEC.first, lowerBandVEC.second );
					else
						randomGraph.vwgt[ i*NUM_CONSTRAINTS + j ] = 1;
				}
				else
				{
					// This node has to be made a par intensive node
					if( j == 0 )
						randomGraph.vwgt[ i*NUM_CONSTRAINTS + j ] = randomInRange( lowerBandMIPS.first, lowerBandMIPS.second );
					else if( j == 1 )
						randomGraph.vwgt[ i*NUM_CONSTRAINTS + j ] = randomInRange( higherBandVEC.first, higherBandVEC.second );
					else
						randomGraph.vwgt[ i*NUM_CONSTRAINTS + j ] = 1;
				}
			}
			else
			{
				// This node has to be a non-skewed node
					// This node has to be made a for intensive node
					if( j == 0 )
						randomGraph.vwgt[ i*NUM_CONSTRAINTS + j ] = randomInRange( (1-delta)*lowerBandMIPS.first, (1+delta)*lowerBandMIPS.first );
					else if( j == 1 )
						randomGraph.vwgt[ i*NUM_CONSTRAINTS + j ] = randomInRange( (1-delta)*lowerBandVEC.second, (1+delta)*lowerBandVEC.second );
					else
						randomGraph.vwgt[ i*NUM_CONSTRAINTS + j ] = 1;
			}
		}
	}

	// Assign Communication values based on CCR
	for(i=0; i<numberOfNodes; ++i)
		for(j=0; j<numberOfNodes;++j)
			if(randomGraph.connectivityMatrix[i][j] != 0 )
				randomGraph.connectivityMatrix[i][j] = ceil(randomGraph.vwgt[ i*NUM_CONSTRAINTS + 0 ] * randomGraph.vwgt[ i*NUM_CONSTRAINTS + 1 ] * CCR );

	
	cout<<"\nNumber of Edges = "<<numberOfEdges;
	string filename("app-");
	ostringstream oss;
	oss<<filename<<numberOfNodes<<"-"<<outDegree<<"-"<<CCR<<"-"<<alpha<<"-"<<beta<<"-"<<gamma << ".grf";
	filename = oss.str();
	randomGraph.createMETISFile(numberOfNodes, numberOfEdges, NUM_CONSTRAINTS, filename);
}

int
main(int argc, char *argv[])
{
	if(argc != 8 )
	{
		cout<<"\nImproper Usage. Arguments are as follows : ";
		cout<<"\n1. (n) Number of Nodes \\in Z^+";
		cout<<"\n2. (i) Indegree \\in Z^+ && i<n (ideally i<<n; currently not taken into account)";
		cout<<"\n3. (o) Outdegree \\in Z^+ && o<n (ideally o<<n)";
		cout<<"\n4. (c) Computation-to-Communication Ratio(CCR) \\in Q^+ (ideally c<<1)";
		cout<<"\n5. (a) Alpha \\in Q^+\n\tSmaller 'a' gives tall and skinny graphs, while larger 'a' gives fat and short graphs";
		cout<<"\n6. (b) Beta \\in Q^+ && 0<=b<100\n\tBeta determines how serial/parallel the graph is going to be. Smaller values of beta makes the actors 'for' loop intensive and larger values of beta makes the graph 'par' loop intensive.";
		cout<<"\n7. (g) Gamma \\in Q^+ && 0<=g<=100\n\tGamma decides how computation is spread across the graph. Smaller values of gamma gives uniformly distributed graphs while larger values give skewed graphs.\n";
		exit( 0 );
	}
	RandomGraph g( atof(argv[1]), atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]), atof(argv[6]), atof(argv[7]) );
	g.run();
}

#endif
