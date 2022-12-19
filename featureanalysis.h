#pragma once

struct featuredata
{
public:

data.alarm.dataset.bagid.feature
data.miss.dataset.bagid.feature

private:


};
//nested structure
struct MyStruct
{
	struct featuredata Add;

};

class statistics
{
public:
	void statistics();
	~statistics();

private:

};

statistics::statistics()
{
}

statistics::~statistics()
{

}

class featurerank
{
public:
	void featurerank();
	~featurerank();
private:


};

featurerank::featurerank()
{

}

featurerank :: ~featurerank()
{

}

class slidewindows
{
public:
	void slidewindows();
	~slidewindows();
	void getvariable();

private:
	double windwidth;
	double pfa_max;
	double pd_min;

};

slidewindows::slidewindows()
{
}

slidewindows::~slidewindows()
{
}