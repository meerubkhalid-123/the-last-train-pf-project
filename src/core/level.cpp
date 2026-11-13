#include "level.h"
#include "util.h"
#include <fstream>
#include <iostream>
using namespace std;
#include <string>
// Modular implementation using primitive arrays for map parsing and verification
bool loadBuildingMap(const char* filename,
                     char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                     char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                     int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR])
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cout << "Error: Unable to open building map." << endl;
        return false;
    }

    string line;

    while (getline(file, line))
    {
        for (int i = 0; i < line.length(); i++)
            {
                cout << line[i] << " ";
            }
            cout << endl;
    }

    file.close();

    return true;
}