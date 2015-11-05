#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <stack>
#include <list>
#include <stdio.h>
#include <string.h>
#include <array>
#include <cstddef>
#include <sstream>

#define INF 0xfffff
#define MAX 12 // max number of children
#define Player2 0
#define Player1 1

using namespace std;

fstream ofile;
int task;
int boardsize;
int matrix[2][MAX] = {{0}};
int max_depth;
string filename = "input_3.txt";
int my_player;
string line;
int sign;

void Input();
bool makeMove(int (*arr)[2][MAX], int player, int x);
int isExistLegalMove(int temp_arr[2][MAX]);
void minmax(int player);
void Greedy(int player);
void alphabeta(int player);
int eVaL(int (*arr)[2][MAX], int player);
int searchDepth(int player, int (*arr)[2][MAX], int depth, string parent_move, bool same_level, vector<vector<int> > &p2DArray);
int searchGreedy(int player, int (*arr)[2][MAX], vector<vector<int> > &p2DArray);
int searchDepthAlphaBeta(int player, int (*arr)[2][MAX], int depth, string parent_move, bool same_level, int alpha, int beta, vector<vector<int> > &p2DArray);
void printMatrix(int (*arr)[2][MAX]);
void printMatrixPointer(int **p2DArray);
void printMatrixVector(vector<vector<int> > &p2DArray);
void captureStones(int (*arr)[2][MAX], int player);
void printNextState(vector<vector<int> > &p2DArray);
void deAllocateMemory(int **p2DArray);
void InitializeMemory(int **p2DArray);

//Read input file name from command line. Many of you had this error for HW-1 and had to be regraded. 

//Create output files using "w" mode on Vocareum so that subsequent execution of your code on Vocareum 
//don't end up appending the output in the existing output files.

int main(int argc, char const * argv[])
{
	if (argc > 2){
		if (strcmp(argv[1],"-i") == 0){
			filename = argv[2];
		}
	}
	Input(); //GET INPUT
	sign = (my_player==Player1) ? 1: -1;
	if (task == 1) //Greedy
	{
		// cout << "Calling Greedy" << endl;
		max_depth = 1;
		Greedy(my_player);
	}
	else if (task == 2) //Min Max
	{
		// cout << "Calling MINMAX" << endl;
		ofile.open("traverse_log.txt", ofstream::out | ofstream::trunc);
		ofile << "Node,Depth,Value" << endl;
		minmax(my_player);
	}
	else if (task == 3) //Alpha Beta
	{
		// cout << "Calling AlphaBeta" << endl;
		ofile.open("traverse_log.txt", ofstream::out | ofstream::trunc);
		ofile << "Node,Depth,Value,Alpha,Beta" << endl;
		alphabeta(my_player);		
	}
	else if (task == 4){
		//Competition
	}

	ofile.close();

	return 0;
}

void Input(){
	ifstream ifile(filename);
	istringstream iss(line);
	ifile >> task;
	ifile >> my_player;
	my_player = my_player%2;
	ifile >> max_depth;
	int i, step;
	getline(ifile, line);
	for (int j = 0; j < 2; j++){
		step = (j+1)%2;
		getline(ifile, line);
		iss.str(line);
		i = 0;
		do{
	    	iss >> matrix[j][i+step];
	    	i++;		
		} while (iss);
    	iss.clear();		
	}
	boardsize = i;
	ifile >> matrix[0][0];
	ifile >> matrix[1][boardsize-1];
	ifile.close();
}

int isExistLegalMove(int temp_arr[2][MAX]){
	bool temp1 = true;
	bool temp2 = true;
	for (int i = 0; i < boardsize-1; i++){
		if (temp_arr[0][i+1]!=0){
			temp1 = false;
		}
		if (temp_arr[1][i]!=0){
			temp2 = false;
		}
	}
	if (temp1 == true) return Player2;
	if (temp2 == true) return Player1;
	return -1;
}

