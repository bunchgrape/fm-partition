#pragma once
#include "Database.h"

namespace db {

class Net {
private:
    string _name;

public:
    int id = -1;
    bool cut = false;
    vector<Cell*> cell_list;


    Net(const std::string& name = "") : _name(name) { }
    ~Net();

    const std::string& name() const { return _name; }

    friend ostream& operator<<(ostream& os, const Net& c) {
        return os << c._name << "\n";
    }
};

}
