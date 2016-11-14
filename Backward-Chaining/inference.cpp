#include <iostream>
#include <vector>
#include <fstream>
#include <set>
#include <sstream>
#include <algorithm>
#include <map>
#include <numeric>
#include <cstring>
#include <math.h>

#define RIGHT 1
#define LEFT -1

using namespace std;

struct element
{
	int attr;
	string predicate;
	vector<string> argument;
};

inline bool operator<(const element& lhs, const element& rhs)
{
  if (lhs.attr < rhs.attr){
  	return true;
  }
  else if (lhs.attr > rhs.attr){
  	return false;
  }
  else{
  	if (lhs.predicate < rhs.predicate){
  		return true;
  	}
  	else if (lhs.predicate > rhs.predicate){
  		return false;
  	}
  	else{
  		return lhs.argument < rhs.argument;
  	}
  }
}

struct item
{
	bool isFact;
	vector<element> clause;
};

element split(string str, int attr)
{
	element re;
	re.attr = attr;
	int j = 0;
	for (int i = 0; i < str.size(); ++i)
	{
		if (str[i] == '(')
		{
			re.predicate = str.substr(j, i);
			j = i+1;
		}
		else if ((str[i] == ',') || (str[i] == ')'))
		{
			re.argument.push_back(str.substr(j, i-j));
			j = i+1;
		}
	}
	return re;
}

// void print(element re)
// {
// 	std::cout << re.predicate << "(";
// 	for (int i = 0; i < re.argument.size(); i++){
// 		std::cout << re.argument[i] << ",";
// 	}
// 	std::cout << ")" << std::endl;
// }

