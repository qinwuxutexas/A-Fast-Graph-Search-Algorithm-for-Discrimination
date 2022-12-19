// a program and multi-branch search-tree algorithm for making automated decision rules
// version: 1.0, author: Qinwu Xu @Leidos Holdings Inc., Date: Feb 2011

#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <vector>
#include <tuple> 
#include <stdexcept>
#include <algorithm>
#include "statistics.h"
#include "feature_rank.h"
#include "readwritefiles.h"
#include "dfs.h"
#include <time.h>
using namespace std;

fvec2d arr2d_filter0(double* arr2d, int rows, int cols, int col, double min, double max) {
	fvec2d array_filter;
	int k = 0;
	for (int i = 0; i < rows; i++) {
		std::vector <double> row1;
		if ((*((arr2d+ i *cols)+col) >= min) && (*((arr2d + i * cols) + col) < max)) {
			for (int j = 0; j < cols; j++) {
				row1.push_back(*((arr2d + i * cols) + j));
			}
			array_filter.push_back(row1);
			k++;
		}
	}
	//cout << *((arr2d + 0 * cols) + col) << endl;
	return array_filter;
	//return *arr;
}

tuple<fvec2d, svec2d, int> arr2d_filter1(fvec2d arr2d, svec2d arr2d1, int row, double min, double max) { //remove columns
	int nd = arr2d[row].size();
	for (int j = 0; j < nd; j++) { //loop over columns given the fixed row number (e.g. row 1 = obj mass)
		if ((arr2d[row][j] < min) || (arr2d[row][j] >= max)) {
			for (unsigned int i = 0; i < arr2d.size(); i++) { //loop over rows, arr2d.size() = total row number
				arr2d[i].erase(arr2d[i].begin() + j); //erase elements of all rows at the jth column
				arr2d1[i].erase(arr2d1[i].begin() + j); // for array of nameinfo accordingly;
			} //arr2d[row].begin(), arr2d[row].end()
			j--;
			nd--;
		}
	}
	return {arr2d, arr2d1, nd};
}
std::tuple <int, double> arr2d_filter (double* arr2d, int rows, int cols, int col, double min, double max) {
	//double array_filter[rows][cols];
	int k = 0, i;
	int* vec = new int [rows];
	for (i = 0; i < rows; i++) {
		if ((*((arr2d + i * cols) + col) >= min) && (*((arr2d + i * cols) + col) < max)) {
			*(vec+k) = i;
			k++;
		}
	}
	double** array_filter = new double* [k];
	for (i = 0; i < k; ++i) {
		array_filter[i] = new double[cols];
	}
	for (int i = 0; i < k; i++) {
		for (int j = 0; j < cols; j++)
			array_filter[i][j] = *((arr2d + (*(vec+i)) * cols) + j);
	}
	return { k,**array_filter }; //<=> return std::make_tuple (k,**array_filter);
}
//

vector<pair<double, int>> vectrank(double vec[], int n) {
	vector<pair<double, int>> vectrank;
	for (int i = 0; i < n; i++) {
		vectrank.push_back(make_pair(vec[i], i)); // k = value, i = original index
	}
	sort(vectrank.begin(), vectrank.end());
	return vectrank;
}

void dfs_call (int decisiongoal,int depth, int nchild, double percent, fvec2d f2d_dt, fvec2d f2d_fa, svec2d name2d_dt,svec2d name2d_fa,
	vector<double> dweight_dt, vector<double> dweight_fa, vector<double> opt_para, int nobj_dt, int nobj_fa,
	vector<int> feaind, vector<int> feadir, vector <string> featurenames) {

	// Create a graph tree	
	Graph g(decisiongoal,depth, nchild, f2d_dt, f2d_fa,name2d_dt, name2d_fa, dweight_dt,dweight_fa, opt_para, nobj_dt, nobj_fa, percent); //input: dpeth, nchild # of each parent node
	g.nodebuild();
	g.addEdge();
	unsigned int i;
	//cout << "Following is Depth First Traversal", to test if the tree node is built correctly, not needed for acutal Rescue/FA reduction
	int start = 0;	
	/*
	g.DFS(start); //input: start node ID
	*/

	
	map<int, vector<int>> findex_dt, findex_fa;

	for (i = 0; i < f2d_dt[0].size(); i++) {
		g.findex_dt[0].push_back(i);
	}
	for (i = 0; i < f2d_fa[0].size(); i++) {
		g.findex_fa[0].push_back(i);
	}
	int nfeature = f2d_dt.size(); //# of features

	bool* visited = new bool[g.totalnode];
	for (int i = 0; i < g.totalnode; i++)
		visited[i] = false;
	g.dfs_decision(start, visited,nfeature, feadir, featurenames); // , findex_dt, findex_fa);	
	g.resultfile.close();

	std::ofstream out("fea_dt_index.csv"); //string inform, dataset, bagID
	for (i = 0; i < findex_dt.size();i++) {
		for (unsigned int j=0;j< findex_dt[i].size(); j++)
			out << findex_dt[i][j] << '\n';
	}
	out.close();
	std::ofstream out1("fea_fa_index.csv"); //string inform, dataset, bagID
	for (i = 0; i < findex_fa.size(); i++) {
		for (unsigned int j = 0; j < findex_fa[i].size(); j++)
			out1 << findex_fa[i][j] << '\n';
	}
	out1.close();
}

