//
// Created by Sunmin Lee on 2018-04-05.
//

#include <string>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>

using namespace std;

static int row, column;
static string result;

template <typename Out>
static void _split (const string s, char delim, Out result) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim))
        *(result++) = atoi(item.c_str());
}

static vector<int> split(const string s, char delim) {
    vector<int> tokens;
    _split(s, delim, back_inserter(tokens));
    return tokens;
}

static unsigned powu(unsigned base, unsigned exp) {
  unsigned result = 1;
  while (exp > 0) {
    if (exp & 1)
      result = result * base;
    base = base * base;
    exp = exp >> 1;
  }
  return result;
}

static vector<string> repeated_permutation(char *list, unsigned perm_len) {
  unsigned list_len = strlen(list);
  char *perm = (char *) calloc(perm_len+1, sizeof(char));
  int total_num = powu(list_len, perm_len);
  vector<string> posibilities = {};

  for (int i = 0; i < total_num; i++) {
    int n = i;
    for (int j =0; j < perm_len; j++) {
      perm[perm_len -j -1] = list[n %list_len];
      n = n / list_len;
    }
    posibilities.push_back(string(perm));
  }
  free(perm);
  return posibilities;
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
  int blocks_per_line = is_row? column : row;
  vector<int> block_nums = get_block_nums(is_row, line_num);
  vector<int> line = split(line_info, ' ');
  string str = "";

  // get the sum of the colored block + in-betweens
  int sum = 0;
  for (auto it = line.begin(); it != line.end(); it++) {
    sum += (*it);
    sum += (it != (line.end() - 1));
  }
  if (blocks_per_line == sum) {
   for (int i = 0; i < line.size(); i++) {
      for (int j = 0; j < line[i];  j++) {
        if (i < line.size() - 1)
          str.append("& ");
        str.append(to_string(block_nums[i + j]));
     }
   }
  }
}



int main(int argc, char *argv[]) {
  row = 0;
  column = 0;
  result = "";

  return 0;
}
