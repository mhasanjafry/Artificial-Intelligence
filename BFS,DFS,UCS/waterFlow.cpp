#include <iostream>
#include <queue>
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <deque>
#include <list>
#include <sstream>
#include <stdio.h>
#include <string.h>

#define MAX 1001 // max number of nodes
#define INF 0x7ffffff
#define HOURS 24 //number of hours in a day

using namespace std;
//using std::string;

// creating nodes
struct vertex
{
	int number;
	string name;
	int cost;
};

string alg;
string line;
string filename = "sampleInput.txt";
string destination_node;
int source;
//int destination;
int rValue; // what is this? return from the algorithms

int numberOfNodes; // number of nodes including source + destinations
int starttime;
int testcases;
std::vector<string> nameOfNodes; // what is this?
//std::vector<string> pathLog; // what is this?
std::vector<int> destination;
istringstream iss;

int martix[MAX][MAX][HOURS]; //adjacency matrix
bool visited[MAX];
int parent[MAX];
int dis[MAX];
int total_cost = 0;

bool openedFlag[MAX]; // open queue
bool closedFlag[MAX]; // closed queue

ifstream ifile;
ofstream outputfile;

list<vertex> opened; //list of opened vertices
list<vertex> closed; //list of closed vertices

int DFS();
int BFS();
int UCS();
int getNumber(string);
string getName(int);
//void printPath(int);
//void printLog();
void Input(int);
void output();
void reset();
bool dfs_compare(const vertex&, const vertex&);
bool ucs_compare(const vertex&, const vertex&);
struct compare;

int main(int argc, char const * argv[])
{
	if (strcmp(argv[1],"-i") == 0){
		filename = argv[2];
	}

	ifstream infile(filename);
    // Using getline() to read one line at a time.
    getline(infile, line);
    iss.str(line);
    iss >> testcases;
    iss.clear();
    infile.close();
    // cout << "Number of testcases:" << testcases << endl;
//    for (int i=1; i< testcases; i++){
    outputfile.open("output.txt");
    for (int i=1; i<= testcases; i++){	
		//cout << "Iteration:" << i << endl;
		Input(i);
		if (alg == "BFS")
		{
			//cout << "BFS Detected" << endl;
			rValue = BFS();
			// cout << "BFS Answer" << rValue << endl;
		}
		else if (alg == "DFS")
		{
			//cout << "DFS Detected" << endl;
			rValue = DFS();
			// cout << "DFS Answer" << rValue << endl;
		}
		else if (alg == "UCS")
		{
			//cout << "UCS Detected" << endl;
			rValue = UCS();
			// cout << "DFS Answer" << rValue << endl;
		}
		output();
		reset();
    }
    outputfile.close();
	return 0;
}

struct compare
{
  bool operator()(const string& left, const string& right)
  {
      return left < right;
  }
};

bool dfs_compare(const vertex& left, const vertex& right)  
{  
  if (left.cost == right.cost) return left.name < right.name;	
  return left.cost == right.cost;
}

