#include "dfs.h"

Graph::Graph(int decisiongoal_,int depth_, int nchild_, fvec2d f2d_dt_, fvec2d f2d_fa_, svec2d name2d_dt_, svec2d name2d_fa_,
	vector<double> weights_dt_, vector<double> weights_fa_,vector<double> opt_para_, int ndt_, int nfa_, double percent_)
{
	totalnode = 0;
	this->depth = depth_;
	this->nchild = nchild_;
	this->f2d_dt = f2d_dt_;
	this->f2d_fa = f2d_fa_;
	this->ndt_obj = ndt_;
	this->nfa_obj = nfa_;
	this->percent = percent_;
	this->name2d_dt = name2d_dt_;
    this->name2d_fa = name2d_fa_;
	this->dweight_dt = weights_dt_;
	this->dweight_fa = weights_fa_;
	this->decisiongoal = decisiongoal_;
	this->opt_para = opt_para_;

	resultfile.open("decisions.csv");
	resultfile << "Node #" << "," << "Tree depth"  << "," <<"Rule" <<"," << "histo_delta_gain" << "," << "histo_delta_loss" << ","
		<< "delta_gain" << "," <<"%delta_gain" << "," <<"delta_loss" << "," <<"%delta_loss"<<endl;

	vector<int> feaind, feadir;
	//myfile << "start of the decision program" << endl;
	for (int d = 0; d < depth+1; d++) {
		totalnode = totalnode + pow(nchild, d);
	}
	adj = new list<int>[totalnode]; //
	//test0 = new list<int>[totalnode];
}

void Graph::nodebuild() {
	int d, k;
	int node = 0;
	for (d = 0; d < depth+1; d++) {
		vector <int> v1;
		for (k = 0; k < pow(nchild, d); k++) {
			v1.push_back(node);
			nodedepth[node]= d;
			node++;
		}
		nodearr.push_back(v1);
	}
}

void Graph::addEdge()
{
	int d, k, j, parent, child;
	for (d = 0; d < depth; d++) {
		for (k = 0; k < pow(nchild, d); k++) {
			parent = nodearr[d][k];
			for (j = 0; j < nchild; j++) {
				child = nodearr[d + 1][k * nchild + j];
				adj[parent].push_back(child);
			}
		}
	}
}

// DFS traversal of the vertices reachable from v.
// It uses recursive DFSUtil()
void Graph::DFS(int start)
{
	// Mark all the vertices as not visited
	bool* visited = new bool[totalnode];
	for (int i = 0; i < totalnode; i++)
		visited[i] = false;
	// Call the recursive helper function
	// to print DFS traversal
	DFSUtil(start, visited);
}

void Graph::DFSUtil(int start, bool visited[]) //add one input: vector<vector<int>> findx
{
	// Mark the current node as visited and
	// print it
	visited[start] = true;
	cout << start << " "; //start node or parent node
	// Recur for all the vertices adjacent to this vertex
	list<int>::iterator i;
	for (i = adj[start].begin(); i != adj[start].end(); ++i) //i is a memoery address, *i is the int value		
		if (!visited[*i])
			DFSUtil(*i, visited);
}

map<int, vector<int>> Graph::featureindex(map<int, vector<int>> findex, fvec2d arr2d, int row, double min, double max,
	int currentnode, int parentnode) {
	//row - given row # of 2d array for a specific feature, e.g for mass or density
	int nd = findex[parentnode].size(); //total column No. of 2d array at given row #
	int fcol; //feature column number
	for (int j = 0; j < nd; j++) { //loop over columns given the fixed row number (e.g. row 1 = obj mass)
		fcol = findex[parentnode][j];
		if ((arr2d[row][fcol] >= min) && (arr2d[row][fcol] < max)) {
			findex[currentnode].push_back(fcol);
		}
	}
	return findex;
}

