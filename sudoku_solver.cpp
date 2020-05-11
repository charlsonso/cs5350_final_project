#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <chrono>
#include <algorithm>
#include <omp.h>

using namespace std;
const int LEN = 9;

void print_board(int board[LEN][LEN]){
	for(int r = 0; r < LEN; r++) {
		for(int c = 0; c < LEN; c++) {
			cout << board[r][c] <<" ";
		}
		cout << endl;
	}
}

void print_pencil_mark(string s, vector<vector<unordered_set<int> > > pencil_mark) {
	cout << s << endl;
	s = "";
	for(int r = 0; r < LEN; r++) {
		for(int c = 0; c < LEN; c++) {
			vector<int> holder(pencil_mark[r][c].begin(), pencil_mark[r][c].end());
			sort(holder.begin(), holder.end());
			for(auto it = holder.begin(); it != holder.end(); ++it) {
				s += to_string(*it) + " ";
			}
			if(s.length() == 0) {
				s = "      ";
			}
			cout << s << "|";
			s = "";
		}
		cout << endl;
	}
}

void print_vec(string s, vector<unordered_set<int> > v) {
	cout << s << endl;
	for(int i = 0; i < LEN; i++) {
		for(auto it = v[i].begin(); it != v[i].end(); ++it) {
			cout << *it << " ";
		}
		cout << endl;
	}
}

void print_empty_grid(vector<vector<int>> v) {
	for(int i=0; i < v.size(); i++) {
		printf("r:%d, c:%d\n", v[i][0],v[i][1]);
	}
}


void initialize_pencil_mark(vector<vector<unordered_set<int> > >& pencil_mark) {
	unordered_set<int> nums({1,2,3,4,5,6,7,8,9});
	#pragma comp parallel for collapse(2)
	for(int r = 0; r < LEN; r++) {
		for(int c = 0; c < LEN; c++) {
			pencil_mark[r][c].insert(nums.begin(), nums.end());
		}
	}
}

void initialize_vec(vector<unordered_set<int> >& v) {
	unordered_set<int> nums({1,2,3,4,5,6,7,8,9});
	#pragma omp for
	for(int i = 0; i < LEN; i++) {
		v[i].insert(nums.begin(), nums.end());
	}
}

unordered_set<int> set_intersection(unordered_set<int> v, unordered_set<int> h, unordered_set<int> b) {
	unordered_set<int> res;
	#pragma omp for
	for(int i = 1; i <= 9; i++) {
		if(v.count(i) > 0 && h.count(i) > 0 && b.count(i) > 0) {
			res.insert(i);
		}
	}
	return res;
}

vector<vector<int> > create_pencil_mark(int board[LEN][LEN], vector<vector<unordered_set<int> > > &pencil_mark) {
	vector<vector<int> > empty_grids;

	vector<unordered_set<int> > v(LEN);
	vector<unordered_set<int> > h(LEN);
	vector<unordered_set<int> > b(LEN);

	initialize_vec(v);
	//print_vec("verticle vec", v);

	initialize_vec(h);
	//print_vec("horizontal vec", h);

	initialize_vec(b);
	//print_vec("box vec", b);

	#pragma omp for collapse(2)
	for(int r = 0; r < LEN; r++) {
		for(int c = 0; c < LEN; c++) {
			if(board[r][c] != 0) {
				v[r].erase(board[r][c]);
				h[c].erase(board[r][c]);
				b[3*(r/3) + c/3].erase(board[r][c]);
			} else {
				vector<int> coor{r, c};
				empty_grids.push_back(coor);
			}
		}
	}

	#pragma omp for
	for(int i = 0; i < empty_grids.size(); i++) {
		int r = empty_grids[i][0];
		int c = empty_grids[i][1];
		auto set = set_intersection(v[r], h[c],b[3*(r/3) + c/3]);
		pencil_mark[r][c].insert(set.begin(), set.end());
	}
	
	return empty_grids;
}
void update_pencil_mark(vector<vector<unordered_set<int> > >& pencil_mark, vector<vector<int>>& set, int r, int c, int val, int op) {
	// op = 0 remove, op = 1 add
	if(op == 0) {
		for(int i = 0; i < LEN; i++) {
			// Add verticle
			if(pencil_mark[i][c].count(val) > 0 ) {
				pencil_mark[i][c].erase(val);
				vector<int> coor{i, c};
				set.push_back(coor);
			}
			// Add horizontal
			if(pencil_mark[r][i].count(val) > 0) {
				pencil_mark[r][i].erase(val);
				vector<int> coor{r, i};
				set.push_back(coor);
			} 
			// Add box
			if(pencil_mark[r/3*3 + i/3][c/3*3 + i%3].count(val) > 0) {
				pencil_mark[r/3*3 + i/3][c/3*3 + i%3].erase(val);
				vector<int> coor{r/3*3 + i/3, c/3*3 + i%3};
				set.push_back(coor);
			}
		}
	}
	else {
		for(int i = 0; i < set.size(); i++) {
			int r = set[i][0];
			int c = set[i][1];
			pencil_mark[r][c].insert(val);
		}
	}
	
}

