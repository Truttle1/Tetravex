from typing import List
from copy import deepcopy, copy
from random import randint
from collections import deque

from tile import Tile
from board import Board


class SolverState:
    board: Board
    tiles_left: List[Tile]
    def __init__(self, board, tiles_left):
        self.board = board
        self.tiles_left = tiles_left

class Solver:
    main_board: Board = []
    queue = []
    tiles = []

    def __init__(self, board, tiles):
        self.main_board = deepcopy(board)
        for tile in tiles:
            self.tiles.append(tile)      
        
        place_x, place_y = self.coord_with_most_constraints()
        next_tiles = self.pick_with_constraints(None, None, None, None)
        for tile in next_tiles:
            new_tiles = copy(self.tiles)
            nextState = SolverState(deepcopy(self.main_board), new_tiles)
            nextState.board.place_tile(place_x, place_y, tile)
            new_tiles.remove(tile)
            self.queue.append(nextState)
        board = self.queue.pop(0)
        self.main_board = board.board
        self.tiles = board.tiles_left  

    def pick_random(self):
        print(f'There are {len(self.tiles)} tiles')
        tile_id = randint(0, len(self.tiles) - 1)
        next_tile = self.tiles[tile_id]
        return next_tile
    
    def pick_with_constraints(self, up, down, left, right):
        tiles = []
        for tile_id in range(len(self.tiles)):
            if up is None or self.tiles[tile_id].up == up:
                if down is None or self.tiles[tile_id].down == down:
                    if left is None or self.tiles[tile_id].left == left:
                        if right is None or self.tiles[tile_id].right == right:
                            tile = self.tiles[tile_id]
                            tiles.append(tile)
        return tiles
    
    def coord_with_least_fits(self):
        min_x = -1
        min_y = -1
        min_fits = 999999
        board = self.main_board.board
        width = self.main_board.width
        height = self.main_board.height


        for x in range(width):
            for y in range(height):
                left = None
                right = None
                up = None
                down = None

                if board[x][y] is not None:
                    continue

                if x > 0 and board[x - 1][y] is not None:
                    left = board[x - 1][y].right
                if x < width - 1 and board[x + 1][y] is not None:
                    right = board[x + 1][y].left
                if y > 0 and board[x][y - 1] is not None:
                    up = board[x][y - 1].down
                if y < height - 1 and board[x][y + 1] is not None:
                    down = board[x][y + 1].up
                
                count = 0
                for tile in self.tiles:
                    if left is None or tile.left == left:
                        if right is None or tile.right == right:
                            if up is None or tile.up == up:
                                if down is None or tile.down == down:
                                    count += 1
                
                if count < min_fits:
                    min_x = x
                    min_y = y
                    min_fits = count
        return min_x, min_y
        

    def every_piece_fits(self):
        board = self.main_board.board
        width = self.main_board.width
        height = self.main_board.height


        for tile in self.tiles:
            found_one = False
            for x in range(width):
                for y in range(height):
                    left = None
                    right = None
                    up = None
                    down = None

                    if board[x][y] is not None:
                        continue

                    if x > 0 and board[x - 1][y] is not None:
                        left = board[x - 1][y].right
                    if x < width - 1 and board[x + 1][y] is not None:
                        right = board[x + 1][y].left
                    if y > 0 and board[x][y - 1] is not None:
                        up = board[x][y - 1].down
                    if y < height - 1 and board[x][y + 1] is not None:
                        down = board[x][y + 1].up
                    
                        if ((left is None or tile.left == left) and 
                            (right is None or tile.right == right) and 
                            (up is None or tile.up == up) and 
                            (down is None or tile.down == down)):
                            found_one = True
            if not found_one:
                return False
        return True
                    

    def coord_with_most_constraints(self):
        max_x = -1
        max_y = -1
        max_constraints = -1
        for x in range(self.main_board.width):
            for y in range(self.main_board.height):
                constraints = 0
                if self.main_board.board[x][y] is not None:
                    continue
                if x > 0 and self.main_board.board[x - 1][y] is not None:
                    constraints += 1
                if x < self.main_board.width - 1 and self.main_board.board[x + 1][y] is not None:
                    constraints += 1
                if y > 0 and self.main_board.board[x][y - 1] is not None:
                    constraints += 1
                if y < self.main_board.height - 1 and self.main_board.board[x][y + 1] is not None:
                    constraints += 1
                
                if constraints > max_constraints:
                    max_x = x
                    max_y = y
                    max_constraints = constraints
        return max_x, max_y

    def churn(self):
        place_x, place_y = self.coord_with_least_fits()
        if True or self.every_piece_fits():
            u = None
            d = None
            l = None
            r = None
            if place_x > 0 and self.main_board.board[place_x - 1][place_y] is not None:
                l = self.main_board.board[place_x - 1][place_y].right
            if place_x < self.main_board.width - 1 and self.main_board.board[place_x + 1][place_y] is not None:
                r = self.main_board.board[place_x + 1][place_y].left
            if place_y > 0 and self.main_board.board[place_x][place_y - 1] is not None:
                u = self.main_board.board[place_x][place_y - 1].down
            if place_y < self.main_board.height - 1 and self.main_board.board[place_x][place_y + 1] is not None:
                d = self.main_board.board[place_x][place_y + 1].up

            next_tiles = self.pick_with_constraints(u, d, l, r)
            for tile in next_tiles:
                new_tiles = copy(self.tiles)
                nextState = SolverState(deepcopy(self.main_board), new_tiles)
                nextState.board.place_tile(place_x, place_y, tile)
                new_tiles.remove(tile)
                self.queue.append(nextState)

        print(len(self.queue))

        if len(self.queue) != 0:
            board = self.queue.pop(0)
            self.main_board = board.board
            self.tiles = board.tiles_left  
