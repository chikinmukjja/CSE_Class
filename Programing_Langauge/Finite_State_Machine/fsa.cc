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
	map<string, string> NFAtable;           // NFA �� ����� ������ �ִ� table
	map<string, string>::iterator IterPosM; // map�� iterator
	set<string>::iterator IterPosS;         // set�� iterator
	set<string> state;                      // �ߺ��� ���°� ������ �����ϰ� �� �������� set
	string cur_state;						// set�� ���� ����
	vector<string> temp_state;              // ���Ϳ� �ӽ������� ���� ����
	vector<string>::iterator IterPosV;      // vector�� �̿��ϱ����� iterator

	string input;                  // input����Ʈ
	string cur_input;              // ���� ���� input
	string temp_next_state;        // input���� �̵��� �ӽ÷� ã�� state

	string temp;                   // input ã�Ƴ��� ���°� 
	int check;                     // � ��Ȳ�� ���ؼ� �ѹ��� �Ͼ�� ���� ��쿡 ���� check

	fsa->accept;
	for (int i = 0; i < num_accept_states - 1; i++)  //accpet�� ���� set���� �޴°� ���߿� ���� ó���� �� ��������� ��ĥ��? 
		fsa->accept += accept_states_array[i] + 48;

//////////////////NFAtable�� ����ºκ� �Լ��� �E��?////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	string elem;
	string elem2;

	for (int j = 0; j < num_elements; j++)           // ��� ��Ҹ� string���� ó���� �����̱⿡ 
	{
		if (j == 0)fsa->start = elements[j].state + 48;  // ó�����ۻ��¸� �̸� �޾Ƴ��´� ! 
		
		// ascii �ڵ忡�� int�� char�� �ٲ�
		
		elem = j + 48;                               // NFA�� map���� �����Ǿ��� ������ �ߺ��� ��������ʴ´� ���� ������ ���� �տ� �߰��ߴ�.
		elem += elements[j].state + 48;                 
		elem += elements[j].input_char;
		
		temp = elements[j].input_char;
		
		// ��� input ã�Ƴ��� 

		if (input.length() == 0)input = temp;      //���� input�� �ƹ��͵� ���ٸ� ó�� input�� ���� temp�� ����

		for (int k = 0; k < input.length(); k++)   //input string�� ��ȸ�ϸ鼭 ���� temp�� ������ input string�� �������� �ʴ´�.
		{
			check = 1;
			if (input.substr(k, 1) == temp)
			{
				check = 0;
				break;
			}
		}

		if (check == 1)input += temp;

		elem2 = elements[j].next_state + 48;      // next string�� NFAtable map�� �ι�° ��ҷ� �����Ѵ�
		NFAtable.insert(map<string, string>::value_type(elem, elem2));
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*	for (IterPosM = NFAtable.begin(); IterPosM != NFAtable.end(); ++IterPosM)   // �ߵ�� ���� Ȯ��
	{
		cout << "root : " << IterPosM->first << IterPosM->second << endl;
	}

	cout << input << endl;
*/
//////////////ó�����ۻ��¸� ���ϴ� �κ�////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//���ۻ��¸� ������
	string grammer;
	string temp_first_state = NFAtable.begin()->first.substr(1,1);
	
	check = 1;
	
	for (IterPosM = NFAtable.begin(); IterPosM != NFAtable.end(); ++IterPosM )
	{//ù��° ���¿��� #���ΰ��� �ִ� ���� ������ �� ���°� ���ۻ��°��ȴ�.
		if (temp_first_state == IterPosM->first.substr(1, 1))
			if (IterPosM->first.substr(2, 1) != "#");
			else
			{
				check = 0;
				FindAllState(temp_first_state,state,NFAtable); // #�� ������ �׻��¿��� #���� �̵��Ҽ� �ִ� �����·� �̵��ϸ鼭 set���� ����. 
			}
	}

	if (check == 1)
	{//���Ƿ��� ��� �׳� ù��° ���°� ���ۻ����̴�
		temp_state.push_back(temp_first_state);
	}
	else
	{// ���Ƿ��� �־ set�� ������Ʈ���� �ٵ��ִ�.
		for (IterPosS = state.begin(); IterPosS != state.end(); ++IterPosS)
		{
			temp_first_state += *IterPosS;
		}
		temp_state.push_back(temp_first_state);
	}
//	cout << "NFA first state : " << temp_first_state << endl;
	fsa->start = temp_first_state;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//���ۻ��¿��� input�� ���� �� �� �ִ� ��� ���¸� ������
	for (int k = 0; k < temp_state.size(); k++)
	{// vector �ȿ� ����� ������Ʈ�� ���� ������
		cur_state = temp_state[k];
		
		if (cur_state.size() != 1)   // ���� ���°� 1���� �ƴϴ� -> ������ ���¿��� ���� �ִ°��� ã�ƾ���
		{
			int x;
			if (input.substr(0, 1) == "#")x = 1;
			else x = 0;
				for (; x < input.length(); x++)
				{
					//��� �׻��¿� ���� ��ǲ�� ����������
					cur_input = input.substr(x, 1);
					temp_next_state = FindNextState(cur_state, cur_input, NFAtable);
					
					fsa->table.insert(map<string, string>::value_type(cur_state + cur_input, temp_next_state));

					// �׷��� ������� state�� ���� ���� ��� ������Ʈ�� ���Ͽ� ������ ����
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
		else        // ���� ���°� 1���̴� �� ���¿� ���ؼ��� �̵���� ����
		{
			int i;
			if (input.substr(0, 1) == "#")i = 1;
			else i = 0;

			for (; i < input.length(); i++)
			{

				//��� �׻��¿� ���� ��ǲ�� ����������
				cur_input = input.substr(i, 1);
				temp_next_state = FindNextState(cur_state, cur_input, NFAtable);
				fsa->table.insert(map<string, string>::value_type(cur_state + cur_input, temp_next_state));

				
					// �׷��� ������� state�� ���� ���� ��� ������Ʈ�� ���Ͽ� ������ ����
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
	//	{// vector �ȿ� ����� ������Ʈ�� ���� ������
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

	// cur_state�� ��Ŀ� ���� �޶�����
	for (int i = 0; i < cur_state.size(); i++)
	{
		for (IterPosM = NFAtable.begin(); IterPosM != NFAtable.end(); ++IterPosM)
		{//ù��° ���¿��� #���ΰ��� �ִ� ���� ������ �� ���°� ���ۻ��°��ȴ�.
			if (cur_state.substr(i, 1) == IterPosM->first.substr(1, 1))// table���� ������¿� ���õ� ���꿡 ���Ͽ�
			{
				if (IterPosM->first.substr(2, 1) != "#");   // �׻��¿� ���� #�� ������  �׻��´� ���� ���·� ����
				else                                        // #�� ������ �������·� �̵�
				{
					result = state.insert(IterPosM->second);         //  �������°� �̹� ���Ŀ� ���¶�� insert�� �ȵɰ��̰� �ƴ϶�� insert
					if (result.second)                               // �� ���°� ó�� �� ���¶�� �׻��·� �̵��ؼ� �ٽ� ����Լ��� #���� Ȯ��
						temp_next_state = FindAllState(IterPosM->second,state, NFAtable);
					    // temp_next_state�� �ǹ̰�����. 

				}
			}
		}
	}

	return temp_next_state;          // ���� �ǹ̰� ���� ������ �ɵ�

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
		{// table�� ��ȸ�ϸ鼭 �׻��¿� ��ǲ�� ���� next�� ��ã�´�.

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

	//cur_state�ȿ� accept�� �����ϰ� ������ ok
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

