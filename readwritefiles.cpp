//Author: qinwu xu, June 2021,Version: 1.0
//data pre-process
#include "readwritefiles.h"

tuple <string, vector<int>, vector<string>, vector<int>, vector<int>, vector<int>, vector <int>, vector<double>,
    vector<int>, int, vector<string>, string, vector<vector<string>>, vector<vector<string>>, vector<double> > readconfig(string filename)
    //    return {path,nameind,featurename,featureind,featuredir, treepara, denmasscol, denmassminmax,
    //objcenter, decisiongoal, eigenfname, bbxname};
{
    ifstream cFile(filename);
    string path;
    vector <int> nameind;
    vector<string> featurename;
    vector <int> featureind;
    vector <int> featuredir;
    vector<int> treepara; //depth and # of child of each parent node
    vector<int> denmasscol;
    vector<double> denmassminmax;
    vector <int> objcenter;
    vector<vector<string>> weights_dt, weights_fa;
    int decisiongoal; //1 - rescue, 2 - fa reduction, 3 - general decision
    vector<string> eigenfname;
    string bbxname;
    vector<double> opt_para; //optimization parameters, alpha--weight of targets between Pd rescue and FA increase, 
    //t--target of Pd rescue, c -- constraint as max FA increase
   /* map <string, int> f_ind_name;*/

    if (cFile.is_open())
    {
        string line;
        ifstream cFile(filename);

        int row = 0;
        while (getline(cFile, line))
        {
            row += 1;
            stringstream  lineStream(line);
            string cell;
            int col = 0;
            if (line.empty() || line[0] == '#')
            {
                continue;
            }
            else if (line[0] == 'dt') { //line start to have weights of attention dataset                
                vector<string> weightrow;
                getline(lineStream, cell, ',');  //the 1st element of line is 'w', doing nothing
                while (getline(lineStream, cell, ',')) { //record the 2nd and 3rd element: dataset name and weight
                    weightrow.push_back(cell);
                }
                weights_dt.push_back(weightrow);
            }
            else if (line[0] == 'fa') { //line start to have weights of attention dataset                
                vector<string> weightrow;
                getline(lineStream, cell, ',');  //the 1st element of line is 'w', doing nothing
                while (getline(lineStream, cell, ',')) { //record the 2nd and 3rd element: dataset name and weight
                    weightrow.push_back(cell);
                }
                weights_fa.push_back(weightrow);
            }
            else
            {
                if (row == 2) {
                    path = line;
                }
                else if (row == 4) {
                    while (getline(lineStream, cell, ','))
                    {
                        nameind.push_back(stoi(cell)); //column No. for path, dataset, bagID,and potential threat
                    }
                }
                else if (row == 6) {
                    while (getline(lineStream, cell, ','))
                    {
                        featurename.push_back(cell);
                    }
                }

                else if (row == 7) {
                    while (getline(lineStream, cell, ','))
                    {
                        featureind.push_back(stoi(cell));
                    }
                }
                else if (row == 8) {
                    while (getline(lineStream, cell, ','))
                    {
                        featuredir.push_back(stoi(cell));
                    }
                }
                else if (row == 10) {
                    while (getline(lineStream, cell, ','))
                    {
                        treepara.push_back(stoi(cell));
                    }
                }
                else if (row == 12) {
                    while (getline(lineStream, cell, ','))
                    {
                        denmasscol.push_back(stoi(cell)); //col # of density and mass
                    }
                }
                else if (row == 14) {
                    while (getline(lineStream, cell, ','))
                    {
                        denmassminmax.push_back(stod(cell)); //min and max of density and min of mass
                    }
                }
                else if (row == 16) {
                    while (getline(lineStream, cell, ','))
                    {
                        objcenter.push_back(stoi(cell)); //col location of objecenter at eigenc ccl
                    }
                }
                else if (row == 18) {
                    decisiongoal = stoi(line); //1, 2, or 3: rescue, FA reduction, or general decision
                }
                else if (row == 20) {
                    while (getline(lineStream, cell, ','))
                    {
                        eigenfname.push_back(cell); //dt and fa eigen excel file names
                    }
                }
                else if (row == 22) {
                    bbxname = line; // cell; //bbx file name
                }
                else if (row == 24) {
                    while (getline(lineStream, cell, ','))
                    {
                        opt_para.push_back(stod(cell)); // cell; //bbx file name
                    }
                }
            }
        }
    }
    else
    {
        cerr << "couldn't open config file for reading.\n";
    }
    return { path,nameind,featurename,featureind,featuredir, treepara, denmasscol, denmassminmax,
        objcenter, decisiongoal, eigenfname, bbxname, weights_dt,weights_fa, opt_para };
}

