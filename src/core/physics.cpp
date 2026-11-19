#include "physics.h"
#include "util.h"

// Movement validation and resource adjustments without structures
// All floor/room values are 1-based (F1-R1 = 1,1), the same as loadInitialState()
// and validateStartingRoom(). Array indices are converted with "- 1" only where
// an array is actually accessed.

// ================= INVENTORY =================

// Search only the filled part of the array (0 .. inventoryCount-1).
bool hasItem(const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, char itemSymbol) {
    for (int i = 0; i < inventoryCount; i++) {
        if (inventory[i] == itemSymbol) return true;
    }
    return false;
}

// Append at the next free slot; refuse if the inventory is full.
bool addItem(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, char itemSymbol) {
    if (inventoryCount >= MAX_INVENTORY_CAPACITY) return false;
    inventory[inventoryCount] = itemSymbol;
    inventoryCount++;
    return true;
}

// Find the item, shift later items one step left, then shrink the count.
bool removeItem(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, char itemSymbol) {
    for (int i = 0; i < inventoryCount; i++) {
        if (inventory[i] == itemSymbol) {
            for (int j = i; j < inventoryCount - 1; j++) {
                inventory[j] = inventory[j + 1];
            }
            inventoryCount--;
            inventory[inventoryCount] = ' ';   // clear the freed slot
            return true;
        }
    }
    return false;
}

// ================= RESOURCES =================

// Costs in config.h are already negative, so the delta is simply added.
// Spec: values are clamped between 0 and the maximum.
void applyResourceDelta(int& energy, int& time, int deltaEnergy, int deltaTime) {
    energy += deltaEnergy;
    time   += deltaTime;

    if (energy > MAX_ENERGY) energy = MAX_ENERGY;
    if (energy < 0)          energy = 0;
    if (time > MAX_TIME)     time = MAX_TIME;
    if (time < 0)            time = 0;
}

void applyScoreDelta(int& score, int deltaScore) {
    score += deltaScore;
}

// ================= CONNECTIVITY =================

bool isDirectlyConnected(int fromF, int fromR, int toF, int toR,
                         const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                         char& outConnType) {
    outConnType = CONN_NONE;

    // 1. Bounds check, so no array is ever indexed out of range.
    if (fromF < 1 || fromF > NUM_FLOORS || toF < 1 || toF > NUM_FLOORS) return false;
    if (fromR < 1 || fromR > ROOMS_PER_FLOOR || toR < 1 || toR > ROOMS_PER_FLOOR) return false;

    // 2. Same floor, neighbouring rooms: horizontalConn[f][i] is the link
    //    between room i and room i+1 (0-based), i.e. the smaller 1-based room - 1.
    if (fromF == toF && (fromR - toR == 1 || toR - fromR == 1)) {
        int left = (fromR < toR) ? fromR : toR;
        char c = horizontalConn[fromF - 1][left - 1];
        if (c == CONN_NONE || c == CONN_SOLID_WALL) return false;
        outConnType = c;                       // D, W or L
        return true;
    }

    // 3. Different floors: stairs and lifts.
    if (fromF != toF) {
        int lo = (fromF < toF) ? fromF : toF;
        int hi = (fromF < toF) ? toF : fromF;

        // Stairs: room 1 only. S1: F1-F2, S2: F2-F3, S3: F4-F5.
        // Spec gives stairs the ordinary movement cost, so they report as a door.
        if (fromR == 1 && toR == 1 &&
            ((lo == 1 && hi == 2) || (lo == 2 && hi == 3) || (lo == 4 && hi == 5))) {
            outConnType = CONN_DOOR;
            return true;
        }

        // Lift 1: Floors 1 and 2, rooms 2 or 3 at each end.
        if (lo == 1 && hi == 2 && (fromR == 2 || fromR == 3) && (toR == 2 || toR == 3)) {
            outConnType = CONN_LIFT;
            return true;
        }

        // Lift 2: room 4 on floors 2, 4 and 5 only (Floor 3 is bypassed).
        bool fromOnLift2 = (fromF == 2 || fromF == 4 || fromF == 5);
        bool toOnLift2   = (toF   == 2 || toF   == 4 || toF   == 5);
        if (fromR == 4 && toR == 4 && fromOnLift2 && toOnLift2) {
            outConnType = CONN_LIFT;
            return true;
        }
    }

    return false;
}

// ================= ACCESS RULES =================

bool canTraverse(int targetFloor, int targetRoom, char connType,
                 const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,
                 int emergencyTripsLeft, bool lift2Operational) {
    (void)targetFloor;   // no rule depends on the target floor

    switch (connType) {
        case CONN_DOOR:          // ordinary door or stairs
        case CONN_WINDOW:
            return true;

        case CONN_LOCKED_DOOR:   // needs the Master Key
            return hasItem(inventory, inventoryCount, ENT_MASTER_KEY);

        case CONN_LIFT:
            if (targetRoom == 4) {   // room 4 is Lift 2
                // Normal use (Fuse installed + Lift Pass) or an emergency trip.
                return lift2Operational || emergencyTripsLeft > 0;
            }
            return true;             // rooms 2/3 are Lift 1: unconditional

        default:                 // CONN_NONE, solid wall, anything unknown
            return false;
    }
} 