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
static vector<string> nono;
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

static vector<string> repeated_combination(const char *list, unsigned perm_len) {
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
        for (int i = 1; i < column + 1; i++) {
            block_nums.push_back((line_num*column)+i);
          }
    }
    else {
        vector<int> column_nums = {};
        for (int i = 0; i < row; i++)
            block_nums.push_back(line_num+1+(i*row));
    }
    return block_nums;
}


string solve_line(bool is_row, int line_num, string line_info) {
    vector<int> block_nums = get_block_nums(is_row, line_num);
    int blocks_per_line = block_nums.size();
    vector<int> line = split(line_info, ' ');
    string str = "";

  // get the sum of the colored block + in-betweens
    int sum = 0;
    for (auto it = line.begin(); it != line.end(); it++) {
        sum += (*it);
        sum += (it != (line.end() - 1));
    }
    cout << "sum " << sum << endl;

    string cand = "";
    for (int i = 0; i <= line.size(); i++) {
        cand.append(to_string(i));
    }
    vector<string> blank_blocks = repeated_combination(cand.c_str(), blocks_per_line - sum);
    int cur = 0;
    for (auto it = blank_blocks.begin(); it != blank_blocks.end(); it++) {
        if (it < blank_blocks.end() - 1)
          str.append("| ");
        for (int i = 0; i < line.size(); i++) {
          // cout << "segment " << line[i] << endl;
          while ((*it).size() > 0 && stoi((*it).substr(0, 1)) <= i) {
            if (cur < blocks_per_line - 1)
              str.append("& ");
            str.append("- ");
            str.append(to_string(block_nums[cur]));
            str.append(" ");
            cur++;
            (*it) = (*it).erase(0, 1);
          }
          for (int j = 0; j < line[i]; j++) {
            if (cur < blocks_per_line - 1)
              str.append("& ");
            str.append(to_string(block_nums[cur]));
            str.append(" ");
            cur++;
            // cout << cur << endl;
          }
          if (i < line.size() - 1) {
            if (cur < blocks_per_line - 1) {
              str.append("& ");
            str.append("- ");
            str.append(to_string(block_nums[cur]));
            str.append(" ");
            cur++;
            // cout << cur << endl;
            }
          }
        }
        while ((*it).size() > 0) {
          if (cur < blocks_per_line - 1)
            str.append("& ");
          str.append("- ");
          str.append(to_string(block_nums[cur]));
          str.append(" ");
          cur++;
          // cout << cur << endl;
          (*it) = (*it).erase(0, 1);
        }
        cur = 0;
      }
    return str;
}

void solve() {
  bool is_row;
  unsigned line_num;
  for (int i = 0; i < nono.size(); i++) {
    is_row = i < row;
    line_num = is_row? i : i - row;
    if (i < nono.size() - 1) {
      result.append("& ");
    }
    result.append(solve_line(is_row, line_num, nono[i]));
  }
}

int main(int argc, char **argv) {
  if (argc != 2)
    return 0;

  result = "";

  ifstream arg_file;
  string parse_str;
  arg_file.open(argv[1]);
  getline(arg_file, parse_str);
  row = stoi(parse_str);
  getline(arg_file, parse_str);
  column = stoi(parse_str);

  while (getline(arg_file, parse_str)) {
    nono.push_back(parse_str);
  }

  solve();
  return 0;
}
