// decisionTree.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <list>

using namespace std;

vector<string> Attribute;
vector<string> classLable;
int classIndex=0;


class node{
public:
	string lable;                       // attribute name
	int lableIndex;
	vector<string> tuple;       // 해당 attribute에 속하는 tuple들
	vector<node> nextNode;              // 다음 속성들
	vector<string> edge;

	void copynode(node &p) {
		this->lable = p.lable;
		this->lableIndex = p.lableIndex;
		this->tuple = p.tuple;
		this->nextNode = p.nextNode;
		this->edge = p.edge;
	};
	bool addNextNode(node N)
	{
		nextNode.push_back(N);
		return true;
	}
	bool addEdge(string edgeName)
	{
		edge.push_back(edgeName);
		return true;
	
	}

	bool addLable(string _lable)
	{
		lable = _lable;

		for (int i = 0; i < Attribute.size(); i++)
		{
			if (Attribute[i] == lable) {
				lableIndex = i;
				break;
			}
		}
		return true;
	}

	int nextNodeIndex(string value)
	{
		for (int i = 0; i < edge.size(); i++)
		{
			if (edge[i] == value)return i;
		}
		return -1;
	}


};

int findMajorty(vector<string>& tuple);
int isClassified(string iter);
bool isAllSameNode(const vector<string>& tuple, const int TargetIndex);
node makeDecisionTree(vector<string>& tuple, vector<string> attribute);
double divideBy(const string attributeName, const vector<string>& tuple, vector<vector<string>>& classified, vector<string>& edgeName,double& splitInfo);
string attributeSelection(const vector<string>& tuple, vector<string>& attribute, vector<vector<string>>& classified, vector<string>& edgeName);
double getInformationGain(const string attributeIndex, const vector<string>& tuple, vector<vector<string>>& classified, vector<string>& edgeName);
void predictClass(node* N, vector<string>& tuple);
double calEntropy(const vector<string>& tuple);

void Tokenize(const string& str, vector<string>& tokens, const string& delimiters = " ");
void showTuple(ofstream& result,const vector<string>& attribute, const vector<string>& tuple);



int main(int argc,char* argv[])
{
	string trainingFileName;
	string testFileName;
	string resultFileName = "dt_result .txt";
	

	if (argc == 3) {
		trainingFileName = argv[1];
		testFileName = argv[2];
	}
	else {
		cout << "argument error" << endl;
		return -1;
	}

	
	resultFileName[resultFileName.size()-5] = testFileName[testFileName.size()-5];

	ifstream trainingFile;
	ifstream testFile;
	ofstream result;


	trainingFile.open(trainingFileName, ios::in);
	testFile.open(testFileName, ios::in);
	result.open(resultFileName, ios::out);

	string fileLine;

	vector<string> attribute;
	vector<string> trainingTuple;
	vector<string> testTuple;

	bool firstLine = true;
	while (trainingFile.good()) {
		getline(trainingFile, fileLine);
		
		if (firstLine) {
			Tokenize(fileLine, attribute, "\t");
			firstLine = false;

		}
		else {
			Tokenize(fileLine, trainingTuple, "\t");
			
		}
	}
	
	firstLine = true;
	while (testFile.good()) {

		getline(testFile, fileLine);
		fileLine = fileLine + "\t ";
		if (firstLine) {
			firstLine = false;
		}else Tokenize(fileLine, testTuple, "\t");
	}

	testTuple.pop_back();  // 마지막 요소제거 (공백)

	Attribute = attribute;
	classIndex = Attribute.size() - 1;
	// classLable 초기화
	for (int i = classIndex; i < trainingTuple.size(); i += Attribute.size())
	{
		if (classLable.size() == 0)classLable.push_back(trainingTuple[i]);
		bool noLable = true;
		for (int j = 0; j < classLable.size(); j++){
			if (classLable[j] == trainingTuple[i]) {
				noLable = false;
				break;
			}
		}

		if (noLable)classLable.push_back(trainingTuple[i]);
	}



	//showTuple(attribute, trainingTuple);
	cout << endl;
	//showTuple(attribute, testTuple);


	node N;
	N = makeDecisionTree(trainingTuple, attribute);
	
	cout << "\n\ntraining is over..."<<endl;
	cout << endl;
	predictClass(&N, testTuple);
	
	
	showTuple(result,attribute, testTuple);
	

    return 0;
}
void predictClass(node* N,vector<string>& tuple)
{	
	for (int i = 0; i < tuple.size(); i += Attribute.size()) {
		node *iter = N;
	
		if (i == 112) {
			cout << "here" << endl;
		}
		bool det = false;
		while (isClassified(iter->lable) == -1) {

			if (iter->nextNodeIndex(tuple[i + iter->lableIndex]) == -1) {
				tuple[i+classIndex]= classLable[findMajorty(iter->tuple)];
				det = true;
				break;
			}
			iter = &(iter->nextNode[iter->nextNodeIndex(tuple[i + iter->lableIndex])]);
		}
		if(det!=true)tuple[i+classIndex] = iter->lable;
	}

}

