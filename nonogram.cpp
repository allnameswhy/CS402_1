//
// Created by Sunmin Lee on 2018-04-05.
//

#include <string>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>

static int row, column;

template <typename Out>
static void _split (const string s, char delim, Out result) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim))
        *(result++) = atoi(item);
}

static vector<int> split(const string s, char delim) {
    vector<int> tokens;
    _split(s, delim, back_inserter(tokens));
    return tokens;
}

vector<int> get_block_nums(bool is_row, int line_num) {
    vector<int> block_nums;
    if (is_row) {
        for (int i = 1; i < 7; i++)
            block_nums.push_back((line_num*column)+i);
    }
    else {
        vector<int> column_nums = {};
        for (int i = 0; i < 6; i++)
            block_nums.push_back(line_num+1+(i*row));
    }
    return block_nums;
}

string solve_line(bool is_row, int line_num, string line_info) {
    vector<int> block_nums = get_block_nums(is_row, line_num);
    vector<int> line = split(line_info, ' ');
    string str = "";

    int position;
    int sum = 0;
    for (int i = 0; i < line.size(); i ++) {
        if (line.size() == 1)
            position = 0 ;
        else if (i == 0)
            position = 1;
        else if (i == line.size()-1)
            position = 2;
        else
            position = 3;

        form_neighbors(line[i], position, block_nums);
        sum += line[i];
    }
    form_atmost(sum);

    free(line);
    free(block_nums);
    return str;
}

string form_neighbors(int n, int position, vector<int> blocks) {
    string str = "";

    switch (position) {
        case 0:
            int i = 0;
            while (i+n )
    }
}