int main()
{	

//variables
	int debug = 1; //1: debug with outputs; 2: not
	int i_eigenfile = 1; //1: process data from eigen ccl; 2: just read in already processed data

	int i, j;
	int idebug = 1; //change to 1 if wants to debug to print out vallues
	double min, max, mean, stdev;
	double cov, ztest, covariance, correlation, cross_correlation;
	mappair histo; //histogram;

	//step 0: read configuration file
	std::string configfile = "config.txt";
	tuple <string, vector<int>, vector<string>, vector<int>, vector<int>, vector<int>, vector <int>, vector<double>,
		vector<int>, int, vector<string>, string, vector<vector<string>>, vector<vector<string>>, vector<double> > datainfo = readconfig(configfile);

	string pathname = get<0>(datainfo);
	vector <int> nameinfo = get<1>(datainfo);
	vector <string> featurenames = get<2>(datainfo); //feature names
	vector <int> feaind = get<3>(datainfo); //feature index
	vector <int> feadir = get<4>(datainfo);
	vector<int> treepara = get<5>(datainfo);
	int ndepth = treepara[0], nchild = treepara[1];
	int pathind = nameinfo[0]; //path index (column #)
	int dataset_ind = nameinfo[1]; // -1, why?
	int bagid_col = nameinfo[2];
	int threat_col = nameinfo[3]; //potential threat of true or false
	int nfeature = feaind.size();
	vector <int> denmasscol = get<6>(datainfo);
	int denID = denmasscol[0], massID = denmasscol[1];
	vector <double> denmassminmax = get<7>(datainfo);
	double denmin = denmassminmax[0], denmax = denmassminmax[1], massmin = denmassminmax[2];
	vector<int> objcenter = get<8>(datainfo);
	int decisiongoal = get<9>(datainfo);
	vector<string> eigenfname = get<10>(datainfo);
	string dt_eigenfile = eigenfname[0]; //"dt_eigenccl.csv", 
	string fa_eigenfile = eigenfname[1];// "fa_eigenccl.csv";
	string bbxname = get<11>(datainfo);
	vector<vector<string>> weights_dt = get<12>(datainfo);
	vector<vector<string>> weights_fa = get<13>(datainfo);
	vector <double> opt_para = get<14>(datainfo); //alpha, target of pd rescue or FA reduction, constraint of max FA increase or Pd loss
	
//build bbx map
	unordered_map<std::string, vector<double>> bbxmap = readbbx(bbxname);
	int  nobj_dt, nobj_dt_alarm, nobj_dt_miss, nobj_dt_wb, nobj_fa, nobj_fa_alarm, nobj_fa_miss;
	svec2d name2d_dt_alarm, name2d_fa_alarm, name2d_dt_miss, name2d_fa_miss;
	fvec2d f2d_dt_alarm, f2d_fa_alarm, f2d_dt_miss, f2d_fa_miss;
	vector<double> dweight_dt, dweight_fa; 
	std::ofstream debugFile;
	if (idebug == 1) { std::ofstream debugFile("TimeTrack.csv"); }
	//read eigen_ccl CSV file

	clock_t tStart = clock();
	if (idebug == 1) {
		debugFile << "Start time of detection dataset pre-process and scoring = " << tStart << endl; 
	}
	ifstream ifile;
	string fn1;
	if (decisiongoal == 1) {
		fn1 = "feature_dt_missed.csv";
	}
	else if (decisiongoal == 2) {
		fn1 = "feature_dt_alarmed.csv";
	}

	ifile.open(fn1);
	if (ifile) {
		f2d_dt_alarm = read_numcsv(fn1); //for a single vector just directly assign as: vtest = vec2d[3];
		nobj_dt_alarm = f2d_dt_alarm[0].size();
		string fn2 = "nobj_dt.csv";
		std::ifstream cFile(fn2);
		if (cFile.is_open()) {
			string line; 
			ifstream cFile(fn2);
			//stringstream  lineStream(line);
			while (getline(cFile, line)) {
				nobj_dt = stoi(line); break;
			}
			while (getline(cFile, line)) { 
				nobj_dt_alarm = stoi(line); break;
			}
			while (getline(cFile, line)) {
				nobj_dt_miss = stoi(line); break;
			}
			//dataFile.ignore(1, '\n');
			nobj_dt_wb = nobj_dt_alarm + nobj_dt_miss;
		}
	}
	else {

		tuple <vector <vector<string>>, vector <vector<string>>, vector<vector<string>>, vector<vector<double>>,vector <vector<double>>, vector <vector<double>>, int, int, int, int> dt_eigenout = read2write_dteigen
		(decisiongoal,dt_eigenfile, pathname, nameinfo, pathind, bagid_col, threat_col, objcenter, bbxmap, feaind);

		name2d_dt_alarm = get<1>(dt_eigenout);
		vector<string> bagid_dt_alarm = name2d_dt_alarm[1];
		auto last = std::unique(bagid_dt_alarm.begin(), bagid_dt_alarm.end());
		bagid_dt_alarm.erase(last, bagid_dt_alarm.end());
		int nbagid_dt_alarm = bagid_dt_alarm.size();

		name2d_dt_miss = get<2>(dt_eigenout);
		f2d_dt_alarm = get<4>(dt_eigenout);
		f2d_dt_miss = get<5>(dt_eigenout);

		nobj_dt = get<6>(dt_eigenout); //nobj
		nobj_dt_alarm = get<8>(dt_eigenout);
		nobj_dt_miss = get<9>(dt_eigenout);
		nobj_dt_wb = get<7>(dt_eigenout);//nobj_dt_alarm + nobj_dt_miss; //dt objects within bounding box

		for (i = 0; i < weights_dt.size(); i++) {
			for (j = 0; j < nobj_dt; j++) {
				if (name2d_dt_alarm[j][dataset_ind] == weights_dt[i][1]) {dweight_dt.push_back(stod(weights_dt[i][2]));}
				else {dweight_dt.push_back(1);}
			}
		}
		std::ofstream out("feature_dt_alarmed.csv");
		for (auto& row : f2d_dt_alarm) {
			for (auto col : row)
				out << col << ',';
			out << '\n';
		}
		out.close();
		
		std::ofstream out1("feature_dt_missed.csv");
		for (auto& row : f2d_dt_miss) {
			for (auto col : row)
				out1 << col << ',';
			out1 << '\n';
		}
		out1.close();

		std::ofstream out2("name_dt_alarmed.csv"); //string inform, dataset, bagID
		for (auto& row : name2d_dt_alarm) {
			for (auto col : row)
				out2 << col << ',';
			out2 << '\n';
		}
		out2.close();

		std::ofstream out3("name_dt_missed.csv"); //string inform, dataset, bagID
		for (auto& row : name2d_dt_miss) {
			for (auto col : row)
				out3 << col << ',';
			out3 << '\n';
		}
		out3.close();

		if (dweight_dt.size() > 0) {
			std::ofstream out4("weight_dt.csv"); //string inform, dataset, bagID
			for (auto& row : dweight_dt) {
				out4 << row << '\n';
			}
			out4.close();
		}

		std::ofstream out5("nobj_dt.csv"); //string inform, dataset, bagID
		out5 << nobj_dt << '\n';
		out5 << nobj_dt_alarm << '\n';
		out5 << nobj_dt_miss << '\n';
		out5 << nbagid_dt_alarm << '\n';
		out5.close();
	
	}	

	double tdt;
	if (idebug == 1) {
		tdt = (double)(clock() - tStart);
		debugFile << "used time for pre-process and scoring detection datasets = " << (double)(tdt / CLOCKS_PER_SEC) << endl;
		debugFile << "total obj and alarms of detection dataset =" << nobj_dt << "," << nobj_dt_alarm << endl;
	}
	ifstream ifile2;
	string fn2;
	if (decisiongoal == 1) { //rescue
		fn2 = "feature_fa_missed.csv";
	}
	else if (decisiongoal ==2) { //fa_reduction
		fn2 = "feature_fa_alarmed.csv";
	}

	ifile2.open(fn2);
	if (ifile2) {
		f2d_fa_alarm = read_numcsv(fn2);
		nobj_fa_alarm = f2d_fa_alarm[0].size();		
		//nobj_fa = nalarm_fa; //pre-assumed, not actual value for debug purpose
		string fn3 = "nobj_fa.csv";
		std::ifstream cFile(fn2);
		if (cFile.is_open()) {
			string line;
			stringstream  lineStream(line);
			while (getline(cFile, line)) 
			{ 
				nobj_fa = stoi(line);
				break;
			}
		}
	}
	else {
		tuple <vector <vector<string>>, vector <vector<string>>, vector<vector<string>>, vector<vector<double>>, vector <vector<double>>, vector <vector<double>>, int, int> fa_eigenout = read2write_faeigen
		(decisiongoal,fa_eigenfile, pathname, nameinfo, pathind, bagid_col, threat_col, feaind);
		name2d_fa_alarm = get<1>(fa_eigenout);
		name2d_fa_miss = get<2>(fa_eigenout);
		f2d_fa_alarm = get<4>(fa_eigenout);
		f2d_fa_miss = get<5>(fa_eigenout);
		nobj_fa = get<6>(fa_eigenout);
		nobj_fa_alarm = get<7>(fa_eigenout);
		nobj_fa_miss = nobj_fa - nobj_fa_alarm;
		vector<string> bagid_fa = name2d_fa_alarm[1];
		auto last = std::unique(bagid_fa.begin(), bagid_fa.end());
		bagid_fa.erase(last, bagid_fa.end());
		int nbagid_fa_alarm = bagid_fa.size();

		for (i = 0; i < weights_fa.size(); i++) {
			for (j = 0; j < nobj_fa; j++) {
				if (name2d_fa_alarm[j][dataset_ind] == weights_fa[i][1]) { dweight_fa.push_back(stod(weights_fa[i][2])); }
				else { dweight_fa.push_back(1); }
			}
		}
		std::ofstream out6("feature_fa_alarmed.csv");
		for (auto& row : f2d_fa_alarm) {
			for (auto col : row)
				out6 << col << ',';
			out6 << '\n';
		}
		out6.close();

		std::ofstream out7("feature_fa_missed.csv");
		for (auto& row : f2d_fa_miss) {
			for (auto col : row)
				out7 << col << ',';
			out7 << '\n';
		}
		out7.close();

		std::ofstream out8("nobj_fa.csv"); //string inform, dataset, bagID
		out8 << nobj_fa << '\n';
		out8 << nobj_fa_alarm << '\n';
		out8 << nobj_fa_miss << '\n'; 
		out8 << nbagid_fa_alarm << '\n';
		out8.close();
		
		std::ofstream out9("name_fa_alarmed.csv"); //string inform, dataset, bagID
		for (auto& row : name2d_fa_alarm) {
			for (auto col : row)
				out9 << col << ',';
			out9 << '\n';
		}
		out9.close();
		std::ofstream out10("name_fa_missed.csv");
		for (auto& row : name2d_fa_miss) {
			for (auto col : row)
				out10 << col << ',';
			out10 << '\n';
		}
		out10.close();

		if (dweight_fa.size() > 0) {
			std::ofstream out11("weight_fa.csv"); //string inform, dataset, bagID
			for (auto& row : dweight_fa) {
				out11 << row << '\n';
			}
			out11.close();
		}
	}

	if (idebug == 1) {
		debugFile << "used time for reading fa data = " << (double)(clock() - tdt - tStart) / CLOCKS_PER_SEC << endl;
		debugFile << "total obj and alarms of fa dataset=" << nobj_fa << "," << nobj_fa_alarm << endl;
	}


	//int uplimit = 1; //1: can have uplimit, e.g. density;
	//if (i_eigenfile == 1) {  alpha= nalarm_fa / nobj_dt_alarm; } //weight factor of nobj_fa vs. nt


//recursive decision tree
	double percent = 0.8; //the mjoirity percentage of overlap region of threat and fa data vectors for decision analysis,hard-coded now.
	if (decisiongoal == 1) { //rescue the missed objects of detection datasets
		dfs_call(decisiongoal,ndepth, nchild, percent, f2d_dt_miss, f2d_fa_miss, name2d_dt_miss, name2d_fa_miss, dweight_dt, dweight_fa, opt_para,
			nobj_dt_miss, nobj_fa_miss, feaind, feadir, featurenames);//Build the dfs tree
		//dfs_call(decisiongoal, ndepth, nchild, percent, f2d_dt_miss, f2d_fa_miss, name2d_dt_miss, name2d_fa_miss, dweight_dt, dweight_fa, opt_para,
		//	nobj_dt_wb, nobj_fa, feaind, feadir, featurenames);//Build the dfs tree
	}	

	else if (decisiongoal == 2) { //FA reduction of alarmed objects for FA datasets, f2d_fa and f2d_dt, nobj_dt and nobj_fa are switched over
		dfs_call(decisiongoal,ndepth, nchild, percent, f2d_fa_alarm, f2d_dt_alarm, name2d_fa_alarm, name2d_dt_alarm, dweight_fa, dweight_dt,opt_para,
			nobj_fa_alarm, nobj_dt_alarm, feaind, feadir, featurenames);
		//dfs_call(decisiongoal, ndepth, nchild, percent, f2d_dt_miss, f2d_fa_miss, name2d_dt_miss, name2d_fa_miss, dweight_dt, dweight_fa, opt_para,
		//	nobj_dt_wb, nobj_fa, feaind, feadir, featurenames);//Build the dfs tree
	}
	debugFile.close();
	return 0;

}
