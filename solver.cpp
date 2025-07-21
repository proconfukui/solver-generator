#include <vector>
#include <iostream>
#include <fstream>
#include "json.hpp"
using namespace std;
using namespace nlohmann;

int field_size;
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
    if (!input) {
        cerr << "Error: problem.json not found." << endl;
        exit(1);
    }
    json problem;
    try {
        input >> problem;
        input.close();

        // contains()でキーの存在を確認してからアクセスする
        if (problem.contains("problem") &&
            problem["problem"].contains("field") &&
            problem["problem"]["field"].contains("size") &&
            problem["problem"]["field"]["size"].is_number_integer())
        {
            field_size = problem["problem"]["field"]["size"].get<int>();
        } else {
            throw std::runtime_error("JSON key 'size' is missing or not an integer.");
        }

        if (problem["problem"]["field"].contains("entities") &&
            problem["problem"]["field"]["entities"].is_array())
        {
            field = problem["problem"]["field"]["entities"].get<vector<vector<int>>>();
        } else {
            throw std::runtime_error("JSON key 'entities' is missing or not a 2D array.");
        }

    } catch (const nlohmann::json::parse_error& e) {
        cerr << "JSON parse error: " << e.what() << endl;
        exit(1);
    } catch (const nlohmann::json::type_error& e) {
        cerr << "JSON type error: " << e.what() << endl;
        exit(1);
    } catch (const std::exception& e) {
        cerr << "An error occurred: " << e.what() << endl;
        exit(1);
    }

    max_number = field_size * field_size / 2;
    pair_coordinates = vector<vector<int>>(max_number, vector<int>(4, -1));
    for (int y = 0; y < field_size; y++)
    {
        for (int x = 0; x < field_size; x++)
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
    cout << "   ";
    for( int i = 0; i < field_size;i++){
        cout<< setw(3) << i << "|";
    }
    cout << endl;
    for (int y = 0; y < field_size; y++)
    {
        cout << setw(2) <<y << "|";
        for (int x = 0; x < field_size; x++)
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
        cout << setw(3) << number 
            << " (" << setw(2) << pair_coordinates[number][0] 
            << ", " << setw(2) << pair_coordinates[number][1] 
            << "), (" << setw(2) << pair_coordinates[number][2] 
            << ", " << setw(2) << pair_coordinates[number][3] 
            << ")" << endl;
    }
    cout << endl;
}

void rotate_field(int x, int y, int n)
{
    if (n < 2 || x < 0 || x + n > field_size || y < 0 || y + n > field_size)
    {
        cout << "導きが無効です(x: " << x << ", y: " << y << ", n: " << n << ")" << endl;
        return;
    }

    for (int number = 0; number < max_number; number++)
    {
        if (x <= pair_coordinates[number][0] && 
            pair_coordinates[number][0] < x + n && 
            y <= pair_coordinates[number][1] && 
            pair_coordinates[number][1] < y + n)
        {
            int dx = pair_coordinates[number][0] - x;
            int dy = pair_coordinates[number][1] - y;
            pair_coordinates[number][0] = x + n - 1 - dy;
            pair_coordinates[number][1] = y + dx;
        }
        if (x <= pair_coordinates[number][2] && 
            pair_coordinates[number][2] < x + n && 
            y <= pair_coordinates[number][3] && 
            pair_coordinates[number][3] < y + n)
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

void move_pair1(int target_entity, int goal_x, int goal_y)
{
    int current_x1 = pair_coordinates[target_entity][0];
    int current_y1 = pair_coordinates[target_entity][1];
    int current_x2 = pair_coordinates[target_entity][2];
    int current_y2 = pair_coordinates[target_entity][3];

    int target_pair_x, target_pair_y;
    int goal_pair_x = goal_x;
    int goal_pair_y = goal_y;

    // goal_x, goal_y にあるのが1つ目と仮定し、2つ目を動かす
    if (current_x1 == goal_x && current_y1 == goal_y)
    {
        target_pair_x = current_x2;
        target_pair_y = current_y2;
    }
    else // goal_x, goal_y にあるのが2つ目と仮定し、1つ目を動かす
    {
        target_pair_x = current_x1;
        target_pair_y = current_y1;
    }

    // targetがgoalのすぐ下にある場合
    if(goal_pair_x == target_pair_x && goal_pair_y + 1 == target_pair_y){
        rotate_field(goal_pair_x,goal_pair_y,2);
    }

    // 動かしたいエンティティが目標の右下にある場合
    if(goal_pair_x < target_pair_x && goal_pair_y < target_pair_y)
    {
        goal_pair_x++;
        int max_rotate_size;
        if(target_pair_x - goal_pair_x < target_pair_y - goal_pair_y){
            max_rotate_size = target_pair_x - goal_pair_x + 1;
        }else{
            max_rotate_size = target_pair_y - goal_pair_y + 1;
        }
        rotate_field(target_pair_x - max_rotate_size + 1, target_pair_y - max_rotate_size + 1,max_rotate_size);
        return;
    }
    // 動かしたいエンティティが目標の左下にある場合
    if(goal_pair_x > target_pair_x && goal_pair_y < target_pair_y){
        goal_pair_x++;
        int max_rotate_size;
        if(goal_pair_x - target_pair_x < target_pair_y - goal_pair_y){
            max_rotate_size = goal_pair_x -target_pair_x + 1;
        }else{
            max_rotate_size = goal_pair_y - target_pair_x + 1;
        }
        rotate_field(target_pair_x, target_pair_y - max_rotate_size + 1 , max_rotate_size);
        return;
    }

    

}

void solve2()
{
    for (int y = 0; y < field_size; y++) // 下から2行を処理対象外にする
    {
        for (int x = 0; x < field_size; x+=2) // 右から2列を処理対象外にする
        {
            if( y == field_size - 2 && x >= field_size - 2) break; // 最後の2×2ブロックは処理しない
            int target = field[y][x];
            // (y, x) にある target のペアを (y, x+1) に移動させる
            // ただし、既にペアが揃っている場合はスキップ
            if (field[y][x+1] == target) continue;

            move_pair1(target, x, y); // target のペアの片方を (x, y) の隣に移動
            print_field(); // デバッグ用にフィールドを表示
            print_pair_coordinates(); // デバッグ用にペアの座標を表示
        }
    }
}


void solve1(){
    for (int y = 0; y < field_size; y+=2)
    {
        for (int x = 0; x < field_size; x+=2)
        {
            int target = field[y][x];
            // ペアが既に揃っていたらスキップ
            if (target == field[y][x+1]) continue; 
            int target_pair_x = pair_coordinates[target][2];
            int target_pair_y = pair_coordinates[target][3];

            // X 0 3 3  
            // 1 1 2 3 
            // 2 1 2 2 
            // 2 2 2 3 
            // 上は4×4の場合の例。何回でXの右にペアを作れるかを示している
            // 下の二つのif文はは1回でペアを揃えられる時の条件とその導きである
            if (x == target_pair_x && target_pair_y < field_size - x - 1){
                rotate_field(x+1, y, target_pair_y +1 );
                continue;
            }
            if(x + 1 == target_pair_x && y == target_pair_y){
                rotate_field(x, y, 2);
                continue;
            }
                

            
        }
    }
}

int main()
{
    load_problem();

    print_field();
    //move_pair1(field[0][0],0,0);
    //move_pair1(field[0][10],10,0);
    // for(int x = 0; x < field_size; x += 2){
    //     while(field[0][x] != field[0][x+1]){
    //         move_pair1(field[0][x],x,0);
    //     }
        
    // }
    print_field();

    export_answer();
}
