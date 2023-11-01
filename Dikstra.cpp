/*****************************************************************//**
 * \file   Dikstra.cpp
 * \brief  run, load, and calculate the shortest path from given points to all other ones
 * 
 * \author fafik
 * \date   10.2023
 *********************************************************************/
#include <sstream>
#include "Dikstra.h"


DikstraErrors Dikstra::run_private()
{
	if (!is_ok()) return DikstraErrors::arg_error;
	if (!load_files()) return DikstraErrors::file_error;
	
	//just a debug print of the graph structure in memory
	printf("\nprinting graph:\n");
	for (const auto& pair : Dweb) {
		printf("[%i]= {\n", pair.first);
		for (const auto& pairpair : pair.second) {
			printf("  %i = %f\n", pairpair.first, pairpair.second);
		}
		printf("}\n");
	}
	printf("\nprinting if V exists:\n");
	for (const auto& item : verticesVector) {
		const auto& finds = Dweb.find(item);
		if (finds == Dweb.end()) printf("  key: %i NOT found\n", item);
		else {
			printf("  key: %i exists\n", finds->first);
		}
	}

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
	std::string line;
	std::istringstream iss;
	std::string tempStr;
	double vDistance;
	bool bothWays;
	//extract each line to make sure if it has content, as we dont want to get stuck with errors
	while (std::getline(inFile, line))
	{
		//too short to contain any info, skipp
		if (line.size() < 4) continue;
		iss.clear();
		iss.str(line);
		iss >> v1 >> tempStr;
		bothWays = !(tempStr.find(">") != tempStr.npos);
		iss >> v2 >> tempStr;
		iss >> vDistance;
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
}


