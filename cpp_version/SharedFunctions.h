#ifndef SHARED_FUNCTIONS_H
#define SHARED_FUNCTIONS_H


#include <iostream>
#include <deque>
#include <utility>
#include <stdlib.h>
#include <chrono>
#include <algorithm>
#include <unordered_map>

#include "BoardClassic.h"
using namespace std;

const int TOP = 0;
const int BOTTOM = 1;
const int LEFT = 2;
const int RIGHT = 3;

const int n = 26;
const int c = 23;

struct Fraction {
    long long numerator;
    long long denominator;
};

struct SolveState {
    Board* board;
    vector<Tile*> tiles;
};

extern vector<Tile*> tile_bin;

extern double fraction_to_decimal(Fraction f);

extern SolveState* create_solve_state(Board* b, vector<Tile*> t);

extern void delete_solve_state(SolveState* ss);

extern void cleanup();

extern Tile* generate_solution();

extern Board generate_solution_board();

extern vector<unsigned> pick_with_constraints(vector<Tile*> tiles, char top, char bottom, char left, char right);

extern vector<char> get_constraints(Board* board, int row, int col);

extern pair<int, int> coord_with_most_constraints(Board* board);

extern bool can_place_tile_at(Tile* tile, int i, int j, Board* board);

extern bool dead_end(Board* board, vector<Tile*>& tiles);

extern bool correct_edges(Board* board);

const int SEEDS[10] = {435345, 123012, 34494903, 559443041, 3934912, 349394, 594945, 30304, 1192293, 12493};

#endif