void alphabeta(int player){
	vector<vector<int> > tempArray;
	tempArray.resize(2);
	ofile << "root,0,-Infinity,-Infinity,Infinity" << endl;
	int d = searchDepthAlphaBeta(player, &matrix, 1, "root", false, -INF, INF, tempArray);
	ofile.close();
	// cout << "Best Cost:" << d << endl;
	printNextState(tempArray);
}

int searchDepthAlphaBeta(int player, int (*arr)[2][MAX], int depth, string parent_move, bool same_level, int alpha, int beta, vector<vector<int> > &p2DArray){
 	int temp_alpha;
	int temp_beta;
	string str1;
	string str2;
 	int legal_move = isExistLegalMove((*arr));
 	if(legal_move == -1){
		int step = (player == Player1) ? 0 : 1;
		int x = -1;
		int cost;
		int depth_increase;
		std::string str;
		std::string limit = (depth%2==1) ? "Infinity" : "-Infinity";
		std::string limit2 = (depth%2==1) ? "-Infinity" : "Infinity";
		while (++x < boardsize-1){ //go for all the possible values except mancala
			vector<vector<int> > tempArray;
			tempArray.resize(2);
			if ((*arr)[player][x+step]!=0){ //ignore pits that have no stones
				int temp[2][MAX] = {0};
				memcpy (&temp, arr, sizeof(temp));
				bool s = makeMove(&temp, player, (x+step));
				bool Leaf_cond = true;
				str = ((player==Player1)?"B":"A") + to_string(x + 2);
				if (s){
					ofile << str << "," << depth << "," << limit2 << "," << ((alpha==-INF)?"-Infinity":to_string(alpha)) << "," << ((beta==INF)?"Infinity":to_string(beta)) << endl;
					cost = searchDepthAlphaBeta(player, &temp, depth, str, true, alpha, beta, tempArray);
					Leaf_cond = false;
				}
				else{
					if (depth < max_depth){
						ofile << str << "," << depth << "," << limit << "," << ((alpha==-INF)?"-Infinity":to_string(alpha)) << "," << ((beta==INF)?"Infinity":to_string(beta)) << endl;
						depth_increase = depth + 1;
						cost = searchDepthAlphaBeta(!player, &temp, depth_increase, str, false, alpha, beta, tempArray);
					}
					else{
						cost = eVaL(&temp, player);
						ofile << str << "," << depth << "," << cost << "," << ((alpha==-INF)?"-Infinity":to_string(alpha)) << "," << ((beta==INF)?"Infinity":to_string(beta)) << endl;
					}
				}
				temp_alpha = alpha;
				temp_beta = beta;
				if (depth%2==1){
					if (cost > alpha){
						alpha = cost;
						if (depth==1){
							if (p2DArray.size()!=0){
								p2DArray[0].clear();
								p2DArray[1].clear();	
							}
							for (int i = 0; i < boardsize; i++){
    							p2DArray[0].push_back((Leaf_cond) ? temp[0][i] : tempArray[0][i]);
    							p2DArray[1].push_back((Leaf_cond) ? temp[1][i] : tempArray[1][i]);
    						}
						}
					}
				}
				else{
					if (cost < beta){
						beta = cost;
					}	
				}
				memset(&temp, 0, sizeof(temp));
				if (alpha >= beta){
					str1 = ((temp_alpha==-INF)?"-Infinity":to_string(temp_alpha));
					str2 = ((temp_beta==INF)?"Infinity":to_string(temp_beta));
					ofile << parent_move << "," << ((same_level==true) ? depth : depth-1) << "," << ((depth%2==1)?alpha:beta) << "," << str1 << "," << str2 << endl;
					return ((depth%2==1)?alpha:beta);
				}
				str1 = ((alpha==-INF)?"-Infinity":to_string(alpha));
				str2 = ((beta==INF)?"Infinity":to_string(beta));
				ofile << parent_move << "," << ((same_level==true) ? depth : depth-1) << "," << ((depth%2==1)?alpha:beta) << "," << str1 << "," << str2 << endl;
			}
		}
		return ((depth%2==1)?alpha:beta);
	}
	else{
		captureStones(arr, !legal_move);
		temp_alpha = alpha;
		temp_beta = beta;
		if (depth%2==1){
			alpha = eVaL(arr, player);
			if (depth==1){
				if (p2DArray.size()!=0){
					p2DArray[0].clear();
					p2DArray[1].clear();	
				}
				for (int i = 0; i < boardsize; i++){
	    			p2DArray[0].push_back((*arr)[0][i]);
    				p2DArray[1].push_back((*arr)[1][i]);
    			}
    		}
		}
		else{
			beta = eVaL(arr, player);
		}
		if (alpha >= beta){
			str1 = ((temp_alpha==-INF)?"-Infinity":to_string(temp_alpha));
			str2 = ((temp_beta==INF)?"Infinity":to_string(temp_beta));
		}
		else{
			str1 = ((alpha==-INF)?"-Infinity":to_string(alpha));
			str2 = ((beta==INF)?"Infinity":to_string(beta));
		}
		ofile << parent_move << "," << ((same_level==true) ? depth : depth-1) << "," << ((depth%2==1)?alpha:beta) << "," << str1 << "," << str2 << endl;
		return ((depth%2==1)?alpha:beta);
	}
}

