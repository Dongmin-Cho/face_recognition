#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
#define MAX_SIZE 4096

std::vector<float> txt_to_vector(string path)
{

	int cnt;
	std::vector<float> descriptor;

        ifstream inFile(path);
    for(int i = 0; i < MAX_SIZE; i++)
    {
        string str1="";
        getline(inFile,str1);
        descriptor.push_back(stof(str1));
    }
	inFile.close();

	return descriptor;
}
