#pragma once
#include <iostream>
#include <map>
#include <vector>
typedef std::map<int, std::pair <int, int>> mappair;
typedef std::vector<double> fvec;
using namespace std;

class statistics {
public:
	double* arr;
	int n; //number of vector element
	double getmin();
	double getmax();
	double getmean();
	double stdev();
	double ztest(double x1, double x2, double sigma1, double sigma2);
	mappair histogram(int bin, double binsize, double low, double high);
	mappair weightedhistogram(int bin, double binsize, double low, double high, vector<double> weights);
	statistics(double* arr_, int n_) {
		arr = arr_;
		n = n_;
	}
};

//class array2d {
//public:
//	double* arr2d;
//	std::vector <std::vector<double>> arr2d_filter(int col, double min, double max);
//	int rows, cols;
//	array2d(double* arr2d_, int rows_, int cols_) {
//		arr2d = arr2d_;
//		rows = rows_;
//		cols = cols_;
//	}
//};
