// Sudoku.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

class Sudoku {
    std::array<std::array<int, 9>, 9> board{};

    std::uint64_t solve_steps = 0;
	int initial_givens = 0;

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
        int k = 0; for (const auto& r : board) for (int v : r) if (v) ++k; initial_givens = k;
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
            oss << std::endl;
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
            return true;
        
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
        auto original = board;

        solve_steps = 0;
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
        int givens = initial_givens;
        std::uint64_t s = solve_steps;

		std::cout << "[debug] solve steps: " << s << ", initial givens: " << givens << std::endl;
        if (givens >= 30 && s < 2'000) return "easy";
        if (givens >= 26 && s < 20'000) return "medium";
        if (givens >= 22 && s < 80'000) return "hard";
        return "samurai";
    }

    std::string estimateGrade() const {
        Sudoku tmp = *this;
        int status = tmp.findSolutions();
  
        if (status == 0) return "unsolvable";
        return tmp.gradeLabel();
    }

    static Sudoku generate(const std::string& level = "medium") {
        const int maxAttempts = 40;
        Sudoku best;
        std::string bestGrade;

        for (int attempt = 0; attempt < maxAttempts; ++attempt) {
            Sudoku s;
 
            s.fillFull();
            s.digHolesUnique(level);
			s.initial_givens = countGivens(s.board);
            std::string g = s.estimateGrade();
            if (g == level) {
                return s;
            }
            if (attempt == 0) {
                best = s;
            }
		}
        return best;
        Sudoku s;
    }

    const std::array<std::array<int, 9>, 9>& data() const { return board; }
    std::array<std::array<int, 9>, 9>& data() { return board; }

private:
    static std::mt19937& randomNumberGenerator() {
        static std::mt19937 gen(std::random_device{}());
        return gen;
    }

    static std::vector<int> shuffledDigits() {
        std::vector<int> v{ 1,2,3,4,5,6,7,8,9 };
        std::shuffle(v.begin(), v.end(), randomNumberGenerator());
        return v;
    }

    bool fillFull() {
        int row, col;
        if (!findEmpty(row, col)) return true; // filled
        auto vals = shuffledDigits();
        for (int v : vals) {
            if (isSafe(row, col, v)) {
                board[row][col] = v;
                if (fillFull()) return true;
                board[row][col] = 0;
            }
        }
        return false;
    }

    void digHolesUnique(const std::string& level) {
        int targetGivens = 28; 
        if (level == "easy") targetGivens = 36;
        else if (level == "hard") targetGivens = 24;
        else if (level == "samurai") targetGivens = 22;

        std::vector<std::pair<int, int>> cells;
        cells.reserve(41);
        for (int r = 0; r < 9; ++r) {
            for (int c = 0; c < 9; ++c) {
                int r2 = 8 - r, c2 = 8 - c;
                if (r < r2 || (r == r2 && c <= c2)) {
                    cells.emplace_back(r, c);
                }
            }
        }
        std::shuffle(cells.begin(), cells.end(), randomNumberGenerator());

        for (const auto& [r, c] : cells) {
            if (countGivens(board) <= targetGivens) break;

            int r2 = 8 - r, c2 = 8 - c;
            int v1 = board[r][c], v2 = board[r2][c2];
            if (v1 == 0 && v2 == 0) continue;
    
            int tmp1 = board[r][c];
            int tmp2 = board[r2][c2];
            board[r][c] = 0; 
            board[r2][c2] = 0;
            
            auto backup = board;
            int n = countSolutions(2);
            board = backup;
            if (n != 1) {
                board[r][c] = tmp1; 
                board[r2][c2] = tmp2;
            }
        }
    }
};

static void usage(const char* prog) {
    std::cerr << "Usage:" << std::endl;
    std::cerr << "  " << prog << " solve <file>" << std::endl;
    std::cerr << "  " << prog << " grade <file>" << std::endl;
    std::cerr << "  " << prog << " generate [easy|medium|hard|samurai]" << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 2) { usage(argv[0]); return 1; }

    std::string cmd = argv[1];

    try {
        if (cmd == "solve" || cmd == "grade") {
            if (argc < 3) { usage(argv[0]); return 1; }
            Sudoku s(argv[2]);

            if (!s.isConsistent()) {
                std::cerr << "Input violates Sudoku rules (duplicate in row/col/box)." << std::endl;
                return 2;
            }
            int status = s.findSolutions();
            
            if (cmd == "solve") {
                if (status == 0) {
                    std::cout << "No solution found." << std::endl;
                }
                else if (status == 2) {
                    std::cout << "Warning: multiple solutions exist. One valid solution:" << std::endl;
                    std::cout << s;
                }
                else {
                    std::cout << "Solved uniquely:" << std::endl;
                    std::cout << s;
                }
            }
            else {
                (void)status;
                std::cout << s.gradeLabel() << std::endl;
            }
        }
        else if (cmd == "generate") {
            std::string level = (argc >= 3 ? argv[2] : std::string("medium"));
            Sudoku p = Sudoku::generate(level);
            Sudoku tmp; tmp.data() = p.data();
            int n = tmp.countSolutions(2);
            std::string achieved = p.estimateGrade();

            if (n != 1) {
                std::cerr << "[warn] generated puzzle may not be unique; consider regenerating." << std::endl;
            }
            if (achieved != level) {
                std::cerr << "[note] requested level='" << level << "' but achieved '" << achieved << "'." << std::endl;
			}
            std::cout << p;
        }
        else {
            usage(argv[0]);
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 3;
    }

    return 0;
}


