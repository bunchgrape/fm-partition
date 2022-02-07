#include "Partition.h"
#include "global.h"


namespace pt {

Partition::~Partition() {
    freecells.clear();
}

bool Partition::load(){
    // initialization
    gain_list.resize(database->nCells);
    num_free = database->nCells;
    areas.resize(3);
    areas[0] = areas[1] = 0;

    // cells
    int balance = 0;
    for (db::Cell* cell : database->cells) {
        // free cells
        freecells.push_back(1);

        // random partition
        int group = 2 * (float)rand() / RAND_MAX;
        balance += pow(-1, group) * cell->size();
        cell->group = group;
        areas[group] += cell->size();

        // maxDegree
        int degree = cell->net_list.size();
        maxDegree = max(maxDegree, degree);
    }
    
    areas[2] = areas[0] + areas[1];
    ratio = areas[0] / areas[2];
    cout << areas[0] << "|" << areas[1] << endl;

    cout << database->nCells << " cells" << endl;
    cout << database->nNets << " nets" << endl;
    cout << "Max Degree " << maxDegree << endl;
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

    outfile.open("./output"+database->designName+".out", ios::out);

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

void Partition::init(){
    initList();
    vertexList.resize(database->nCells);

    log() << "Creating Graph\n";
    int Gm = 0;
    for (db::Cell* cell : database->cells) {
        int FS = 0;
        int TE = 0;
        int gain = 0;
        for (db::Net* net : cell->net_list) {
            int fss = 0;   // cell itself
            int tes = 0;
            for (db::Cell* cell_tmp : net->cell_list) {
                int grouper = (cell_tmp->group == cell->group);
                fss += grouper;     //same group
                tes += !grouper;    //dif group
            }
            if (tes == 0)
                TE++;
            if (fss==1)
                FS++;
            // cout << fss << "|" << tes << endl;
        }
        // set gain
        gain += FS - TE;
        cell->gain = gain;
        gain_list[cell->id] = gain;

        // add to bucket list
        addVertex(cell);
    }
} //END MODULE

//-------------------------------------------------------------------------------

void Partition::init_bucket(){
    log() << "Initializing BucketList\n";
    // clear list
    clear();

    int Gm = 0;
    for (db::Cell* cell : database->cells) {
        vertexList[cell->id]->next = NULL;
        int FS = 0;
        int TE = 0;
        int gain = 0;
        for (db::Net* net : cell->net_list) {
            int fss = 0;   // cell itself
            int tes = 0;
            for (db::Cell* cell_tmp : net->cell_list) {
                int grouper = (cell_tmp->group == cell->group);
                fss += grouper;     //same group
                tes += !grouper;    //dif group
            }
            if (tes == 0)
                TE++;
            if (fss==1)
                FS++;
            // cout << fss << "|" << tes << endl;
        }
        // set gain
        gain += FS - TE;
        cell->gain = gain;
        gain_list[cell->id] = gain;

        // add to bucket list
        reVertex(cell);
    }
} //END MODULE

//-------------------------------------------------------------------------------

void Partition::update_ratio(db::Cell* cell_mov){
    areas[cell_mov->group] -= cell_mov->size();
    areas[!(cell_mov->group)] += cell_mov->size();
    ratio = areas[0] / areas[2];
    // cout << areas[0] << "|" << areas[1] << endl;

} //END MODULE

//-------------------------------------------------------------------------------

void Partition::cutsize(){
    int cut = 0;
    for (db::Net* net : database->nets){
        int partition[2] = {0,0};
        for (db::Cell* cell : net->cell_list){
            partition[cell->group]++;
        }
        cut += ((partition[0]*partition[1]) > 0);
    }
    log() << "========cutsize: " << cut << " ============ " << endl;
    cut_size = cut;
} //END MODULE

//-------------------------------------------------------------------------------

bool Partition::check_balance(db::Cell* cell_mov){
    if ((areas[cell_mov->group] -  cell_mov->size()) < (areas[2] * (0.5 - max_dif/2)) ){
        return false;
    }
    else
        return true;
} //END MODULE

//-------------------------------------------------------------------------------

int Partition::pop_max(){
    int idx_0 = -1;
    int idx_1 = -1;

    if(BucketList[0][maxGainIndex[0]]->next != NULL)
        idx_0 = BucketList[0][maxGainIndex[0]]->next->id;
    if(BucketList[1][maxGainIndex[1]]->next != NULL)
        idx_1 = BucketList[1][maxGainIndex[1]]->next->id;

    if(idx_0 == -1){
        db::Cell* cell_1 = database->cells[idx_1];
        if(check_balance(cell_1))
            return idx_1;
        else
            return -1;
    }
    if(idx_1 == -1){
        db::Cell* cell_0 = database->cells[idx_0];
        if(check_balance(cell_0))
            return idx_0;
        else
            return -1;
    }

    db::Cell* cell_1 = database->cells[idx_1];
    db::Cell* cell_0 = database->cells[idx_0];

    bool check0 = check_balance(cell_0);
    bool check1 = check_balance(cell_1);

    if(!check0 && !check1)
        return -1;
    
    if(check0){
        if(check1)
            return (cell_0->gain > cell_1->gain ? idx_0 : idx_1);
        else
            return idx_0;
    }
    else
        return idx_1;

} //END MODULE


//-------------------------------------------------------------------------------
void Partition::remove_vertex(int& cell, int& gain_index){
    int group = database->cells[cell]->group;

    VertexPtr vertex = vertexList[cell];
    BucketList[group][gain_index]->next = vertex->next;

    if(vertex->next != NULL)
        vertex->next->pre = BucketList[group][gain_index];

    // set null
    vertex->next = NULL;
    vertex->pre = NULL;

    // refresh max index
    int maxIndexOld = maxGainIndex[group];
    while((BucketList[group][maxIndexOld]->next == NULL) &&
                maxIndexOld != -maxDegree){
        maxIndexOld--;   
    }
    maxGainIndex[group] = maxIndexOld;
}

//-------------------------------------------------------------------------------

void Partition::update_vertex(int& cell, int& gain_index){
    int new_gain = gain_list[cell];
    int group = database->cells[cell]->group;

    // new max index
    int maxIndex = maxGainIndex[group];
    maxGainIndex[group] = max(maxIndex, new_gain);


    VertexPtr vertex = vertexList[cell];
    // remove
    VertexPtr previous = vertex->pre;
    previous->next = vertex->next;
    if(vertex->next != NULL)
        vertex->next->pre = previous;


    // insert
    VertexPtr tmp = BucketList[group][new_gain]->next;
    BucketList[group][new_gain]->next = vertex;
    vertex->next = tmp;
    
    // previous
    vertex->pre = BucketList[group][new_gain];
    if(tmp!=NULL)
        tmp->pre = vertex;

} //END MODULE

//-------------------------------------------------------------------------------

void Partition::update_gain(db::Cell* cell_mov){
    // remove moved cell
    remove_vertex(cell_mov->id, cell_mov->gain);
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
                    vertexList[cell->id]->gain--;
                    gain_list[cell->id]--;
                    update_vertex(cell->id, gain_index);
                }
            }
        }
        else if (fss == 1){
            if (freecells[critical_cell_1->id]){
                gain_index = critical_cell_1->gain;
                critical_cell_1->gain++;
                vertexList[critical_cell_1->id]->gain++;
                gain_list[critical_cell_1->id]++;
                update_vertex(critical_cell_1->id, gain_index);
            }
        }
        if (tes == 0){
            for(db::Cell* cell : net->cell_list){
                if (freecells[cell->id]){
                    gain_index = cell->gain;
                    cell->gain++;
                    vertexList[cell->id]->gain++;
                    gain_list[cell->id]++;
                    update_vertex(cell->id, gain_index);
                }
            }
        }
        else if (tes == 1){
            if (freecells[critical_cell_2->id]){
                gain_index = critical_cell_2->gain;
                critical_cell_2->gain--;
                vertexList[critical_cell_2->id]->gain--;
                gain_list[critical_cell_2->id]--;
                update_vertex(critical_cell_2->id, gain_index);
            }
        }
    }
} //END MODULE

