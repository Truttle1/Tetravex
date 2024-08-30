from typing import List, Optional
from random import randint

import pygame

from tile import Tile
from tile import COLOR_BANK
from tile import NO_COLOR

class Board:
    width: int = 4
    height: int = 4

    offset_x: int = 32
    offset_y: int = 32

    board: List[List[Optional[Tile]]]

    tile_size: int = 32

    show_empty = False

    def __init__(self, offset_x= 32, offset_y=32, size=4):
        self.offset_x = offset_x
        self.offset_y = offset_y

        self.width = size
        self.height = size

    def generate(self):
        self.board = []
        for _ in range(self.width):
            xx: List[Tile] = []
            for __ in range(self.height):
                xx.append(Tile(blank=True))
            self.board.append(xx)
        
        for x in range(self.width):
            for y in range(self.height):
                tile = self.board[x][y]
                if x == 0:
                    tile.left = COLOR_BANK[randint(0, len(COLOR_BANK) - 1)]
                else:
                    tile.left = self.board[x - 1][y].right
                tile.right = COLOR_BANK[randint(0, len(COLOR_BANK) - 1)]

                if y == 0:
                    tile.up = COLOR_BANK[randint(0, len(COLOR_BANK) - 1)]
                else:
                    tile.up = self.board[x][y - 1].down
                tile.down = COLOR_BANK[randint(0, len(COLOR_BANK) - 1)]
        
        self.shuffle()
                

    def generate_empty(self):
        self.board = []
        for _ in range(self.width):
            xx: List[Tile] = []
            for __ in range(self.height):
                xx.append(None)
            self.board.append(xx)

    def shuffle(self, max_swaps=100):
        swaps = randint(0, max_swaps)
        for _ in range(swaps):
            x0 = randint(0, self.width-1)
            y0 = randint(0, self.height-1)

            x1 = randint(0, self.width-1)
            y1 = randint(0, self.height-1)

            tmp = self.board[x0][y0]
            self.board[x0][y0] = self.board[x1][y1]
            self.board[x1][y1] = tmp
    
    def get_tile_list(self):
        tiles = []
        for x in range(self.width):
            for y in range(self.height):
                tiles.append(self.board[x][y])
        return tiles
    
    def render(self, surface):
        for x in range(self.width):
            for y in range(self.height):
                pygame.draw.rect(surface, 
                                 (128, 128, 128),
                                 pygame.Rect(self.offset_x + x * self.tile_size, self.offset_y + y * self.tile_size, self.tile_size, self.tile_size),
                                 width=1
                            )
                if self.board[x][y] is not None:
                    self.board[x][y].draw(surface, self.offset_x + x * self.tile_size, self.offset_y + y * self.tile_size, self.tile_size)
        if self.show_empty:
            self.render_empty(surface)

    def render_empty(self, surface):
        for x in range(self.width):
            for y in range(self.height):
                if self.board[x][y] is None:
                    up = None
                    down = None
                    left = None
                    right = None
                    size = self.tile_size
                    

                    if x > 0 and self.board[x - 1][y] is not None:
                        left = self.board[x - 1][y].right
                    if x < self.width - 1 and self.board[x + 1][y] is not None:
                        right = self.board[x + 1][y].left
                    if y > 0 and self.board[x][y - 1] is not None:
                        up = self.board[x][y - 1].down
                    if y < self.height - 1 and self.board[x][y + 1] is not None:
                        down = self.board[x][y + 1].up

                    if up is not None:
                        xx = x * size + self.offset_x
                        yy = y * size + self.offset_y
                        pygame.draw.polygon(surface, up, 
                                            [
                                                (xx, yy),
                                                (xx + size/2, yy + size/2),
                                                (xx + size, yy)
                                            ],
                                            width=2
                        )
                    
                    if down is not None:
                        xx = x * size + self.offset_x
                        yy = y * size + self.offset_y
                        pygame.draw.polygon(surface, down, 
                                            [
                                                (xx, yy + size),
                                                (xx + size/2, yy + size/2),
                                                (xx + size, yy + size)
                                            ],
                                            width=2
                        )
                    
                    if left is not None:
                        xx = x * size + self.offset_x
                        yy = y * size + self.offset_y
                        pygame.draw.polygon(surface, left, 
                                            [
                                                (xx, yy),
                                                (xx + size/2, yy + size/2),
                                                (xx, yy + size)
                                            ],
                                            width=2
                        )
                    
                    if right is not None:
                        xx = x * size + self.offset_x
                        yy = y * size + self.offset_y
                        pygame.draw.polygon(surface, right, 
                                            [
                                                (xx + size, yy),
                                                (xx + size/2, yy + size/2),
                                                (xx + size, yy + size)
                                            ],
                                            width=2
                        )


    def handle_click(self, x, y, current_selection_board, current_selection_x, current_selection_y):
        result = (current_selection_board, current_selection_x, current_selection_y)
        for xx in range(self.width):
            for yy in range(self.height):
                tx = self.offset_x + xx * self.tile_size
                ty = self.offset_y + yy * self.tile_size

                if self.board[xx][yy] is not None and x > tx and y > ty and x < tx + self.tile_size and y < ty + self.tile_size:
                    self.board[xx][yy].selected = True
                    result = (self, xx, yy)
                elif self.board[xx][yy] is not None:
                    self.board[xx][yy].selected = False
                elif self.board[xx][yy] is None and x > tx and y > ty and x < tx + self.tile_size and y < ty + self.tile_size:
                    if current_selection_board is not None:
                        tmp = current_selection_board.board[current_selection_x][current_selection_y]
                        current_selection_board.board[current_selection_x][current_selection_y] = None
                        self.board[xx][yy] = tmp
                        tmp.selected = False
                        result = (None, None, None)
        return result
    
    def place_tile(self, x, y, tile):
        self.board[x][y] = tile
    
    def solved(self):
        for x in range(self.width):
            for y in range(self.height):
                if self.board[x][y] is None:
                    return False
                if x != 0 and self.board[x][y] is not None and self.board[x - 1][y] is not None:
                    if self.board[x - 1][y].right != self.board[x][y].left:
                        return False
                if y != 0 and self.board[x][y] is not None and self.board[x][y - 1] is not None:
                    if self.board[x][y - 1].down != self.board[x][y].up:
                        return False
        return True

