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

const int DONE = -1;
const int EMPTY = 0;

const int MANAGER = 0;

//Rebalancing IDs
const int DONOR = 0;
const int RECIPIENT = 1;

const int END_REBALANCE = 0;

struct CountAndID {
    int count;
    unsigned int ID;
};

void manager(const int P, const int ID) {
    unsigned int WORK_TO_GEN = BLOCK_SIZE * (P - 1);
    MPI_Status status;
    deque<SolveState*> generate_work;
    Board* start_board = new Board(n);
    SolveState* start_state = create_solve_state(start_board, tile_bin);
    generate_work.push_back(start_state);
    vector<int*> arrays;

    while(generate_work.size() > 0 && generate_work.size() < WORK_TO_GEN) {
        SolveState* current_state = generate_work.front();
        generate_work.pop_front();
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

    bool done = false;
    while(!done) {
        deque<CountAndID> countsAndID;
        MPI_Barrier(MPI_COMM_WORLD);
        for(int i = 1; i < P; i++) {
            int* recv = (int*) malloc(sizeof(int*) * 1);
            MPI_Recv(recv, 1, MPI_INT, i, i, MPI_COMM_WORLD, &status);
            ///
            CountAndID cid;
            cid.count = recv[0];
            cid.ID = i;

            if(recv[0] == -1) {
                done = true;
            }

            countsAndID.push_back(cid);
            free(recv);
        }
        
        bool end_rebalance = false;

        while(!end_rebalance) {
            sort(countsAndID.begin(), countsAndID.end(), [](const CountAndID& lhs, const CountAndID& rhs) {
                return lhs.count < rhs.count;
            });

            int* bcast = (int*) malloc(sizeof(int*) * 2);
            bcast[DONOR] = END_REBALANCE;
            end_rebalance = true;

            if(done) {
                bcast[DONOR] = DONE;
            }

            //There is an idling process, tell most busy process to give it work.
            cout << "SMALLEST IS " << countsAndID.front().ID << " WITH " << countsAndID.front().count << "\n";
            cout << "LARGEST IS " << countsAndID.back().ID << " WITH " << countsAndID.back().count << "\n";
            if(countsAndID.front().count == 0) {
                end_rebalance = false;
                CountAndID smallest = countsAndID.front();
                countsAndID.pop_front();

                CountAndID largest = countsAndID.back();
                countsAndID.pop_back();

                smallest.count += 1;
                largest.count -= 1;

                //This will be sorted later
                countsAndID.push_back(smallest);
                countsAndID.push_back(largest);

                bcast[DONOR] = largest.ID;
                bcast[RECIPIENT] = smallest.ID;
            }

            MPI_Bcast(bcast, 2, MPI_INT, MANAGER, MPI_COMM_WORLD);
            free(bcast);
        }
        cout << "--------------\n";
        
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

    while(!found) {
        int counter = 500;
        while(search.size() > 0 && counter > 0) {
            SolveState* current_state = search.back();
            search.pop_back();

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
                search.push_back(s);
            }

            /*
            if(ID == 1) {
                current_state->board->print();
                cout << "........\n";
            }
            */

            if(current_state->board->validate() && !found) {
                cout << "/////// WORKER " << ID << " FOUND///////\n";
                cout << "AT TIME " << MPI_Wtime() <<"\n";
                current_state->board->print();
                cout << current_state->board->board_as_str() << "\n";
                cout << "/////// END WORKER " << ID << "/////////\n";
                found = true;
            }
            delete_solve_state(current_state);
            counter--;
        }

        if(search.size() == 0 && !found) {
            cout << "WORKER " << ID << " RAN OUT\n";
        }


        if(found) {
            while(search.size() > 0) {
                delete_solve_state(search.front());
                search.pop_front();
            }
        }

        bool end_rebalance = false;

        MPI_Barrier(MPI_COMM_WORLD);
        int* send = (int*) malloc(sizeof(int*) * 1);
        send[0] = search.size();
        if(found) {
            send[0] = DONE;
        }
        MPI_Send(send, 1, MPI_INT, 0, ID, MPI_COMM_WORLD);
        free(send);

        while(!end_rebalance) {
            int* bcast = (int*) malloc(sizeof(int*) * 2);
            bool is_donor = false;
            bool is_recip = false;

            MPI_Bcast(bcast, 2, MPI_INT, MANAGER, MPI_COMM_WORLD);

            /*
            Possibilities:
                Done Searching: Terminate everything
                Done Rebalancing: Continue to DFS
                Is Donor: Send a state to recipient
                Is Recipient: Recieve a state from donor
                Other: There will be more rebalancing; wait for next rebalance broadcast.
            */
            if(bcast[DONOR] == DONE) {
                found = true;
                end_rebalance = true;
            }
            else if(bcast[DONOR] == END_REBALANCE) {
                end_rebalance = true;
            }
            else if(bcast[DONOR] == ID) {
                is_donor = true;
            }
            else if(bcast[RECIPIENT] == ID) {
                is_recip = true;
            }

            //The DONOR will take its front state and send it to the recipient.
            if(is_donor) {
                int* to_send = (int*) malloc(sizeof(int*) * n * n);
                int index = 0;
                SolveState* state = search.front();
                search.pop_front();
                for(int r = 0; r < n; r++) {
                    for(int c = 0; c < n; c++) {
                        Tile* t = state->board->get_tile(r, c);
                        if(tile_to_index.find(t) == tile_to_index.end()) {
                            to_send[index] = -1;
                        }
                        else {
                            to_send[index] = tile_to_index[t];
                        }
                        index += 1;
                    }
                }
                cout << "SENDING FROM " << ID << " TO " << bcast[RECIPIENT] << "\n";
                MPI_Send(to_send, n * n, MPI_INT, bcast[RECIPIENT], 0, MPI_COMM_WORLD);
                free(to_send);
            }

            //The RECIPIENT will recieve a state from the donor
            if(is_recip) {
                int* recv = (int*) malloc(sizeof(int*) * n * n);
                MPI_Recv(recv, n * n, MPI_INT, bcast[DONOR], 0, MPI_COMM_WORLD, &status);

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
                cout << "RECIEVING BY " << ID << " FROM " << bcast[DONOR] << "\n";
                free(recv);
            }
            free(bcast);
        }
    }
    cout << "WORKER " << ID << " REACHED THE END\n";
}

const int SEED = 0;

int main(int argc, char **argv)
{
    srand(SEEDS[8]);
    Board b = generate_solution_board();

    int index = 0;
    for(Tile* t : tile_bin) {
        tile_to_index[t] = index;
        index_to_tile[index] = t;
        index += 1;
    }

    MPI_Init(&argc, &argv);

    int P;
    int ID;
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &ID);
    
    if(ID == MANAGER) {
        cout << MPI_Wtime() <<": STARTED!\n";
        manager(P, ID);
    }
    else {
        worker(P, ID);
    }

    if(ID == MANAGER) {
        cout << MPI_Wtime() <<": ENDED!\n";
    }
    
    MPI_Finalize();
    return 0;
}