tuple <mappair, mappair, int, double, double, double, int, int> Graph::get_histo_pair(fvec faf, fvec dt) {
	int nt = dt.size(); //sizeof(dt) / sizeof(dt[0]);
	int nfa = faf.size(); //sizeof(faf) / sizeof(faf[0]);
	double* pfaf = &faf[0]; //convert from vector to array
	statistics sta_fa(pfaf, nfa); //construct a class with fa vector and its size as inputs
	double* pdt = &dt[0];
	statistics sta_t(pdt, nt);
	double min_t, max_t, min_fa, max_fa;
	min_t = sta_t.getmin();
	min_fa = sta_fa.getmin();
	max_t = sta_t.getmax();
	max_fa = sta_fa.getmax();
	double low = std::max(min_t, min_fa);
	double high = std::min(max_t, max_fa);
	if (high < low) { swap(low, high); }
	int npbin = 2; //number of data poiont per bin size
	mappair histo_fa, histo_t;
	//int nbin = int((high - low) / (max_t - min_t) * nt / npbin); //number of bins ?? not good method
	int np = 0;
	for (int i = 0; i < nt; i++) {
		if (dt[i] >= low && dt[i] <= high) {np++;} //the bin # is determined by the # of dt dataset is used because
		//the main goal is to rescue; for FA reduction, the named "dt" dataset here is the actual "FA datasets" by switch over, 
	}

	int nbin = (int)(np/ npbin)+2; //+2 is to add the two ends beyond the overlapped region of [low, high]
	if (nbin < 3) { nbin = 3; }
	double binsize0 = (high - low) / (nbin - 2);//size per bin
	if (dweight_fa.size() > 0) {
		histo_fa = sta_fa.weightedhistogram(nbin, binsize0, low, high, dweight_fa);
	}
	else {
		histo_fa = sta_fa.histogram(nbin, binsize0, low, high); //construct histogram for the input vector
	}
	if (dweight_dt.size() > 0) {
		histo_t = sta_t.weightedhistogram(nbin, binsize0, low, high, dweight_dt);
	}
	else {
		histo_t = sta_t.histogram(nbin, binsize0, low, high);
	}
	return { histo_fa, histo_t, nbin, low, high, binsize0, nt, nfa };
}

double Graph::overlap(fvec faf, fvec dt) {
	tuple <mappair, mappair, int, double, double, double, int, int> histopair = get_histo_pair(faf, dt);
	// { histo_fa, histo_t, nbin, low, high, binsize0, nt, nfa };
	feature_rank feature_ana(get<0>(histopair), get<1>(histopair), get<2>(histopair), ndt_obj,nfa_obj, get<6>(histopair), get<7>(histopair),percent);
	double overlap = feature_ana.featureoverlap();
	return overlap;
}
//
tuple<double, int, double, double> Graph::decison(fvec faf, fvec dt, int uplimit, int d) {
	
	tuple <mappair, mappair, int, double, double, double, int, int> histopair = get_histo_pair(faf, dt);

	mappair histo_fa_ = get<0>(histopair);
	mappair histo_t_ = get<1>(histopair);
	int bin_ = get<2>(histopair);
	int ndt_histo = get<6>(histopair);
	int nfa_histo = get<7>(histopair);
	feature_rank feature_ana(histo_fa_, histo_t_, bin_, ndt_obj, nfa_obj, ndt_histo, nfa_histo, percent);
	tuple<int, int, double, double> outcome = feature_ana.feature_boundary(uplimit, decisiongoal, opt_para,d); //dt is up (2,1,0), x>=8; dt is lower (2,-1,0) x<8
	//return { boundary, direction, delta_pd, delta_fa};
	double bound = get<3>(histopair) + (get<0>(outcome) - 1) * (get<5>(histopair));
	double delta_pd = (double)get<2>(outcome);
	double delta_pfa  = (double)get<3>(outcome);
	return { bound, get<1>(outcome), delta_pd, delta_pfa}; //<double boundary, int direction, pfa%, pd%>
}

