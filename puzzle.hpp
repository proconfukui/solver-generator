// puzzle.hpp
#pragma once
#include <vector>
#include <string>
#include <set>
#include "json.hpp"
#include "utils.hpp"

using namespace std;
using namespace nlohmann;

class Puzzle
{
public:
    int field_size;
    int max_pair_number;
    Field field;
    
    // pair_coordinates[n][0]: 値がnのエンティティ1つ目のX
    // pair_coordinates[n][1]: 値がnのエンティティ1つ目のY
    // pair_coordinates[n][2]: 値がnのエンティティ2つ目のX
    // pair_coordinates[n][3]: 値がnのエンティティ2つ目のY
    vector<vector<int>> pair_coordinates;
    vector<Operation> ops;
    set<Field> field_history;

    Puzzle(const string& filename); // コンストラクタ

    void apply_rotation(Operation op);
    void rotate_for_simulation(Operation op);
    void undo_rotation(Operation op);
    void random_rotation();

    bool check_rotation_value(Operation op);
    
    void export_answer(const string& filename) const;
    void print_field() const;

private:
    void load_problem(const string& filename);
    void rotate_field_internal(Operation op, int direction);
};