# TetraVex Solver

This is a TetraVex solver! TetraVex is an edge matching puzzle game, known for appearing in Microsoft Entertainment Pack 3. The object is simple: match the edges of the same symbol/color until the entire square puzzle is tiled. Despite the simple rules, the puzzle is NP-Complete. An even more difficult variation, [Eternity II](https://en.wikipedia.org/wiki/Eternity_II_puzzle), which added piece rotation, promised a $2 million cash prize for a solution between 2007 and 2010. Despite this high reward, the prize was never claimed, and the puzzle remains unsolved in 2025.

This repository contains a solver for regular TetraVex puzzles, where pieces can be placed but not rotated. I tried several parallelization techniques to optimize the solver and solve the puzzles as quickly as possible. This was originally a term project for [EECS 587 at the University of Michigan](https://web.eecs.umich.edu/~qstout/587_Overview.pdf), and it contains the project write up.
