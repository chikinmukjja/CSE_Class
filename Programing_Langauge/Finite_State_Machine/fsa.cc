// PL homework: hw1
// fsa.cc
#include <iostream>
#include <vector>
#include <string>
#include "fsa.h"

#define DISABLE_LOG true
#define LOG \
    if (DISABLE_LOG) {} else std::cerr

const char kEps = '#';

using namespace std;
string FindNextState(string cur_state, string cur_input, map<string, string> NFAtable);
string FindAllState(string cur_state, set<string>& state, map<string, string> NFAtable);

bool CheckIfNFA(const TableElement* elements, int num_elements) {
	
	typedef pair < TableElement, int > Key_pair;
	map<TableElement, int> table;

	
	for (int i=0; i < num_elements;i++)
	{
		table.insert(Key_pair(elements[i], elements[i].next_state));
		if (elements[i].input_char == '#')
		{
			
			return true;
		}
	}
	
		if (table.size() == num_elements)return false;
  

  return true;
}

bool BuildDFA(const TableElement* elements, int num_elements,
	const int* accept_states, int num_accept_states,
	FiniteStateAutomaton* fsa) {

	map<string, string>::iterator IterPos;
	fsa->accept = "";
	for (int i = 0; i < num_accept_states-1; i++)
		fsa->accept += accept_states[i] + 48;
	
	string elem;
	string elem2;
	
	for (int j = 0; j < num_elements; j++){
		if (j == 0)fsa->start = elements[j].state + 48;
		 elem = elements[j].state + 48;
		 elem += elements[j].input_char;
		 elem2 = elements[j].next_state+48;
		fsa->table.insert(map<string, string>::value_type(elem, elem2));
	}
    
	/*
	for (IterPos = fsa->table.begin(); IterPos != fsa->table.end(); ++IterPos)
	{
		cout << "root : " << IterPos->first << IterPos->second<< endl;
	}
	*/
	
  LOG << "num_elements: " << num_elements << endl;
  if (num_elements <= 0) return false;
  return true;
}

