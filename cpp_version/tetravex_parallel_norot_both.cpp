#include <mpi.h>
#include <iostream>
#include <deque>
#include <utility>
#include <float.h>
#include <vector>
#include <math.h>
#include <algorithm>
#include <iomanip>
#include <unordered_map>

#include "SharedFunctions.h"
#include "BoardClassic.h"


using namespace std;

const unsigned BLOCK_SIZE = 4;

unordered_map<Tile*, int> tile_to_index;
unordered_map<int, Tile*> index_to_tile;


void manager(const int P, const int ID) {
    unsigned int WORK_TO_GEN = BLOCK_SIZE * (P - 1);
    deque<SolveState*> generate_work;
    Board* start_board = new Board(n);
    SolveState* start_state = create_solve_state(start_board, tile_bin);
    generate_work.push_back(start_state);
    vector<int*> arrays;

    while(generate_work.size() > 0 && generate_work.size() < WORK_TO_GEN) {
        SolveState* current_state = generate_work.front();
        generate_work.pop_front();
        //current_state->board->print();
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
            generate_work.push_back(s);
        }

        
        if(current_state->board->validate()) {
            int* new_arr = (int*) malloc(sizeof(int*) * n * n);
            int index = 0;
            for(int r = 0; r < n; r++) {
                for(int c = 0; c < n; c++) {
                    Tile* t = current_state->board->get_tile(r, c);
                    if(tile_to_index.find(t) == tile_to_index.end()) {
                        new_arr[index] = -1;
                    }
                    else {
                        new_arr[index] = tile_to_index[t];
                    }
                    index += 1;
                }
            }
            arrays.push_back(new_arr);
            current_state->board->print();
        }
        delete_solve_state(current_state);
    }

    cout << "We now have " << generate_work.size() << " states!\n";

    for(SolveState* state : generate_work) {
        int* new_arr = (int*) malloc(sizeof(int*) * n * n);
        int index = 0;
        for(int r = 0; r < n; r++) {
            for(int c = 0; c < n; c++) {
                Tile* t = state->board->get_tile(r, c);
                if(tile_to_index.find(t) == tile_to_index.end()) {
                    new_arr[index] = -1;
                }
                else {
                    new_arr[index] = tile_to_index[t];
                }
                index += 1;
            }
        }

        arrays.push_back(new_arr);
    }

    for(int* arr : arrays) {
        for(int i = 0; i < n * n; i++) {
            cout << arr[i] << " ";
        }
        cout << "\n";
    }

    
    int z = 0;
    for(int i = 1; i < P; i++) {
        for(unsigned j = 0; j < BLOCK_SIZE; j++) {
            MPI_Send(arrays[z], n * n, MPI_INT, i, j, MPI_COMM_WORLD);
            free(arrays[z]);
            z += 1;
        }
    }
    
}

void worker(const int P, const int ID) {
    MPI_Status status;
    
    deque<SolveState*> search;
    MPI_Wtime();
    
    for(unsigned j = 0; j < BLOCK_SIZE; j++) {
        int* recv = (int*) malloc(sizeof(int*) * n * n);
        MPI_Recv(recv, n * n, MPI_INT, 0, j, MPI_COMM_WORLD, &status);

        SolveState* next_state = create_solve_state(new Board(n), tile_bin);
        for(unsigned i = 0; i < n * n; i++) {
            if(recv[i] != -1) {
                unsigned row = i / n;
                unsigned col = i % n;

                Tile* t = index_to_tile[recv[i]];
                next_state->tiles.erase(remove(next_state->tiles.begin(), next_state->tiles.end(), t), next_state->tiles.end());
                next_state->board->place_tile(t, row, col);
            }
            else {
                break;
            }
        }
        search.push_back(next_state);
    }

    bool found = false;

    while(!found && search.size() > 0) {
        int size = search.size();
        #pragma omp parallel for num_threads(36)
        for(int thread = 0; thread < size; thread++) {
            SolveState* current_state;
            #pragma omp critical 
            {
                current_state = search.front();
                search.pop_front();
            }

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

            #pragma omp critical 
            {
                for(SolveState* s : to_push) {
                    search.push_back(s);
                }
            }

            /*
            if(ID == 1) {
                current_state->board->print();
                cout << "........\n";
            }
            */

            if(current_state->board->validate()) {
                cout << "/////// WORKER " << ID << " FOUND///////\n";
                cout << "AT TIME " << MPI_Wtime() <<"\n";
                current_state->board->print();
                cout << current_state->board->board_as_str() << "\n";
                cout << "/////// END WORKER " << ID << "/////////\n";
                found = true;
            }
            delete_solve_state(current_state);
        }
    }


    while(search.size() > 0) {
        delete_solve_state(search.front());
        search.pop_front();
    }
    cout << "WORKER " << ID << " REACHED THE END\n";
}

const int SEED = 0;

int main(int argc, char **argv)
{
    srand(SEEDS[5]);
    Board b = generate_solution_board();

    int index = 0;
    for(Tile* t : tile_bin) {
        tile_to_index[t] = index;
        index_to_tile[index] = t;
        index += 1;
    }

    MPI_Status status;
    MPI_Init(&argc, &argv);

    int P;
    int ID;
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &ID);
    
    if(ID == 0) {
        cout << MPI_Wtime() <<": STARTED!\n";
        manager(P, ID);
    }
    else {
        worker(P, ID);
    }

    if(ID == 0) {
        cout << MPI_Wtime() <<": ENDED!\n";
    }
    
    MPI_Finalize();
    return 0;
}