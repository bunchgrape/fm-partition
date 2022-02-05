#include "Partition.h"



namespace pt {

Partition::~Partition() {
    freecells.clear();
}

bool Partition::load(){
    gain_list.resize(2);
    gain_list[0].resize(database->nCells);
    gain_list[1].resize(database->nCells);
    num_free = database->nCells;

    areas.resize(3);
    areas[0] = areas[1] = 0;

    // cell
    int balance = 0;
    for (db::Cell* cell : database->cells) {
        // free cells
        freecells.push_back(1);

        // random partition
        // int group = balance <= 0 ? 0 : 1;
        int group = 2 * (float)rand() / RAND_MAX;

        balance += pow(-1, group) * cell->size();
        cell->group = group;
        areas[group] += cell->size();
        // this->partition_list[group].push_back(cell->id);
        // this->partition_sizes[group].push_back(cell->size());
    }
    
    areas[2] = areas[0]+ areas[1];
    ratio = areas[0] / areas[2];
    cout << areas[0] << "|" << areas[1] << endl;;

    return true;
}


bool Partition::write(){
    vector<string> A;
    vector<string> B;

    for(db::Cell* cell : database->cells){
        if(cell->group == 0)
            A.push_back(cell->name());
        else
            B.push_back(cell->name());
    }

    ofstream outfile;
    outfile.open("/data/ssd/bqfu/hw/HW1/p2.out", ios::out);

    outfile << "\n\n" << "A " << A.size() << "\n";
    for(string name : A){
        outfile << name << endl;
    }
    outfile << "\n\n" << "B " << B.size() << "\n";
    for(string name : B){
        outfile << name << endl;
    }

    outfile.close();
    

    return true;

}



}

