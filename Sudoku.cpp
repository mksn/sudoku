// Sudoku.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Sudoku {
    std::array<std::array<int, 9>, 9> board{}; // 0 for empty, 1..9 for digits

public:
    // Default constructor
    Sudoku() = default;

    // Construct directly from an input stream
    explicit Sudoku(std::istream& in) {
        if (!load(in)) {
            throw std::runtime_error("Failed to load Sudoku from stream");
        }
    }

    // Construct directly from a filename
    explicit Sudoku(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        if (!load(file)) {
            throw std::runtime_error("Failed to load Sudoku from file: " + filename);
        }
    }

    bool load(std::istream& in) {
        std::string line;
        int row = 0;

        while (row < 9 && std::getline(in, line)) {
            if ((int)line.size() < 9)
                return false;

            for (int col = 0; col < 9; col++) {
                char ch = line[col];

                if (isdigit(ch))
                    board[row][col] = ch - '0';
                else
                    board[row][col] = 0;
            }
            row++;
        }
        return row == 9;
    }

    // Return the ASCII representation of the board as a string
    std::string toString() const {
        std::ostringstream oss;
        for (int row = 0; row < 9; row++) {
            for (int col = 0; col < 9; col++) {
                if (board[row][col] == 0)
                    oss << '.';
                else
                    oss << board[row][col];
            }
            oss << '\n';
        }
        return oss.str();
    }

    // Stream insertion operator uses toString()
    friend std::ostream& operator<<(std::ostream& out, const Sudoku& s) {
        return out << s.toString();
    }
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <inputfile>\n";
        return 1;
    }

    try {
        Sudoku s(argv[1]);
        std::cout << "Read Sudoku board:" << "\n";
        std::cout << s; // uses operator<<
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }

    return 0;
}


