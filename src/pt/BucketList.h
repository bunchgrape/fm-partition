#pragma once
#include "db/Database.h"
#include "global.h"

class Vertex;
typedef shared_ptr<Vertex> VertexPtr;

class Vertex{
public:
    string _name;
    int id;

public:
    Vertex() {_name = "null";}
    Vertex(const string& name) : _name(name) {;}

    int gain;
    int num_list = 0;

    VertexPtr   pre = NULL;
    VertexPtr   next = NULL;
};


class VertexList{
public:
    int maxDegree = 0;
    vector<VertexPtr> vertexList;

	vector<map<int, VertexPtr>> BucketList;
    vector<int> maxGainIndex;

    void clear();
    void printBucket();
    void sizeBucket();
    void refreshmax();

    void reVertex(db::Cell* cell);
    void addVertex(db::Cell* cell);
    void initList();
};
