#include <iostream>
#include <fstream>
#include <vector>
#include <dlib/matrix.h>

using namespace std;
using namespace dlib;
#define MAX_SIZE 128

string* str_split(string str_origin, string str_tok){
	int cut_at;  
	int index = 0; 
	string* str_result = new string[MAX_SIZE];

	while((cut_at = str_origin.find_first_of(str_tok)) != str_origin.npos)
	{
		if(cut_at >  0)
		{
			str_result[index++] = str_origin.substr(0,cut_at);
		}

		str_origin = str_origin.substr(cut_at +1);
	}
	if(str_origin.length() > 0)
	{
		str_result[index++] = str_origin.substr(0,cut_at);
	}
	return str_result;
}

/*
txt_to_matrix :
input : txt file path
output : 128D matrix (128rows, 1col)
*/
matrix<float,MAX_SIZE,1> txt_to_matrix(string path){

	int cnt;
	float f[MAX_SIZE];

	string str1;
	ifstream inFile(path);
	getline(inFile,str1);

	string* str_ary = str_split(str1," ");
	inFile.close();


	for(int j=0; j<MAX_SIZE;j++){
		f[j] = std::stof(str_ary[j]);
	}

	matrix<float,MAX_SIZE,1> mat;
	mat = f;
	return mat;
}

int main(int argc, char** argv){

	cout << txt_to_matrix(argv[1]) << endl;
	return 0;
}

