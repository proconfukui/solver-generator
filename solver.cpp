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
    cout << "手数: " << ops_x.size() << endl;
    for (int y = 0; y < size; y++)
    {
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
            int dx = pair_coordinates[number][0] - x;
            int dy = pair_coordinates[number][1] - y;
            pair_coordinates[number][0] = x + n - 1 - dy;
            pair_coordinates[number][1] = y + dx;
        }
        if (x <= pair_coordinates[number][2] && pair_coordinates[number][2] < x + n && y <= pair_coordinates[number][3] && pair_coordinates[number][3] < y + n)
        {
            int dx = pair_coordinates[number][2] - x;
            int dy = pair_coordinates[number][3] - y;
            pair_coordinates[number][2] = x + n - 1 - dy;
            pair_coordinates[number][3] = y + dx;
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

int main()
{
    load_problem();

    print_field();
    print_pair_coordinates();
    rotate_field(0, 0, size);
    print_field();
    print_pair_coordinates();

    export_answer();
}
