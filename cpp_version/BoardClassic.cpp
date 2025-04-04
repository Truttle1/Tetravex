#include "BoardClassic.h"

char tile_get_side(Tile* t, int side, int rotation) {
    int place = (side + rotation) % 4;
    switch(place) {
        case R_TOP:
            return t->top;
            break;
        case R_RIGHT:
            return t->right;
            break;
        case R_BOTTOM:
            return t->bottom;
            break;
        case R_LEFT:
            return t->left;
            break;
    }
    return ' ';
}

vector<string> tile_as_str(Tile* t, int rotation) {
    vector<string> result;
    for(int i = 0; i < 5; i++) {
        result.push_back("");
    }
    if(t == nullptr) {
        result[0] = "/---\\";
        result[1] = "|   |";
        result[2] = "|   |";
        result[3] = "|   |";
        result[4] = "\\---/";
    }
    else {
        result[0] = "/---\\";
        result[1] = "| " + string(1, tile_get_side(t, R_TOP, rotation)) + " |";
        result[2] = "|" + string(1, tile_get_side(t, R_LEFT, rotation)) + " " + string(1, tile_get_side(t, R_RIGHT, rotation)) + "|";
        result[3] = "| " + string(1, tile_get_side(t, R_BOTTOM, rotation)) + " |";
        result[4] = "\\---/";
    }
    return result;
}

string tile_as_nonvectorized_str(Tile* t, int rotation) {
    vector<string> result = tile_as_str(t, rotation);
    string str = "";
    for(string element : result) {
        str += element + "\n";
    }
    return str;
}

string tile_as_inline_str(Tile* t, int rotation) {
    return "(" + string(1, tile_get_side(t, R_TOP, rotation)) + ", " + string(1, tile_get_side(t, R_RIGHT, rotation)) + ", " + string(1, tile_get_side(t, R_BOTTOM, rotation)) + ", " + string(1, tile_get_side(t, R_LEFT, rotation)) + ")";
}

string tile_as_short_str(Tile* t, int rotation) {
    return string(1, tile_get_side(t, R_TOP, rotation)) + string(1, tile_get_side(t, R_RIGHT, rotation)) + string(1, tile_get_side(t, R_BOTTOM, rotation)) + string(1, tile_get_side(t, R_LEFT, rotation));
}

Tile* create_tile(char top, char bottom, char left, char right) {
    Tile* t = new Tile();
    t->top = top;
    t->bottom = bottom;
    t->left = left;
    t->right = right;

    return t;
}

void tile_rotate_right(Tile* t) {
    char tmp = t->left;
    t->left = t->bottom;
    t->bottom = t->right;
    t->right = t->top;
    t->top = tmp;
}

/////////////////////////////

Board::Board(int n, int colors) {
    dimension = n;
    for(int r = 0; r < n; r++) {
        vector<Tile*> row;
        for(int c = 0; c < n; c++) {
            row.push_back(create_tile(' ', ' ', ' ', ' '));
        }
        tiles.push_back(row);
    }
    //srand(time(NULL));
    randomize(colors);
}

Board::Board() {
    dimension = 16;
    for(int r = 0; r < 16; r++) {
        vector<Tile*> row;
        for(int c = 0; c < 16; c++) {
            row.push_back(create_tile(' ', ' ', ' ', ' '));
        }
        tiles.push_back(row);
    }
    //srand(time(NULL));
    randomize_e2();
}

Board::Board(int n) {
    dimension = n;
    for(int r = 0; r < n; r++) {
        vector<Tile*> row;
        for(int c = 0; c < n; c++) {
            row.push_back(nullptr);
        }
        tiles.push_back(row);
    }
}

Board::Board(const Board& other) {
    dimension = other.dimension;
    for(int r = 0; r < dimension; r++) {
        vector<Tile*> row;
        for(int c = 0; c < dimension; c++) {
            row.push_back(other.tiles[r][c]);
        }
        tiles.push_back(row);
    }
}

Board::~Board() {

}

void Board::randomize(int colors) {
    char start_color = framed ? 'B' : 'A';
    if(!framed) {
        colors += 1;
    }

    for(int r = 0; r < dimension; r++) {
        for(int c = 0; c < dimension; c++) {
            if(r == 0) {
                tiles[r][c]->top = start_color + rand() % (colors - 1);
            }
            else {
                tiles[r][c]->top = tiles[r - 1][c]->bottom;
            }
            if(c == 0) {
                tiles[r][c]->left = start_color + rand() % (colors - 1);
            }
            else {
                tiles[r][c]->left = tiles[r][c - 1]->right;
            }
            tiles[r][c]->right = start_color + rand() % (colors - 1);
            tiles[r][c]->bottom = start_color + rand() % (colors - 1);

            if(framed) {
                if(r == 0) {
                    tiles[r][c]->top = 'A';
                }

                if(r == dimension - 1) {
                    tiles[r][c]->bottom = 'A';
                }

                if(c == 0) {
                    tiles[r][c]->left = 'A';
                }

                if(c == dimension - 1) {
                    tiles[r][c]->right = 'A';
                }
            }
        }
    }
}

