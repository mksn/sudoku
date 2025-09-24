// Sudoku.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Sudoku {
    std::array<std::array<int, 9>, 9> board{};
    std::array<std::array<int, 9>, 9> original{};

    std::uint64_t solve_steps = 0;

public:
    Sudoku() = default;

    explicit Sudoku(std::istream& in) {
        if (!load(in)) {
            throw std::runtime_error("Failed to load Sudoku from stream");
        }
    }

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

    friend std::ostream& operator<<(std::ostream& out, const Sudoku& s) {
        return out << s.toString();
    }

    bool isConsistent() const {
        for (int row = 0; row < 9; ++row) {
            bool seen[10] = { false };
            for (int col = 0; col < 9; ++col) {
                int v = board[row][col];

                if (v && seen[v])
                    return false;
                if (v)
                    seen[v] = true;
            }
        }

        for (int col = 0; col < 9; ++col) {
            bool seen[10] = { false };
            for (int row = 0; row < 9; ++row) {
                int v = board[row][col];

                if (v && seen[v])
                    return false;
                if (v)
                    seen[v] = true;
            }
        }

        for (int br = 0; br < 9; br += 3) {
            for (int bc = 0; bc < 9; bc += 3) {
                bool seen[10] = { false };

                for (int dr = 0; dr < 3; ++dr) {
                    for (int dc = 0; dc < 3; ++dc) {
                        int v = board[br + dr][bc + dc];

                        if (v && seen[v])
                            return false;
                        if (v)
                            seen[v] = true;
                    }
                }
            }
        }
        return true;
    }

    bool isSafe(int row, int col, int val) const {
        for (int c = 0; c < 9; ++c)
            if (board[row][c] == val)
                return false;

        for (int r = 0; r < 9; ++r)
            if (board[r][col] == val)
                return false;

        int br = (row / 3) * 3, bc = (col / 3) * 3;
        for (int dr = 0; dr < 3; ++dr)
            for (int dc = 0; dc < 3; ++dc)
                if (board[br + dr][bc + dc] == val)
                    return false;
        return true;
    }

    bool findEmpty(int& row, int& col) const {
        for (row = 0; row < 9; ++row)
            for (col = 0; col < 9; ++col)
                if (board[row][col] == 0)
                    return true;
        return false;
    }

    bool solveFirst() {
        int row, col;
        
        if (!findEmpty(row, col)) 
            return true; // solved
        
        for (int val = 1; val <= 9; ++val) {
            if (isSafe(row, col, val)) {
                board[row][col] = val;
				++solve_steps;
                if (solveFirst())
                    return true;
                board[row][col] = 0;
            }
        }
        return false;
    }

    int countSolutions(int cap) {
        int row, col;
        if (!findEmpty(row, col)) return 1;
        int count = 0;

        for (int val = 1; val <= 9; ++val) {
            if (isSafe(row, col, val)) {
                board[row][col] = val;
                count += countSolutions(cap - count);
                if (count >= cap) {
                    board[row][col] = 0;
                    return count;
                }
                board[row][col] = 0;
            }
        }
        return count;
    }

    int findSolutions() {
        original = board;

        if (!solveFirst()) {
            board = original;
            return 0;
        }
        auto solvedOnce = board;

        board = original;
        int cnt = countSolutions(2);

        if (cnt == 1) {
            board = solvedOnce;
            return 1;
        }
        else if (cnt >= 2) {
            board = solvedOnce;
            return 2;
        }

        board = original;
        return 0;
    }

    static int countGivens(const std::array<std::array<int, 9>, 9>& b) {
        int k = 0; 
        for (const auto& r : b) 
            for (int v : r) 
                if (v > 0) ++k;
        return k;
    }

    std::string gradeLabel() const {
        int givens = countGivens(original);
        std::uint64_t s = solve_steps;

        if (givens >= 30 && s < 2000) return "easy";
        if (givens >= 26 && s < 20000) return "medium";
        if (givens >= 22 && s < 70000) return "hard";
        return "samurai";
    }
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <inputfile>\n";
        return 1;
    }

    try {
        Sudoku s(argv[1]);

        if (!s.isConsistent()) {
            std::cerr << "Input violates Sudoku rules (duplicate in row/col/box).\n";
            return 2;
        }
        
        int status = s.findSolutions();
        if (status == 0) {
            std::cout << "No solution found." << std::endl;
            return 0;
        }
        else if (status == 2) {
            std::cout << "Warning: multiple solutions exist. One valid solution:" << std::endl;
        }
        else {
            std::cout << "Solved uniquely:" << std::endl;
        }
        std::cout << s;

        std::cout << "Estimated difficulty: " << s.gradeLabel()
			      << " (givens and backtracking steps considered)" << std::endl;
    }

    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 2;
    }

    return 0;
}