int DFS()
{
	list<vertex>::iterator it;
	vector<int>::iterator it2;

	vertex current;
	vertex child;

	vertex start;
	start.number = source;
	start.name = getName(source);
	start.cost = 0;
	parent[start.number] = -1;
	opened.push_front(start);
	// for (unsigned i=0; i<destination.size(); ++i)
 //    	std::cout << ' ' << getName(destination[i]);
 //  	std::cout << '\n';

	for (int i = 0; i < numberOfNodes; ++i)
	{
		openedFlag[i] = false;
		closedFlag[i] = false;
		parent[i] = -1;
	}

	while (1)
	{
		// if destination not found, and all nodes have been explored, quit!
		if (opened.empty()) return -1;

		// start exploring with the node of the highest priority
		current = opened.front();
		// cout << "Expanding Node New:" << current.name << endl;

		// visit node
		opened.pop_front();
		openedFlag[current.number] = false;

		// return cost if find dest
		it2 = find(destination.begin(), destination.end(), current.number);
		if (it2 != destination.end())
		{
			std::cout << "Element found in destination: " << getName(*it2) << ",with cost:" << current.cost << '\n';
			destination_node = getName(*it2);
			return ((starttime+current.cost)%HOURS);
		}
		// cout << "The current node is" << getName(current.number) << endl;
		// for (int ii = 0; ii < numberOfNodes; ++ii){
		// 	cout << "The current matrix is:"<< ii<<": " << martix[current.number][ii][0] << endl;
		// }		

		// search children
		for (int i = 0; i < numberOfNodes; ++i)
		{
			// find child 
			if (martix[current.number][i][(starttime+current.cost)%HOURS] != 0)
			{
//				cout << "For "<< getName(current.number) << "starttime:" << starttime << ",current.cost:" << current.cost << endl;
//				cout << "numberOfNodes:" << numberOfNodes << endl;
				child.number = i;
				child.name = getName(child.number);
				child.cost = current.cost + 1;
//				cout << "Expanding:" << current.name <<",child found: " << child.name << endl;
				// if no node in opened or closed has child's state
				//cout << current.name << "->" << getName(i) << " " << openedFlag[i] << " " << closedFlag[i] << endl;
				if (!openedFlag[i] && !closedFlag[i]) //if node i has never been explored before
				{
					openedFlag[i] = true; //setting it true means that node i has been reached out.
					opened.push_front(child); //pushing the ith node on the open nodes stack
					parent[child.number] = current.number; //setting the ith node parent to where the ith node was expanded
				}
				// else if node i (child) has already been explored before and exists in open nodes stack 
				else if (openedFlag[i])
				{//the next for loop is put in place to find location of child on open nodes stack					
					for (it = opened.begin(); it != opened.end(); it++) 
					{// searching for ith node(child) in open nodes stack
						if (it->number == i) break; // if child found in open nodes stack, our searching is complete
					}
					// if new path to the child node has lower cost than the past cost earlier found, update path cost for child
					if (child.cost < it->cost)
					{
						openedFlag[it->number] = false;
						opened.erase(it);
						openedFlag[child.number] = true;
						opened.push_front(child);
						parent[child.number] = current.number;
					}
				}
				//else if node i (child) exists in closed nodes stack
				else if (closedFlag[i])
				{	// search for location of node i in closed nodes stack
					for (it = closed.begin(); it != closed.end(); it++)
					{
						if (it->number == i) break;
					}
					// if new path to the child node has lower cost than the past cost earlier found, update path cost for child
					if (child.cost < it->cost)
					{
						closedFlag[it->number] = false;
						closed.erase(it);
						openedFlag[child.number] = true;
						opened.push_front(child);
						parent[child.number] = current.number;
					}
				}
			}
		}
		// current node has been expanded and all children have been recovered so close current node
		closedFlag[current.number] = true;
		closed.push_front(current); //add current node to closed nodes stack
		// cout << "Before priortiy queue: ";
		// for (it = opened.begin(); it != opened.end(); it++)
		// {
		// 	cout << it-> name << " ";
		// }
		// cout << endl;

		// put nodes in queue in priority, so that nodes are later expanded in alphabetical order.
		opened.sort(dfs_compare); 

		cout << "Open queue: ";
		for (it = opened.begin(); it != opened.end(); it++)
		{
			cout << it-> name << " ";
		}
		cout << endl;
	}
}
 
bool ucs_compare(const vertex& left, const vertex& right)  
{  
  if (left.cost == right.cost)
  {
  		return left.name < right.name;	
  }
  else
  {
  		return left.cost < right.cost;
  }
}