void remove_pencil_mark(vector<vector<unordered_set<int> > >& pencil_mark, int r, int c, int val) {
	#pragma omp for
	for(int i = 0; i < LEN; i++) {
		// Remove verticle
		if(pencil_mark[i][c].count(val) > 0) pencil_mark[i][c].erase(val);
		// Remove horizontal
		if(pencil_mark[r][i].count(val) > 0) pencil_mark[r][i].erase(val);
		// Remove box
		if(pencil_mark[r/3*3+i/3][c/3*3+i%3].count(val) > 0) pencil_mark[r/3*3 + i/3][c/3*3 + i%3].erase(val);
	}
}

bool elimination(int board[LEN][LEN], vector<vector<unordered_set<int> > >& pencil_mark, vector<vector<int> >& empty_grids) {
	bool can_eliminate = true;
	//cout << "before elimination " << empty_grids.size() << endl;
	bool update_board = false;
	while(can_eliminate) {
		can_eliminate = false;
		vector<int> remove_list;

		for(int i = 0; i < empty_grids.size(); i++) {
			int r = empty_grids[i][0];
			int c = empty_grids[i][1];
			if(pencil_mark[r][c].size() == 1) {
				int val = *pencil_mark[r][c].begin();
				//printf("Found row lone ranger  row: %d, col:%d, val: %d \n", r, c, val);
				remove_list.push_back(i);
				board[r][c] = val;
				// Remove val from pencil_mark
				remove_pencil_mark(pencil_mark, r, c, val);
				can_eliminate = true;
			}
		}

		for(int i = remove_list.size() - 1; i >=0; i--) {
			int index = remove_list[i];
			int r = empty_grids[index][0];
			int c = empty_grids[index][1];
			empty_grids.erase(empty_grids.begin() + remove_list[i]);
			//printf("Removing r:%d c:%d\n", r, c);
			pencil_mark[r][c].clear();
			update_board = true;
		}
		remove_list.clear();
	}
	//cout << "after elimination " << empty_grids.size() << endl;
	//print_pencil_mark("after elimination", pencil_mark);
	return update_board;
}

bool find_lone_ranger(int board[LEN][LEN], vector<vector<unordered_set<int> > >& pencil_mark, vector<vector<int> >& empty_grids) {
	vector<unordered_map<int, int> > v_unique_val(LEN);
	vector<unordered_map<int, int> > h_unique_val(LEN);
	vector<unordered_map<int, int> > b_unique_val(LEN);
	vector<int> remove_list;
	bool update_board = false;

	//cout << "before lone ranger " << empty_grids.size() << endl;
	#pragma comp parallel for collapse(3)
	for(int r = 0; r < LEN; r++) {
		for(int c = 0; c < LEN; c++) {
			if(pencil_mark[r][c].size() > 0) {
				for(int val: pencil_mark[r][c]) {
					if(v_unique_val[r].count(val) == 0) {v_unique_val[r][val] = 1;} 
					else { v_unique_val[r][val]++;}
					if(h_unique_val[c].count(val) == 0) {h_unique_val[c][val] = 1;}
					else { h_unique_val[c][val]++;}
					if(b_unique_val[3*(r/3) + c/3][val] == 0) {b_unique_val[3*(r/3) + c/3][val] = 1;}
					else {b_unique_val[3*(r/3) + c/3][val]++;}
				}
			}
		}
	}

	for(int i = 0; i < empty_grids.size(); i++) {
		int r = empty_grids[i][0];
		int c = empty_grids[i][1];
		vector<int> remove_val;

		for(int val: pencil_mark[r][c]) {
			if(v_unique_val[r][val] == 1) {
				//printf("Found row lone ranger  row: %d, col:%d, val: %d\n", r, c, val);
				board[r][c] = val;
				remove_val.push_back(val);
				remove_list.push_back(i);
			} else if(h_unique_val[c][val] == 1) {
				//printf("Found col lone ranger  row: %d, col:%d, val: %d\n", r, c, val);
				board[r][c] = val;
				remove_val.push_back(val);
				remove_list.push_back(i);
			} else if(b_unique_val[3*(r/3) + c/3][val] == 1) {
				//printf("Found box lone ranger  row: %d, col:%d, val: %d\n", r, c, val);
				board[r][c] = val;
				remove_val.push_back(val);
				remove_list.push_back(i);
			}
		}

		for(int rm_val: remove_val) {
			remove_pencil_mark(pencil_mark, r, c, rm_val);
		}
	}

	for(int i = remove_list.size() - 1; i >= 0; i--) {
		int index = remove_list[i];
		int r = empty_grids[index][0];
		int c = empty_grids[index][1];
		empty_grids.erase(empty_grids.begin() + remove_list[i]);
		pencil_mark[r][c].clear();
		update_board = true;
	}
	//cout << "after lone ranger " << empty_grids.size() << endl;
	//print_pencil_mark("after lone ranger", pencil_mark);
	return update_board;
}

