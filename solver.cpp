#include <vector>
#include <iostream>
#include <fstream>
#include "json.hpp"
using namespace std;
using namespace nlohmann;

int size = 0;
vector<vector<int>> entities;

void print_field(){
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            cout << entities[y][x] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

int main()
{
    ifstream input("problem.json");
    json problem;
    input >> problem;
    input.close();
    size = problem["problem"]["field"]["size"].get<int>();
    entities = problem["problem"]["field"]["entities"].get<vector<vector<int>>>();

    print_field();
}
