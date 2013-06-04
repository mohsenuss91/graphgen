#ifndef GRAPHIR_CPP
#define GRAPHIR_CPP

#include "GraphIR.hpp"

using namespace std;

	int
GraphIR::parseMETIS(std::string fileName)
{
	const string& delimiters = " ";
	ifstream input_file;
	input_file.open( fileName.c_str() );
	int64_t line_no = 0;
	while( input_file.good() )
	{
		string line;
		vector < int64_t > tokens;
		getline( input_file, line );
		string::size_type lastPos = line.find_first_not_of( delimiters, 0 );
		string::size_type pos = line.find_first_of( delimiters, lastPos );
		while( string::npos != pos || string::npos != lastPos )
		{
			string temp = line.substr( lastPos, pos - lastPos );
			tokens.push_back( atoll ( temp.c_str() ) );
			lastPos = line.find_first_not_of( delimiters, pos );
			pos = line.find_first_of( delimiters, lastPos );
		}
		if( tokens.size() == 0)
			continue;
		else if( line_no == 0 )
		{
			if( tokens.size() != 4 )
			{
				cout << "Error: Input graph file format incorrect" << endl;
				return -1;
			}
			if( tokens[ 2 ] != 11 )
			{
				cout << "Error: Input graph without constraints specified" << endl;
				return -1;
			}
			nvtxs = tokens[ 0 ];
			ncon = tokens[ 3 ];
			line_no++;
			for(int i=0; i<nvtxs; ++i)
			{
				vector < double > tempVector( nvtxs );
				connectivityMatrix.push_back( tempVector );
			}
			singleConstraintVwgt.resize( nvtxs, 0 );
			vwgt.resize( nvtxs * ncon, 0 );
		}
		else
		{
			if( ncon > 1 )
				if( tokens[ 1 ] == 0 && tokens[ 0 ] == 0 )
					singleConstraintVwgt[ line_no - 1 ] = 1;
				else
				{
					if( tokens[ 1 ] == 0 )
						singleConstraintVwgt[ line_no - 1 ] = tokens[ 0 ] * 1;
					else if( tokens[ 0 ] == 0 )
						singleConstraintVwgt[ line_no - 1 ] = tokens[ 1 ] * 1;
					else
						singleConstraintVwgt[ line_no - 1 ] = tokens[ 0 ] * tokens[ 1 ];
				}
			else if( ncon == 1 )
				singleConstraintVwgt[ line_no - 1 ] = tokens[ 0 ];
			for(int i = 0; i<ncon; ++i)
			{
				if(tokens[i]==0)
					tokens[i]=1;
				vwgt[ (line_no - 1) * ncon + i ] = tokens[i];
			}
			for(int i = ncon; i<tokens.size(); i=i+2)
				connectivityMatrix[ line_no - 1 ][ tokens[ i ] - 1 ] = tokens[ i+1 ];
			line_no++;
		}
	}
	input_file.close();
	cout<<"===> parseMETIS : Exiting\n";
}

int
GraphIR :: createMETISFile( int64_t numberOfNodes, int64_t numberOfEdges, int ncon, string fileName )
{
	fstream metis, udmetis;
	int64_t i, j, counter = 0;
	string temp("grf"), temp1("-udag"), temp2;
	string::iterator it;
	for(it=fileName.end()-3; it<fileName.end(); it++, counter++)
		*it = temp[counter];
	temp2 = fileName + temp1;
	metis.open(fileName.c_str(), fstream::out);
	metis<<fixed<<numberOfNodes<<" "<<numberOfEdges<<" 011 "<<ncon;
	for(i=0; i<numberOfNodes; i++)
	{
		metis<<endl;
		for(j=0; j<ncon; ++j)
			metis<<vwgt[i*ncon + j]<<" ";
		for(j=0; j<numberOfNodes; ++j)
			if( connectivityMatrix[i][j] > 0 )
				metis<<j+1<<" "<<(int64_t)connectivityMatrix[i][j]<<" ";
	}
	numberOfEdges = 0;
	for(i=0; i<numberOfNodes; ++i)
	{
		for(j=i+1; j<numberOfNodes; ++j)
		{
			if( connectivityMatrix[i][j] != connectivityMatrix[j][i] )
			{
				if( connectivityMatrix[i][j] == 0)
					connectivityMatrix[i][j] = connectivityMatrix[j][i];
				else if( connectivityMatrix[j][i] == 0)
					connectivityMatrix[j][i] = connectivityMatrix[i][j];
				else
					connectivityMatrix[i][j] = connectivityMatrix[j][i] = connectivityMatrix[i][j] + connectivityMatrix[j][i];
			}
			if( connectivityMatrix[i][j] != 0 )
				++numberOfEdges;
		}
	}

	udmetis.open(temp2.c_str(), fstream::out);
	udmetis<<fixed<<numberOfNodes<<" "<<numberOfEdges<<" 011 "<<ncon;
	for(i=0; i<numberOfNodes; i++)
	{
		udmetis<<endl;
		for(j=0; j<ncon; ++j)
			udmetis<<vwgt[i*ncon + j]<<" ";
		for(j=0; j<numberOfNodes; ++j)
			if( connectivityMatrix[i][j] > 0 )
				udmetis<<j+1<<" "<<(int64_t)connectivityMatrix[i][j]<<" ";
	}
	udmetis.close();

	metis.close();
}

