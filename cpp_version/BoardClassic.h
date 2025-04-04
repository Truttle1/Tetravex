#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <stdlib.h>
#include <time.h>

using namespace std;

struct Tile {
    char top;
    char bottom;
    char left;
    char right;
};

vector<string> tile_as_str(Tile* t);
string tile_as_inline_str(Tile* t);
string tile_as_short_str(Tile* t);
string tile_as_nonvectorized_str(Tile* t);

Tile* create_tile(char top, char bottom, char left, char right);

const int R_TOP = 0;
const int R_RIGHT = 1;
const int R_BOTTOM = 2;
const int R_LEFT = 3;

const bool framed = true;

char tile_get_side(Tile* t, int side, int rotation);

void tile_rotate_right(Tile* t);

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

        void place_tile(Tile* tile, int row, int col);
        Tile* get_tile(int row, int col);
        vector<Tile*> get_tiles();
    
        int get_size();
        bool validate();
        string board_as_str();
        int score();


    private:
        int dimension;
        vector<vector<Tile*>> tiles;
        void randomize(int colors);
        void randomize_e2();
};
#endif