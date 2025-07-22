#include <vector>
#include <iostream>
#include <fstream>
#include "json.hpp"
using namespace std;
using namespace nlohmann;

int size;
vector<vector<int>> field;

int max_number;
// pair_coordinates[n][0]: 値がnのエンティティ1つ目のX
// pair_coordinates[n][1]: 値がnのエンティティ1つ目のY
// pair_coordinates[n][2]: 値がnのエンティティ2つ目のX
// pair_coordinates[n][3]: 値がnのエンティティ2つ目のY
vector<vector<int>> pair_coordinates;

vector<int> ops_x = vector<int>(0);
vector<int> ops_y = vector<int>(0);
vector<int> ops_n = vector<int>(0);

void load_problem()
{
    ifstream input("problem.json");
    json problem;
    input >> problem;
    input.close();
    size = problem["problem"]["field"]["size"].get<int>();
    field = problem["problem"]["field"]["entities"].get<vector<vector<int>>>();
    max_number = size * size / 2;
    pair_coordinates = vector<vector<int>>(max_number, vector<int>(4, -1));
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            int number = field[y][x];
            if (pair_coordinates[number][0] == -1)
            {
                pair_coordinates[number][0] = x;
                pair_coordinates[number][1] = y;
            }
            else
            {
                pair_coordinates[number][2] = x;
                pair_coordinates[number][3] = y;
            }
        }
    }
}

// デバッグ用
void print_field()
{
    cout << "手数: " << ops_x.size() << endl
         << "   ";
    for (int y = 0; y < size; y++)
    {
        cout << setw(3) << y << "|";
    }
    cout << endl;
    for (int y = 0; y < size; y++)
    {
        cout << setw(2) << y << "|";
        for (int x = 0; x < size; x++)
        {
            cout << setw(3) << field[y][x] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// デバッグ用
void print_pair_coordinates()
{
    for (int number = 0; number < max_number; number++)
    {
        cout << setw(3) << number << ": (" << setw(2) << pair_coordinates[number][0] << ", " << setw(2) << pair_coordinates[number][1] << "), (" << setw(2) << pair_coordinates[number][2] << ", " << setw(2) << pair_coordinates[number][3] << ")" << endl;
    }
    cout << endl;
}

void rotate_field(int x, int y, int n)
{
    if (n < 2 || x < 0 || x + n > size || y < 0 || y + n > size)
    {
        cout << "導きが無効です(x: " << x << ", y: " << y << ", n: " << n << ")" << endl;
        return;
    }

    for (int number = 0; number < max_number; number++)
    {
        if (x <= pair_coordinates[number][0] && pair_coordinates[number][0] < x + n && y <= pair_coordinates[number][1] && pair_coordinates[number][1] < y + n)
        {
            pair_coordinates[number][0] = x + y + n - 1 - pair_coordinates[number][1];
            pair_coordinates[number][1] = y - x + pair_coordinates[number][0];
        }
        if (x <= pair_coordinates[number][2] && pair_coordinates[number][2] < x + n && y <= pair_coordinates[number][3] && pair_coordinates[number][3] < y + n)
        {
            pair_coordinates[number][2] = x + y + n - 1 - pair_coordinates[number][3];
            pair_coordinates[number][3] = y - x + pair_coordinates[number][2];
        }
    }

    vector<vector<int>> memo = vector<vector<int>>(n, vector<int>(n));
    for (int dy = 0; dy < n; dy++)
    {
        for (int dx = 0; dx < n; dx++)
        {
            memo[dy][dx] = field[y + n - 1 - dx][x + dy];
        }
    }
    for (int dy = 0; dy < n; dy++)
    {
        for (int dx = 0; dx < n; dx++)
        {
            field[y + dy][x + dx] = memo[dy][dx];
        }
    }

    ops_x.push_back(x);
    ops_y.push_back(y);
    ops_n.push_back(n);
}

void export_answer()
{
    json answer;
    for (int index = 0; index < ops_x.size(); index++)
    {
        answer["ops"][index]["x"] = ops_x[index];
        answer["ops"][index]["y"] = ops_y[index];
        answer["ops"][index]["n"] = ops_n[index];
    }
    ofstream output("answer.json");
    output << setw(4) << answer << endl;
    output.close();
}

void move_pair1(int target_entity, int goal_x, int goal_y)
{
    int target_x, target_y;
    if (pair_coordinates[target_entity][0] == goal_x && pair_coordinates[target_entity][1] == goal_y)
    {
        // ペアの1つ目が(goal_x,goal_y)にある。2つ目を動かす
        target_x = pair_coordinates[target_entity][2];
        target_y = pair_coordinates[target_entity][3];
    }
    else
    {
        // ペアの2つ目が(goal_x,goal_y)にある。1つ目を動かす
        target_x = pair_coordinates[target_entity][0];
        target_y = pair_coordinates[target_entity][1];
    }

    // targetがgoalの直下にある
    if (goal_x == target_x && goal_y + 1 == target_y)
    {
        rotate_field(goal_x, goal_y, 2);
        return;
    }

    
}

// g++ -o solver solver.cpp -std=c++14 でコンパイルしないとエラーを吐く
int main()
{
    load_problem();

    print_field();
    for (int y = 0; y < 2; y++)
    {
        for (int x = 0; x < size; x += 2)
        {
            int target_number = field[y][x];
            if (field[y][x + 1] == target_number) // 既にペアが揃っている
            {
                continue;
            }
            if (field[y + 1][x] == target_number) // 直下にペアがある
            {
                rotate_field(x, y, 2);
                print_field();
                continue;
            }
            // 1手でペアが揃うパターン
            if (pair_coordinates[target_number][2] == x + 1 && pair_coordinates[target_number][3] - y < size - 1 - x)
            {
                rotate_field(x + 1, y, pair_coordinates[target_number][3] + 1 - y);
                print_field();
                continue;
            }
            break;
        }
        break;
    }

    export_answer();
}
