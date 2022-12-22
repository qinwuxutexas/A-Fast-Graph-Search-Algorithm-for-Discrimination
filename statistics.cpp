//Author: Qinwu Xu, Date: 02/2021, at Leidos Inc
#include <iostream>
#include <cmath>
#include <algorithm>
#include <map>
#include <vector>
#include "statistics.h"

double statistics::getmax () {
	int i;
	double max;
	max = arr[0];
	for (i = 1; i < n; i++)
		if (arr[i] > max)
			max = arr[i];
	return max;
}

double statistics::getmin() {
	double min = arr[0];
	for (int i = 1; i < n; i++)
		if (arr[i]<min)
			min = arr[i];
	return min;
}

double statistics::getmean () {
	double sum = 0;
	double mean;
	for (int i = 0; i < n; i++)	{
		sum += arr[i];
	}
	mean = sum/n;
	return mean;
}

double statistics::stdev ()
{
	double sum = 0;
	double average;
	average = getmean();
	for (int i = 0; i < n; i++)	{
		sum += pow(arr[i]-average,2);
	}
	return sqrt(sum / n);
}

mappair statistics::histogram(int bin, double binsize, double low, double high) {
	mappair histo = {};
	for (int i = 0; i < bin; i++) {
		histo[i].first = 0; //initilization of the histogram #
	}

	for (int i = 0; i < n; i++) { //n is the total number of element <> bin, e.g. nt or nfa
		if (arr[i] < low) { //arr is the input of vector, e.g. vec_fa, vec_dt
			histo[0].first++;
		}
		else if (arr[i] >= high) {
			histo[bin-1].first++;
		}
		else {
			int m = int((arr[i] - low) / binsize)+1;
			histo[m].first++;
		}
	}

	histo[0].second = histo[0].first;
	for (int i = 1; i < bin; i++) {
		histo[i].second = histo[i - 1].second + histo[i].first;
	}
	return histo;
}

mappair statistics::weightedhistogram(int bin, double binsize, double low, double high, vector<double> & weights) {
	mappair histo = {};
	for (int i = 0; i < bin; i++) {
		histo[i].first = 0; //initilization of the histogram #
	}

	for (int i = 0; i < n; i++) { //n is the total number of element <> bin, e.g. nt or nfa
		if (arr[i] < low) { //arr is the input of vector, e.g. vec_fa, vec_dt
			histo[0].first += weights[i]; //weights[i]= specificed for attention datasets, otherwise it = 1
		}
		else if (arr[i] >= high) {
			histo[bin - 1].first+=weights[i];
		}
		else {
			int m = int((arr[i] - low) / binsize) + 1;
			histo[m].first+=weights[i];
		}
	}

	histo[0].second = histo[0].first;
	for (int i = 1; i < bin; i++) {
		histo[i].second = histo[i - 1].second + histo[i].first;
	}
	return histo;
}

double statistics::ztest(double x1, double x2, double sigma1, double sigma2) {
	return abs(x1 - x2) / sqrt(pow(sigma1, 2) + pow(sigma2, 2));
}


//std::map<int, std::string> sample_map{ { 1, "one"}, { 2, "two" } };
//sample_map[3] = "three";
//sample_map.insert({ 4, "four" });
//sample_map[5] = "five";
