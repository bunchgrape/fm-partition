#pragma once
#include "global.h"

namespace db {
class Cell;
class Net;
}  // namespace db

#include "Cell.h"
#include "Net.h"


namespace db {

class Database {
public:
    string designName;

    int nCells;
    int nNets;

    unordered_map<string, Cell*> name_cells;
    unordered_map<string, Net*> name_nets;

    vector<Cell*> cells;
    vector<Net*> nets;


// buffer
private:
    static const size_t _bufferCapacity = 128 * 1024;
    size_t _bufferSize = 0;
    char* _buffer = nullptr;

    Cell* addCell(const string& name, const int size);
    Net* addNet(const string& name);

public:
    Database();
    ~Database();
    void clear();

    Cell* getCell(const string& name);
    Net* getNet(const string& name);

    inline unsigned getNumCells() const { return cells.size(); }
    inline unsigned getNumNets() const { return nets.size(); }

public:
    /* defined in io/ */
    bool read(const std::string& cellFile, const std::string& netFile);
    bool readCells(const std::string& file);
    bool readNets(const std::string& file);
    void write(ofstream& ofs);

};

} // namespace db