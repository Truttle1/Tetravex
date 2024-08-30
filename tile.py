import pygame 

COLOR_BANK = [
    pygame.Color(255, 0, 0),
    pygame.Color(0, 255, 0),
    pygame.Color(0, 0, 255),
    pygame.Color(255, 255, 0),
    pygame.Color(0, 255, 255),
    pygame.Color(255, 0, 255),
    pygame.Color(255, 128, 0),
    pygame.Color(64, 64, 64),
    pygame.Color(128, 128, 128),
    pygame.Color(200, 200, 200),
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
            self.left = NO_COLOR
            self.right = NO_COLOR
            self.up = NO_COLOR
            self.down = NO_COLOR

        
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