void find_unique(vector<int>& unique_num) {
	sort(unique_num.begin(), unique_num.end());
	unique_num.erase(unique(unique_num.begin(), unique_num.end()), unique_num.end());
}

void add_pairs(vector<vector<int> >& pairs, vector<int>& unique_num) {
	for(int fir = 0; fir < unique_num.size() - 1; fir++) {
		for(int sec = fir + 1; sec < unique_num.size(); sec++) {
			vector<int> pair{unique_num[fir],unique_num[sec]};
			pairs.push_back(pair);
		}
	}
}

bool find_twin (int board[LEN][LEN], vector<vector<unordered_set<int> > >& pencil_mark, vector<vector<int> >& empty_grids) {
	//print_pencil_mark("find_twin",pencil_mark);
	print_board(board);
	vector<vector<int> > v_unique_num(LEN);
	vector<vector<int> > h_unique_num(LEN);
	vector<vector<int> > b_unique_num(LEN);

	for(int r = 0; r < LEN; r++) {
		for(int c = 0; c < LEN; c++) {
			if(pencil_mark[r][c].size() > 0) {
				for(int val: pencil_mark[r][c]) {
					v_unique_num[r].push_back(val);
					h_unique_num[c].push_back(val);
					b_unique_num[3*(r/3) + c/3].push_back(val);
				}
			}
		}
	}

	// Unique elements
	for(int i = 0; i < LEN; i++) {
		find_unique(v_unique_num[i]);
		find_unique(h_unique_num[i]);
		find_unique(b_unique_num[i]);
	}

	vector<vector<vector<int> > > v_pairs(LEN);
	vector<vector<vector<int> > > h_pairs(LEN);
	vector<vector<vector<int> > > b_pairs(LEN);

	for(int i = 0; i < LEN; i++) {
		add_pairs(v_pairs[i], v_unique_num[i]);
		add_pairs(h_pairs[i], h_unique_num[i]);
		add_pairs(b_pairs[i], b_unique_num[i]);
	}

	vector<int> temp;

	// Find verticle
	for(int i = 0; i < LEN; i++) {
		for(int j = 0; j < v_pairs[i].size(); j++) {
			for(int c = 0; c < LEN; c++) {
				if(pencil_mark[i][c].count(v_pairs[i][j][0]) > 0 and pencil_mark[i][c].count(v_pairs[i][j][1]) > 0) {
					temp.push_back(c);
				}
			}
			printf("Pair: %d %d\n",v_pairs[i][j][0],v_pairs[i][j][1]);
			for(int t:temp) {
				cout << t << " ";
			}
			cout << endl;
			temp.clear();
		}
	}
	
	return true;
}

