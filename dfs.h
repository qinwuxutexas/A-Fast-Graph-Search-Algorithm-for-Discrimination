#pragma once
#include<iostream>
#include <fstream>
#include<list>
#include<cmath>
#include <algorithm>
#include <vector>
#include <map>
#include "statistics.h"
#include "feature_rank.h"
using namespace std;
typedef std::vector <std::vector<double>> fvec2d;
typedef std::vector <std::vector<string>> svec2d;

// Graph class represents a directed graph
// using adjacency list representation
class Graph
{
public:
	list<int>* adj;// Pointer to an array containing adjacency lists
	map<int, int> nodedepth;
	double percent; //majority percentage of the overlap region of dt and fa, e.g. 80%
	vector<vector<int>> nodearr;
	// A recursive function used by DFS
	void DFSUtil(int start, bool visited[]);
	int depth, nchild; //depth of tree (e.g. 4), and # of children of each parent node (e.g. 3)
	int totalnode;
	int decisiongoal;
	fvec2d f2d_dt, f2d_fa;
	svec2d name2d_dt, name2d_fa;
	vector<double> dweight_dt, dweight_fa;
	vector<double> opt_para;
	int ndt_obj, nfa_obj;
	vector<int> feaind, feadir;
	map<int,vector<int >> findex_dt;
	map<int, vector<int>> findex_fa;
	std::ofstream resultfile;

	Graph(int, int, int, fvec2d, fvec2d, svec2d,svec2d, vector<double>, vector<double>, vector<double>, int, int, double); // Constructor
	void addEdge();
	void nodebuild();
	double overlap(fvec faf, fvec tf);
	tuple<double, int, double, double> decison(fvec faf, fvec tf, int uplimit, int d);
	tuple <mappair, mappair, int, double, double, double, int, int> get_histo_pair(fvec faf, fvec tf);
	tuple <mappair, mappair, int, double, double, double, int, int> weighted_histopair(fvec faf, fvec dt);
	void DFS(int start);
	map<int, vector<int>> featureindex(map<int, vector<int>> findex, fvec2d arr2d, int row, double min, double max,
		int currentnode, int parentnode);
	void dfs_decision(int parentnode, bool visited[], int nfeature,vector<int> featuredir,
		vector<string> featurenames); // , map<int, vector<int>> findex_dt, map<int, vector<int>> findex_fa);
};

