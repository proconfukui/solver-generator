#include <iostream> // cout, endl, cin
#include <vector>
#include <algorithm>
#include <random>
#include "json.hpp"
using namespace std;
using namespace nlohmann;

int main()
{
    int size = 0;
    do
    {
        cout << "フィールドのサイズ(4~24の偶数)? ";
        cin >> size;
    } while (size % 2 != 0 || size < 4 || size > 24);
    vector<vector<int>> entities(size, vector<int>(size));
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            entities[y][x] = (y * size + x) / 2;
        }
        // entities[index] = index / 2;
        // entities[index + 1] = index / 2;
    }
    random_device seed_gen;
    mt19937 engine(seed_gen());
    shuffle(entities.begin(), entities.end(), engine);
    // json problem;
    // problem["startsAt"] = 0;
    // problem["problem"]["field"]["size"] = size;
    // problem["problem"]["field"]["entities"] = entities;
}
