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
// n*n �� ���� ��ǲ���� �޴´�.
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
			// ������ 8���� ������ ����

			for (int i = 0; i < 4; i++)
			{
				//���ʰ�
				if ( current.posX + dX[i] < 0 || current.posX + dX[i] >= Map.size())continue;
				if ( current.posY + dY[i] < 0 || current.posY + dY[i] >= Map.size())continue;
			
				//��
				if (Map[current.posX + dX[i]][current.posY + dY[i]]==WALL)continue;
				fringe.push(makeNode(current, current.posX + dX[i], current.posY + dY[i]));
				
			}
		}
		else 
		{
			// ���� ��η� �̹� ���ų� �� ������ ��忡 �����ϰ� �Ǹ� 
			// g`���� ������ g�� ���Ͽ� ���� ��� fringe�� �����Ͽ����Ѵ�
			// �ٸ�, ���� ���������� �ٽ� �����ϰ� �Ǵ� ���� ó�� �� �ͺ��� �� ������ ������ ���� ���� 
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

	//���ݱ��� �°�� ����
	succ.G = current.G + 1;
	//���⼭ G������ ������
	succ.H = (End.posX - succ.posX)*(End.posX - succ.posX)
		+ (End.posY - succ.posY)*(End.posX - succ.posY);

	succ.parent = next;

	return succ;
}
