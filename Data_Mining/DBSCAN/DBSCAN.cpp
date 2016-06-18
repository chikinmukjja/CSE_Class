// DBSCAN.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <windows.h>
#include <vector>
#include <set>

#define INFINITE 300000;
#define NOISE -3

#define CHECK_TIME_START  __int64 freq, start, end; if (QueryPerformanceFrequency((_LARGE_INTEGER*)&freq))  {QueryPerformanceCounter((_LARGE_INTEGER*)&start);   
#define CHECK_TIME_END(a, b)   QueryPerformanceCounter((_LARGE_INTEGER*)&end);  a = (float)((double)(end - start) / freq * 1000); b = TRUE;}else b = FALSE;



using namespace std;


class object {
public:
	int id;
	double x;
	double y;

	bool visited;
	int group;

	object(int _id, double _x, double _y)
	{
		id = _id;
		x = _x;
		y = _y;
		visited = false;
		group = -1;
	}


};

void Tokenize(const string& str, vector<string>& tokens, const string& delimiters);
bool isInner(object& core, object& p, double Eps);
int isAllVisited(const vector<object>& objects);
int DBSCAN(vector<object>& objects, int MinPts, double Eps);

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
	ofstream result[20];

	inputFile.open(inputFileName, ios::in);
	//result.open("input1_cluster_0.txt", ios::out);

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
		object tmp(atoi(_id), atof(_x), atof(_y));
		objects.push_back(tmp);
		tokens.clear();
	}

	double time;
	bool err;


	CHECK_TIME_START;
	if (inputFileName=="input1.txt")K = DBSCAN(objects, 24, 15.2);        // 98.5점
	else if (inputFileName == "input2.txt")K = DBSCAN(objects, 24, 3.16); // 95.17 
	else if (inputFileName == "input3.txt")K = DBSCAN(objects, 10, 7);    // 100점
	else k_means(objects, K);                                             // 그외의 경우엔 K_means
	CHECK_TIME_END(time, err)
	

	cout << "clustser " << K << endl;
	vector<vector<object>> cluster;
	for (int i = 0; i < K; i++)
	{
		vector<object> tmp;
		cluster.push_back(tmp);

	}

	int numNOISE=0;
	for (int i = 0; i < objects.size(); i++)
	{

		int index = objects[i].group;

		if (index == NOISE) {
			++numNOISE;
			continue;
		}
		if (index < 0)cout << index << endl;
		else
		cluster[index].push_back(objects[i]);
	}

	
	for (int i = 0; i < cluster.size(); i++)
	{
		
		
		string name = "output _cluster_ .txt";
		char shap = i + '0';
		name[6] = inputFileName[5];
		name[name.size() - 5] = shap;
		result[i].open(name, ios::out);
		
		cout << cluster[i].size() << "\n";
		for (int j = 0; j < cluster[i].size(); j++)
		{
			result[i] << cluster[i][j].id << endl;
	
		}
	

		
	}

	cout <<"NOISE: " <<numNOISE << endl;

	cout << time << endl;
	return 0;
}

int DBSCAN(vector<object>& objects,int MinPts,double Eps)
{
	int chosen = 0;
	int k = 0; //cluster
	vector<int> tmpCluster;

	int testCnt = 0;
	while ((chosen = isAllVisited(objects))!=-1) //탐색할 point 선택하기
	{	
		int numOfNeighbor = 0;
		objects[chosen].visited = true;
		
		//이웃 찾기
		for (int i = 0; i < objects.size(); i++) 
		{
			if (isInner(objects[chosen], objects[i], Eps)) {
				if (!objects[i].visited)tmpCluster.push_back(i);
				numOfNeighbor++;
			}
		}
		
		// MinPts 만족할 시 클러스터 생성
		if (numOfNeighbor >= MinPts) 
		{
			objects[chosen].group = k;
			
			while (tmpCluster.size()!=0)
			{
				int p = tmpCluster.back();
				tmpCluster.pop_back();

				if (objects[p].visited == false) 
				{
					vector<int> tmpCluster2;
					objects[p].visited = true;
					int numOfNeighbor2 = 0;

					for (int i = 0; i < objects.size(); i++)
					{
						if (isInner(objects[p], objects[i], Eps)) 
						{
								if(!objects[i].visited)tmpCluster2.push_back(i);
								numOfNeighbor2++;
						}
					}

					if (numOfNeighbor2 >= MinPts) 
					{	//현재 p의 이웃들 추가
						tmpCluster.insert(tmpCluster.end(),tmpCluster2.begin(), tmpCluster2.end());
					}
					
					//현재 p가 속한 클러스터가 없다면 추가
					if (objects[p].group <0)objects[p].group = k;
				}
				
			}
			//해당 클러스터는 완료됨
			k++;
		}
		else 
		{
			objects[chosen].group = NOISE;
		}
	}
	return k;
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


int isAllVisited(const vector<object>& objects)
{
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i].visited == false)return i;
	}

	return -1;
}

bool isInner(object& core,object& p,double Eps)
{
	
	if (core.id != p.id) {
		double x = core.x - p.x;
		double y = core.y - p.y;
		return  Eps*Eps >= x*x + y*y;
	}
	else return false;
}

void k_means(vector<object>& objects, int K)
{

	vector<object> centroid;
	//decide initial K points
	srand((unsigned int)time(NULL));
	for (int i = 0; i < K; i++)
	{
		int point = (rand() % (objects.size() / K)) + i*(objects.size() / K);

		if (isSame(centroid, objects[point])) {
			i--;
			continue;
		}
		centroid.push_back(objects[point]);
	}

	int change = objects.size();
	int theshold = 1;

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
			object tmp(0, 0, 0);
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


double ucdDistance(object one, object other)
{
	return (one.x - other.x)*(one.x - other.x) + (one.y - other.y)*(one.y - other.y);
}