int isClassified(string iter)
{
	for (int i = 0; i < classLable.size(); i++){
		if (iter == classLable[i])return i;
	}
	return -1;
}

int findMajorty(vector<string>& tuple)
{
	int targetIndex = Attribute.size() - 1;
	int sizeOfAttribute = Attribute.size();
	vector<int> numOfEachLable;
	
	for (int i = 0; i < classLable.size(); i++)
	{
		numOfEachLable.push_back(0);
	}

	for (int i = targetIndex; i < tuple.size(); i += sizeOfAttribute)
	{
		int index = -1;
		if ((index = isClassified(tuple[i])) != -1)numOfEachLable[index]++;
	}
	int max = -1;
	for (int i = 0; i < classLable.size(); i++)
	{
		if (numOfEachLable[i] > max)max = i;
	}

	return max;
}

node makeDecisionTree(vector<string>& tuple,vector<string> attribute)
{
	node N;
	
	//전부 같은 클래스로 분류된다면 -> 말단 노드 반환
	if (isAllSameNode(tuple, classIndex)) {
		
		N.addLable(tuple[classIndex]);     // no or yes
		N.tuple = tuple;
		return N;
	}

	//속성이 남아있지 않다면 다수결의 원칙에 따라서 yes no 결정
	if (attribute.size() == 0) {
		vector<int> numOfLable;
		for (int i = 0; i < classLable.size(); i++) {
			numOfLable.push_back(0);
		}

		for (int i = classIndex; i < tuple.size(); i += Attribute.size())
		{
			int index = -1;
			if ((index = isClassified(tuple[i])) == -1)numOfLable[index]++;
		}

		int max = 0;
		for (int i = 0; i<numOfLable.size(); i++){
			if (numOfLable[i]>max)max = i;
		}

		N.lable = classLable[max];
		N.tuple = tuple;

		return N;
	}

	//attribute selection, attribute를 목록에서 제거
	vector<vector<string>> classified;
	vector<string> edgeName;
	string debug = attributeSelection(tuple, attribute, classified, edgeName);
	if (debug == "NULL") {
		
	}
	N.addLable(debug);
	N.tuple = tuple;

	for (int i = 0; i < classified.size(); i++)
	{
		N.addNextNode(makeDecisionTree(classified[i], attribute));
		N.addEdge(edgeName[i]);
	}

	return N;

}

string attributeSelection(const vector<string>& tuple, vector<string>& attribute, vector<vector<string>>& classified,vector<string>& edgeName)
{
	//informationGain method
	double best = 0.0;
	double tmp = 0.0;
	int attributeName = -1;
	if (attribute.size() == 1) {
	
		cout << "here";
	}

	for (int i = 0; i < attribute.size() - 1; i++) // -1 마지막 속성은 제외함
	{
		vector<vector<string>> tmpClassified;
		vector<string> tmpEdge;
		//	cout<< getInformationGain(i, tuple, classified)<<endl;
		tmp = getInformationGain(attribute[i], tuple, tmpClassified,tmpEdge);
		
		if (best < tmp) {     // 선택된 특성만 저장하기 위해서
			cout << tmp << " ";
			best = tmp;
			attributeName = i;
			classified.clear();
			edgeName.clear();
			
			classified = tmpClassified;
			edgeName = tmpEdge;
		}

	}

	cout << endl;
	if (attributeName == -1)return "NULL";

	string selected = attribute[attributeName];
	attribute.erase(attribute.begin() + attributeName);

	return selected;
}


double getInformationGain(const string attributeName, const vector<string>& tuple, vector<vector<string>>& classified,vector<string>& edgeName)
{
	double splitInfo = 0.0;
	double x = (calEntropy(tuple) - divideBy(attributeName, tuple, classified,edgeName,splitInfo));
	//cout <<attributeName <<"  infogain  " << x<<endl;
	return x/splitInfo;
}