bool isEqual(const element & l, const element & r)
{
	if ((l.predicate == r.predicate) && (l.argument == r.argument))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool backwardChain(vector<item> & kb, element q, set<element> & goals)
{
	// find fact
	for (vector<item>::iterator it = kb.begin(); it != kb.end(); it++)
	{
		if (it->isFact && (isEqual(q, it->clause[0])))
		{
			return true;
		}
	}

	bool testing;
	for (vector<item>::iterator it = kb.begin(); it != kb.end(); it++)
	{
		if (!it->isFact)
		{
			for (vector<element>::iterator eright = it->clause.begin(); eright != it->clause.end(); eright++)
			{
				if ((eright->attr == RIGHT) && (isEqual(*eright, q)))
				{
					testing = true;
					for (vector<element>::iterator eleft = it->clause.begin(); eleft != it->clause.end(); eleft++)
					{
						if (eleft->attr == LEFT){
							set<element> goal_new (goals);
							std::pair<std::set<element>::iterator,bool> ret = goal_new.insert(*eleft);
							if ((ret.second==false) || (backwardChain(kb, *eleft,goal_new) == false)){ 
								testing = false;
								break;
							}
						}
					}
					if (testing)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

// void permute(int k, std::vector<int> &arr, std::vector<int> &tempArray){
//     int i;
//     if (k==0){
//         for (int p = 0; p < arr.size(); p++)
//             tempArray.push_back(arr[p]);
//     }
//     else{
//         for (i=k-1;i>=0;i--){
//             std::iter_swap((arr.begin()+i), (arr.begin()+(k-1)));
//             permute(k-1, arr, tempArray);
//             std::iter_swap((arr.begin()+i), (arr.begin()+(k-1)));
//         }
//     }
//     return;
// }

int main(int argc, char const * argv[])
{
	string filename;
	if (argc < 3){
		filename = "input_1.txt";
	}
	else{
		filename = argv[2];
	}

	ifstream infile;
	ofstream ofile;
	infile.open(filename.c_str());
	if (!infile) {
  		std::cout << "Can't open input file:" << filename << ".Correct syntax: ./filename -i inputFile" << endl;
  		return 0;
	}
	
	int numQueries;
	int N;

	// input from the file
	vector<string> in;
	vector<element> queries;
	string qstring;
	string tmp;
	string line;
    
	std::cout << "For large num of variables and kb, the output may take long, as hashing hasnot been implemented. Do not think it goes into while loop!" << std::endl;

    getline(infile, line);
	std::istringstream iss(line);
    iss >> numQueries;
    iss.clear();

    // std::cout << numQueries << std::endl;
	for (int i = 0; i < numQueries; ++i)
	{
  	  	getline(infile, qstring);
		queries.push_back(split(qstring, 0));
	}

    getline(infile, line);
    iss.str(line);
    iss >> N;

	for (int i = 0; i < N; ++i)
	{
  	  	getline(infile, tmp);
		in.push_back(tmp);
	}
	infile.close();


	// Unify the input
	vector<item> kb;
	// save all possible variables
	set<string> variablesInputKB;

		// process input kb
	for (vector<string>::iterator it = in.begin(); it != in.end(); it++)
	{
		item tmpItem;
		element tmpElement;
		string src = *it;
		if (src.find("=>") != std::string::npos)
		{
			int j = 0;
			tmpItem.isFact = false;
			for (int i = 0; i <= src.size(); ++i)
			{
				if (src[i] == '^')
				{
					tmpElement = split(src.substr(j, i-j-1), LEFT);
					tmpItem.clause.push_back(tmpElement);
					j = i+2;
				}
				else if (src[i] == '=')
				{
					tmpElement = split(src.substr(j, i-j-1), LEFT);
					tmpItem.clause.push_back(tmpElement);
					j = i+3;
				}
				else if (i == src.size())
				{
					tmpElement = split(src.substr(j, i-j), RIGHT);
					tmpItem.clause.push_back(tmpElement);					
				}
				for (int i = 0; i < tmpElement.argument.size(); i++){
					if (isupper((tmpElement.argument[i]).at(0))){ //not a variable e.g. x, y, z
						variablesInputKB.insert(tmpElement.argument[i]);
					}
				}
			}
		}
		else
		{
			tmpItem.isFact = true;
			tmpElement = split(src, LEFT);
			for (int i = 0; i < tmpElement.argument.size(); i++){
				if (isupper((tmpElement.argument[i]).at(0))){ //not a variable e.g. x, y, z
					variablesInputKB.insert(tmpElement.argument[i]);
				}
			}
			tmpItem.clause.push_back(tmpElement);
		}
		kb.push_back(tmpItem);
	}

	ofile.open("output.txt");
	// process query clause
	for (int j = 0; j < queries.size(); j++){
		std::set<string> variables (variablesInputKB);
		for (int i = 0; i < queries[j].argument.size(); i++){
			variables.insert(queries[j].argument[i]);
		}

		// create new kb
		vector<item> newkb;
		std::vector<string> varVec(variables.begin(), variables.end());	
		int n = varVec.size();

		for (vector<item>::iterator it = kb.begin(); it != kb.end(); it++)
		{
			set<string> tempVar;
			bool replaced = false;
			for (vector<element>::iterator e = (*it).clause.begin(); e != (*it).clause.end(); e++)
			{
				for (int i = 0; i < e->argument.size(); i++){
					if (!isupper((e->argument[i]).at(0))){
						tempVar.insert(e->argument[i]);
						replaced = true;
					}
				}
			}
			item newItem = *it;
			if (replaced){
				std::vector<string> varClause(tempVar.begin(), tempVar.end());
				int r = varClause.size();

				std::vector<int> permutedArray;
				std::vector<int> temp (r+1,0);

				for (int i = 0; i < ((int)round(pow(n,r))); i++){
					permutedArray.insert(permutedArray.end(), temp.begin()+1, temp.end());
					temp[r] = temp[r] + 1;
					for (int j = temp.size()-1; j>0; j--){
						if (temp[j] == n){
							temp[j] = 0;
							temp[j-1] = temp[j-1] + 1;
						}
					}
				}
				std::map<string, string> mymap;
    			for (int i = 0; i < permutedArray.size(); i++){
    				mymap[varClause[i%r]]=varVec[permutedArray[i]];
    				if ((i+1)%r==0){
    					for (vector<element>::iterator q = newItem.clause.begin(); q != newItem.clause.end(); q++)
						{
							for (int i = 0; i < q->argument.size(); i++){
								if (!isupper((q->argument[i]).at(0))){
									q->argument[i] = mymap.find(q->argument[i])->second;
								}
							}
						}
						newkb.push_back(newItem);
						newItem = *it;
	    				mymap.clear();
    				}
    			}
    			tempVar.clear();
    		}
    		else{
				newkb.push_back(newItem);
    		}
		}
		set<element> goals;
		goals.insert(queries[j]);

		if (backwardChain(newkb, queries[j], goals))
		{
			ofile << "TRUE" << endl;
		}
		else
		{
			ofile << "FALSE" << endl;
		}
		variables.clear();
	}

	ofile.close();

	return 0;
}

				// std::vector<bool> v(n);
				// std::fill(v.begin() + n - r, v.end(), true);
				// do {
    //     			std::vector<int> temp;
    //     			for (int i = 0; i < n; ++i) {
    //         			if (v[i]) {
    //             			temp.push_back(i);
    //         			}
    //     			}
    //     			permute(temp.size(), temp, permutedArray);
    //     			temp.clear();
    // 			} while (std::next_permutation(v.begin(), v.end()));
