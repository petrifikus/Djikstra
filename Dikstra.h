/*****************************************************************//**
 * \file   Dikstra.h
 * \brief  
 * 
 * \author fafik
 * \date   10.2023
 *********************************************************************/
#pragma once
#include <vector>

/* wyjaśnienie
*  jest lista kropek o nr: 1,2,3,... i kazda kropka ma? odnogi do (1badz wiecej kropek) o podanych dlugosciach
*  np. 1= [{2; 50.2}; {3; 12.1}]  3=[{1; 12.1}]		@see "PPK dikstra.jpg"
*  1 -> 2 : 50.2	1 - 3 : 12.1
*/












 /**
  * @brief each point in the graph
 */
struct DItem {
	/**
	 * @brief connects to Id.
	*/
	int connectsTo;
	/**
	 * @brief distance beetween parent-this
	*/
	float distance;
};
/**
 * @brief all connections for a point in the graph,
 * vector is sorted Asc by: Id
*/
struct DItems {
	/**
	 * @brief point id
	*/
	int id;
	/**
	 * @brief list of connections from this point to others (with distance)  [{id; distance},..]
	*/
	std::vector<DItem> cenncections;
};


/**
 * @brief all the points and connections,
 * vector* is sorted Asc by: Id
*/
class Dikstra
{
	std::vector<DItems*> web;


	//1) aby policzyc najkrotsze odleglosci potrzebujemy wybrac pkt startowy, i przejsc wszystkie bezposrednie polaczenia..
	//   ..zapisujemy odleglosci w tablicy punktow, oraz ich inicjatora .roota
	//2) kontunujemy te kroki dla najkrutszego odcinka |root a pkt|
	//pamietamy o tym zeby w miare mozliwosci odwiedzic wszystkie punkty
};