template <class Container>
void split1(const string& str, Container& cont)
{
    istringstream iss(str);
    copy(istream_iterator<string>(iss),
        istream_iterator<string>(),
        back_inserter(cont));
}

template <class Container>
void split2(const string& str, Container& cont, char delim = ',')
{
    stringstream ss(str);
    string token;
    while (getline(ss, token, delim)) {
        cont.push_back(token);
    }
}
void split3(const string& str, vector<pair<string, vector<string>>> result, char delim = ',')
{
    int colIdx = 0;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delim)) {
        result.at(colIdx).second.push_back(token);
    }
    colIdx++;
}
vector<double> convertStringVectortodoubleVector0(vector<string>& stringVector) {
    vector<double> fvect(stringVector.size());
    transform(stringVector.begin(), stringVector.end(), fvect.begin(), [](string& val)
        {
            return stod(val);
        });
    return fvect;
}
vector<double> convertStringVectortodoubleVector(vector<string> stringVector) {
    vector<double> fvect(stringVector.size());
    for (int i = 0; i < stringVector.size(); i++) {
        fvect [i]= stod (stringVector[i]);
    }
    return fvect;
}

void write_csv(string filename, vector<pair<string, vector<int>>> dataset) {
    // Make a CSV file with one or more columns of integer values
    // Each column of data is represented by the pair <column name, column data>
    //   as pair<string, vector<int>>
    // The dataset is represented as a vector of these columns
    // Note that all columns should be the same size

    // Create an output filestream object
    ofstream myFile(filename);

    // Send column names to the stream
    for (int j = 0; j < dataset.size(); ++j)
    {
        myFile << dataset.at(j).first;
        if (j != dataset.size() - 1) myFile << ","; // No comma at end of line
    }
    myFile << "\n";

    // Send data to the stream
    for (int i = 0; i < dataset.at(0).second.size(); ++i)
    {
        for (int j = 0; j < dataset.size(); ++j)
        {
            myFile << dataset.at(j).second.at(i);
            if (j != dataset.size() - 1) myFile << ","; // No comma at end of line
        }
        myFile << "\n";

    }

    // Close the file
    myFile.close();
}

vector<vector<double>> read_numcsv(string filename) {
    // Create a vector of <string, int vector> pairs to store the result
    vector<vector<double>> result;
    ifstream myFile(filename);
    if (!myFile.is_open()) throw runtime_error("Could not open file");
    string line1;
    int nrow;
    string val;

    // Read data, line by line after the 1st line of header
    while (getline(myFile, line1))
    {
        // Create a stringstream of the current line
        stringstream ss(line1);
        vector<string> words;
        split2(line1, words);
        vector<double> row;
        for (int i = 0; i < words.size(); i++) {
            val = words[i];
            row.push_back(stod(val));
        }
        result.push_back(row);
        }

    myFile.close();
    return result;
}

vector<vector<string>> read_strcsv(string filename) {
    // Create a vector of <string, int vector> pairs to store the result
    vector<vector<string>> result;
    ifstream myFile(filename);
    if (!myFile.is_open()) throw runtime_error("Could not open file");
    string line1;
    int nrow;
    string val;

    // Read data, line by line after the 1st line of header
    while (getline(myFile, line1))
    {
        // Create a stringstream of the current line
        stringstream ss(line1);
        vector<string> words;
        split2(line1, words);
        vector<string> row;
        for (int i = 0; i < words.size(); i++) {
            val = words[i];
            row.push_back(val);
        }
        result.push_back(row);
    }

    myFile.close();
    return result;
}