void Board::randomize_e2() {
    unordered_map<char, int> count;
    for(char c = 'B'; c <= 'F'; c++) {
        count[c] = 12;
    }
    for(char c = 'G'; c <= 'K'; c++) {
        count[c] = 24;
    }
    for(char c = 'L'; c <= 'W'; c++) {
        count[c] = 25;
    }

    for(int r = 0; r < dimension; r++) {
        for(int c = 0; c < dimension; c++) {
            if(r == 0) {
                tiles[r][c]->top = 'A';
            }
            else {
                tiles[r][c]->top = tiles[r - 1][c]->bottom;
            }
            
            if(c == 0) {
                tiles[r][c]->left = 'A';
            }
            else {
                tiles[r][c]->left = tiles[r][c - 1]->right;
            }

            if(r == dimension - 1) {
                tiles[r][c]->bottom = 'A';
            }
            else {
                if(c == 0 || c == dimension - 1) {
                    bool found = false;
                    while(!found) {
                        char color = 'B' + rand() % 5;
                        if(count[color] > 0) {
                            count[color] -= 1;
                            tiles[r][c]->bottom = color;
                            found = true;
                        }
                    }
                }
                else {
                    bool found = false;
                    while(!found) {
                        char color = 'G' + rand() % 17;
                        if(count[color] > 0) {
                            count[color] -= 1;
                            tiles[r][c]->bottom = color;
                            found = true;
                        }
                    }
                }
            }

            if(c == dimension - 1) {
                tiles[r][c]->right = 'A';
            }
            else {
                if(r == 0 || r == dimension - 1) {
                    bool found = false;
                    while(!found) {
                        char color = 'B' + rand() % 5;
                        if(count[color] > 0) {
                            count[color] -= 1;
                            tiles[r][c]->right = color;
                            found = true;
                        }
                    }
                }
                else {
                    bool found = false;
                    while(!found) {
                        char color = 'G' + rand() % 17;
                        if(count[color] > 0) {
                            count[color] -= 1;
                            tiles[r][c]->right = color;
                            found = true;
                        }
                    }
                }
            }
        }
    }
}

void Board::place_tile(Tile* tile, int row, int col) {
    tiles[row][col] = tile;
}

void Board::shuffle() {
    shuffle(false);
}

void Board::shuffle(bool rotate) {
    for(int r = 0; r < dimension; r++) {
        for(int c = 0; c < dimension; c++) {
            int newR = rand() % dimension;
            int newC = rand() % dimension;
            Tile* temp = tiles[r][c];
            tiles[r][c] = tiles[newR][newC];
            tiles[newR][newC] = temp;
            if(rotate) {
                for(int i = 0; i < rand() % 4; i++) {
                    tile_rotate_right(tiles[r][c]);
                }
                for(int i = 0; i < rand() % 4; i++) {
                    tile_rotate_right(tiles[newR][newC]);
                }
            }
        }
    }
}

vector<Tile*> Board::get_tiles() {
    vector<Tile*> result;

    for(int r = 0; r < dimension; r++) {
        for(int c = 0; c < dimension; c++) {
            result.push_back(tiles[r][c]);
        }
    }
    return result;
}

bool Board::validate() {
    for(int r = 1; r < dimension; r++) {
        for(int c = 1; c < dimension; c++) {
            if(tiles[r][c] == nullptr) {
                return false;
            }
            
            if(tiles[r - 1][c] == nullptr || tile_get_side(tiles[r - 1][c], R_BOTTOM, 0) != tile_get_side(tiles[r][c], R_TOP, 0)) {
                return false;
            }
            if(tiles[r][c - 1] == nullptr || tile_get_side(tiles[r][c - 1], R_RIGHT, 0) != tile_get_side(tiles[r][c], R_LEFT, 0)) {
                return false;
            }
        }
    }
    return true;
}

void Board::print() {
    for(int r = 0; r < dimension; r++) {
        vector<string> row;
        for(int i = 0; i < 5; i++) {
            row.push_back("");
        }

        for(int c = 0; c < dimension; c++) {
            vector<string> tile = tile_as_str(tiles[r][c], 0);
            for(int i = 0; i < 5; i++) {
                row[i] += tile[i];
            }
        }

        for(int i = 0; i < 5; i++) {
            cout << row[i] << "\n";
        }
    }
}

string Board::board_as_str() {
    string result = "";

    for(auto v : tiles) {
        for(Tile* t : v) {
            if(t == nullptr) {
                result += " ";
            }
            else {
                result += tile_as_short_str(t, 0);
            }
        }
    }
    return result;
}

int Board::score() {
    int score = 0;
    for(auto v : tiles) {
        for(Tile* t : v) {
            if(t != nullptr) {
                score += 1;
            }
        }
    }
    return score;
}

int Board::get_size() {
    return dimension;
}

Tile* Board::get_tile(int row, int col) {
    return tiles[row][col];
}