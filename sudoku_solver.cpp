#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <typeinfo>

using namespace std;
const int LEN = 9;

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

void initialize_pencil_mark(vector<vector<unordered_set<int> > >& pencil_mark) {
	unordered_set<int> nums({1,2,3,4,5,6,7,8,9});
	for(int r = 0; r < LEN; r++) {
		for(int c = 0; c < LEN; c++) {
			pencil_mark[r][c].insert(nums.begin(), nums.end());
		}
	}
}

void initialize_vec(vector<unordered_set<int> >& v) {
	unordered_set<int> nums({1,2,3,4,5,6,7,8,9});
	for(int i = 0; i < LEN; i++) {
		v[i].insert(nums.begin(), nums.end());
	}
}

unordered_set<int> set_intersection(unordered_set<int> v, unordered_set<int> h, unordered_set<int> b) {
	unordered_set<int> res;
	for(int i=1; i <=9; i++) {
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

	for(int r = 0; r < LEN; r++) {
		for(int c = 0; c < LEN; c++) {
			if(board[r][c] != -1) {
				v[r].erase(board[r][c]);
				h[c].erase(board[r][c]);
				b[3*(r/3) + c/3].erase(board[r][c]);
			} else {
				vector<int> coor{r, c};
				empty_grids.push_back(coor);
			}
		}
	}


	for(int i = 0; i < empty_grids.size(); i++) {
		int r = empty_grids[i][0];
		int c = empty_grids[i][1];
		auto set = set_intersection(v[r], h[c],b[3*(r/3) + c/3]);
		pencil_mark[r][c].insert(set.begin(), set.end());
	}
	
	return empty_grids;
}

void remove_pencil_mark(vector<vector<unordered_set<int> > >& pencil_mark, int r, int c, int val) {
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
	cout << "before elimination " << empty_grids.size() << endl;
	bool update_board = false;
	while(can_eliminate) {
		can_eliminate = false;
		vector<int> remove_list;
		for(int i = 0; i < empty_grids.size(); i++) {
			int r = empty_grids[i][0];
			int c = empty_grids[i][1];
			if(pencil_mark[r][c].size() == 1) {
				int val = *pencil_mark[r][c].begin();
				printf("Found row lone ranger  row: %d, col:%d, val: %d \n", r, c, val);
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
			printf("Removing r:%d c:%d\n", r, c);
			pencil_mark[r][c].clear();
			update_board = true;
		}
		remove_list.clear();
	}
	cout << "after elimination " << empty_grids.size() << endl;
	print_pencil_mark("after elimination", pencil_mark);
	return update_board;
}

bool find_lone_ranger(int board[LEN][LEN], vector<vector<unordered_set<int> > >& pencil_mark, vector<vector<int> >& empty_grids) {
	vector<unordered_map<int, int> > v_unique_val(LEN);
	vector<unordered_map<int, int> > h_unique_val(LEN);
	vector<unordered_map<int, int> > b_unique_val(LEN);
	vector<int> remove_list;
	bool update_board = false;

	cout << "before lone ranger " << empty_grids.size() << endl;

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
				printf("Found row lone ranger  row: %d, col:%d, val: %d\n", r, c, val);
				board[r][c] = val;
				remove_val.push_back(val);
				remove_list.push_back(i);
			} else if(h_unique_val[c][val] == 1) {
				printf("Found col lone ranger  row: %d, col:%d, val: %d\n", r, c, val);
				board[r][c] = val;
				remove_val.push_back(val);
				remove_list.push_back(i);
			} else if(b_unique_val[3*(r/3) + c/3][val] == 1) {
				printf("Found box lone ranger  row: %d, col:%d, val: %d\n", r, c, val);
				board[r][c] = val;
				remove_val.push_back(val);
				remove_list.push_back(i);
			}
		}
		for(int rm_val: remove_val) {
			remove_pencil_mark(pencil_mark, r, c,rm_val);
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
	cout << "after lone ranger " << empty_grids.size() << endl;
	print_pencil_mark("after lone ranger", pencil_mark);
	return update_board;

}

void solve(int board[LEN][LEN]) {
	vector<vector<unordered_set<int> > >  pencil_mark(LEN, vector<unordered_set<int> > (LEN));
	vector<vector<int> > empty_grids = create_pencil_mark(board, pencil_mark);
	bool update_board = false;
	while(true) {
		update_board = elimination(board,pencil_mark,empty_grids);
		if(update_board) {
			cout << "Eliminated something" << endl;
			continue;
		}
		update_board = find_lone_ranger(board, pencil_mark, empty_grids);
		if(update_board){
			cout << "find lone ranger" << endl;
			continue;	
		}
		break;
	}
	
}

int main() {
	/*
	int board [LEN][LEN] = {
							{5,3,-1,-1,7,-1,-1,-1,-1},
					  		{6,-1,-1,1,9,5,-1,-1,-1},
					  		{-1,9,8,-1,-1,-1,-1,6,-1},
					  		{8,-1,-1,-1,6,-1,-1,-1,3},
					  		{4,-1,-1,8,-1,3,-1,-1,1},
					  		{7,-1,-1,-1,2,-1,-1,-1,6},
					  		{-1,6,-1,-1,-1,-1,2,8,-1},
					  		{-1,-1,-1,4,1,9,-1,-1,5},
					  		{-1,-1,-1,-1,8,-1,-1,7,9}
						};
	*/
	//https://www.extremesudoku.info/
	//5/2/2020 Excessive
	int board [LEN][LEN] = {
							{-1,4,-1,-1,6,-1,-1,9,-1},
					  		{1,-1,-1,8,7,-1,-1,-1,5},
					  		{-1,-1,9,-1,-1,-1,7,-1,-1},
					  		{-1,-1,-1,-1,-1,-1,-1,2,-1},
					  		{5,7,-1,-1,8,-1,-1,6,3},
					  		{-1,1,-1,-1,-1,-1,-1,-1,-1},
					  		{-1,-1,6,-1,-1,-1,8,-1,-1},
					  		{8,-1,-1,-1,2,1,-1,-1,4},
					  		{-1,9,-1,-1,5,-1,-1,3,-1}
						};
	solve(board);
}