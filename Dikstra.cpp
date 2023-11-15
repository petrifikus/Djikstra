/*****************************************************************//**
 * \file   Dikstra.cpp
 * \brief  run, load, and calculate the shortest path from given points to all other ones
 * 
 * \author fafik
 * \date   10.2023
 *********************************************************************/
//#include <sstream>
#include "Dikstra.h"


DikstraErrors Dikstra::run_private()
{
	if (!is_ok()) return DikstraErrors::arg_error;
	if (!load_files()) return DikstraErrors::file_error;
	
	//just a debug print of the graph structure in memory
	/*printf("\nprinting graph:\n");
	for (const auto& pair : Dweb) {
		printf("[%i]= {\n", pair.first);
		for (const auto& pairpair : pair.second) {
			printf("  %i = %f\n", pairpair.first, pairpair.second);
		}
		printf("}\n");
	}
	printf("\nprinting if V exists:\n");*/

	std::ofstream outFile;
	outFile.open(outputFile);
	if (!outFile.is_open()) {
		printf("could not open output file \"%s\"\n", get_outputFile().c_str());
		return DikstraErrors::file_error;
	}
	outFileP = &outFile;

	for (const auto& item : verticesVector) {
		const auto& finds = Dweb.find(item);
		if (finds == Dweb.end()) {
			outFile << "  Vertice: " << item << " NOT found\n";
			printf("  Vertice: %i NOT found\n", item);
		}
		else {
			outFile << "  Vertice: " << finds->first << " paths\n";
			printf("  Vertice: %i paths\n", finds->first);
			findShortestPath(finds->first);
		}
		outFile << "\n";
		printf("\n");
	}

	outFile.flush();
	outFile.close();
	return DikstraErrors::ok;
}

bool Dikstra::load_files()
{
	std::ifstream inFile;
	int v1, v2;

	inFile.open(verticesFile);
	if (!inFile || !inFile.is_open()) return 0;
	while (inFile >> v1)
	{
		verticesVector.push_back(v1);
	}
	inFile.close();

	inFile.open(graphFile);
	if (!inFile || !inFile.is_open()) return 0;
	std::string tempStr;
	double vDistance;
	bool bothWays;
	while ( !inFile.eof() )
	{
		inFile >> v1 >> tempStr;
		//15-11 fixed loading errors when file ends in empty line
		if (inFile.rdstate()) break;
		bothWays = !(tempStr.find(">") != tempStr.npos);
		inFile >> v2 >> tempStr;
		inFile >> vDistance;
		//check if point points to itself (if so then give a warning)
		if (v1 == v2) {
			printf("!Point %i points to itself!\n", v1);
			continue;
		}
		//check if both points are >= 0
		if (v1 < 0 || v2 < 0) {
			printf("!Point %i or %i is negative!\n", v1, v2);
			continue;
		}
		//save values
		Dweb[v1][v2] = vDistance;
		if (bothWays) {
			Dweb[v2][v1] = vDistance;
		}
	}
	inFile.close();

	return 1;
}