//-------------------------------------------------------------------------------

bool Partition::pass(){
    int gain_local = 0;
    num_free = database->nCells;
    sizeBucket();
    // printBucket();
    while (num_free){
        // pop the max gain cell
        int cell_mov_idx = pop_max();
        if(cell_mov_idx == -1)
            break;
        // point to cell moved in this round
        db::Cell* cell_mov = database->cells[cell_mov_idx];

        // GAIN summation
        gain_local += gain_list[cell_mov_idx];
    
        // lock moved cell
        num_free--;
        freecells[cell_mov_idx] = 0;

        // update gains
        update_gain(cell_mov);

        //update partition
        update_ratio(cell_mov);

        // make the move
        cell_mov->group = !cell_mov->group;

        // tracker
        tracker.push_back(cell_mov_idx);
        gain_history.push_back(gain_local);
    }

    if(gain_history.empty())
        return false;

    // update free via track
    int maxGAINIndex = max_element(gain_history.begin(),
                                gain_history.end()) - gain_history.begin();

    // update gain
    gain_local = gain_history[maxGAINIndex];
    GAIN += gain_local; // FIXME:

    // recall the moves
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

void Partition::iter(){
    // init cut_size
    init();
    cutsize();
    
    GAIN = INT_MAX;
    int GAIN_dif = INT_MAX;
    GAINS.push_back(0);
    int iteration = 0;
    while(GAIN_dif > database->nCells / 1e5){
        if (iteration == 0)
            GAIN = 0;
        else
            init_bucket();

        // move a step
        pass();

        // refresh gain
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

