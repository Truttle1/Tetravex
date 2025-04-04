#include "Board.h"

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

bool tile_has_color(Tile* t, char color) {
    return t->left == color || t->right == color || t->top == color || t->bottom == color;
}

bool tile_is_edge(Tile* t) {
    return t->left == 'A' || t->right == 'A' || t->top == 'A' || t->bottom == 'A';
}


/////////////////////////////

Board::Board(int n, int colors) {
    dimension = n;
    for(int r = 0; r < n; r++) {
        vector<pair<Tile*, int>> row;
        for(int c = 0; c < n; c++) {
            row.push_back(make_pair(create_tile(' ', ' ', ' ', ' '), 0));
        }
        tiles.push_back(row);
    }
    //srand(time(NULL));
    randomize(colors);
}

Board::Board() {
    dimension = 16;
    for(int r = 0; r < 16; r++) {
        vector<pair<Tile*, int>> row;
        for(int c = 0; c < 16; c++) {
            row.push_back(make_pair(create_tile(' ', ' ', ' ', ' '), 0));
        }
        tiles.push_back(row);
    }
    //srand(time(NULL));
    randomize_e2();
}

Board::Board(int n) {
    dimension = n;
    for(int r = 0; r < n; r++) {
        vector<pair<Tile*, int>> row;
        for(int c = 0; c < n; c++) {
            row.push_back(make_pair(nullptr, 0));
        }
        tiles.push_back(row);
    }
}

Board::Board(const Board& other) {
    dimension = other.dimension;
    for(int r = 0; r < dimension; r++) {
        vector<pair<Tile*, int>> row;
        for(int c = 0; c < dimension; c++) {
            row.push_back(other.tiles[r][c]);
        }
        tiles.push_back(row);
    }
}

Board::~Board() {

}

void Board::randomize(int colors) {
    for(int r = 0; r < dimension; r++) {
        for(int c = 0; c < dimension; c++) {
            if(r == 0) {
                tiles[r][c].first->top = 'B' + rand() % (colors - 1);
            }
            else {
                tiles[r][c].first->top = tiles[r - 1][c].first->bottom;
            }
            if(c == 0) {
                tiles[r][c].first->left = 'B' + rand() % (colors - 1);
            }
            else {
                tiles[r][c].first->left = tiles[r][c - 1].first->right;
            }
            tiles[r][c].first->right = 'B' + rand() % (colors - 1);
            tiles[r][c].first->bottom = 'B' + rand() % (colors - 1);

            if(r == 0) {
                tiles[r][c].first->top = 'A';
            }

            if(r == dimension - 1) {
                tiles[r][c].first->bottom = 'A';
            }

            if(c == 0) {
                tiles[r][c].first->left = 'A';
            }

            if(c == dimension - 1) {
                tiles[r][c].first->right = 'A';
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
                tiles[r][c].first->top = 'A';
            }
            else {
                tiles[r][c].first->top = tiles[r - 1][c].first->bottom;
            }
            
            if(c == 0) {
                tiles[r][c].first->left = 'A';
            }
            else {
                tiles[r][c].first->left = tiles[r][c - 1].first->right;
            }

            if(r == dimension - 1) {
                tiles[r][c].first->bottom = 'A';
            }
            else {
                if(c == 0 || c == dimension - 1) {
                    bool found = false;
                    while(!found) {
                        char color = 'B' + rand() % 5;
                        if(count[color] > 0) {
                            count[color] -= 1;
                            tiles[r][c].first->bottom = color;
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
                            tiles[r][c].first->bottom = color;
                            found = true;
                        }
                    }
                }
            }

            if(c == dimension - 1) {
                tiles[r][c].first->right = 'A';
            }
            else {
                if(r == 0 || r == dimension - 1) {
                    bool found = false;
                    while(!found) {
                        char color = 'B' + rand() % 5;
                        if(count[color] > 0) {
                            count[color] -= 1;
                            tiles[r][c].first->right = color;
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
                            tiles[r][c].first->right = color;
                            found = true;
                        }
                    }
                }
            }
        }
    }
}

void Board::place_tile(Tile* tile, int row, int col) {
    place_tile(tile, 0, row, col);
}

