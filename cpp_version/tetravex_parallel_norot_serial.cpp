#include <iostream>
#include <deque>
#include <utility>
#include <stdlib.h>
#include <chrono>
#include <algorithm>
#include <unordered_map>

#include "BoardClassic.h"
#include "SharedFunctions.h"

using namespace std;

const int SEED = 0;

int main() {
    srand(SEED);
    Tile* upper_left = generate_solution();
    time_t timer;

    auto now = chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    int start
        = chrono::duration_cast<chrono::milliseconds>(
              duration)
              .count();
    /////////////

    bool found = false;tetravex/make_openmp
    Fraction branchingFactor = {0, 0};
    deque<SolveState*> bfs;
    Board* start_board = new Board(n);
    SolveState* start_state = create_solve_state(start_board, tile_bin);
    bfs.push_back(start_state);
    while(bfs.size() > 0 && !found) {
        int size = bfs.size();
        SolveState* current_state;
        current_state = bfs.front();
        bfs.pop_front();

        pair<int, int> to_place = coord_with_most_constraints(current_state->board);
        vector<char> constraints = get_constraints(current_state->board, to_place.first, to_place.second);
        vector<unsigned> tile_idx = pick_with_constraints(current_state->tiles, constraints[TOP], constraints[BOTTOM], constraints[LEFT], constraints[RIGHT]);

        vector<SolveState*> to_push;
        for(unsigned i = 0; i < tile_idx.size(); i++) {
            vector<Tile*> new_tiles = current_state->tiles;
            new_tiles.erase(new_tiles.begin() + tile_idx[i]);
            Board* new_board = new Board(*current_state->board);
            new_board->place_tile(current_state->tiles[tile_idx[i]], to_place.first, to_place.second);
            if(!dead_end(new_board, new_tiles) && correct_edges(new_board)) {
                SolveState* new_state = create_solve_state(new_board, new_tiles);
                to_push.push_back(new_state);
            }
            else {
                delete new_board;
            }
        }

        for(SolveState* s : to_push) {
            bfs.push_back(s);
            branchingFactor.numerator += 1;
        }
        branchingFactor.denominator += 1;
        
        if(bfs.size() == 0 || current_state->board->validate()) {
            cout << "///////\n";
            current_state->board->print();
            cout << current_state->board->board_as_str() << "\n";
            found = current_state->board->validate();
        }
        delete_solve_state(current_state);
    }
    
    time(&timer);

    now = chrono::system_clock::now();
    duration = now.time_since_epoch();
    int end
        = chrono::duration_cast<chrono::milliseconds>(
              duration)
              .count();

    cout << "Time Elapsed: " << ((end - start) / 1000.0) << "\n";
    /////////////
    cleanup();

}