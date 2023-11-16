/*****************************************************************//**
 * \file   ZadPPK.cpp
 * \brief  main function that takes arguments and runs the program
 * 
 * \author petrifikus (github.com/petrifikus/)
 * \date   10.2023
 *********************************************************************/


#include "Dikstra.h"


/**
 * @brief takes in cmd arguments
 * @param argC int num of arguments
 * @param argV string arguments
 * @return 0 ok
 * @return 1 error - help shown
 * @return 2 error - file not accessible
*/
int main(const int argC, const char* argV[])
{
	//no args given, show Help
	if (argC == 1) {
		printf("Help for Dijkstra, ANSI only build v1.05\n  -g graph file in\n  -w vertices file in\n  -o output file out\n");
		return 1;
	}

	//create class to hold computation data
	Dikstra DjDisktra;
	std::string arg_str;
	//process agrs
	for (int i = 1; i < argC; ++i) {
		arg_str.assign(argV[i]);
		if (arg_str == "-g") {
			DjDisktra.set_graphFile(argV[++i]);
		}
		else if (arg_str == "-w" || arg_str == "-v") {
			DjDisktra.set_verticesFile(argV[++i]);
		}
		else if (arg_str == "-o") {
			DjDisktra.set_outputFile(argV[++i]);
		}
	}

	//run the main program & return error_code
	DikstraErrors error = DjDisktra.run();
	printf("\n");
	switch (error)
	{
	case DikstraErrors::ok:
		printf("Dijkstra finished successfully, see output file \"%s\" for results.\n", DjDisktra.get_outputFile().c_str());
		break;
	case DikstraErrors::arg_error:
		printf("Error missing arguments, see Help\n");
		return 1;
		break;
	case DikstraErrors::file_error:
		printf("Error Counld not open all necessary files.\n");
		return 2;
		break;
	default:
		break;
	}

	return 0;
}
