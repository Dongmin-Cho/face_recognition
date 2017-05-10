#include <iostream>
#include <fstream>
#include <vector>
#include <dlib/matrix.h>

using namespace std;
using namespace dlib;
#define MAX_SIZE 128

string* strSplit(string strOrigin, string strTok){
	int cutAt;  
	int index = 0; 
	string* strResult = new string[MAX_SIZE];

	while((cutAt = strOrigin.find_first_of(strTok)) != strOrigin.npos)
	{
		if(cutAt >  0)
		{
			strResult[index++] = strOrigin.substr(0,cutAt);
		}

		strOrigin = strOrigin.substr(cutAt +1);
	}
	if(strOrigin.length() > 0)
	{
		strResult[index++] = strOrigin.substr(0,cutAt);
	}
	return strResult;
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

	string* strAry = strSplit(str1," ");
	inFile.close();


	for(int j=0; j<MAX_SIZE;j++){
		f[j] = std::stof(strAry[j]);
	}

	matrix<float,MAX_SIZE,1> mat;
	mat = f;
	return mat;
}

int main(int argc, char** argv){

	cout << txt_to_matrix(argv[1]) << endl;
	return 0;
}

