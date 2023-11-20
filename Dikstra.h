/*****************************************************************//**
 * \file   Dikstra.h
 * \brief  this class contains all necessary things to run Dikstra
 * 
 * \author petrifikus
 * \date   10.2023
 *********************************************************************/
#pragma once
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
#include <iomanip>



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
	 * @brief set graphFile file name
	 * @param graphFile in file, where lines are specified like "2 -> 3 : 37.05"
	*/
	inline void set_graphFile(const std::string& graphFile) { this->graphFile = graphFile; }
	/**
	 * @brief set verticesFile file name
	 * @param verticesFile in file, where each line contains a number(id) of vertice to make shortest paths to every other connected point
	*/
	inline void set_verticesFile(const std::string& verticesFile) { this->verticesFile = verticesFile; }
	/**
	 * @brief add a single vertice (with no duplicates)
	 * @param vertice id to trace
	*/
	inline void add_vertice(const int vertice);
	/**
	 * @brief set outputFile file name
	 * @param outputFile out file, all results will be saved there
	*/
	inline void set_outputFile(const std::string& outputFile) { this->outputFile = outputFile; }
	/**
	 * @brief get get_outputFile file name
	 * @return outputFile name
	*/
	inline const std::string& get_outputFile()const { return this->outputFile; }
	/**
	 * @brief checks if parameters are ok to calculate
	 * @return 0 fail, 1 ok
	*/
	inline const bool is_ok()const { return graphFile.size() && (verticesFile.size() || verticesVector.size()) && outputFile.size(); }
	/**
	 * @brief gets last error of run()
	 * @return last error from run()
	*/
	inline DikstraErrors get_lastError()const { return lastError; }

	/**
	 * @brief runs the Dikstra program to graph vertices && saves the error code
	 * @return DikstraErrors type of error
	*/
	inline DikstraErrors run() { return lastError = run_private(); }


protected:
	//double = +inf
	static constexpr double _infDouble = std::numeric_limits<double>::infinity();
	/**
	 * @brief stores double, defaults to {+inf}
	*/
	struct doubleInf
	{
		double value;
		doubleInf(const double other= _infDouble) : value(other) {}
		inline double& operator = (const double& other) { value = other; return value; }
		operator double&() { return value; }
		operator const double&()const { return value; }
	};
	/**
	 * @brief list of individual points
	*/
	typedef std::map<int, double> DItemList;
	/**
	 * @brief list of all points, that store a list of points they connect to
	*/
	typedef std::map<int, DItemList> DwebT;
	/**
	 * @brief (point) to point distance
	*/
	typedef std::pair<int, doubleInf> pointDistance;
	/**
	 * @brief distance (between) points (the reverse of pointDistance)
	*/
	typedef std::pair<doubleInf, int> distancePoint;

	/**
	 * @brief {distance, id} sorted list that allows key-duplicates but not key+value duplicates
	*/
	struct sortedVector_distancePoint
	{
		typedef std::vector<distancePoint>::iterator iterator;
		typedef std::vector<distancePoint>::const_iterator const_iterator;

		/**
		 * @brief always adds item to sorted list
		 * @param item to add
		 * @return iterator to added item
		*/
		iterator add(const distancePoint& item);
		/**
		 * @brief binary search for item
		 * @param item to search for(reverse the pointDistance)
		 * @return iterator to matching item, or end()
		*/
		iterator get(const distancePoint& item);
		/**
		 * @brief will update the item (by calling find, remove, add)
		 * @param itemOld old item to look for
		 * @param itemNew new item to replace it with
		 * @return iterator to new item
		*/
		iterator update(const distancePoint& itemOld, const distancePoint& itemNew);
		/**
		 * @brief removes the 1st item from the list and returns it
		 * @return copy if 1st item on the list
		*/
		distancePoint pop_front();

		inline void remove(iterator itemToRemove) { if (itemToRemove != SortedItems.end()) SortedItems.erase(itemToRemove); }
		inline iterator end() { return SortedItems.end(); }
		inline iterator begin() { return SortedItems.begin(); }
		inline void clear() { SortedItems.clear(); }
		inline size_t size()const { return SortedItems.size(); }
		inline bool empty()const { return SortedItems.empty(); }
	protected:
		std::vector<distancePoint> SortedItems;
	};

	/**
	 * @brief data structure passed for graph solving
	*/
	struct SolveGraphStruct
	{
		/**
		 * @brief ctor that will init all values based on DwebIn
		 * @param DwebIn: graph of connections
		*/
		SolveGraphStruct(const Dikstra::DwebT& DwebIn) { init(DwebIn); }
		/**
		 * @brief will init all values based on DwebIn
		 * @param DwebIn: graph of connections 
		*/
		void init(const Dikstra::DwebT& DwebIn);
		/**
		 * @brief adds vertice to queues and updates the Previous_Distance table
		 * @param vertice id
		 * @param distance beetween points
		 * @param PreviousVertice previous vertice
		 * @return true when added; false when already in queue
		*/
		bool addVerticeToQueue(const int& vertice, const double& distance, const int& PreviousVertice);
		/**
		 * @brief adds vertice to queue and updates the Previous_Distance table
		 * @param pointDistance_ {vertice id, distance beetween points}
		 * @param PreviousVertice previous vertice
		 * @return true when added; false when already in queue
		*/
		inline bool addVerticeToQueue(const pointDistance& pointDistance_, const int& PreviousVertice) {
			return addVerticeToQueue(pointDistance_.first, pointDistance_.second, PreviousVertice);
		}
		/**
		 * @brief gets the next closest vertice from the pending list and removes it
		 * (items are stored twice in ordered lists)
		 * @param output: vertice closest
		 * @return true if returned a value, false if error
		*/
		bool popClosestVerticeFromQueue(pointDistance& output);
	
		//one map that stores all vertices; where 0 means NOT visited yet, 1 means Visited already.
		std::map<int, bool> visitedToVisit;
		//one map that stores previous vertice & shortest distance
		std::map<int, pointDistance> Previous_Distance;
		//amount of vertices that have not been solved yet || couldn't be reached (starts with Dweb.size();)
		size_t leftToVisit;
		//amount of connections made from the starting vertice
		size_t connectsCount;
	protected:
		//11.11 queue for vertices to scan (auto sorted by id)
		std::map<int, double> listPending;
		//16.11 queue for vertices to scan (in sorted order by distance)
		sortedVector_distancePoint listPendingSV;
	};


	/**
	 * @brief stores all vertices their conections and distances loaded from file
	*/
	DwebT Dweb;
	std::map<int,bool> verticesVectorBlock;
	std::vector<int> verticesVector;
	std::string graphFile;
	std::string verticesFile;
	std::string outputFile;
	DikstraErrors lastError;
	std::ofstream* outFileP;
	int _longestId;

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
	 * @brief finds the shortest path from given point(vertice) to all others & prints out the results to file
	 * @param vertice id of the point to start tracing roads from
	*/
	void findShortestPath(const int& vertice)const;
	/**
	 * @brief print out to file all connected vertices
	 * @param SolveGraphCin struct made by findShortestPath()
	 * @param vertice to start going back from
	*/
	void writeShortestPathFor(const SolveGraphStruct& SolveGraphCin, const int vertice)const;

};



