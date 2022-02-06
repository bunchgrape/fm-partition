#pragma once
#include "db/Database.h"
#include "BucketList.h"
#include "global.h"

// db::Database database;

// extern db::Database database;

namespace pt {

class Partition : public BucketList{
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


    // bucket
	vector<BucketPtr> List_A;
	vector<BucketPtr> List_B;

    map<int, vector<int>> BucketListA;
    map<int, vector<int>>::iterator indexA = BucketListA.end(); // index for gainA
    vector<int> gain_list_bucket;

public:
    Partition(db::Database* database_) : database(database_) {}
    ~Partition();

public:
    /* defined in parter */
    void init_gain();
    void update_gain(db::Cell* cell);
    void update_ratio(db::Cell* cell);
    bool check_balance(db::Cell* cell_mov);
    bool pass();
    void iter();

    /* buckets */
    void initializeBuckets();
    void init_bucket();
    void update_buckets(int& cell, int& gain_index);
    void update_gain_bucket(db::Cell* cell);
    bool pass_bucket();
    void iter_bucket();

    /* utils */
    bool load();
    bool write();
    void cutsize();
};

}