void Graph::dfs_decision(int parentnode, bool visited[], int nfeature,vector<int> featuredir,vector<string> featurenames) { 

	visited[parentnode] = true;
	int currentnode;
	double delta, delta_pd, delta_pfa, bounds;
	int dir = 1;
	int row, node = 0;
	double xmin, xmax;
	
	//double ol;
	int i, j, k;
	int uplimit;
	string sign;
	tuple<double, int, double, double> boundary;
	
	//Step 1: find overlap of the vector feature of detection and fa dataset by loop over all feature vectors
	//std::vector <std::pair <double, int>> delta_feaind; //record the overlap and corresponding feature index, redefine for each depth so that it clear out the previous results
	vector < pair<double, vector<double>>> delta_feaind;
	int d = nodedepth[parentnode];
	for (i = 0; i < nfeature; i++) {
		fvec tvec, favec;
		if (d >= depth) { break; }
		else if (d == 0) {
			tvec = f2d_dt[i]; // tarr[j]; //tarr is unit test of detection dataset
			favec = f2d_fa[i];
		}		
		else if(d > 0 && d < depth) {
			for (j = 0; j < findex_dt[parentnode].size(); j++) {
				tvec.push_back(f2d_dt[i][j]); // tarr[j]; //tarr is unit test of detection dataset
			}
			for (j = 0; j < findex_fa[parentnode].size(); j++) {
				favec.push_back(f2d_fa[i][j]);
			}	
		}
		uplimit = featuredir[i];
		boundary = decison(favec, tvec, uplimit, d); //<double boundary, int direction, pfa%, pd%>		
		bounds = get<0>(boundary);
		dir = get<1>(boundary); //1: x>=x0; -1: x < x0;		
		delta_pd = get<2>(boundary);
		delta_pfa = get<3>(boundary);
		//ol = overlap(tvec, favec);
		delta = delta_pd - delta_pfa;
		vector <double> vectemp = { (double)dir, (double)i, delta_pd, delta_pfa, bounds }; //direction, feature id,... 
		//ol_feaind.push_back(make_pair(ol, i));
		delta_feaind.push_back(make_pair(delta, vectemp));
	}
	
	//step 2: reorder overlaps array from smallest to largest
	sort(delta_feaind.rbegin(), delta_feaind.rend()); //from highest to lowest
	//sort(delta_feaind); //from lowest to highest
	// 
	// Recur for all the vertices adjacent to this vertex
	list<int>::iterator m;
	for (m = adj[parentnode].begin(); m != adj[parentnode].end(); ++m) //loop over the parentnode's child nodes (e.g. 3)
	{ //calcuate boundaries of the first 3 features with smallest overlaps 
		k = *m - *adj[parentnode].begin(); //k=0,1,2 for 3 childs
		//favec = f2d_fa[ol_feaind[k].second]; // ordered feature index, e.g. 1 for density
		//tvec = f2d_dt[ol_feaind[k].second]; //
		dir = delta_feaind[k].second[0]; //this is empty

		if (dir == -1) { //<max, can not have upper limit
			xmin = -99999.0;
			xmax = delta_feaind[k].second[4];
			sign = "<";

		}
		else if (dir == 1) { //>= min
			xmin = delta_feaind[k].second[4];
			xmax = 99999.0;
			sign = ">=";
		}

		row = delta_feaind[k].second[1]; //id of feature

		currentnode = *m; //parent node of the next node below it
		findex_dt = featureindex(findex_dt, f2d_dt, row, xmin, xmax, currentnode, parentnode); //start is the parent
		findex_fa = featureindex(findex_fa, f2d_fa, row, xmin, xmax, currentnode, parentnode);
		int ndt_obj_delta = findex_dt[currentnode].size();
		int nfa_obj_delta = findex_fa[currentnode].size();
		resultfile << *m << "," << nodedepth[*m] << "," << featurenames[row]
			<< sign << delta_feaind[k].second[4] << ", " << delta_feaind[k].second[2] << "," << delta_feaind[k].second[3] 
			<<","<<ndt_obj_delta <<","<< (double) ndt_obj_delta/(double)ndt_obj <<"," <<nfa_obj_delta <<","<< (double)nfa_obj_delta/(double)nfa_obj << endl;

		if (!visited[currentnode]) { 
			if (ndt_obj_delta > 0 && nfa_obj_delta > 0) {
				dfs_decision(*m, visited, nfeature, featuredir, featurenames); // , findex_dt, findex_fa);
			}
		}
	}
}
