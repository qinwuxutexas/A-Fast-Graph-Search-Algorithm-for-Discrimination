#pragma once
#include <iostream>
#include <vector>
#include "statistics.h"
using namespace std;
typedef std::vector <std::vector<double>> fvec2d;

//https://sites.ualberta.ca/~chipuije/pca_cpp.html PCA analysis but need eigen library which isn't on this PC

class feature_rank {
public:
	feature_rank(mappair histo_fa_, mappair histo_t_, int bin_, int nt_, int nfa_, int ndt_histo_, int nfa_histo_,double percent_) { //features are 2D array input
		histo_fa = histo_fa_;
		histo_t = histo_t_;
		bin = bin_;
		nt = nt_;
		nfa = nfa_;
		ndt_histo = ndt_histo_;
		nfa_histo = nfa_histo_;
		percent = percent_;
	}
	
	double featureoverlap();
	tuple<int, int, double, double>  feature_boundary(int uplimit, int decisiongoal, vector<double> & opt_para, int d);
private:
	int bin;
	int nt;
	int nfa;
	int ndt_histo;
	int nfa_histo;
	double percent;
	mappair histo_fa;
	mappair histo_t;
};

//class feature_report {
//public:
//	feature_report(double* threatfeatures, double* fafeatures) {
//		threatfeatures = threatarrdecisiongoal_
//		faarr = fafeatures;
//	}
//	double *threatarr, *faarr;
//}
