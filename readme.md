# Disclaimer before you continue

This is a Work in progress. the majority of example puzzles work (not including rigid objects or randomness)

But if this project is interesting to you, consider contributing! Making a new implementation of PuzzleScript is a huge undertaking. I look forward to making puzzles with you.

# Puzzle Engine

A C implementation of the PuzzleScript language and runtime

It has a number of renders you can use:

## 3d

![3d Puzzle](./img/3d_puzzle_demo.gif)

## 2d

![2d Puzzle](./img/2d_puzzlescript.gif)

# Text Only

![Text Puzzle](./img/nc_puzzlescript.gif)

## Required libs

`check`, `flex`, `bison`, `gcc`, `sdl2`, `raylib`

## Mac build instructions

```bash
brew install check flex bison gcc
make
```

## Run tests

```bash
make check
```
