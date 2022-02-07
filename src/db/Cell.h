#pragma once
#include "Database.h"

namespace db {

class Cell {
private:
    string _name;

public:
    int _size = 0;
    int id = -1;
    int group = -1;
    int gain = -1;
    bool free = true;

    
    vector<Net*> net_list;

    Cell(const string& name = "", const int size = 0) : _name(name), _size(size) {;}
    ~Cell();

    const std::string& name() const { return _name; }
    int size() const {return _size;}

    friend ostream& operator<<(ostream& os, const Cell& c) {
        return os << c._name << "\t(" << c._size << ')';
    }
};

}