void minmax(int player){
	ofile << "root" << "," << 0 << "," << "-Infinity" << endl;
	vector<vector<int> > tempArray;
	tempArray.resize(2);
	int d = searchDepth(player, &matrix, 1, "root", false, tempArray);
	ofile.close();
	// cout << "Best Cost:" << d << endl;
	printNextState(tempArray);
}

int searchDepth(int player, int (*arr)[2][MAX], int depth, string parent_move, bool same_level, vector<vector<int> > &p2DArray){
 	int max_val;
 	int legal_move = isExistLegalMove((*arr));
 	if(legal_move == -1){
		int step = (player == Player1) ? 0 : 1;
		int x = -1;
		max_val = (depth%2==1) ? -INF : INF;
		int cost;
		int depth_increase;
		std::string str;
		std::string limit = (depth%2==1) ? "Infinity" : "-Infinity";
		std::string limit2 = (depth%2==1) ? "-Infinity" : "Infinity";
		while (++x < boardsize-1){ //go for all the possible values except mancala
			vector<vector<int> > tempArray;
			tempArray.resize(2);
			if ((*arr)[player][x+step]!=0){ //ignore pits that have no stones
				int temp[2][MAX] = {0};
				memcpy (&temp, arr, sizeof(temp));
				bool s = makeMove(&temp, player, (x+step));
				bool Leaf_cond = true;
				str = ((player==Player1)?"B":"A") + to_string(x + 2);
				if (s){
					ofile << str << "," << depth << "," << limit2 << endl;
					cost = searchDepth(player, &temp, depth, str, true, tempArray);
					Leaf_cond = false;
				}
				else{
					if (depth < max_depth){
						ofile << str << "," << depth << "," << limit << endl;
						depth_increase = depth + 1;
						cost = searchDepth(!player, &temp, depth_increase, str, false, tempArray);
					}
					else{
						cost = eVaL(&temp, player);
						ofile << str << "," << depth << "," << cost << endl;
					}
				}
				if (depth%2==1){
					if (cost > max_val){
						max_val = cost;
						if (depth==1){
							if (p2DArray.size()!=0){
								p2DArray[0].clear();
								p2DArray[1].clear();	
							}
							for (int i = 0; i < boardsize; i++){
    							p2DArray[0].push_back((Leaf_cond) ? temp[0][i] : tempArray[0][i]);
    							p2DArray[1].push_back((Leaf_cond) ? temp[1][i] : tempArray[1][i]);
    						}
						}
					}
				}
				else{
					if (cost < max_val){
						max_val = cost;
					}	
				}
			ofile << parent_move << "," << ((same_level==true) ? depth : depth-1) << "," << max_val << endl;
			memset(&temp, 0, sizeof(temp));
			}
		}
		return max_val;			
	}
	else{
		captureStones(arr, !legal_move);
		max_val = eVaL(arr, player);
		ofile << parent_move << "," << ((same_level==true) ? depth : depth-1) << "," << max_val << endl;
		if (depth==1){
			if (p2DArray.size()!=0){
				p2DArray[0].clear();
				p2DArray[1].clear();	
			}
			for (int i = 0; i < boardsize; i++){
    			p2DArray[0].push_back((*arr)[0][i]);
    			p2DArray[1].push_back((*arr)[1][i]);
    		}
		}
		return max_val;
	}
}

