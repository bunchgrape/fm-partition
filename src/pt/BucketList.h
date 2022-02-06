#pragma once
#include "db/Database.h"
#include "global.h"

class Bucket;
typedef shared_ptr<Bucket> BucketPtr;

class Bucket{
public:
    string _name;

public:
    Bucket() {;}
    Bucket(const string& name) : _name(name) {;}

    int gain;

    int distance = INT_MAX;
    BucketPtr   pre;
    BucketPtr   next;
    vector<BucketPtr> adjacentList;
};


class BucketList{
public:
    int maxDegree;
    int maxLeftGainIndex = 0;
	int maxRightGainIndex = 0;


    void initList(vector<BucketPtr>& buckets);
    void SortList(int vertexIndex, int gain, vector<BucketPtr>& buckets, const vector<BucketPtr>& vertices, bool side);
};
