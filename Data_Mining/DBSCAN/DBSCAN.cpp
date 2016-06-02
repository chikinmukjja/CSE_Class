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
		object tmp(atoi(_id), atof(_x), atof(_y));
		objects.push_back(tmp);
		tokens.clear();
	}

	double time;
	bool err;



	CHECK_TIME_START; // 정해진 minpts가 있음 find eps 20 12 적당한듯
	K = DBSCAN(objects,20,12);
	CHECK_TIME_END(time, err)
	
	vector<vector<object>> cluster;
	for (int i = 0; i < K; i++)
	{
		vector<object> tmp;
		cluster.push_back(tmp);

	}

	for (int i = 0; i < objects.size(); i++)
	{
		int index = objects[i].group;

		if (index == NOISE)continue;
		if (index < 0)cout << index << endl;
		else
		cluster[index].push_back(objects[i]);
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


	cout << time << endl;
	return 0;
}

int DBSCAN(vector<object>& objects,int MinPts,double Eps)
{
	int chosen = 0;
	int k = 0; //cluster
	set<int> tmpCluster;

	
	while ((chosen = isAllVisited(objects))!=-1) //
	{
	
		int numOfNeighbor = 0;
		objects[chosen].visited = true;
		 //이웃 찾기
		for (int i = 0; i < objects.size(); i++) 
		{
			if (isInner(objects[chosen], objects[i], Eps)) {
				if (!objects[i].visited)tmpCluster.insert(i);
				numOfNeighbor++;
			}
		}
		
		// MinPts 만족할 시 클러스터 생성
		if (numOfNeighbor >= MinPts) 
		{
			objects[chosen].group = k;
			
			while (tmpCluster.size()!=0)
			{
				set<int>::iterator IterPos = tmpCluster.begin();
				int p = *IterPos;
				//지금 현재 index 제거
				tmpCluster.erase(IterPos);

				if (objects[p].visited == false) 
				{
					set<int> tmpCluster2;
					objects[p].visited = true;
					int numOfNeighbor2 = 0;

					for (int i = 0; i < objects.size(); i++)
					{
						if (isInner(objects[p], objects[i], Eps)) 
						{
								if(!objects[i].visited)tmpCluster2.insert(i);
								numOfNeighbor2++;
						}
					}

					if (numOfNeighbor2 >= MinPts) 
					{	//현재 p의 이웃들 추가
						tmpCluster.insert(tmpCluster2.begin(), tmpCluster2.end());
					}
					
					//현재 p가 속한 클러스터가 없다면 추가
					if (objects[p].group < 0&&objects[p].group!=NOISE)objects[p].group = k;
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
		return  Eps*Eps > x*x + y*y;
	}
	else return false;
}