bool brute_force_optimized(int board[LEN][LEN], vector<vector<unordered_set<int> > >pencil_mark, vector<vector<int> > empty_grids, int index) {
	if(index == empty_grids.size()) {
		return true;
	}

	int r = empty_grids[index][0];
	int c = empty_grids[index][1];
	unordered_set<int> avails(pencil_mark[r][c].begin(), pencil_mark[r][c].end());
	pencil_mark[r][c].clear();

	for(int val: avails) {

		board[r][c] = val;
		vector<vector<int> > removed_elements;
		update_pencil_mark(pencil_mark, removed_elements, r, c, val, 0);

		if(brute_force_optimized(board, pencil_mark, empty_grids, index + 1)) {
			return true;
		}
		update_pencil_mark(pencil_mark, removed_elements, r, c, val, 1);
		board[r][c] = 0;	
	}
	return false;
}


void solve_optimized(int board[LEN][LEN]) {
	vector<vector<unordered_set<int> > >  pencil_mark(LEN, vector<unordered_set<int> > (LEN));
	vector<vector<int> > empty_grids = create_pencil_mark(board, pencil_mark);
	bool update_board = false;
	
	while(true) {
		update_board = elimination(board,pencil_mark,empty_grids);
		if(update_board) {continue;}

		update_board = find_lone_ranger(board, pencil_mark, empty_grids);
		if(update_board){ continue;}

		break;
	}


	brute_force_optimized(board, pencil_mark, empty_grids, 0);	
}

bool is_valid(int board[LEN][LEN], int r, int c, int num) {
	for(int i = 0; i < LEN; i++) {
		if(board[r][i] == num || board[i][c] == num 
			|| board[r/3 * 3 + i/3][c/3 * 3 + i%3] == num) {
			return false;
		}
	}
	return true;
}

bool helper(int board[LEN][LEN]) {
	for(int r = 0; r < LEN; r++) {
		for(int c = 0; c < LEN; c++) {
			if(board[r][c] == 0) {
				for(int i = 1; i <= 9; i++) {
					if(is_valid(board, r, c, i)) {
						board[r][c] = i;
						if(helper(board)) {
							return true;
						} else {
							board[r][c] = 0;
						}
					}
				}
				return false;
			}
		}
	}
	return true;
}

void solve_brute_force(int board[LEN][LEN]) {
	helper(board);
}

void verify(int board[LEN][LEN], int soln[LEN][LEN]) {
	for(int r = 0; r < LEN; r++) {
		for(int c = 0; c < LEN; c++) {
			if(board[r][c] != soln[r][c]) {
				printf("Error at r:%d, c:%d  %d != %d\n", r,c,board[r][c],soln[r][c]);
				return;
			}
		}
	}
	cout << "Pass" << endl;
}

int main() {
	//https://www.extremesudoku.info/
	//5/2/2020 Excessive
	int board [LEN][LEN] = {
							{0,4,0,0,6,0,0,9,0},
					  		{1,0,0,8,7,0,0,0,5},
					  		{0,0,9,0,0,0,7,0,0},
					  		{0,0,0,0,0,0,0,2,0},
					  		{5,7,0,0,8,0,0,6,3},
					  		{0,1,0,0,0,0,0,0,0},
					  		{0,0,6,0,0,0,8,0,0},
					  		{8,0,0,0,2,1,0,0,4},
					  		{0,9,0,0,5,0,0,3,0}
						};

	int soln [LEN][LEN] = {
							{7,4,5,1,6,2,3,9,8},
					  		{1,2,3,8,7,9,6,4,5},
					  		{6,8,9,3,4,5,7,1,2},
					  		{3,6,8,5,1,7,4,2,9},
					  		{5,7,2,9,8,4,1,6,3},
					  		{9,1,4,2,3,6,5,8,7},
					  		{2,5,6,4,9,3,8,7,1},
					  		{8,3,7,6,2,1,9,5,4},
					  		{4,9,1,7,5,8,2,3,6}
						};
	int board2[LEN][LEN];
	copy(&board[0][0], &board[0][0]+LEN*LEN, &board2[0][0]);

	// Brute force algorithm
	auto start = chrono::high_resolution_clock::now();
	solve_brute_force(board);
	auto end = chrono::high_resolution_clock::now();
	cout << chrono::duration_cast<chrono::milliseconds>(end - start).count() <<endl;

	print_board(board);
	verify(board, soln);

	// Optimized algorithm
	start = chrono::high_resolution_clock::now();
	solve_optimized(board2);
	end = chrono::high_resolution_clock::now();
	cout << chrono::duration_cast<chrono::milliseconds>(end - start).count() <<endl;
	
	print_board(board2);
	verify(board2, soln);

}