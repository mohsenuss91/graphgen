#ifndef _RANDOMGRAPH_CPP
#define _RANDOMGRAPH_CPP

#include "RAGGenerator.hpp"
#define _NUM_CONSTRAINTS 1

RAGGenerator :: RAGGenerator()
{
	cout<<"Improper constructor call. Call with required graph parameters";
	exit( 0 );
}

RAGGenerator :: RAGGenerator(int64_t n, int64_t i, int64_t o, double c, double a, double b, double g)
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
		ConnectivityMatrix tempVector;
		randomGraph.connectivityMatrix.push_back( tempVector );
	}
	randomGraph.singleConstraintVwgt.resize( n, 0 );
	randomGraph.vwgt.resize( n * _NUM_CONSTRAINTS, 0 );
}

int64_t // Returns random number in range [a,b]
randomInRange( int64_t a, int64_t b )
{
	return( (b-a == 0)? b : a + rand()%(b-a+1) );
}

int
RAGGenerator :: printDOTFile()
{
	filename = string("app-");
	ostringstream oss;
	oss<<filename<<numberOfNodes<<"-"<<outDegree<<"-"<<CCR<<"-"<<alpha<<"-"<<beta<<"-"<<gamma << ".dot";
	filename = oss.str();
	fstream file( filename.c_str(), fstream::out );
	file<<"digraph{\n";
	file<<"\tsplines=spline\n\tsplines=true\n\tarrowType=open\n";
	file<<"\n// Printing vertex IDs and attributes\n";
	for(int64_t i=0; i<height; ++i)
	{
		for(int64_t j=0; j<nodesInList[i].size(); ++j)
		{
			if( i==0 || i==height-1)
				file<<"\t"<<(nodesInList[i][j] + 1)<<"[label=\""<<(nodesInList[i][j] + 1)<<"("<<randomGraph.singleConstraintVwgt[ nodesInList[i][j] ]<<")\",shape=rounded]\n";
			else
				file<<"\t"<<(nodesInList[i][j] + 1)<<"[label=\""<<(nodesInList[i][j] + 1)<<"("<<randomGraph.singleConstraintVwgt[ nodesInList[i][j] ]<<")\"]\n";
		}

	}
	file<<"\n// Printing edge IDs and attributes\n";
	for(int64_t i=0; i<numberOfNodes; ++i)
	{
		int64_t numberOfNeighbours = randomGraph.connectivityMatrix[i].size();
		for(int64_t j=0; j<numberOfNeighbours; ++j)
		{
			file<<"\t"<<(i + 1)<<" -> "<<(randomGraph.connectivityMatrix[i].list[j].first + 1);
			file<<" [label=\""<< randomGraph.connectivityMatrix[i].list[j].second <<"\"]\n";
		}
	}
	file<<"}\n";
	file.close();
}

int
RAGGenerator :: run()
{
	srand( time(NULL) );
	int i=0, j=0;
	level=0;
	int64_t numberOfEdges = 0;
	int64_t nodesAllocated = 0;
	double off = 0.3;

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

	double weightOfDAG = randomInRange( pow(10,4), pow(10,5) );
	// Assign vertex weights
	for(i=0; i<numberOfNodes; ++i)
	{
		randomGraph.singleConstraintVwgt[ i ] = 1;
		for(j=0; j<_NUM_CONSTRAINTS; ++j)
		{
//			randomGraph.vwgt[ i*NUM_CONSTRAINTS + j ] = 1;
			randomGraph.vwgt[ i*_NUM_CONSTRAINTS + j ] = randomInRange( 0, 2*weightOfDAG );
			randomGraph.singleConstraintVwgt[ i ] *= randomGraph.vwgt[ i*_NUM_CONSTRAINTS + j ];
		}
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
				int64_t t2 = randomInRange(0, nodesInList[t1].size()-1);
				
				pair < int64_t, double > temp;
				temp.first = nodesInList[t1][t2];
				temp.second = ceil( randomGraph.singleConstraintVwgt[ (*it) ] * CCR );
				randomGraph.connectivityMatrix[(*it)].list.push_back( temp );
				numberOfEdges++;
			}			
		}
	}

	// Create transpose adjacency lists to get parents of all nodes
	GraphIR randomGraphTranspose;
	for(i=0; i<numberOfNodes; ++i)
	{
		ConnectivityMatrix tempConnectivityMatrix;
		randomGraphTranspose.connectivityMatrix.push_back(tempConnectivityMatrix);
	}
	for(i=0; i<numberOfNodes; ++i)
	{
		for(int k=0; k<randomGraph.connectivityMatrix[i].size(); ++k)
		{
			pair < int64_t, double > tempPair;
			tempPair.first = i;
			tempPair.second = randomGraph.connectivityMatrix[i].list[k].second;
			randomGraphTranspose.connectivityMatrix[ randomGraph.connectivityMatrix[i].list[k].first ].list.push_back(tempPair);
		}
	}
	
	for(i=1; i<height; ++i)
	{
		// Final pass to clean up communication.
		// Iterate once over to figure out if there are any straddlers, i.e. nodes with 0 inDegree.
		// If there are any such nodes called i, make one connection to it from some random node j, where level(j) < level(i);
		int flip = 0;
		for(j=0; j<nodesInList[i].size(); ++j)
		{
			int64_t currentNode = nodesInList[i][j];
			if( randomGraphTranspose.connectivityMatrix[ currentNode ].size() == 0 ) // If currentNode has no parent
			{
				int64_t t1 = randomInRange(0, i-1);
				int64_t t2 = randomInRange(0, nodesInList[t1].size()-1);
				
				pair < int64_t, double > temp;
				temp.first = currentNode;
				temp.second = ceil( randomGraph.singleConstraintVwgt[ nodesInList[t1][t2] ] * CCR );
				randomGraph.connectivityMatrix[ nodesInList[t1][t2] ].list.push_back( temp );
				numberOfEdges++;
		
			}
		}
	}
	
	cout<<"\nNumber of Edges = "<<numberOfEdges;
	filename=string("app-");
	ostringstream oss;
	oss<<filename<<numberOfNodes<<"-"<<outDegree<<"-"<<CCR<<"-"<<alpha<<"-"<<beta<<"-"<<gamma << ".grf";
	filename = oss.str();
	randomGraph.createMETISFile(numberOfNodes, numberOfEdges, _NUM_CONSTRAINTS, filename);
	printDOTFile();
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
	RAGGenerator g( atof(argv[1]), atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]), atof(argv[6]), atof(argv[7]) );
	g.run();
}

#endif