bool isAllSameNode(const vector<string>& tuple, const int TargetIndex)
{
	vector<int> numOfEachClass;
	for (int i = 0; i < classLable.size(); i++)
	{
		numOfEachClass.push_back(0);
	}
	int index = -1;
	int prevIndex = -1;
	for (int i = TargetIndex; i < tuple.size(); i += Attribute.size())
	{
		
		if ((index = isClassified(tuple[i]))!=-1)numOfEachClass[index]++;
		if(prevIndex != -1 && prevIndex != index)return false;
		prevIndex = index;

	}

	
	return true;
}



void Tokenize(const string& str, vector<string>& tokens, const string& delimiters)
{
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos) {

		tokens.push_back(str.substr(lastPos, pos - lastPos));

		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}

}

void showTuple(ofstream& result,const vector<string>& attribute, const vector<string>& tuple) {

	for (int i = 0; i < attribute.size(); i++)
	{
		if ((i %attribute.size()) == classIndex)result << attribute[i];
		else result << attribute[i]<<"\t";
	}
	result << endl;
	for (int i = 0; i < tuple.size(); i++) {
		

		if ((i %attribute.size()) == classIndex)result << tuple[i] << endl;
		else result << tuple[i] << "\t";
	}

	cout << endl;

}
//  나누는 기준 attribute, 나눌 투퓰들, 나눠진 클래스들, 나눠진 edge
double divideBy(const string attributeName, const vector<string>& tuple, vector<vector<string>>& classified, vector<string>& edgeName,double& splitInfo)
{
	int numOfAllTuple = tuple.size();
	int attributeIndex = -1;
	int sizeOfAttribute = Attribute.size();
	

	//find Attribute Index;
	for (int i = 0; i < Attribute.size(); i++)
	{
		if (attributeName == Attribute[i])attributeIndex = i;
	}
	// 분류를 시작함
	for (int i = attributeIndex; i < tuple.size(); i += sizeOfAttribute) {
		//가장 처음 초기화
		if (i == attributeIndex) {
			//분류항목 초기화
			vector<string> tmp;
			for (int j = i - attributeIndex; j < sizeOfAttribute; j++){
				tmp.push_back(tuple[j]);
			}	
			classified.push_back(tmp);
			//edge 추가
			edgeName.push_back(classified[edgeName.size()][attributeIndex]);
		}
		else {
			bool isNew = true;
			//기존의 튜플인지 검사
			for (int j = 0; j < classified.size(); j++){
				if (classified[j][attributeIndex] == tuple[i]) {
					// 클래수에 추가
					for (int k = i - attributeIndex; k <i - attributeIndex + sizeOfAttribute; k++) {
						classified[j].push_back(tuple[k]);
					}
					isNew = false;
					break;
				}
			}

			if (isNew) {
				//분류추가
				vector<string> tmp;
				for (int j = i - attributeIndex; j < i - attributeIndex + sizeOfAttribute; j++) {
					tmp.push_back(tuple[j]);
				}
				classified.push_back(tmp);
				//edge 추가
				edgeName.push_back(classified[edgeName.size()][attributeIndex]);
			}

		}

	}
	// entropy 계산
	double entropy = 0.0;
	splitInfo = 0.0;
	for (int i = 0; i < classified.size(); i++)
	{
		double weight = classified[i].size() /(double)numOfAllTuple;
		entropy = entropy + weight*calEntropy(classified[i]);
		splitInfo = splitInfo - weight*(log(weight) / log(2));

	}
	
	//cout << entropy;
	return entropy;//splitInfo;

	
}

double calEntropy(const vector<string>& tuple)
{
	double entropy = 0.0;
	vector<int> numOfEachLable;
	int targetIndex = Attribute.size()-1;
	int sizeOfAttribute = Attribute.size();

	for (int i = 0; i < classLable.size(); i++)
	{
		numOfEachLable.push_back(0);
	}
	
	for (int i = targetIndex; i < tuple.size();i += sizeOfAttribute)
	{
		int index = -1;
		if((index = isClassified(tuple[i])) != -1)numOfEachLable[index]++;
	}

	for (int i = 0; i < classLable.size(); i++)
	{
		double p = numOfEachLable[i] / ((double)tuple.size()/sizeOfAttribute);
		if (p != 0)entropy = entropy - p*(log(p) / log(classLable.size()));
	}

	return entropy;
}

