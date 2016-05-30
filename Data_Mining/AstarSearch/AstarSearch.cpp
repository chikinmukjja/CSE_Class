// AstarSearch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <queue>

#define START '2'
#define END '3'
#define WALL '1'
using namespace std;
// n*n 을 맵을 인풋으로 받는다.
// 000S
// 0110
// G100
// 0000



class node
{
public:
	int posX;
	int posY;

	int G;
	int H;
	node* parent;

public:

	friend std::less<node>;

};

namespace std
{
	template <> class less<node> {
	public:
		bool operator()(const node &lhs, const node &rhs) const {
			return lhs.G + sqrt(lhs.H) > rhs.G + sqrt(rhs.H);
		}
	};

};


node* AStarSearch(priority_queue<node>& fringe, const node& Start);
bool isClosed(vector<node> closed, node& current);
node makeNode(node current, int nextX, int nextY);


int dX[4] = { -1,0,0,1 };
int dY[4] = { 0,-1,1,0 };


vector<vector<char>> Map;
node Start;
node End;
node current;

int main()
{
	int size = 0;
	

	cin >> size;

	for (int i = 0; i < size; i++)
	{
		vector<char> tmp;
		for (int j = 0; j < size; j++)
		{
			tmp.push_back('-1');
			cin >> tmp[j];
			if (tmp[j] == START) {
				Start.posX = i;
				Start.posY = j;
				
			}
			if (tmp[j] == END) {
				End.posX = i;
				End.posY = j;
			}
		}
		Map.push_back(tmp);
	}

	//initialize Start Node
	Start.G = 0;
	Start.H = (End.posX - Start.posX)*(End.posX - Start.posX)
		+ (End.posY - Start.posY)*(End.posX - Start.posY);
	Start.parent = NULL;
	
	// search start
	priority_queue<node> fringe;
	node* path = AStarSearch(fringe,Start);

	//show path
	node* iter = path;

	while (iter != NULL)
	{
		Map[iter->posX][iter->posY] = '-';
		iter = iter->parent;
	}

	//print map
	for (int i = 0; i < Map.size(); i++)
	{
		for (int j = 0; j < Map.size(); j++)
		{
			cout << Map[i][j]<<" ";
		}
		cout << endl;
	}

    return 0;
}

node* AStarSearch(priority_queue<node>& fringe,const node& Start)
{
	
	vector<node> closed;
	fringe.push(Start);

	while (fringe.size() != 0)
	{
		current = fringe.top();
		fringe.pop();
		//goal test
		if (Map[current.posX][current.posY] == END)return &current; 
		if (!isClosed(closed, current)) {
			closed.push_back(current);
			// 나머지 8방향 석세서 삽입

			for (int i = 0; i < 4; i++)
			{
				//맵초과
				if ( current.posX + dX[i] < 0 || current.posX + dX[i] >= Map.size())continue;
				if ( current.posY + dY[i] < 0 || current.posY + dY[i] >= Map.size())continue;
			
				//벽
				if (Map[current.posX + dX[i]][current.posY + dY[i]]==WALL)continue;
				fringe.push(makeNode(current, current.posX + dX[i], current.posY + dY[i]));
				
			}
		}
		else 
		{
			// 같은 경로로 이미 오거나 갈 예정인 노드에 도착하게 되면 
			// g`값과 기존의 g를 비교하여 작을 경우 fringe에 삽입하여야한다
			// 다만, 현재 문제에서는 다시 도착하게 되는 경우는 처음 온 것보다 도 빠르게 도착할 수가 없음 
		}
	}
	
}

bool isClosed(vector<node> closed,node& current)
{
	for (int i = 0; i < closed.size(); i++){
		if (closed[i].posX == current.posX &&
			closed[i].posY == current.posY)
		{
			return true;
		}
	}
	return false;
}

node makeNode(node current,int nextX,int nextY)
{
	node succ;
	node *next = new node(current);
	succ.posX = nextX;
	succ.posY = nextY;

	//지금까지 온경로 길이
	succ.G = current.G + 1;
	//여기서 G까지의 직선경
	succ.H = (End.posX - succ.posX)*(End.posX - succ.posX)
		+ (End.posY - succ.posY)*(End.posX - succ.posY);

	succ.parent = next;

	return succ;
}
