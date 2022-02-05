#pragma once
#include "db/Database.h"
#include "global.h"

// db::Database database;

// extern db::Database database;

namespace pt {

class Partition{
public:
    db::Database* database;
    int GAIN = 0;
    vector<int> GAINS;
    
    int num_free = 0;
    int cuts;

    // vector<vector<int>> netCells;

    //gains
    vector<bool> freecells;
    vector<vector<int>> gain_list;


    // balance
    double ratio;
    double max_dif = 0.1;
    vector<double> areas;

    // tracker
    vector<int> tracker;
    vector<int> gain_history;

public:
    Partition(db::Database* database_) : database(database_) {}
    ~Partition();

public:
    bool load();
    bool write();

    /* defined in parter */
    void init_gain();
    void update_gain(db::Cell* cell);
    void update_ratio(db::Cell* cell);
    bool check_balance(db::Cell* cell_mov);
    bool pass();
    void iter();

    void cutsize();
};

}

