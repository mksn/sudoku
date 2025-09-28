This project is builds a program solves, grades and generates sudokus. 
To run the program, use the commmand ./x64/Debug/Sudoku.exe

To solve any given text file that contains a sudoku puzzle, use the command:
./x64/Debug/Sudoku.exe solve <input file>

The file needs to be in the following format: 1-9 for the numbers and . for the empty cells.
The solved sudok is the printed to the console.

To grade a given text file that contains a sudoku puzzle, use the command:
./x64/Debug/Sudoku.exe grade <input file>

To generate a sudoku puzzle with a unique solution, use the command:
./x64/Debug/Sudoku.exe generate <difficulty>

where difficulty is one of the following: easy, medium, hard, samurai