int UCS()
{
	list<vertex>::iterator it;
	std::vector<int>::iterator it2;

	vertex current, child, start;
	start.number = source;
	start.name = getName(source);
	start.cost = 0;
	parent[start.number] = -1;
	opened.push_front(start);

	for (int i = 0; i < numberOfNodes; ++i){
		openedFlag[i] = false;
		closedFlag[i] = false;
		parent[i] = -1;
	}

	while (1)
	{
		// quit if not find dest
		if (opened.empty())	return -1;

		// current node
		current = opened.front();

		// remove the node that we are currently exploring from openNodes queue
		opened.pop_front();
		openedFlag[current.number] = false; //the current node has now been explored

		// search if the current node is any of the destinations. If yes, return the time it took to get there
		it2 = find(destination.begin(), destination.end(), current.number);
		if (it2 != destination.end()){
//			std::cout << "Element found in destination: " << getName(*it2) << '\n';
			destination_node = getName(*it2);
			return ((starttime+current.cost)%HOURS);
		}

		// search through the list of all nodes to find children of the current node
		for (int i = 0; i < numberOfNodes; ++i){
			// check if any node is a child + available at that time of hour
			if (martix[current.number][i][(starttime+current.cost)%HOURS] != 0){
				child.number = i;
				child.name = getName(child.number);
				child.cost = current.cost + martix[current.number][i][(starttime+current.cost)%HOURS];

				if (!openedFlag[i] && !closedFlag[i]) //if node i has never been explored before
				{
					openedFlag[i] = true; //setting it true means that node i has been reached out.
					opened.push_front(child); //pushing the ith node on the open nodes stack
					parent[child.number] = current.number; //setting the ith node parent to where the ith node was expanded

					openedFlag[i] = true;				
					opened.push_back(child);
					parent[child.number] = current.number;
				}
				// else if there exists child in opened 
				else if (openedFlag[i])
				{
					// find the child in opened
					for (it = opened.begin(); it != opened.end(); it++)
					{
						if (it->number == i)
						{
							break;
						}
					}

					// update child
					if (child.cost < it->cost)
					{
						openedFlag[it->number] = false;
						opened.erase(it);
						openedFlag[child.number] = true;
						opened.push_back(child);
						parent[child.number] = current.number;
					}
				}
				// else if there exists child in closed
				else if (closedFlag[i])
				{
					// find the child in closed
					for (it = closed.begin(); it != closed.end(); it++)
					{
						if (it->number == i)
						{
							break;
						}
					}

					// update child
					if (child.cost < it->cost)
					{
						closedFlag[it->number] = false;
						closed.erase(it);
						openedFlag[child.number] = true;
						opened.push_back(child);
						parent[child.number] = current.number;
					}
				}
			}
		}

		// close current node
		closedFlag[current.number] = true;
		closed.push_back(current);

		// keep priority queue
		opened.sort(ucs_compare);
	}
}

int BFS()
{
	std::vector<int>::iterator it;

	for (int i = 0; i < numberOfNodes; ++i)
	{
		visited[i] = false;
		parent[i] = -1;
		dis[i] = INF;
	}

	int current;
	deque <int> bfs_queue;
	visited[source] = true;
	dis[source] = 0;

	bfs_queue.push_back(source);

	priority_queue<string, vector<string>, compare> pri_queue;

	while (1)
	{
		if (bfs_queue.empty())
		{
			return -1;
		}

		// make queue have alp order
		current = dis[bfs_queue.front()];
		while (!bfs_queue.empty() && current == dis[bfs_queue.front()])
		{
			pri_queue.push(getName(bfs_queue.front()));
			bfs_queue.pop_front();
		}

		while (!pri_queue.empty())
		{
			bfs_queue.push_front(getNumber(pri_queue.top()));
			pri_queue.pop();
		}

		// run BFS
		current = bfs_queue.front();
		bfs_queue.pop_front();

		it = find(destination.begin(), destination.end(), current);
		if (it != destination.end())
		{
			// std::cout << "Element found in destination: " << getName(*it) << '\n';
			destination_node = getName(*it);
			return ((starttime+dis[current])%HOURS);
		}
		
		for (int i = 0; i < numberOfNodes; ++i)
		{
			if (martix[current][i][(starttime+dis[current])%HOURS])
			{
				// If not visited or has smallar cost
				if (!visited[i])
				{	
					visited[i] = true;
					parent[i] = current;
					dis[i] = dis[current] + 1;
					bfs_queue.push_back(i);
				}
			}
		}

	}
}

