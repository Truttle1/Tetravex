#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <utility>

using namespace std;

struct Tile {
    char top;
    char bottom;
    char left;
    char right;
};

vector<string> tile_as_str(Tile* t, int rotation);
string tile_as_inline_str(Tile* t, int rotation);
string tile_as_short_str(Tile* t, int rotation);
string tile_as_nonvectorized_str(Tile* t, int rotation);

Tile* create_tile(char top, char bottom, char left, char right);

const int R_TOP = 0;
const int R_RIGHT = 1;
const int R_BOTTOM = 2;
const int R_LEFT = 3;

char tile_get_side(Tile* t, int side, int rotation);

void tile_rotate_right(Tile* t);

bool tile_is_edge(Tile* t);

bool tile_has_color(Tile* t, char color);

class Board {
    public:
        Board();
        Board(int n);
        Board(int n, int colors);
        Board(const Board& other);
        ~Board();
        void print();
        void shuffle(bool rotate);
        void shuffle();

        void place_tile(Tile* tile, int rotation, int row, int col);
        void place_tile(Tile* tile, int row, int col);
        
        pair<Tile*, int> get_tile(int row, int col);
        vector<Tile*> get_tiles();
    
        int get_size();
        bool validate();
        bool validate_border();
        string board_as_str();
        int score();


    private:
        bool constrain_edges = true;
        int dimension;
        vector<vector<pair<Tile*, int>>> tiles;
        void randomize(int colors);
        void randomize_e2();
};
#endif