void Board::place_tile(Tile* tile, int rotation, int row, int col) {
    tiles[row][col].first = tile;
    tiles[row][col].second = rotation;
}

void Board::shuffle() {
    shuffle(false);
}

void Board::shuffle(bool rotate) {
    for(int r = 0; r < dimension; r++) {
        for(int c = 0; c < dimension; c++) {
            int newR = rand() % dimension;
            int newC = rand() % dimension;
            Tile* temp = tiles[r][c].first;
            tiles[r][c].first = tiles[newR][newC].first;
            tiles[newR][newC].first = temp;
            if(rotate) {
                if(!constrain_edges || !(r == 0 || r == dimension - 1 || c == 0 || c == dimension - 1)) {
                    for(int i = 0; i < rand() % 20; i++) {
                        tile_rotate_right(tiles[r][c].first);
                    }
                    for(int i = 0; i < rand() % 20; i++) {
                        tile_rotate_right(tiles[newR][newC].first);
                    }

                }
            }
        }
    }
}

vector<Tile*> Board::get_tiles() {
    vector<Tile*> result;

    for(int r = 0; r < dimension; r++) {
        for(int c = 0; c < dimension; c++) {
            result.push_back(tiles[r][c].first);
        }
    }
    return result;
}

bool Board::validate() {
    for(int r = 1; r < dimension; r++) {
        for(int c = 1; c < dimension; c++) {
            if(tiles[r][c].first == nullptr) {
                return false;
            }
            
            if(tiles[r - 1][c].first == nullptr || tile_get_side(tiles[r - 1][c].first, R_BOTTOM, tiles[r - 1][c].second) != tile_get_side(tiles[r][c].first, R_TOP, tiles[r][c].second)) {
                return false;
            }
            if(tiles[r][c - 1].first == nullptr || tile_get_side(tiles[r][c - 1].first, R_RIGHT, tiles[r - 1][c].second) != tile_get_side(tiles[r][c].first, R_LEFT, tiles[r][c].second)) {
                return false;
            }
        }
    }
    return true;
}

bool Board::validate_border() {
    for(int r = 1; r < dimension; r++) {
        if(tiles[r][0].first == nullptr) {
            return false;
        }
        if(tiles[r][dimension - 1].first == nullptr) {
            return false;
        }
        if(tiles[r - 1][0].first == nullptr || tile_get_side(tiles[r - 1][0].first, R_BOTTOM, tiles[r - 1][0].second) != tile_get_side(tiles[r][0].first, R_TOP, tiles[r][0].second)) {
            return false;
        }
        if(tiles[r - 1][dimension - 1].first == nullptr || tile_get_side(tiles[r - 1][dimension - 1].first, R_BOTTOM, tiles[r - 1][dimension - 1].second) != tile_get_side(tiles[r][dimension - 1].first, R_TOP, tiles[r][dimension - 1].second)) {
            return false;
        }
    }

    for(int c = 1; c < dimension; c++) {
        if(tiles[0][c].first == nullptr) {
            return false;
        }
        if(tiles[0][c - 1].first == nullptr || tile_get_side(tiles[0][c - 1].first, R_RIGHT, tiles[0][c - 1].second) != tile_get_side(tiles[0][c].first, R_LEFT, tiles[0][c].second)) {
            return false;
        }
        if(tiles[dimension - 1][c].first == nullptr) {
            return false;
        }
        if(tiles[dimension - 1][c - 1].first == nullptr || tile_get_side(tiles[dimension - 1][c - 1].first, R_RIGHT, tiles[dimension - 1][c - 1].second) != tile_get_side(tiles[dimension - 1][c].first, R_LEFT, tiles[dimension - 1][c].second)) {
            return false;
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
            vector<string> tile = tile_as_str(tiles[r][c].first, tiles[r][c].second);
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
        for(pair<Tile*, int> p : v) {
            Tile* t = p.first;
            if(t == nullptr) {
                result += " ";
            }
            else {
                result += tile_as_short_str(t, 0) + " ";
            }
        }
    }
    return result;
}

int Board::score() {
    int score = 0;
    for(auto v : tiles) {
        for(pair<Tile*, int> p : v) {
            Tile* t = p.first;
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

pair<Tile*, int> Board::get_tile(int row, int col) {
    return tiles[row][col];
}