// K-means.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>

#define INFINITE 300000;

using namespace std;


class object {
public:
	int id;
	double x;
	double y;

	int group;

	object(int _id,double _x, double _y)
	{
		id = _id;
		x = _x;
		y = _y;
		group = -1;
	}


};

void Tokenize(const string& str, vector<string>& tokens, const string& delimiters);
void k_means(vector<object>& objects, int K);
bool isSame(vector<object>& centroids, object choose);
double ucdDistance(object one, object other);

int main(int argc, char* argv[])
{

	if (argc != 3) {
		cout << "argument is insufficent" << endl;
		return -1;
	}

	string inputFileName = argv[1];
	int K = atoi(argv[2]);

	ifstream inputFile;
	ofstream result;

	inputFile.open(inputFileName, ios::in);
	result.open("input1_cluster_0.txt", ios::out);

	string  fileLine;
	vector<string> tokens;
	vector<object> objects;

	while (inputFile.good()) {
		getline(inputFile, fileLine);
		Tokenize(fileLine, tokens, "\t");
		char _id[30];
		char _x[30];
		char _y[30];
		if (tokens.size() != 3)break;
		sprintf_s(_id, "%s", tokens[0].c_str());
		sprintf_s(_x, "%s", tokens[1].c_str());
		sprintf_s(_y, "%s", tokens[2].c_str());
		object tmp(atoi(_id),atof(_x), atof(_y));
		objects.push_back(tmp);
		tokens.clear();
	}

	k_means(objects, K);

	vector<vector<object>> cluster;
	for (int i = 0; i < K; i++)
	{
		vector<object> tmp;
		cluster.push_back(tmp);

	}

	for (int i = 0; i < objects.size(); i++)
	{
		cluster[objects[i].group].push_back(objects[i]);
	}

	for (int i = 0; i < cluster.size(); i++)
	{
		result << "# " << i << "cluster size: " << cluster[i].size() << endl;
		for (int j = 0; j < cluster[i].size(); j++)
		{
			result << cluster[i][j].id << " " << cluster[i][j].x << " " << cluster[i][j].y << endl;
		}
		result << endl;
	}
	


    return 0;
}

void Tokenize(const string& str, vector<string>& tokens, const string& delimiters)
{
	 string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	 string::size_type pos = str.find_first_of(delimiters, lastPos);
	

	 while (string::npos != pos || string::npos != lastPos) 
	 {
		 	tokens.push_back(str.substr(lastPos, pos - lastPos));
			lastPos = str.find_first_not_of(delimiters, pos);
			pos = str.find_first_of(delimiters, lastPos);
	}
	

}

void k_means(vector<object>& objects,int K)
{

	vector<object> centroid;
	//decide initial K points
	srand((unsigned int)time(NULL));
	for (int i = 0; i < K; i++) 
	{
		int point= (rand() % (objects.size() / K)) + i*(objects.size()/K);

		if (isSame(centroid,objects[point])) {
			i--;
			continue;
		}
		centroid.push_back(objects[point]);
	}

	int change = objects.size();
	int theshold = 10;

	while (change > theshold) {

		change = 0;
		for (int i = 0; i < objects.size(); i++)
		{
			double min = INFINITE;
			int group = -1;
			for (int j = 0; j < K; j++)
			{
				double tmp = 0;
				if (min >= (tmp = ucdDistance(objects[i], centroid[j])))
				{
					min = tmp;
					group = j;
				}
			
			}
			if (objects[i].group != group) {
				objects[i].group = group;
				change++;
			}

		}

		vector<object> newCentroid;
		for (int i = 0; i < K; i++)
		{
			object tmp(0,0, 0);
			tmp.group = 0;
			newCentroid.push_back(tmp);
			
		}

		for (int i = 0; i < objects.size(); i++)
		{
			if (objects[i].group == -1)return;
			newCentroid[objects[i].group].x += objects[i].x;
			newCentroid[objects[i].group].y += objects[i].y;
			newCentroid[objects[i].group].group++;
		}

		centroid.clear();

		for (int i = 0; i < K; i++)
		{
			newCentroid[i].x = newCentroid[i].x / newCentroid[i].group;
			newCentroid[i].y = newCentroid[i].y / newCentroid[i].group;
		}

		centroid = newCentroid;
	}


}

bool isSame(vector<object>& centroids, object choose)
{
	for (int i = 0; i < centroids.size(); i++)
	{
		if (centroids[i].x == choose.x&&centroids[i].y == choose.y)return true;
	}
	return false;
}


double ucdDistance(object one,object other) 
{
	return (one.x - other.x)*(one.x - other.x) + (one.y - other.y)*(one.y - other.y);
}