void Dikstra::findShortestPath(const int& vertice) const
{
	std::ofstream& outFile = *outFileP;
	//check if vertice exists
	const auto& exists = Dweb.find(vertice);
	if (exists == Dweb.end()) {
		outFile << "vertice [" << vertice << "] does not exist;\n";
		printf("vertice [%i] does not exist;\n", vertice);
		return;
	}
	//make structure for solving graph from this [vertice] && initialize the starting data
	SolveGraphStruct SolveGraph(Dweb);
	//init the first point, invalid previous & zero distance of the starting point
	SolveGraph.addVerticeToQueue(vertice, 0.0, -1);

	pointDistance closestVertice;
	//solve all connections starting from given vertice
	while (SolveGraph.popClosestVerticeFromQueue(closestVertice)) {
		const auto& id = closestVertice.first;
		const auto& weight = closestVertice.second;
//		printf("tracing V[%i]\n", id);
		for (const auto& iter : Dweb.at(id)) {
			SolveGraph.addVerticeToQueue(iter, id);
//			printf("  To V[%i], +%f = %f\n", iter.first, iter.second, iter.second + weight);
		}
	}

	//iterate all made connections and print them
	int verticeConnections = 0;
	for (const auto& iter : SolveGraph.visitedToVisit) {
		//skipp self A->A : 0.00
		if (iter.first == vertice) continue;
		//just in case make sure it was added to connections table
		if (iter.second == true) {
			++verticeConnections;
			writeShortestPathFor(SolveGraph, iter.first);
		}
	}
	//this vertice does not connect to anything
	if (verticeConnections == 0) {
		outFile << " vertice [" << vertice << "] exists but does not connect to anything\n";
		printf(" vertice [%i] exists but does not connect to anything\n", vertice);
	}

	//just a debug print of the connections Table in memory
	/*printf("\n");
	for (const auto& listing : SolveGraph.listPending) {
		printf("pending [%i]=\t %f\n", listing.first, listing.second);
	}
	printf("\n");
	for (const auto& listing : SolveGraph.Previous_Distance) {
		printf("prevDis [%i]=\t (%i, %f, %i)\n", listing.first, listing.second.first, listing.second.second, SolveGraph.visitedToVisit[listing.first]);
	}
	printf("_-_ visited= %lli, left= %lli\n", SolveGraph.connectsCount, SolveGraph.leftToVisit);*/
}

void Dikstra::writeShortestPathFor(const SolveGraphStruct& SolveGraphCin, const int vertice) const
{
	std::ofstream& outFile = *outFileP;
	int parentVertice = vertice;
	//reverse the order of print
	std::vector<int> VList;
	//get list of route
	while (parentVertice != -1) {
		VList.push_back(parentVertice);
		parentVertice = SolveGraphCin.Previous_Distance.at(parentVertice).first;
	}
	//print that list in reverse order
	for (std::vector<int>::reverse_iterator item = VList.rbegin(); item != VList.rend(); ++item) {
		outFile << (*item) << " ";
		printf("%i ", *item);
		if (*item != vertice) {
			outFile << "-> ";
			printf("-> ");
		}
	}
	outFile << ": " << SolveGraphCin.Previous_Distance.at(vertice).second << "\n";
	printf(": %f\n", SolveGraphCin.Previous_Distance.at(vertice).second);
}



void Dikstra::SolveGraphStruct::init(const Dikstra::DwebT& DwebIn)
{
	leftToVisit = DwebIn.size();
	Previous_Distance.clear();
	visitedToVisit.clear();
	listPending.clear();
	connectsCount = 0;
	//set the starting data
	for (const auto& iter : DwebIn) {
		const auto& id = iter.first;
		Previous_Distance[id] = { -1,_infDouble };
		//visitedToVisit[id] = 0;
	}
}

bool Dikstra::SolveGraphStruct::addVerticeToQueue(const int& vertice, const double& distance, const int& PreviousVertice)
{
	if (distance < 0) return false;
	//check if we already visited it
	auto& visitedStatus = visitedToVisit[vertice];
	double totalDistance = distance;
	if (PreviousVertice != -1) {
		totalDistance += Previous_Distance[PreviousVertice].second;
	}
	auto& distanceToV = Previous_Distance[vertice];
	//set new lower distance
	if (totalDistance < distanceToV.second) {
		if (distanceToV.second == _infDouble) ++connectsCount;
		distanceToV = { PreviousVertice, totalDistance };
	}

	if (visitedStatus == true) {
		return false;
	}

	auto setPending = listPending.find(vertice);
	//not new, update(only to lower value)
	if (setPending != listPending.end()) {
		if (distance < setPending->second) setPending->second = distance;
		return true;
	}
	//new
	listPending[vertice] = distance;
	return true;
}


bool Dikstra::SolveGraphStruct::popClosestVerticeFromQueue(Dikstra::pointDistance& output)
{
	//cant get element(queue empty)
	if (listPending.empty()) return false;
	pointDistance closest{ -1, _infDouble };
	for (auto& iter : listPending) {
		if (iter.second < closest.second) {
			closest = iter;
		}
	}
	//pop from pending list, dont revisit
	listPending.erase(closest.first);
	visitedToVisit[closest.first] = true;
	--leftToVisit;
	output = closest;
	return true;
}
