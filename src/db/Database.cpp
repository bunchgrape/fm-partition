#include "Database.h"


using namespace db;

/***** Database *****/
Database::Database() {

    _buffer = new char[_bufferCapacity];
}

Database::~Database() {
    delete[] _buffer;
    _buffer = nullptr;
    // for regions.push_back(new Region("default"));

    log() << "destruct rawdb" << std::endl;
}


Cell* Database::addCell(const string& name, const int size) {
    Cell* cell = getCell(name);
    if (cell) {
        printlog(LOG_WARN, "cell re-defined: %s", name.c_str());
        return cell;
    }
    cell = new Cell(name);
    cell->_size = size;
    name_cells.emplace(name, cell);
    cells.push_back(cell);
    return cell;
}


Net* Database::addNet(const string& name) {
    Net* net = getNet(name);
    if (net) {
        printlog(LOG_WARN, "Net re-defined: %s", name.c_str());
        return net;
    }
    net = new Net(name);
    name_nets[name] = net;
    nets.push_back(net);
    return net;
}

Cell* Database::getCell(const string& name) {
    unordered_map<string, Cell*>::iterator mi = name_cells.find(name);
    if (mi == name_cells.end()) {
        return nullptr;
    }
    return mi->second;
}

Net* Database::getNet(const string& name) {
    unordered_map<string, Net*>::iterator mi = name_nets.find(name);
    if (mi == name_nets.end()) {
        return nullptr;
    }
    return mi->second;
}

