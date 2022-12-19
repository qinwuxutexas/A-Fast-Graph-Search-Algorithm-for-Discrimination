#ifndef readwrite // To make sure don't declare the function more than once by including the header multiple times.
#define readwrite
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <utility> // pair
#include <stdexcept> // runtime_error
#include <sstream> // stringstream
#include <iterator>
#include <algorithm>
#include <iostream>
#include <tuple>
#include <map>
#include <stdio.h>
#include <unordered_map>
using namespace std;
typedef vector<vector<string>> svec2d;
typedef vector<vector<double>> fvec2d;

tuple <string, vector<int>, vector<string>, vector<int>, vector<int>, vector<int>, vector <int>, vector<double>,
	vector<int>, int, vector<string>, string, vector<vector<string>>, vector<vector<string>>, vector<double> > readconfig(string filename);
//return { path,nameind,featurename,featureind,featuredir, treepara, denmasscol, denmassminmax,
//	objcenter, decisiongoal, eigenfname, bbxname };

template <class Container>
void split2(string& str, Container& cont, char delim);
void write_csv(string filename, vector<pair<string, vector<int>>> dataset);
vector<double> convertStringVectortodoubleVector(const vector<string> stringVector);
vector <vector<double>> read_numcsv(string filename);
vector <vector<string>> read_strcsv(string filename);


tuple<vector<pair<string, vector<string>>>, vector<pair<string, vector<string>>>, vector<pair<string, vector<string>>>, int, int, int,int> readstr_dteigencsv(int idecision, string filename,
	string pathname, int pathind,int bagid_col, int threat_col, vector<int> objcenter, unordered_map<string, vector<double>> bbxmap);
tuple <svec2d, svec2d, svec2d, fvec2d, fvec2d, fvec2d, int, int, int, int> read2write_dteigen
(int decisiongoal,string filename, string pathname, vector<int> nameinfo, int pathind, int bagid_col, int threat_col,
	vector<int> objcenter, unordered_map<string, vector<double>> bbxmap, vector<int> find);

tuple<vector<pair<string, vector<string>>>, vector<pair<string, vector<string>>>, vector<pair<string, vector<string>>>, int, int> readstr_faeigencsv(int idecision, vector<pair<string, vector<string>>>, string filename, string pathname, int pathind,
	int bagid_col, int threat_col);
tuple <svec2d, svec2d, svec2d, fvec2d, fvec2d, fvec2d, int, int> read2write_faeigen
(int decisiongoal,string filename, string pathname, vector<int> nameinfo, int pathind, int bagid_col, int threat_col,vector<int> find);

bool findbbx(string bagID, double xcenter, double ycenter, double zcenter, unordered_map<string, vector<double>> mymap);
unordered_map<string, vector<double>> readbbx(string);
#endif