tuple <vector<pair<string, vector<string>>>, vector<pair<string, vector<string>>>, vector<pair<string, vector<string>>>, int, int> readstr_faeigencsv
(int idecision, string filename, string pathname, int pathind,int bagid_col, int threat_col) {

    // Reads a CSV file into a vector of <string, vector<int>> pairs where
    // each pair represents <column name, column values>

    // Create a vector of <string, int vector> pairs to store the result
    vector<pair<string, vector<string>>> s2d_fa, s2d_fa_alarm, s2d_fa_miss;

    // Create an input filestream
    ifstream myFile(filename);

    // Make sure the file is open
    if (!myFile.is_open()) throw runtime_error("Could not open file");

    // Helper vars
    string line1;
    string colname;
    string val;

    // Read the column names per headline
    if (myFile.good())
    {
        // Extract the first line in the file
        getline(myFile, line1);

        // Create a stringstream from line
        stringstream ss(line1);

        // Extract each column name from the 1st line or header
        while (getline(ss, colname, ',')) {
            // Initialize and add <colname, int vector> pairs to result
            s2d_fa.push_back({ colname, vector<string> {} }); //initilize it as zero
            s2d_fa_alarm.push_back({ colname, vector<string> {} }); //initilize it as zero
            s2d_fa_miss.push_back({ colname, vector<string> {} }); //initilize it as zero
        }
    }

    // Read data, line by line after the 1st line of header
    int nobj = 0, nalarm = 0;
    string bagID;
    while (getline(myFile, line1))
    {
        // Create a stringstream of the current line
        stringstream ss(line1);

        // Keep track of the current column index
        int colIdx = 0, colIdx1= 0;
        vector<string> words;
        split2(line1, words);
        //copy(words.begin(), words.end(),

        //while (ss >> val) { //
        if (words[5] == "no_obj")
            continue;
        else if (words[pathind] != pathname)
            continue;
        else {
            nobj++;
            for (int i = 0; i < words.size(); i++) {
                val = words[i];
                s2d_fa.at(colIdx).second.push_back(val);
                if (ss.peek() == ',') ss.ignore();
                colIdx++;
            }

            bagID = words[bagid_col];
            if ((words[threat_col] == "True") || (words[threat_col] == "TRUE")) { //alarmed object, no bbx for fa dataset
                nalarm++;
                for (int i = 0; i < words.size(); i++) {
                    val = words[i];
                    s2d_fa_alarm.at(colIdx).second.push_back(val);
                    if (ss.peek() == ',') ss.ignore();
                    colIdx++;
                }
            }
            else { //missing ojbect
                for (int i = 0; i < words.size(); i++) {
                    val = words[i];
                    s2d_fa_miss.at(colIdx1).second.push_back(val);
                    if (ss.peek() == ',') ss.ignore();
                    colIdx1++;
                }
                }
        }
    }

    // Close file
    myFile.close();
    return {s2d_fa, s2d_fa_alarm,s2d_fa_miss, nobj, nalarm};
}
tuple <svec2d, svec2d, svec2d, fvec2d, fvec2d, fvec2d, int, int> 
read2write_faeigen (int decisiongoal, string filename, string pathname, vector<int> nameinfo, int pathind, int bagid_col,
    int threat_col, vector<int> find) {
    //ofstream myFile("completed.csv");
    int i;

    tuple<vector<pair<string, vector<string>>>, vector<pair<string, vector<string>>>, vector<pair<string, vector<string>>>, int, int> fileoutput =
        readstr_faeigencsv(decisiongoal, filename, pathname, pathind, bagid_col, threat_col);
    vector<pair<string, vector<string>>> csv2d_fa = get<0>(fileoutput);
    vector<pair<string, vector<string>>> csv2d_alarm = get<1>(fileoutput);
    vector<pair<string, vector<string>>> csv2d_miss = get<2>(fileoutput);
    int nobj = get<3>(fileoutput);
    int nalarm = get<4>(fileoutput);
    vector <vector<double>> f2d_fa, f2d_fa_alarm, f2d_fa_miss; //for a single vector just directly assign as: vtest = vec2d[3];
    for (i = 0; i < find.size(); i++) {
        vector<double> fvec = convertStringVectortodoubleVector(csv2d_fa[find[i]].second);
        f2d_fa.push_back(fvec);
        vector<double> fvec1 = convertStringVectortodoubleVector(csv2d_alarm[find[i]].second);
        f2d_fa_alarm.push_back(fvec1);
        vector<double> fvec2 = convertStringVectortodoubleVector(csv2d_miss[find[i]].second);
        f2d_fa_miss.push_back(fvec2);
    }
    vector <vector<string>> name2d_fa, name2d_fa_alarm, name2d_fa_miss; //string array of dataset-name, bagID, Potential_threat inforation which is aligend with the f2d (nuemrical feature 2D)
    for (i = 1; i < nameinfo.size(); i++) {
        vector<string> svec = csv2d_fa[nameinfo[i]].second;
        name2d_fa.push_back(svec);
        vector<string> svec1 = csv2d_alarm[nameinfo[i]].second;
        name2d_fa_alarm.push_back(svec1);
        vector<string> svec2 = csv2d_miss[nameinfo[i]].second;
        name2d_fa_miss.push_back(svec2);
    }
    return {name2d_fa, name2d_fa_alarm,name2d_fa_miss,f2d_fa, f2d_fa_alarm,f2d_fa_miss,nobj, nalarm };
}