int eVaL(int (*arr)[2][MAX], int player){
	return sign*((*arr)[1][boardsize-1] - (*arr)[0][0]);
}

bool makeMove(int (*arr)[2][MAX], int player, int x){
	int dir = (player == Player1) ? 1 : -1;
	int m = player;
	int n = x;
	while ((*arr)[player][x] != 0){
		if (player == Player1){
			if ((n==1) && (m==0)){ //first floor
				m = 1;
				n = -1;
				dir = -dir;
			}
			else if ((n==boardsize-1) && (m==1)){
				m = 0;
				dir = -dir;
				n = boardsize;
			}
		}
		else if (player == Player2){
			if ((n==0) && (m==0)){ //first floor
				m = 1;
				dir = -dir;
				n = -1;
			}
			else if ((n==boardsize-2) && (m==1)){
				m = 0;
				n = boardsize;
				dir = -dir;
			}
		}
		n+= dir;
		(*arr)[m][n]++;
		(*arr)[player][x]--;
	}
	if (player == Player1){
		if ((n==boardsize-1) && (m==1)){
//			cout << "Another Turn has been awarded" << endl;
			return true; //last stone ends in mancala; player gets another turn
		}
		else if (((*arr)[m][n]==1) && (m==Player1)){
//			cout << "(*arr)[m][n]" << (*arr)[m][n] << endl; 
			(*arr)[Player1][boardsize-1] += (*arr)[m][n] + (*arr)[!m][n+1]; //last stone ends in player 1 empty slots:BINGO
			(*arr)[m][n] = 0;
			(*arr)[!m][n+1] = 0;
		}
	}
	if (player == Player2){
		if ((n==0) && (m==0)){
//			cout << "Another Turn has been awarded" << endl;
			return true; //last stone ends in mancala; player gets another turn
		}
		else if (((*arr)[m][n]==1) && (m==Player2)){
			(*arr)[Player2][0] += (*arr)[m][n] + (*arr)[!m][n-1]; //last stone ends in player 1 empty slots:BINGO
			(*arr)[m][n] = 0;
			(*arr)[!m][n-1] = 0;
		}
	}
	return false;
}

void captureStones(int (*arr)[2][MAX], int player){
	int z = (player == Player1) ? (boardsize-1) : 0;
	int step = (player == Player1) ? 0 : 1;
	int x = -1;
	while (++x < boardsize-1){ //sum all the possible values except mancala
		(*arr)[player][z] += (*arr)[player][x+step];
		(*arr)[player][x+step] = 0;
	}
}

void printMatrix(int (*arr)[2][MAX]){
	for (int k = 0; k < 2; k++){
		for (int g = 0; g < boardsize; g++){
			cout << (*arr)[k][g] << " ";
		}
		cout << endl;
	}
}

void printMatrixVector(vector<vector<int> > &p2DArray){
	// cout << p2DArray.size() << endl;
	if (p2DArray[0].size()!=0){
		for (int j = 0; j < 2; j++){
    		for (int i = 0; i < boardsize; i++){
	    		cout << p2DArray[j][i] << " ";
    		}
	    	cout << endl;
		}
	}
	else{
		cout << "Empty Matrix Vector" << endl;
	}
}

