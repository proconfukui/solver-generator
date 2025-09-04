// puzzle.cpp
#include "puzzle.hpp"
#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <random>

Puzzle::Puzzle(const string& filename) {
    load_problem(filename);
}

void Puzzle::load_problem(const string& filename) {
    ifstream input(filename);
    if (!input) {
        cerr << "Error: " << filename << " not found." << endl;
        exit(1);
    }
    json problem;
    try {
        input >> problem;
        input.close();

        if (problem.contains("problem") && problem["problem"].contains("field") &&
            problem["problem"]["field"].contains("size") && problem["problem"]["field"]["size"].is_number_integer()) {
            field_size = problem["problem"]["field"]["size"].get<int>();
        } else {
            throw runtime_error("JSON key 'size' is missing or not an integer.");
        }

        if (problem["problem"]["field"].contains("entities") && problem["problem"]["field"]["entities"].is_array()) {
            field = {problem["problem"]["field"]["entities"].get<vector<vector<int>>>()};
        } else {
            throw runtime_error("JSON key 'entities' is missing or not a 2D array.");
        }
    }
    catch (const json::parse_error &e) {
        cerr << "JSON parse error: " << e.what() << endl;
        exit(1);
    }
    catch (const exception &e) {
        cerr << "An error occurred: " << e.what() << endl;
        exit(1);
    }

    max_number = field_size * field_size / 2;
    pair_coordinates = vector<vector<int>>(max_number, vector<int>(4, -1));
    for (int y = 0; y < field_size; y++) {
        for (int x = 0; x < field_size; x++) {
            int number = field.grid[y][x];
            if (pair_coordinates[number][0] == -1) {
                pair_coordinates[number][0] = x;
                pair_coordinates[number][1] = y;
            } else {
                pair_coordinates[number][2] = x;
                pair_coordinates[number][3] = y;
            }
        }
    }
}

void Puzzle::rotate_field_internal(Operation op, int direction) {
    int x = op.x;
    int y = op.y;
    int n = op.n;
    if (!check_rotation_value(op)) {
        cout << "This step is invalid(x: " << x << ", y: " << y << ", n: " << n << ")" << endl;
        return;
    }

    for (int number = 0; number < max_number; number++) {
        for (int i = 0; i <= 2; i += 2) {
            int& px = pair_coordinates[number][i];
            int& py = pair_coordinates[number][i + 1];
            if (x <= px && px < x + n && y <= py && py < y + n) {
                int dx = px - x;
                int dy = py - y;
                if (direction == 1) { // Clockwise
                    px = x + n - 1 - dy;
                    py = y + dx;
                } else { // Counter-clockwise
                    px = x + dy;
                    py = y + n - 1 - dx;
                }
            }
        }
    }

    vector<vector<int>> memo(n, vector<int>(n));
    for (int dy = 0; dy < n; dy++) {
        for (int dx = 0; dx < n; dx++) {
            if (direction == 1)
                memo[dy][dx] = field.grid[y + n - 1 - dx][x + dy];
            else
                memo[dy][dx] = field.grid[y + dx][x + n - 1 - dy];
        }
    }
    for (int dy = 0; dy < n; dy++) {
        for (int dx = 0; dx < n; dx++) {
            field.grid[y + dy][x + dx] = memo[dy][dx];
        }
    }
}

void Puzzle::apply_rotation(Operation op) {
    rotate_field_internal(op, 1);
    ops.push_back(op);
    field_history.insert(field);
}

void Puzzle::rotate_for_simulation(Operation op) {
    rotate_field_internal(op, 1);
}

void Puzzle::undo_rotation(Operation op) {
    rotate_field_internal(op, -1);
}

void Puzzle::random_rotation(){
    random_device seed_gen;
    mt19937 engine(seed_gen());
    Operation random_op;
    while(true){
        uniform_int_distribution<> distrib(0 , this->field_size -1);
        random_op.x = distrib(seed_gen);
        random_op.y = distrib(seed_gen);
        random_op.n = distrib(seed_gen);
        if(check_rotation_value(random_op)) break;
    }
    apply_rotation(random_op);
}

bool Puzzle::check_rotation_value(Operation op){
    int x = op.x;
    int y = op.y;
    int n = op.n;
    if (n < 2 || x < 0 || x + n > field_size || y < 0 || y + n > field_size) {
        return false;
    }
    return true;
}

void Puzzle::export_answer(const string& filename) const {
    json answer;
    for (size_t i = 0; i < ops.size(); i++) {
        answer["ops"][i]["x"] = ops[i].x;
        answer["ops"][i]["y"] = ops[i].y;
        answer["ops"][i]["n"] = ops[i].n;
    }
    ofstream output(filename);
    output << setw(4) << answer << endl;
    output.close();
}

void Puzzle::print_field() const {
    cout << "step: " << ops.size() << endl;
    if(!ops.empty()) ops.back().print();
    cout << "   ";
    for (int i = 0; i < field_size; i++) {
        cout << setw(3) << i << "|";
    }
    cout << endl;
    for (int y = 0; y < field_size; y++) {
        cout << setw(2) << y << "|";
        for (int x = 0; x < field_size; x++) {
            cout << setw(3) << field.grid[y][x] << " ";
        }
        cout << endl;
    }
    cout << endl;
}