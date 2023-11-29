/*****************************************************************//**
 * \file   Dikstra.cpp
 * \brief  run, load, and calculate the shortest path from given points to all other ones
 * 
 * \author petrifikus
 * \date   10.2023
 *********************************************************************/
#include "Dikstra.h"


void Dikstra::add_vertice(const int vertice)
{
	verticesListUnique[vertice] = true;
}

DikstraErrors Dikstra::run_private()
{
	if (!is_ok()) return DikstraErrors::arg_error;
	if (!load_files()) return DikstraErrors::file_error;
	//set the max lenght of id
	_longestId = (int)std::to_string(Dweb.rbegin()->first).size();
	
	//just a debug print of the graph structure in memory
	/*printf("\nprinting graph:\n");
	for (const auto& pair : Dweb) {
		printf("[%i]= {\n", pair.first);	//int id version
		//printf("[%c]= {\n", pair.first+'A');	//char A+id version
		for (const auto& pairpair : pair.second) {
			printf("  %i = %f\n", pairpair.first, pairpair.second);	//int id version
			//printf("  %c = %f\n", pairpair.first+'A', pairpair.second);	//char A+id version
		}
		printf("}\n");
	}*/

	std::ofstream outFile;
	outFile.open(outputFile);
	if (!outFile.is_open()) {
		printf("Error! Could not open output file \"%s\"\n", get_outputFile().c_str());
		return DikstraErrors::file_error;
	}
	outFileP = &outFile;

	for (const auto& item : verticesListUnique) {
		const auto& finds = Dweb.find(item.first);
		if (finds == Dweb.end()) {
			outFile << "  Vertice: " << item.first << " NOT found\n";
			printf("  Vertice: %i NOT found\n", item.first);
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
	if (inFile && inFile.is_open()) {
		while (inFile >> v1)
		{
			add_vertice(v1); //makes sure to add it with no duplicates
		}
		inFile.close();
	}
	if (verticesListUnique.empty()) return 0;

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
			printf("!Vertice %i points to itself!\n", v1);
			continue;
		}
		//check if both points are >= 0
		if (v1 < 0 || v2 < 0) {
			printf("!Vertice %i or %i is negative!\n", v1, v2);
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
		outFile << "Vertice [" << vertice << "] does not exist;\n";
		printf("Vertice [%i] does not exist;\n", vertice);
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
		auto ItemExists = Dweb.find(id);
		if (ItemExists == Dweb.end()) {
			//printf("---- [%i] is not in the main table of connections\n", id);
			continue;
		}
		//try to add all connected verticess to queue
		for (const auto& iter : ItemExists->second) {
			SolveGraph.addVerticeToQueue(iter, id);
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
		outFile << std::setfill(' ') << std::setw(_longestId) << (*item) << " ";	//int id version
		//outFile << char((*item)+'A') << " ";	//char A+id version
		printf("%i ", *item);
		if (*item != vertice) {
			outFile << "-> ";
			printf("-> ");
		}
	}
	outFile << ": " << std::setprecision(2) << std::fixed << SolveGraphCin.Previous_Distance.at(vertice).second << "\n";
	printf(": %f\n", (double)SolveGraphCin.Previous_Distance.at(vertice).second);
}



void Dikstra::SolveGraphStruct::init(const Dikstra::DwebT& DwebIn)
{
	leftToVisit = DwebIn.size();
	connectsCount = 0;
	Previous_Distance.clear();
	visitedToVisit.clear();
	listPending.clear();
	listPendingSV.clear();
	//setting the starting data as of 16.11 is uneccessary as the default ctor will init {0,+inf}
}

bool Dikstra::SolveGraphStruct::addVerticeToQueue(const int& vertice, const double& distance, const int& PreviousVertice)
{
	if (distance < 0) return false;
	//check if we already visited it/create it set to {0}
	auto& visitedStatus = visitedToVisit[vertice];
	double totalDistance = distance;
	if (PreviousVertice != -1) {
		totalDistance += Previous_Distance[PreviousVertice].second;
	}

	//&ref to item in map; if it doesnt exist it will be created with {0, +inf} (as of 16.11)
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
		if (distance < setPending->second) {
			listPendingSV.update({ setPending->second, vertice }, { distance, vertice });
			setPending->second = distance;
		}
		return true;
	}
	//new
	listPending[vertice] = distance;
	listPendingSV.add({ distance, vertice });
	return true;
}


bool Dikstra::SolveGraphStruct::popClosestVerticeFromQueue(Dikstra::pointDistance& output)
{
	//cant get element(queue empty)
	if (listPending.empty()) return false;
	//pop from pending list(s), dont revisit
	distancePoint closestSV = listPendingSV.pop_front();
	auto iterP = listPending.find(closestSV.second);
	//reverse it (from distancePoint -> pointDistance )
	output = pointDistance{ closestSV.second, closestSV.first };
	listPending.erase(iterP);
	visitedToVisit[closestSV.second] = true;
	--leftToVisit;
	return true;
}

Dikstra::sortedVector_distancePoint::iterator Dikstra::sortedVector_distancePoint::add(const distancePoint& item)
{
	iterator iter = std::lower_bound(begin(), end(), item);
	//check if that exact element already exists, if so the skip it
	if (iter != end() && *iter == item) {
		return iter;
	}
	return SortedItems.insert(iter, item);
}

Dikstra::sortedVector_distancePoint::iterator Dikstra::sortedVector_distancePoint::get(const distancePoint& item)
{
	iterator iter = std::lower_bound(begin(), end(), item);
	//check if that element actually exists
	if (iter != end() && *iter == item) {
		return iter;
	}
	return SortedItems.end();
}

Dikstra::sortedVector_distancePoint::iterator Dikstra::sortedVector_distancePoint::update(const distancePoint& itemOld, const distancePoint& itemNew)
{
	remove(get(itemOld));
	return add(itemNew);
}

Dikstra::distancePoint Dikstra::sortedVector_distancePoint::pop_front()
{
	if (SortedItems.size()) {
		distancePoint val{SortedItems.front()};
		SortedItems.erase(SortedItems.begin());
		return val;
	}
	return distancePoint{_infDouble, -1};
}