void Input(int test_case)
{

	int numedges, lengthCost, numofftimes, startedge, endedge, start_time, end_time;
    string dest;
    string tmp;
    string tmp2;
	string srcNmae;
    string startnode, endnode;
    vector<int> myvector (HOURS);
    vector<string> destinationz;
    string delimiter = "-";
    size_t pos;

	// opened input file
    ifstream infile(filename);
  	// Using getline() to read one line at a time.
    getline(infile, line);
	
	for (int tt=1; tt<test_case; tt++){
   	while (getline(infile, line)) {
   		if (line.size() <= 1) break; //if an empty line is encounted, break the loop!
   		}
	}
 	  getline(infile, line);
  	  stringstream ss(line);
  	  ss >> alg;
//  	  cout << "algorithm: " << alg << endl;
  	  ss.clear();
  	  getline(infile, line);
  	  ss.str(line);
  	  ss >> srcNmae;
//  	  cout << "srcNmae: " << srcNmae << endl;
  	  ss.clear();
  	  nameOfNodes.push_back(srcNmae);
  	  getline(infile, line);
      ss.str(line);
//      cout << "destinationz: ";
      while (ss >> dest){
      	nameOfNodes.push_back(dest);
      	destinationz.push_back(dest);
//      	cout << dest << " ";
      }
      ss.clear();
      getline(infile, line);
      ss.str(line);
      while (ss >> tmp) nameOfNodes.push_back(tmp);
	  sort(nameOfNodes.begin(), nameOfNodes.end());
	  numberOfNodes = nameOfNodes.size();
      getline(infile, line);
      iss.str(line);
      iss >> numedges;
//      cout << "numedges: " << numedges << endl;

	// find the position of source and destination	
	vector<string>::iterator it;

	it = find(nameOfNodes.begin(), nameOfNodes.end(), srcNmae);
	source = it - nameOfNodes.begin();
	for (int i=0; i < destinationz.size(); i++){
		it = find(nameOfNodes.begin(), nameOfNodes.end(), destinationz[i]);
		destination.push_back(it - nameOfNodes.begin());
	}
	// add edge offtime information into martix	
    for (int i = 0; i<numedges; i++){
      	ss.clear();
      	getline(infile, line);
      	ss.str(line);
      	ss >> startnode >> endnode >> lengthCost >> numofftimes;      	
      	// find the position of source and destination	
		it = find(nameOfNodes.begin(), nameOfNodes.end(), startnode);
		startedge = it - nameOfNodes.begin();
		it = find(nameOfNodes.begin(), nameOfNodes.end(), endnode);
		endedge = it - nameOfNodes.begin();
     	std::fill (myvector.begin(),myvector.end(),lengthCost);
//     	cout << "Starting edge:"<<getName(startedge) << "End edge:"<<getName(endedge) << endl;
      	if (alg == "UCS"){
      	for (int p=0; p<numofftimes; p++){
      		ss >> tmp2;
			pos = tmp2.find(delimiter);
			start_time = std::stoi (tmp2.substr(0, pos),nullptr,10);
			tmp2.erase(0, pos + delimiter.length());
			end_time = std::stoi (tmp2,nullptr,10);
     	    std::fill (myvector.begin()+start_time,myvector.end()-(HOURS-end_time-1),0); //fill offtimes with zero
      	}      		
      	}
      	for (int q=0; q<HOURS; q++){
     		martix[startedge][endedge][q] = myvector[q];
     	//	cout << "myvector for q is:" << q << " and value = " << martix[startedge][endedge][q] <<endl;    		
      	}
//      	    cout << "myvector for endedge:" << getName(endedge) << ",value = " << martix[startedge][endedge][0] <<endl;
    }

//    	cout << "The current node is" << getName(current.number) << endl;
		// for (int ii = 0; ii < numberOfNodes; ++ii){
		// 	cout << "The current matrix is:"<< ii<<": " << martix[current.number][ii][0] << endl;
		// }

      iss.clear();
      getline(infile, line);
      iss.str(line);
      iss >> starttime;
//      cout << "Input File Read" << endl;	
	// close input file
      infile.close();
}

void reset()
{
	alg.clear();
	line.clear();
	destination_node.clear();
	source = -1;
	rValue = -1;
	numberOfNodes = -1; // number of nodes including source + destinations
	starttime = -1;
	nameOfNodes.clear();
	destination.clear();
	iss.clear();
	memset (visited, 0, sizeof(bool) * MAX);
	memset (openedFlag, 0, sizeof(bool) * MAX);
	memset (closedFlag, 0, sizeof(bool) * MAX);
	memset (parent, -1, sizeof(parent));
	memset (dis, INF, sizeof(dis));
	memset(martix, 0, sizeof(martix));

//int martix[MAX][MAX][HOURS]; //adjacency matrix
//bool visited[MAX];
//int parent[MAX];
//int dis[MAX];
	total_cost = 0;

//bool openedFlag[MAX]; // open queue
//bool closedFlag[MAX]; // closed queue

	opened.clear(); //list of opened vertices
	closed.clear(); //list of closed vertices
}

void output()
{

	if (rValue < 0)
	{
		outputfile << "None" << endl;
	}
	else
	{
		cout << "Output Sent" << endl;
		outputfile << destination_node << " ";
		outputfile << rValue << endl;
	}
}

int getNumber(string name)
{
	vector<string>::iterator it = find(nameOfNodes.begin(), nameOfNodes.end(), name);
	return (it - nameOfNodes.begin());
}

string getName(int number)
{
	return nameOfNodes[number];
}