bool BuildNFA(const TableElement* elements, int num_elements,
	const int* accept_states_array, int num_accept_states,
	FiniteStateAutomaton* fsa) {
	map<string, string> NFAtable;           // NFA 의 룰들을 가지고 있는 table
	map<string, string>::iterator IterPosM; // map의 iterator
	set<string>::iterator IterPosS;         // set의 iterator
	set<string> state;                      // 중복된 상태가 있을시 무시하고 또 정렬해줄 set
	string cur_state;						// set에 들어가는 상태
	vector<string> temp_state;              // 벡터에 임시적으로 넣을 상태
	vector<string>::iterator IterPosV;      // vector를 이용하기위한 iterator

	string input;                  // input리스트
	string cur_input;              // 현재 들어온 input
	string temp_next_state;        // input으로 이동중 임시로 찾을 state

	string temp;                   // input 찾아낼때 쓰는거 
	int check;                     // 어떤 상황에 대해서 한번도 일어나지 않을 경우에 쓰일 check

	fsa->accept;
	for (int i = 0; i < num_accept_states - 1; i++)  //accpet를 받자 set으로 받는게 나중에 버그 처리할 시 상관없을듯 고칠까? 
		fsa->accept += accept_states_array[i] + 48;

//////////////////NFAtable을 만드는부분 함수로 뺼까?////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	string elem;
	string elem2;

	for (int j = 0; j < num_elements; j++)           // 모든 요소를 string으로 처리할 생각이기에 
	{
		if (j == 0)fsa->start = elements[j].state + 48;  // 처음시작상태를 미리 받아놓는다 ! 
		
		// ascii 코드에서 int를 char로 바꿈
		
		elem = j + 48;                               // NFA는 map으로 구성되었기 때문에 중복을 허용하지않는다 따라서 순서를 제일 앞에 추가했다.
		elem += elements[j].state + 48;                 
		elem += elements[j].input_char;
		
		temp = elements[j].input_char;
		
		// 모든 input 찾아내기 

		if (input.length() == 0)input = temp;      //아직 input에 아무것도 없다면 처음 input을 가진 temp를 저장

		for (int k = 0; k < input.length(); k++)   //input string을 순회하면서 현재 temp와 같으면 input string에 저장하지 않는다.
		{
			check = 1;
			if (input.substr(k, 1) == temp)
			{
				check = 0;
				break;
			}
		}

		if (check == 1)input += temp;

		elem2 = elements[j].next_state + 48;      // next string을 NFAtable map의 두번째 요소로 저장한다
		NFAtable.insert(map<string, string>::value_type(elem, elem2));
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*	for (IterPosM = NFAtable.begin(); IterPosM != NFAtable.end(); ++IterPosM)   // 잘들어 갔나 확인
	{
		cout << "root : " << IterPosM->first << IterPosM->second << endl;
	}

	cout << input << endl;
*/
//////////////처음시작상태를 구하는 부분////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//시작상태를 구하자
	string grammer;
	string temp_first_state = NFAtable.begin()->first.substr(1,1);
	
	check = 1;
	
	for (IterPosM = NFAtable.begin(); IterPosM != NFAtable.end(); ++IterPosM )
	{//첫번째 상태에서 #으로갈수 있는 곳이 없으면 그 상태가 시작상태가된다.
		if (temp_first_state == IterPosM->first.substr(1, 1))
			if (IterPosM->first.substr(2, 1) != "#");
			else
			{
				check = 0;
				FindAllState(temp_first_state,state,NFAtable); // #이 있으면 그상태에서 #으로 이동할수 있는 모든상태로 이동하면서 set으로 저장. 
			}
	}

	if (check == 1)
	{//엡실론이 없어서 그냥 첫번째 상태가 시작상태이다
		temp_state.push_back(temp_first_state);
	}
	else
	{// 엡실론이 있어서 set에 스테이트들이 다들어가있다.
		for (IterPosS = state.begin(); IterPosS != state.end(); ++IterPosS)
		{
			temp_first_state += *IterPosS;
		}
		temp_state.push_back(temp_first_state);
	}
//	cout << "NFA first state : " << temp_first_state << endl;
	fsa->start = temp_first_state;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//시작상태에서 input에 따라 갈 수 있는 모든 상태를 구하자
	for (int k = 0; k < temp_state.size(); k++)
	{// vector 안에 저장된 스테이트가 없을 때까지
		cur_state = temp_state[k];
		
		if (cur_state.size() != 1)   // 현재 상태가 1개가 아니다 -> 각각의 상태에서 갈수 있는곳을 찾아야함
		{
			int x;
			if (input.substr(0, 1) == "#")x = 1;
			else x = 0;
				for (; x < input.length(); x++)
				{
					//모든 그상태에 대한 인풋이 없을때까지
					cur_input = input.substr(x, 1);
					temp_next_state = FindNextState(cur_state, cur_input, NFAtable);
					
					fsa->table.insert(map<string, string>::value_type(cur_state + cur_input, temp_next_state));

					// 그렇게 만들어진 state에 대해 현재 모든 스테이트와 비교하여 없으면 삽입
					for (int y = 0; y < temp_state.size(); y++)
					{
						check = 1;
						if (temp_state[y] == temp_next_state){
							check = 0;
							break;
						}
					}
					if (check == 1)
					{
						temp_state.push_back(temp_next_state);
					}
					temp_next_state.clear();

				}
		}
		else        // 현재 상태가 1개이다 그 상태에 대해서만 이동경로 조사
		{
			int i;
			if (input.substr(0, 1) == "#")i = 1;
			else i = 0;

			for (; i < input.length(); i++)
			{

				//모든 그상태에 대한 인풋이 없을때까지
				cur_input = input.substr(i, 1);
				temp_next_state = FindNextState(cur_state, cur_input, NFAtable);
				fsa->table.insert(map<string, string>::value_type(cur_state + cur_input, temp_next_state));

				
					// 그렇게 만들어진 state에 대해 현재 모든 스테이트와 비교하여 없으면 삽입
					for (int y = 0; y < temp_state.size(); y++)
					{
						check = 1;
						if (temp_state[y] == temp_next_state){
							check = 0;
							break;
						}
					}
					if (check == 1)
					{
						temp_state.push_back(temp_next_state);
					}
					temp_next_state.clear();
				
				
			}
		}


	//	for (int k = 0; k < temp_state.size(); k++)
	//	{// vector 안에 저장된 스테이트가 없을 때까지
	//		state.insert(temp_state[k]);
	//	}

		

		
	}
/*	for (IterPosM = fsa->table.begin(); IterPosM != fsa->table.end(); ++IterPosM)
	{
		cout << "DNFTABLE : " << IterPosM->first << " -> " << IterPosM->second << endl;
	}
*/
	return true;
}
string FindAllState(string cur_state,set<string>& state, map<string, string> NFAtable)
{
	map<string, string>::iterator IterPosM;
	string temp_next_state;
//	string next_state;

	pair<set<string>::iterator, bool> result;
	int check = 1;

	// cur_state가 몇개냐에 따라 달라진다
	for (int i = 0; i < cur_state.size(); i++)
	{
		for (IterPosM = NFAtable.begin(); IterPosM != NFAtable.end(); ++IterPosM)
		{//첫번째 상태에서 #으로갈수 있는 곳이 없으면 그 상태가 시작상태가된다.
			if (cur_state.substr(i, 1) == IterPosM->first.substr(1, 1))// table에서 현재상태에 관련된 모든룰에 대하여
			{
				if (IterPosM->first.substr(2, 1) != "#");   // 그상태에 대해 #이 없으면  그상태는 지금 상태로 유지
				else                                        // #이 있으면 다음상태로 이동
				{
					result = state.insert(IterPosM->second);         //  다음상태가 이미 거쳐온 상태라면 insert가 안될것이고 아니라면 insert
					if (result.second)                               // 그 상태가 처음 온 상태라면 그상태로 이동해서 다시 재귀함수로 #여부 확인
						temp_next_state = FindAllState(IterPosM->second,state, NFAtable);
					    // temp_next_state는 의미가없다. 

				}
			}
		}
	}

	return temp_next_state;          // 아직 의미가 없음 지워도 될듯

}
string FindNextState(string cur_state, string cur_input, map<string, string> NFAtable)
{
	map<string, string>::iterator IterPosM;
	map<string, string>::iterator IterPosM2;
	set<string>::iterator IterPosS;
	set<string> state;
	string temp_next_state;


	for (int i = 0; i < cur_state.size(); i++)
	{
		for (IterPosM = NFAtable.begin(); IterPosM != NFAtable.end(); ++IterPosM)
		{// table을 순회하면서 그상태와 인풋에 대한 next을 다찾는다.

			if (IterPosM->first.substr(1, 1) == cur_state.substr(i, 1) &&
				IterPosM->first.substr(2, 1) == cur_input)
			{
				for (IterPosM2 = NFAtable.begin(); IterPosM2 != NFAtable.end(); ++IterPosM2)
				{
					if (IterPosM2->first.substr(1, 1) == IterPosM->second
						&& IterPosM2->first.substr(2, 1) == "#")
						FindAllState(IterPosM->second,state, NFAtable);

				}
				state.insert(IterPosM->second);
				
			}
		}

	}
	for (IterPosS = state.begin(); IterPosS != state.end(); ++IterPosS)
	{
		temp_next_state += *IterPosS;
	}

	return temp_next_state;
}

// Homework 1.1
bool RunFSA(const FiniteStateAutomaton* fsa, const char* str) {
	string input = str;
	string cur_state = fsa->start;
	string grammer = cur_state;
	map<string, string> temp_table = fsa->table;
	map<string, string>::iterator it;
//	cout << "now run fsa" << endl;
	
	for (int i = 0; i < input.length(); i++)
	{
		grammer += input[i];

		it = temp_table.find(grammer);
		if (it == temp_table.end())return  false;
		else if (it->second == "\0")return false;
		else {
			cur_state = it->second;
			//cout << "cur_state : " << cur_state << endl;
			grammer = cur_state;
		}
		
	
	}

	//cur_state안에 accept를 포함하고 있으면 ok
	for (int i = 0; i < fsa->accept.length();i++)
		if (cur_state.find(fsa->accept.substr(i,1))!= -1)return true;
	return false;
}

// Homework 1.1 and 1.2
bool BuildFSA(const TableElement* elements, int num_elements,
              const int* accept_states, int num_accepts,
              FiniteStateAutomaton* fsa) {
  if (CheckIfNFA(elements, num_elements)) {
	  cout<<"NFA"<<endl;
    return BuildNFA(elements, num_elements, accept_states, num_accepts, fsa);
  } else {
	  cout<<"DFA"<<endl;
    return BuildDFA(elements, num_elements, accept_states, num_accepts, fsa);
  }
}

// Homework 1.3
bool BuildFSA(const char* regex, FiniteStateAutomaton* fsa) {
  // Implement this function.
  return false;
}

