#include "Partition.h"
#include "global.h"


namespace pt {

Partition::~Partition() {
    freecells.clear();
}

bool Partition::load(){
    gain_list.resize(2);
    gain_list[0].resize(database->nCells);
    gain_list[1].resize(database->nCells);
    gain_list_bucket.resize(database->nCells);

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
    cout << areas[0] << "|" << areas[1] << endl;

    cout << database->nCells << " cells" << endl;
    cout << database->nNets << " nets" << endl;

    return true;
} //END MODULE

//-------------------------------------------------------------------------------

bool Partition::write(){
    int cut = 0;
    for (db::Net* net : database->nets){
        int partition[2] = {0,0};
        for (db::Cell* cell : net->cell_list){
            partition[cell->group]++;
        }
        cut += (partition[0]*partition[1] > 0);
    }
    
    vector<string> A;
    vector<string> B;

    for(db::Cell* cell : database->cells){
        if(cell->group == 0)
            A.push_back(cell->name());
        else
            B.push_back(cell->name());
    }
    ofstream outfile;

    outfile.open("."+database->designName+".out", ios::out);

    outfile << "cut_size " << cut << "\n";

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
} //END MODULE

//-------------------------------------------------------------------------------

void Partition::update_buckets(int& cell, int& gain_index){
    int new_gain;
	
	new_gain = database->cells[cell]->gain;
    
	BucketListA[gain_index].erase(remove(BucketListA[gain_index].begin(),BucketListA[gain_index].end(),cell),BucketListA[gain_index].end());
	
	BucketListA[new_gain].push_back(cell);

} //END MODULE

//-------------------------------------------------------------------------------

void Partition::update_gain_bucket(db::Cell* cell_mov){
    for (db::Net* net : cell_mov->net_list) {
        int fss = -1;   // cell itself
        int tes = 0;
        int gain_index;
        db::Cell* critical_cell_1;
        db::Cell* critical_cell_2;
        for (db::Cell* cell : net->cell_list) {
            int grouper = (cell->group == cell_mov->group);
            fss += grouper;     //same group
            tes += !grouper;    //dif group
            if (grouper && (cell->id != cell_mov->id))
                critical_cell_1 = cell;
            if (!grouper)
                critical_cell_2 = cell;
        }
        if (fss == 0){
            for(db::Cell* cell : net->cell_list){
                if (freecells[cell->id]){
                    gain_index = cell->gain;
                    cell->gain--;
                    gain_list_bucket[cell->id]--;
                    update_buckets(cell->id, gain_index);
                }
            }
        }
        else if (fss == 1){
            if (freecells[critical_cell_1->id]){
                gain_index = critical_cell_1->gain;
                critical_cell_1->gain++;
                gain_list_bucket[critical_cell_1->id]++;
                update_buckets(critical_cell_1->id, gain_index);
            }
        }
        if (tes == 0){
            for(db::Cell* cell : net->cell_list){
                if (freecells[cell->id]){
                    gain_index = cell->gain;
                    cell->gain++;
                    gain_list_bucket[cell->id]++;
                    update_buckets(cell->id, gain_index);
                }
            }
        }
        else if (tes == 1){
            if (freecells[critical_cell_2->id]){
                gain_index = critical_cell_2->gain;
                critical_cell_2->gain--;
                gain_list_bucket[critical_cell_2->id]--;
                update_buckets(critical_cell_2->id, gain_index);
            }
        }
    }
} //END MODULE

//-------------------------------------------------------------------------------

bool Partition::pass_bucket(){
    indexA = BucketListA.end();
    indexA--;
    int cell_mov_idx;
    int gain_local = 0;
    int cell_mov_idx_pre;
    int sub_index = 0;
    num_free = database->nCells;
    LOOP:while(true){
        if(BucketListA.empty())
		    break;
        if(indexA->second.empty())
            if(indexA != BucketListA.begin()){
                indexA--;
                sub_index = 0;
                goto LOOP;
            }
            else
                break;
        cell_mov_idx = indexA->second.back();

        while(!check_balance(database->cells[cell_mov_idx]))
        {
            if(cell_mov_idx == indexA->second.front())
            {
                if(indexA == BucketListA.begin())
                {
                    break;
                }
                indexA--;
                sub_index = 0;
                goto LOOP;
            }
            sub_index++;
            cell_mov_idx = *(&indexA->second.back() - sub_index);
        }

        // point to cell moved in this round
        db::Cell* cell_mov = database->cells[cell_mov_idx];

        // GAIN summation
        gain_local += gain_list_bucket[cell_mov_idx];

        // lock moved cell
        num_free--;
        freecells[cell_mov_idx] = 0;
        indexA->second.erase(indexA->second.end() - (sub_index + 1));

        // update gains
        update_gain_bucket(cell_mov);

        // lock by remove item
        if(indexA->second.empty()){
            BucketListA.erase(indexA);
            // cout << indexA->first <<"------------------empty--------------------\n";
        }

        //update partition
        update_ratio(cell_mov);
        cell_mov->group = !cell_mov->group;

        // tracker
        tracker.push_back(cell_mov_idx);
        gain_history.push_back(gain_local);

        // reset pointer
        if(!BucketListA.empty())
        {
            indexA = BucketListA.end();
            indexA--;
            sub_index = 0;
        }
    }


    // update free via track
    int maxGAINIndex = max_element(gain_history.begin(),
                                gain_history.end()) - gain_history.begin();

    // update gain
    gain_local = gain_history[maxGAINIndex];
    GAIN += gain_local; // FIXME:

    db::Cell* cell;
    for (int i = maxGAINIndex + 1; i < gain_history.size(); i++){
        cell = database->cells[tracker[i]];
        update_ratio(cell);
        cell->group = !cell->group;
    }

    log() << maxGAINIndex+1 << " cells moved"
                         << "-----ratio: " << areas[0] << " | " << areas[1] << " | " << areas[2] << " | " << endl;
    return true;

} //END MODULE

//-------------------------------------------------------------------------------

void Partition::iter_bucket(){

    cutsize();
    
    GAIN = INT_MAX;
    int GAIN_dif = INT_MAX;
    GAINS.push_back(0);
    int iteration = 0;
    while(GAIN_dif > database->nCells / 1e5){
        if (iteration == 0)
            GAIN = 0;

        init_bucket();
        // move a step
        pass_bucket();
        GAIN_dif = GAIN - GAINS.back();
        GAINS.push_back(GAIN);

        // renew tracks
        freecells.assign(freecells.size(), 1);
        tracker.clear();
        gain_history.clear();

        log() << "-----------------------iteration: " << iteration << "| Gm: " << GAIN << "------------------------" << endl;
        cutsize();

        write();
        iteration++;
    }

} //END MODULE

//-------------------------------------------------------------------------------


}

