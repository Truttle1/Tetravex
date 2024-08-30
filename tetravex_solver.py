import pygame
from tile import Tile
from board import Board
from solver import Solver
from solver import SolverState

pygame.init()
screen = pygame.display.set_mode((640, 480))
clock = pygame.time.Clock()
running = True

sz = 8

board = Board(size=sz)
pieces_board = Board(320, 32, size=sz)
board.show_empty = True

board.generate_empty()
pieces_board.generate()
pieces_board.show_empty = False
tiles = pieces_board.get_tile_list()

solver = Solver(board, tiles)

sel_board = None
sel_x = None
sel_y = None

my_font = pygame.font.SysFont("Arial", 72, bold=True)

while running:
    image = pygame.Surface((640, 480))
    pygame.draw.rect(image, (255, 255, 255), (0, 0, 640, 480))
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        
    screen.fill('white')
    solver.main_board.render(image)
    solver.churn()

    screen.blit(image, (0, 0))

    if solver.main_board.solved():
        txt = my_font.render("Solved!", 0, (0, 0, 0))
        screen.blit(txt, (128, 400))

    pygame.display.flip()
    clock.tick(999999999999)
    
pygame.quit()