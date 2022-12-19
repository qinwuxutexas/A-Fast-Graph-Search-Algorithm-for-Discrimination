
#include <cmath>
#include <algorithm> ////https://en.cppreference.com/w/cpp/header/algorithmusing namespace std;
#include "feature_rank.h"
//
double feature_rank::featureoverlap() {
	//int noverlap = ((histo_t[bin - 1].second - histo_t[0].second) + (histo_fa[bin - 1].second - histo_fa[0].second));
	//double histo_overlap = (double)histo_overlap / (double)(nt + nfa);
	int i,pd_low, pd_high, fa_low, fa_high;
	int i1 =0, i2 =0,  i3 =0, i4 = 0;
	
	for (i = 0; i < bin; i++) {
		if (i1 == 1 && i2 == 1 && i3 == 1 && i4 == 1) {
			break;
		}
		else {
			if ((i1 == 0) && ((double)histo_t[i].second / (double)nt >= (1 - percent) / 2)) { pd_low = i; i1 = 1; }
			if ((i2 == 0) && ((double)histo_t[i].second / nt >= percent)) { pd_high = i; i2 = 1; }
			if ((i3 == 0) && ((double)histo_fa[i].second / nfa >= (1 - percent) / 2)) { fa_low = i; i3 = 1; }
			if ((i4 == 0) && ((double)histo_fa[i].second / nfa >= percent)) { fa_high = i; i4 = 1; }
		}
	}
	int low = max(pd_low, fa_low);
	int high = min(pd_high, fa_high);
	if (low > high) { swap(low, high); }

	int nol = (histo_t[high].second - histo_t[low].second) + (histo_fa[high].second - histo_fa[low].second);
	double ol = (double)(nol)/(double)(nt + nfa);
	return ol;
}
//
tuple<int,int, double, double> feature_rank::feature_boundary (int uplimit, int decisiongoal, vector<double> opt_para, int d) {
	//double error, pfa0, pfa, pd_loss0, pd_loss;
	double delta, delta_fa0, delta_fa,delta_pd0, delta_pd;
	double alpha = opt_para[0];
	double t = opt_para[1]; 
	double  c = opt_para[2];
	double delta_max =-(double)(nt*alpha + nfa);
	int direction;
	//double err_min = (nfa + nt)*alpha;
	int i,boundary;
	switch (uplimit) {
	case 0: //cann't have upper lmit for rescue,> = threshold only, this is for rescue
		if (decisiongoal == 1) { //pd rescue
			for (i = 0; i < bin; i++) {
				if (i == 0) {
					//pfa0 = nfa;
					//pd_loss0 = 0;
					//error = pd_loss0 + alpha * pfa0; //for rescue
					delta_pd0 = (double)(nt / nt);
					delta_fa0 = (double)(nfa / nfa);
					//delta = alpha*delta_pd0*pow(t,d)  - delta_fa0/pow(c,d); //nonliner model to meet constraint conditions
					//delta = alpha * delta_pd0  - delta_fa0 / pow(c, d);
					delta = alpha * delta_pd0 - delta_fa0;
					//delta = nfa/nfa-nt/nt; //for FA reduction
				}
				else {
					//pfa0 = ((double)nfa - (double)histo_fa[i - 1].second)/(double)nfa;
					//pd_loss0 = (double)histo_t[i - 1].second / (double)nt;
					//error = alpha*pd_loss0 + pfa0;
					delta_pd0 = (double)(ndt_histo - histo_t[i - 1].second) / double(ndt_histo); //the 1st nt shall not be nt, shall be only updated & alarmed obj
					delta_fa0 = (double)(nfa_histo - histo_fa[i - 1].second) / double(nfa_histo);
					//delta = alpha * delta_pd0 * pow(t, d) - delta_fa0 / pow(c, d);; //for rescue
					delta = alpha * delta_pd0 - delta_fa0;
					//for FA reduction, the "histo_fa" and "hist_t" is switched over for this function, and nt is actually nfa and the actual function effect is as follows: 				
					//delta = (double)(nfa - histo_fa[i - 1].second) / double(nfa) - (double)histo_t[-1].second / (double)nt; //for FA reduction
					//delta + loss = 1
				}
				// > = threshold
				//if (delta >= delta_max) {//
				if ((delta >= delta_max) && (delta_fa0 < c*pow(3/(d+1),2.0))) { //linear model to reach constraint goal with depth gradually

				//err_min = error;
					delta_max = delta;
					direction = 1;//>=
					boundary = i; //>=i
					//pfa = pfa0;
					//pd_loss = pd_loss0;
					delta_fa = delta_fa0;
					delta_pd = delta_pd0;
				}
			}
		}
		else if (decisiongoal ==2) { //fa_reduction, < threhold only
			for (i = 1; i < bin; i++) {
				delta_pd0 = (double)(histo_t[i - 1].second) / double(ndt_histo);
				delta_fa0 = (double)(histo_fa[i - 1].second) / double(nfa_histo);
				//delta = alpha * delta_pd0 * pow(t, d) - delta_fa0 / pow(c, d); //
				delta = alpha * delta_pd0 - delta_fa0; //
				// > = threshold
				if ((delta >= delta_max) && (delta_fa0 < c * pow(3 / (d + 1), 2.0))) {
					delta_max = delta;
					direction = -1;//<
					boundary = i; //>=i
					delta_fa = delta_fa0;
					delta_pd = delta_pd0;
				}
			}
		}
		break;
	case 1: //it could have upper limit, so it has two sub-cases: 1) >= threhold and 2) < threhold		
		for (i = 1; i < bin; i++) {
			// sub-case 1: > = threshold
			//pfa0 = ((double)nfa - (double)histo_fa[i - 1].second);// /(double)nfa;
			//pd_loss0 = (double)histo_t[i - 1].second; ///(double)nt
			//error = pd_loss0 + alpha * pfa0;
			delta_pd0 = (double)(ndt_histo - histo_t[i - 1].second) / double(ndt_histo);
			delta_fa0 = (double)(nfa_histo - histo_fa[i - 1].second) / double(nfa_histo);
			//delta = alpha * delta_pd0 * pow(t, d) - delta_fa0 / pow(c, d);
			delta = alpha * delta_pd0 - delta_fa0;

			//if (error <= err_min) {
			if ((delta >= delta_max) && (delta_fa0 < c* pow(3 / (d + 1), 2.0))) {
				//err_min = error;
				delta_max = delta;
				direction = 1; //>= threhold sub-case
				boundary = i; //>=i
				//pfa = pfa0;
				//pd_loss = pd_loss0;
				delta_fa = delta_fa0;
				delta_pd = delta_pd0;
			}
			
			// sub-case 2: < threshold
			//pfa0 = (double)histo_fa[i - 1].second; // /(double)nfa;
			//pd_loss0= ((double)nt - (double)histo_t[i - 1].second); // /(double)nt
			//error = pd_loss0 + alpha * pfa0; // < threshold
			delta_pd0 = (double)histo_t[i - 1].second / (double)ndt_histo;
			delta_fa0 = (double)histo_fa[i - 1].second / (double)nfa_histo;
			//delta = alpha * delta_pd0 * pow(t, d) - delta_fa0 / pow(c, d);
			delta = alpha * delta_pd0 - delta_fa0;
			//if (error <= err_min) {
			if ((delta >= delta_max) && (delta_fa0 < c * pow(3 / (d + 1), 2.0))) {
				//err_min = error;
				delta_max = delta;
				direction = -1; //<i
				boundary = i; // x < i sub-case
				//pfa = pfa0;
				//pd_loss = pd_loss0;
				delta_pd = delta_pd0;
				delta_fa = delta_fa0;
			}
			//if (error == 0) { return { boundary, direction, pfa };}
		}
		break;
	}
	//return {boundary, direction, pfa, pd_loss};
	return { boundary, direction, delta_pd, delta_fa};
}
//


//double feature_report::featurefilter(int col, double low, double upper, int row, int column) {
//	int m, n;
//	*double arr_filter = new double [m*n];
//
//	int m = 0;
//	for (int i = 0; i < row; i++) {
//		for (int j = 0; j < column; j++) {
//			if (arr[i][j] >= low && arr[i][col] < upper) {
//				/*arr_filter[k][j] = arr[i][j];*/
//				*(arr_filter + m * n + j) = arr[i][j];
//				m++;
//			}
//		}
//	}
//	// deallocate memory
//	delete[] arr_filter;
//	return arr_filter; //the array aftger filtering
//}

