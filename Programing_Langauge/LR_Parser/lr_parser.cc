// PL homework: hw2
// lr_parser.cc

#include <assert.h>

#include <iostream>
#include <vector>
#include <stack>

#include "lr_parser.h"

#define DISABLE_LOG false
#define LOG \
    if (DISABLE_LOG) {} else std::cerr

using namespace std;

bool BuildLRParser(const LRTableElement* elements, int num_elements,
                   const LRRule* rules, int num_rules,
                   LRParser* lr_parser) {

  string temp;
  string tempS;
  int temp2;

  for (int i = 0; i < num_elements; i++)
  {
	  temp.clear();
	  tempS.clear();
	  if (elements[i].action == GOTO)
	  {//map<string, string>::value_type(elem, elem2));
		  temp = elements[i].state + 48;  
		  temp += elements[i].symbol;      // 하나의 키로 만들기위해

		  temp2 = elements[i].next_state;  // 다음스테이트

		  lr_parser->GotoT.insert(map<string, int>::value_type(temp, temp2));

	  }// goto 인지 ?
	  else
	  {
		  temp = elements[i].state + 48;
		  temp += elements[i].symbol;      // 하나의 키로 만들기위해
		  
		  tempS = elements[i].action;
		  tempS += elements[i].next_state;      // 하나의 값으로 만들기위해
		  
		  lr_parser->ActionT.insert(map<string, string>::value_type(temp, tempS));
	  }
  }

  for (int i = 0; i < num_rules; i++)
  {
	  lr_parser->Rule.push_back(rules[i]);
  }

  return true;
}

bool RunLRParser(const LRParser* lr_parser, const char* str) {

	map<string,string> actionT = lr_parser->ActionT;
	map<string, int> gotoT = lr_parser->GotoT;
	vector<LRRule> rule = lr_parser->Rule;
	stack<int> S;
	stack<int> input;
	string key;
	int action;
	string tstr = str;
	map<string, string>::iterator it;
	map<string, int>::iterator gotoIt;
	int temp_go;
	for (unsigned i = 1; i <= tstr.length(); i++)
		input.push(tstr[tstr.length() - i]);
	S.push(0); // start state
	
	while (true)
	{
		key = S.top()+48;
		key += input.top();
		//action 찾기
		it = actionT.find(key);
		if (it == actionT.end())return false;

		action = it->second[0]; // action

		switch (action)
		{
			case 1: // shift
				S.push(input.top());
				input.pop();
				S.push(it->second[1]);// push next state
				break;
			case 2: // reduce
				
				for (int i = 0; i < rule[it->second[1] - 1].num_rhs; i++){
					S.pop(); // state 빼고
					S.pop(); // symbol 빼고
				}
				key.clear();
				key = S.top()+48;
				key += rule[it->second[1] - 1].lhs_symbol;

				gotoIt = gotoT.find(key);

				S.push(rule[it->second[1] - 1].lhs_symbol);
				if (gotoIt != gotoT.end())S.push(gotoIt->second);  //state 삽입
				else return false;
				break;
			case 3: // accept
				return true;
		}
	}

  return false;
}

