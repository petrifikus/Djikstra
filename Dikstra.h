/*****************************************************************//**
 * \file   Dikstra.h
 * \brief  this class contains all necessary things to run Dikstra
 * 
 * \author fafik
 * \date   10.2023
 *********************************************************************/
#pragma once
#include <map>
#include <vector>
#include <string>
#include <fstream>

/* wyjaśnienie
*  jest lista kropek o nr: 1,2,3,... i kazda kropka ma? odnogi do (1badz wiecej kropek) o podanych dlugosciach
*  np. 1= [{2; 50.2}; {3; 12.1}]  3=[{1; 12.1}]		@see "PPK dikstra.jpg"
*  1 -> 2 : 50.2	1 - 3 : 12.1
*/

//1) aby policzyc najkrotsze odleglosci potrzebujemy wybrac pkt startowy, i przejsc wszystkie bezposrednie polaczenia..
//   ..zapisujemy odleglosci w tablicy punktow, oraz ich inicjatora .roota
//2) kontunujemy te kroki dla najkrutszego odcinka |root a pkt|
//pamietamy o tym zeby w miare mozliwosci odwiedzic wszystkie punkty





/**
 * @brief list of return values for Dikstra::run()
*/
enum class DikstraErrors {
	no_errors,
	ok = no_errors,
	arg_error,
	file_error,
};

/**
 * @brief contains the graph structure, and will do calculations on it
*/
class Dikstra
{
public:
	/**
	 * @brief 
	 * @param graphFile in file, where lines are specified like "2 -> 5 -> 3 : 37.05"
	*/
	inline void set_graphFile(const std::string graphFile) { this->graphFile = graphFile; }
	/**
	 * @brief 
	 * @param verticesFile in file, where each line contains a number(id) of vertice to make shortest paths to every other connected point
	*/
	inline void set_verticesFile(const std::string verticesFile) { this->verticesFile = verticesFile; }
	/**
	 * @brief 
	 * @param outputFile out file, all results will be saved there
	*/
	inline void set_outputFile(const std::string outputFile) { this->outputFile = outputFile; }
	/**
	 * @brief 
	 * @return outputFile
	*/
	inline const std::string& get_outputFile()const { return this->outputFile; }
	/**
	 * @brief checks if parameters are ok to calculate
	 * @return 0 fail, 1 ok
	*/
	inline const bool is_ok()const { return graphFile.size() && verticesFile.size() && outputFile.size(); }
	/**
	 * @brief
	 * @return last error from run()
	*/
	inline DikstraErrors get_lastError()const { return lastError; }

	/**
	 * @brief runs the Dikstra program to graph vertices && saves the error code
	 * @return DikstraErrors type of error
	*/
	inline DikstraErrors run() { return lastError = run_private(); }



private:
	/**
	 * @brief inside call: runs the Dikstra program to graph vertices
	 * @return DikstraErrors
	*/
	DikstraErrors run_private();
	/**
	 * @brief loads 2/3 files given
	 * @return 0 fail, 1 ok
	*/
	bool load_files();
	/**
	 * @brief finds the shortest path from given point(vertice) to all others
	 * @param vertice id of the point to start tracing roads from
	*/
	void findShortestPath(const int& vertice)const;

protected:
	/**
	 * @brief list of individual points
	*/
	typedef std::map<int, double> DItemList;
	/**
	 * @brief list of all points, that store a list of points they connect to
	*/
	std::map<int, DItemList> Dweb;
	std::vector<int> verticesVector;
	std::string graphFile;
	std::string verticesFile;
	std::string outputFile;
	DikstraErrors lastError;
};