void printMatrixPointer(int **p2DArray){
	for (int j = 0; j < 2; j++){
    	for (int i = 0; i < boardsize; i++){
	    	cout << p2DArray[j][i] << " ";
    	}
	    cout << endl;
	}
}

void Greedy(int player){
	vector<vector<int> > p2DArray;
	p2DArray.resize(2);
	int d = searchGreedy(player, &matrix, p2DArray);
	// cout << "Best Greedy Cost:" << d << endl;
	printNextState(p2DArray);
}

int searchGreedy(int player, int (*arr)[2][MAX], vector<vector<int> > &p2DArray){
	// cout << "player:" <<player << endl;
	int legal_move = isExistLegalMove((*arr));
 	if(legal_move == -1){
		int step = (player == Player1) ? 0 : 1;
		int x = -1;
		int max = (player == Player1) ? -INF : INF;
		int cost;
		std::string str;
		while (++x < boardsize-1){ //go for all the possible values except mancala
			vector<vector<int> > tempArray;
			tempArray.resize(2);
			bool Leaf_cond;
			if ((*arr)[player][x+step]!=0){ //ignore pits that have no stones
				int temp[2][MAX] = {0};
				memcpy (&temp, arr, sizeof(temp));
				// cout << "Before Matrix" << endl;
				// printMatrix(&temp);
				bool s = makeMove(&temp, player, (x+step));
				// str = ((player==Player1)?"B":"A") + to_string(x + 2);
				// cout << "After Matrix with move:" << str << endl;
				// printMatrix(&temp);
				if (s){
					cost = searchGreedy(player, &temp, tempArray);
					Leaf_cond = false;
				}
				else{
					cost = eVaL(&temp, player);
					Leaf_cond = true;
				}
				if (cost > max){
					max = cost;
					if (p2DArray.size()!=0){
						p2DArray[0].clear();
						p2DArray[1].clear();	
					}
					for (int i = 0; i < boardsize; i++){
    					p2DArray[0].push_back((Leaf_cond) ? temp[0][i] : tempArray[0][i]);
    					p2DArray[1].push_back((Leaf_cond) ? temp[1][i] : tempArray[1][i]);
    				}
				}
			memset(&temp, 0, sizeof(temp));
			}
		}
		return max;			
	}
	else{
		captureStones(arr, !legal_move);
		if (p2DArray.size()!=0){
			p2DArray[0].clear();
			p2DArray[1].clear();	
		}
		for (int i = 0; i < boardsize; i++){
    		p2DArray[0].push_back((*arr)[0][i]);
    		p2DArray[1].push_back((*arr)[1][i]);
    	}
		return eVaL(arr, player);
	}
}

void deAllocateMemory(int **p2DArray){
	// De-Allocate memory to prevent memory leak
    for (int i = 0; i < boardsize; ++i){
        delete [] p2DArray[i];    	
    }
	delete [] p2DArray;	
}

void printNextState(vector<vector<int> > &p2DArray){
	ofile.open("next_state.txt", ofstream::out | ofstream::trunc);
	int step;
	for (int j = 0; j < 2; j++){
    	step = (j+1)%2;
    	for (int i = 0; i < boardsize-1; i++){
	    	// cout << p2DArray[j][i+step];
	    	// if (i!= boardsize-2) cout << " ";
	    	ofile << p2DArray[j][i+step];
	        if (i!= boardsize-2) ofile << " ";
    	}
	    // cout << endl;
	    ofile << endl;
	} 
    // cout << p2DArray[0][0] << endl;
	ofile << p2DArray[0][0] << endl;
	// cout << p2DArray[1][boardsize-1] << endl;
	ofile << p2DArray[1][boardsize-1] << endl;
	ofile.close();
}