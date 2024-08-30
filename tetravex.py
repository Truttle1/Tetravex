import pygame
from tile import Tile
from board import Board

pygame.init()
screen = pygame.display.set_mode((640, 480))
clock = pygame.time.Clock()
running = True

board = Board()
pieces_board = Board(320, 32)
board.show_empty = True

board.generate_empty()
pieces_board.generate()
pieces_board.show_empty = False

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
        
        if event.type == pygame.MOUSEBUTTONDOWN:
            x, y = event.pos

            sel_board, sel_x, sel_y = board.handle_click(x, y, sel_board, sel_x, sel_y)
            sel_board, sel_x, sel_y = pieces_board.handle_click(x, y, sel_board, sel_x, sel_y)
        
    screen.fill('white')
    board.render(image)
    pieces_board.render(image)
    screen.blit(image, (0, 0))

    if board.solved():
        txt = my_font.render("You did it!", 0, (0, 0, 0))
        screen.blit(txt, (128, 400))

    pygame.display.flip()
    clock.tick(60)
    
pygame.quit()