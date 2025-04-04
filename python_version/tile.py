import pygame 

COLOR_BANK = [
    pygame.Color(64, 64, 64),  # Dark Gray for border
    pygame.Color(23, 160, 229),
    pygame.Color(229, 23, 202),
    pygame.Color(80, 23, 229),
    pygame.Color(23, 229, 160),
    pygame.Color(229, 23, 100),
    pygame.Color(182, 229, 23),
    pygame.Color(23, 229, 58),
    pygame.Color(131, 23, 229),
    pygame.Color(229, 49, 23),
    pygame.Color(80, 229, 23),
    pygame.Color(23, 109, 229),
    pygame.Color(229, 202, 23),
    pygame.Color(23, 229, 211),
    pygame.Color(29, 23, 229),
    pygame.Color(229, 23, 151),
    pygame.Color(229, 100, 23),
    pygame.Color(229, 23, 49),
    pygame.Color(229, 23, 23),
    pygame.Color(23, 211, 229),
    pygame.Color(23, 58, 229),
    pygame.Color(29, 229, 23),
    pygame.Color(131, 229, 23),
    pygame.Color(23, 229, 109),
    pygame.Color(182, 23, 229),
    pygame.Color(229, 151, 23),
]

NO_COLOR = pygame.Color(0, 0, 0)

class Tile:
    left: pygame.Color = pygame.Color(255, 0, 0) 
    right: pygame.Color = pygame.Color(0, 255, 0) 
    up: pygame.Color = pygame.Color(0, 0, 255) 
    down: pygame.Color = pygame.Color(255, 255, 0)
    selected: bool = False

    def __init__(self, blank = False):
        if blank:
            self.blank()
    
    def blank(self):
        self.left = NO_COLOR
        self.right = NO_COLOR
        self.up = NO_COLOR
        self.down = NO_COLOR

    def is_blank(self):
        return self.left == NO_COLOR and self.right == NO_COLOR and self.up == NO_COLOR and self.down == NO_COLOR
        
    def equivalent(self, tile):
        return tile.left == self.left and tile.right == self.right and tile.up == self.up and tile.down == self.down

    def draw(self, surface, x, y, size):
        pygame.draw.polygon(surface, self.left, 
                            [
                                (x, y),
                                (x + size/2, y + size/2),
                                (x, y + size)
                            ]
        )
        pygame.draw.polygon(surface, self.right, 
                            [
                                (x + size, y),
                                (x + size/2, y + size/2),
                                (x + size, y + size)
                            ]
        )
        pygame.draw.polygon(surface, self.down, 
                            [
                                (x, y + size),
                                (x + size/2, y + size/2),
                                (x + size, y + size)
                            ]
        )
        pygame.draw.polygon(surface, self.up, 
                            [
                                (x, y),
                                (x + size/2, y + size/2),
                                (x + size, y)
                            ]
        )

        if self.selected:
            pygame.draw.rect(surface, pygame.Color(0, 0, 0), (x, y, size, size), width = 4)

class Tile2x2:
    upperLeft: Tile
    upperRight: Tile
    lowerLeft: Tile
    lowerRight: Tile

    def up(self):
        return [self.upperLeft.up, self.upperRight.up]
    
    def down(self):
        return [self.lowerLeft.down, self.lowerRight.down]
    
    def right(self):
        return [self.upperRight.right, self.lowerRight.right]
    
    def left(self):
        return [self.upperLeft.left, self.lowerLeft.left]

    def validate(self):
        if (self.upperLeft.right == self.upperRight.left
            and self.upperLeft.down == self.lowerLeft.up
            and self.upperRight.down == self.lowerRight.up
            and self.lowerLeft.right == self.lowerRight.left):
            return True
        return False

def Tile2x2MatchHorizontal(left: Tile2x2, right: Tile2x2):
    if left.upperRight.right == right.upperLeft.left and left.lowerRight.right == right.lowerLeft.left:
        return True
    return False

def Tile2x2MatchVertical(up: Tile2x2, down: Tile2x2):
    if up.lowerLeft.down == down.upperLeft.up and up.lowerRight.down == down.upperRight.up:
        return True
    return False