tuple <vector<pair<string, vector<string>>>, vector<pair<string, vector<string>>>, vector<pair<string, vector<string>>>, 
    int, int, int, int> readstr_dteigencsv(int idecision, string filename, string pathname, int pathind,
    int bagid_col, int threat_col, vector<int> objcenter, unordered_map<string, vector<double>> bbxmap) {
    // Reads a CSV file into a vector of <string, vector<int>> pairs where
    // each pair represents <column name, column values>

    // Create a vector of <string, int vector> pairs to store the result
    vector<pair<string, vector<string>>> dt2d_alarmed, dt2d_missed, dt2d_wb;

    // Create an input filestream
    ifstream myFile(filename);

    // Make sure the file is open
    if (!myFile.is_open()) throw runtime_error("Could not open file");

    // Helper vars
    string line1;
    string colname;
    string val;

    // Read the column names per headline
    if (myFile.good())
    {
        // Extract the first line in the file
        getline(myFile, line1);

        // Create a stringstream from line
        stringstream ss(line1);

        // Extract each column name from the 1st line or header
        while (getline(ss, colname, ',')) {
            //dt2d_allobj.push_back({ colname, vector<string> {} }); //initilize it as zero
            dt2d_alarmed.push_back({ colname, vector<string> {} }); //initilize it as zero
            dt2d_missed.push_back({ colname, vector<string> {} }); //initilize it as zero
            dt2d_wb.push_back({ colname, vector<string> {} }); //initilize it as zero
        }
    }

    // Read data, line by line after the 1st line of header
    int nobj = 0, nalarm = 0, nmiss =0, nobj_dt_wb=0;
    bool inbbx = 0;
    int xc_col = objcenter[0],yc_col=objcenter[1],zc_col=objcenter[2];
    string bagID;
    int i;
    while (getline(myFile, line1))
    {
        // Create a stringstream of the current line
        stringstream ss(line1);

        // Keep track of the current column index
        int colIdx = 0, colIdx1=0;

        vector<string> words;
        split2(line1, words);
        //copy(words.begin(), words.end(),

        //while (ss >> val) { //
        if (words[5] == "no_obj")
            continue;
        else if (words[pathind] != pathname)
            continue;
        else { //it is an object
            nobj++;
            double xcenter = stod(words[xc_col]), ycenter = stod(words[yc_col]), zcenter = stod(words[zc_col]);
            bagID = words[bagid_col];
            string pt = words[threat_col];
            if (findbbx(bagID, xcenter, ycenter, zcenter, bbxmap)) {
                nobj_dt_wb++;
                for (i = 0; i < words.size(); i++) {
                    val = words[i];
                    dt2d_wb.at(colIdx).second.push_back(val);
                    if (ss.peek() == ',') ss.ignore();
                    colIdx++;
                }

                if ((pt == "True") || (pt == "TRUE")) { //alarmed
                    nalarm++;
                    for (i = 0; i < words.size(); i++) {
                        val = words[i];
                        dt2d_alarmed.at(colIdx).second.push_back(val);
                        if (ss.peek() == ',') ss.ignore();
                        colIdx++;
                    }
                }
                else { //missed or not alarmed
                    nmiss++;
                    for (i = 0; i < words.size(); i++) {
                        val = words[i];
                        dt2d_missed.at(colIdx1).second.push_back(val);
                        if (ss.peek() == ',') ss.ignore();
                        colIdx1++;
                    }
                }
            }
        }
    }
    myFile.close();
    return {dt2d_wb, dt2d_alarmed,dt2d_missed, nobj, nobj_dt_wb,nalarm, nmiss};
}

