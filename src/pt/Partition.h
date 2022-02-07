#pragma once
#include "db/Database.h"
#include "BucketList.h"
#include "global.h"

namespace pt {

class Partition : public VertexList{
public:
    db::Database* database;
    int GAIN = 0;
    vector<int> GAINS;
    
    int num_free = 0;
    int cut_size;


    //gains
    vector<bool> freecells;
    vector<int> gain_list;


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
    /* partition */
    void update_ratio(db::Cell* cell_mov);
    void update_gain(db::Cell* cell_mov);
    bool check_balance(db::Cell* cell_mov);
    bool pass();
    void iter();

    /* buckets */
    void init_bucket();
    int pop_max();
    void update_vertex(int& cell, int& gain_index);
    void remove_vertex(int& cell, int& gain_index);


    /* utils */
    void init();
    bool load();
    bool write();
    void cutsize();
};

}