int
GraphIR :: parseXML( std::string fileName )
{
	enum
	{
		_NODE = 1,
		_EDGE = 1,
		_NEND = 3,
		_NSTART = 5,
		_WEIGHT = 7
	};
	const string& delimiters = "\"";
	ifstream input_file;
	input_file.open( fileName.c_str() );
	int64_t line_no = 0, numberOfNodes = 0, numberOfEdges = 0;
	int i, blade = 0;
	while( input_file.good() )
	{
		string line;
		vector < string > tokens;
		getline( input_file, line );
		string::size_type lastPos = line.find_first_not_of( delimiters, 0 );
		string::size_type pos = line.find_first_of( delimiters, lastPos );
		while( string::npos != pos || string::npos != lastPos )
		{
			string temp = line.substr( lastPos, pos - lastPos );
			tokens.push_back( temp );
			lastPos = line.find_first_not_of( delimiters, pos );
			pos = line.find_first_of( delimiters, lastPos );
		}
		if(tokens.size() == 1)
		{
			if( tokens[0].find("<Nodes>") != string::npos )
			{
				blade = 1;
				ncon = 1;
			}
			else if( tokens[0].find("</Nodes>") != string::npos )
			{
				nvtxs = numberOfNodes;
				for(int i=0; i<nvtxs; ++i)
				{
					vector < double > tempVector( nvtxs );
					connectivityMatrix.push_back( tempVector );
				}
				singleConstraintVwgt.resize( nvtxs, 0 );
				cout<<"\nTotal number of nodes found = "<<numberOfNodes;
				fflush( stdout );
			}
			else if( tokens[0].find("<Edges>") != string::npos )
				blade = 3;
			else if( tokens[0].find("</Edges>") != string::npos )
			{
				blade = 4;
				createMETISFile( numberOfNodes, numberOfEdges, ncon, fileName );
			}
			else if( tokens[0].find("</Graph>") != string::npos )
				break;
		}
		else
		{
			switch( blade )
			{
				case 1:
				{
					IDMap.push_back( atoll( tokens[_NODE].c_str() ) );
					for(int i = 0; i<ncon; ++i)
						vwgt.push_back( 0 ); // Assume equal weights for all intersections
					++numberOfNodes;
					break;
				}
				case 3:
				{
					int64_t temp = atoll( tokens[ _NSTART ].c_str() );
					int64_t nStart, nEnd;
					for(i=0; i<IDMap.size(); ++i)
						if( IDMap[i] == temp )
						{	nStart = i; break;	}
					temp = atoll( tokens[ _NEND ].c_str() );
					for(i=0; i<IDMap.size(); ++i)
						if( IDMap[i] == temp )
						{	nEnd = i; break;	}
					connectivityMatrix[ nStart ][ nEnd ] = atoll( tokens[ _WEIGHT ].c_str() );
					vwgt[ nStart*ncon ] += connectivityMatrix[ nStart ][ nEnd ];
					vwgt[ nEnd*ncon ] += connectivityMatrix[ nStart ][ nEnd ];
					++numberOfEdges;
					break;
				}
				default:
					break;
			}
		}
		line_no++;
	}
	singleConstraintVwgt.resize(nvtxs, 0);
	for(i=0; i<nvtxs; ++i)
		for(int j=0; j<ncon; ++j)
			singleConstraintVwgt[i] += vwgt[i*ncon + j];
		
	input_file.close();
}

GraphIR::GraphIR()
{
	nvtxs = ncon = 0;
}

GraphIR::~GraphIR()
{
	// free memory for the all the variables. call vector.clear();
}

GraphIR GraphIR::operator = (GraphIR param)
{
	GraphIR temp;
	temp.nvtxs = param.nvtxs;
	temp.ncon = param.ncon;
	temp.vwgt = param.vwgt;
	return (temp);
}

#endif