tuple <svec2d, svec2d, svec2d, fvec2d, fvec2d, fvec2d, int, int, int, int> read2write_dteigen
(int idecision, string filename, string pathname, vector<int> nameinfo, int pathind, int bagid_col, int threat_col,
    vector<int> objcenter, unordered_map<string, vector<double>> bbxmap, vector<int> find) {

    int i;

    tuple<vector<pair<string, vector<string>>>, vector<pair<string, vector<string>>>, vector<pair<string, vector<string>>>, int, int, int, int> fileoutput =
        readstr_dteigencsv(idecision, filename, pathname, pathind, bagid_col, threat_col, objcenter,bbxmap);
    vector<pair<string, vector<string>>> csv2d_dt_wb = get<0>(fileoutput);
    vector<pair<string, vector<string>>> csv2d_alarmed = get<1>(fileoutput);
    vector<pair<string, vector<string>>> csv2d_missed = get<2>(fileoutput);
    int nobj = get<3>(fileoutput);
    int nobj_dt_wb = get<4>(fileoutput);
    int nalarm = get<5>(fileoutput);
    int nmiss = get<6>(fileoutput);
    vector <vector<double>> f2d_dt_wb,f2d_alarmed, f2d_missed; //for a single vector just directly assign as: vtest = vec2d[3];
    for (i = 0; i < find.size(); i++) {
        vector<double> fvec = convertStringVectortodoubleVector(csv2d_dt_wb[find[i]].second);
        vector<double> fvec1 = convertStringVectortodoubleVector(csv2d_alarmed[find[i]].second);
        vector<double> fvec2 = convertStringVectortodoubleVector(csv2d_missed[find[i]].second);
        f2d_alarmed.push_back(fvec);
        f2d_alarmed.push_back(fvec1);
        f2d_missed.push_back(fvec2);
    }
    vector <vector<string>> name2d_dt_wb, name2d_alarmed, name2d_missed; //string array of dataset-name, bagID, Potential_threat inforation which is aligend with the f2d (nuemrical feature 2D)
    for (i = 1; i < nameinfo.size(); i++) {
        vector<string> svec = csv2d_dt_wb [nameinfo[i]].second;
        vector<string> svec1 = csv2d_alarmed[nameinfo[i]].second;
        vector<string> svec2 = csv2d_missed[nameinfo[i]].second;
        name2d_dt_wb.push_back(svec);
        name2d_alarmed.push_back(svec1);
        name2d_missed.push_back(svec2);
    }
    return {name2d_dt_wb,name2d_alarmed,name2d_missed,f2d_dt_wb, f2d_alarmed, f2d_missed,nobj, nobj_dt_wb,nalarm, nmiss};
}

bool findbbx(string bagID, double xcenter, double ycenter, double zcenter, unordered_map<string, vector<double>> mymap)
{
    // mymap = {
    //   {"mom", {5.4, 3.5}},
    //   {"dad",{6.1,2.3}},
    //   {"bro", {5.9, 2.1}} 
    //};
//    string input = "dad";

    unordered_map<string, vector<double>>::const_iterator got = mymap.find(bagID);

    if (got == mymap.end()) {
        return false;
        cout << bagID << "not found";
    }
    else if ((xcenter <= got->second[3]) && (xcenter >= got->second[0]) 
        && (ycenter <= got->second[4]) && (ycenter >= got->second[1]) 
        && (zcenter <= got->second[5]) && (zcenter >= got->second[2])) {
            return true;
    }
    return false;
}

unordered_map<string, vector<double>> readbbx(string filename) {
    ifstream cFile(filename);
    unordered_map<string, vector<double>> bbxmap;
    string line;    
    string cell;

    if (cFile.is_open())
    {
        getline(cFile, line); //get the first line as header, don't need it
        while (getline(cFile, line)) //continue to read line by line
        {
            stringstream  lineStream(line);
            vector<string> strline;
            while (getline(lineStream, cell, ',')) { //divide line by , to get element
                strline.push_back(cell);
            }
            bbxmap[strline[0]] = {stod(strline[12]),stod(strline[13]),stod(strline[14]),
                stod(strline[15]),stod(strline[16]),stod(strline[17])};
        }
    }
    return bbxmap;
}