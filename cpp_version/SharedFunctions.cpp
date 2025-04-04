#include "SharedFunctions.h"


vector<Tile*> tile_bin;

double fraction_to_decimal(Fraction f) {
    return (f.numerator * 1.0) / (f.denominator * 1.0);
}

SolveState* create_solve_state(Board* b, vector<Tile*> t) {
    SolveState* ss = new SolveState();
    ss->board = b;
    ss->tiles = t;
    return ss;
}

void delete_solve_state(SolveState* ss) {
    delete ss->board;
    delete ss;
}

void cleanup() {
    for(Tile* tile : tile_bin) {
        delete tile;
    }
}

Tile* generate_solution() {
    Board solution(n, c);
    //solution.print();
    //cout << solution.validate() << "\n";
    Tile* upper_left = solution.get_tile(0, 0);
    solution.shuffle();
    tile_bin = solution.get_tiles();
    return upper_left;
}

Board generate_solution_board() {
    Board solution(n, c);
    //solution.print();
    //cout << solution.validate() << "\n";
    solution.shuffle();
    tile_bin = solution.get_tiles();
    return solution;
}

vector<unsigned> pick_with_constraints(vector<Tile*> tiles, char top, char bottom, char left, char right) {
    vector<unsigned> result;

    for(unsigned i = 0; i < tiles.size(); i++) {
        Tile* t = tiles[i];
        if((top == ' ' || t->top == top) && (bottom == ' ' || t->bottom == bottom)
            && (left == ' ' || t->left == left) && (right == ' ' || t->right == right)) {
            result.push_back(i);
        }
    }

    return result;
}



vector<char> get_constraints(Board* board, int row, int col) {
    vector<char> result;
    for(int i = 0; i < 4; i++) {
        result.push_back(' ');
    }


    if(row > 0 && board->get_tile(row - 1, col) != nullptr) {
        result[TOP] = board->get_tile(row - 1, col)->bottom;
    }
    if(row < board->get_size() - 1 && board->get_tile(row + 1, col) != nullptr) {
        result[BOTTOM] = board->get_tile(row + 1, col)->top;
    }
    if(col < board->get_size() - 1 && board->get_tile(row, col + 1) != nullptr) {
        result[RIGHT] = board->get_tile(row, col + 1)->left;
    }
    if(col > 0 && board->get_tile(row, col - 1) != nullptr) {
        result[LEFT] = board->get_tile(row, col - 1)->right;
    }
    return result;
}


pair<int, int> coord_with_most_constraints(Board* board) {
    unsigned int constraints = 0;
    unsigned int best_r = 0;
    unsigned int best_c = 0;
    unsigned int best_count = 0;

    for(int r = 0; r < board->get_size(); r++) {
        for(int c = 0; c < board->get_size(); c++) {
            unsigned int tmp_constraints = 0;
            if(board->get_tile(r, c) != nullptr) {
                continue;
            }
            if(r > 0 && board->get_tile(r - 1, c) != nullptr) {
                tmp_constraints += 1;
            }
            if(r < board->get_size() - 1 && board->get_tile(r + 1, c) != nullptr) {
                tmp_constraints += 1;
            }
            if(c > 0 && board->get_tile(r, c - 1) != nullptr) {
                tmp_constraints += 1;
            }
            if(c < board->get_size() - 1 && board->get_tile(r, c + 1) != nullptr) {
                tmp_constraints += 1;
            }

            if(tmp_constraints > constraints) {
                constraints = tmp_constraints;
                best_count = get_constraints(board, r, c).size();
                best_r = r;
                best_c = c;
            }

            if(tmp_constraints == constraints) {
                vector<char> cand_constraints = get_constraints(board, r, c);
                if(cand_constraints.size() < best_count) {
                    constraints = tmp_constraints;
                    best_r = r;
                    best_c = c;
                    best_count = cand_constraints.size();
                }
            }
        }
    }

    return make_pair(best_r, best_c);
}


bool can_place_tile_at(Tile* tile, int i, int j, Board* board) {
    int rows = n;
    int cols = n;

    if (i > 0 && board->get_tile(i-1, j) != nullptr) {
        if (tile->top != board->get_tile(i-1, j)->bottom) {
            return false;
        }
    }

    if (i < rows - 1 && board->get_tile(i+1, j) != nullptr) {
        if (tile->bottom != board->get_tile(i+1, j)->top) {
            return false;
        }
    }

    if (j > 0 && board->get_tile(i, j-1) != nullptr) {
        if (tile->left != board->get_tile(i, j-1)->right) {
            return false;
        }
    }

    if (j < cols - 1 && board->get_tile(i, j+1) != nullptr) {
        if (tile->right != board->get_tile(i, j+1)->left) {
            return false;
        }
    }
    return true;
}


bool dead_end(Board* board, vector<Tile*>& tiles) {
    int rows = n;
    int cols = n;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (board->get_tile(i, j) == nullptr) {
                bool canFill = false;
                for (Tile* tile : tiles) {
                    if (can_place_tile_at(tile, i, j, board)) {
                        canFill = true;
                        break;
                    }
                }

                if (!canFill) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool correct_edges(Board* board) {
    if(!framed) {
        return true;
    }
    for(unsigned i = 0; i < n; i++) {
        for(unsigned j = 0; j < n; j++) {
            Tile* t = board->get_tile(i, j);
            if(t == nullptr) {
                continue;
            }
            if(t->top == 'A' && i != 0) {
                return false;
            }
            if(t->top != 'A' && i == 0) {
                return
                 false;
            }
            if(t->bottom == 'A' && i != n - 1) {
                return false;
            }
            if(t->bottom != 'A' && i == n - 1) {
                return false;
            }

            if(t->left == 'A' && j != 0) {
                return false;
            }
            if(t->left != 'A' && j == 0) {
                return false;
            }
            if(t->right == 'A' && j != n - 1) {
                return false;
            }
            if(t->right != 'A' && j == n - 1) {
                return false;
            }
        }
    }
    return true;
}