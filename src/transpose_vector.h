#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
#define MAX_SIZE 4096

std::vector<float> txt_to_vector(string path)
{
	std::vector<float> descriptor;

    ifstream in_file(path);
    if(in_file.is_open())
    {
        for(int i = 0; i < MAX_SIZE; i++)
        {
            string str1="";
            getline(in_file,str1);
            descriptor.push_back(stof(str1));
        }
        in_file.close();
    }
    else
    {
        cout << "error in txt to vector" << endl;
	    return descriptor;
    }
}
