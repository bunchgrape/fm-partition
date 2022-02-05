#include "global.h"
#include "pt/Partition.h"

using namespace pt;

//-------------------------------------------------------------------------------

void Partition::init_gain(){
    // update gain by cut number
    int Gm = 0;
    for (int i = 0; i < database->nCells; i++) {
        db::Cell* cell = database->cells[i];
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
        gain += FS - TE;
        cell->gain = gain;
        Gm += gain;
        gain_list[cell->group][i] = gain;
        gain_list[!cell->group][i] = INT_MIN;
    }
} //END MODULE

//-------------------------------------------------------------------------------

void Partition::update_gain(db::Cell* cell_mov){
    for (int j = 0; j < cell_mov->net_list.size(); j++) {
        int fss = -1;   // cell itself
        int tes = 0;
        db::Cell* critical_cell_1;
        db::Cell* critical_cell_2;
        for (db::Cell* cell : cell_mov->net_list[j]->cell_list) {
            int grouper = (cell->group == cell_mov->group);
            fss += grouper;     //same group
            tes += !grouper;    //dif group
            if (grouper && (cell->id != cell_mov->id))
                critical_cell_1 = cell;
            if (!grouper)
                critical_cell_2 = cell;
        }
        if (fss == 0){
            for(db::Cell* cell : cell_mov->net_list[j]->cell_list){
                if (freecells[cell->id]){
                    cell->gain--;
                    gain_list[cell->group][cell->id]--;
                }
            }
        }
        if (fss == 1){
            if (freecells[critical_cell_1->id]){
                critical_cell_1->gain++;
                gain_list[critical_cell_1->group][critical_cell_1->id]++;
            }
        }
        if (tes == 0){
            for(db::Cell* cell : cell_mov->net_list[j]->cell_list){
                if (freecells[cell->id]){
                    cell->gain++;
                    gain_list[cell->group][cell->id]++;
                }
            }
        }
        if (tes == 1){
            if (freecells[critical_cell_2->id]){
                critical_cell_2->gain--;
                gain_list[critical_cell_2->group][critical_cell_2->id]--;
            }
        }
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

bool Partition::check_balance(db::Cell* cell_mov){
    if ((areas[cell_mov->group] -  cell_mov->size()) < (areas[2] * (0.5 - max_dif/2)) ){
        return false;
    }
    else
        return true;
} //END MODULE

//-------------------------------------------------------------------------------

bool Partition::pass(){
    int gain_local = 0;
    num_free = database->nCells;
    while (num_free){
        int maxElementIndex;
        db::Cell* cell_mov;

        // find maximum gain
        int maxElementIndex_0 = max_element(gain_list[0].begin(),
                                gain_list[0].end()) - gain_list[0].begin();
        int maxElementIndex_1 = max_element(gain_list[1].begin(),
                                    gain_list[1].end()) - gain_list[1].begin();


        db::Cell* cell_mov_0 = database->cells[maxElementIndex_0];
        db::Cell* cell_mov_1 = database->cells[maxElementIndex_1];
    
        bool check0 = check_balance(cell_mov_0);
        bool check1 = check_balance(cell_mov_1);

        if (!check0 && !check1){
            // cout << "---history sizes-----" << gain_history.size() << "-------------------\n";
            // cout << areas[0] << " | " << areas[1] << " | \n" ;
            // for (int i = 0; i < gain_list[0].size(); i++)
            //     cout << gain_list[0][i] << " | " << gain_list[1][i] << " | \n" ;
            if(gain_history.size() == 0)
                return false;
            break;
        }
        else if( ((gain_list[0][maxElementIndex_0] >= gain_list[1][maxElementIndex_1]) && check0) ||
                 ((gain_list[0][maxElementIndex_0] < gain_list[1][maxElementIndex_1]) && !check1) )
        {   
            gain_list[0][maxElementIndex_0] = INT_MIN;
            gain_list[1][maxElementIndex_0] = INT_MIN;
            cell_mov = cell_mov_0;
            maxElementIndex = maxElementIndex_0;
        }
        else{
            gain_list[1][maxElementIndex_1] = INT_MIN;
            gain_list[0][maxElementIndex_1] = INT_MIN;
            cell_mov = cell_mov_1;
            maxElementIndex = maxElementIndex_1;
        }

        // GAIN summation
        gain_local += cell_mov->gain;

        // update gains
        freecells[maxElementIndex] = 0;
        update_gain(cell_mov); 
        num_free--;
        
        //update partition
        update_ratio(cell_mov);
        cell_mov->group = !cell_mov->group;
        
        // tracker
        tracker.push_back(cell_mov->id);
        gain_history.push_back(gain_local);
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

    cout << maxGAINIndex+1 << " cells moved" << "| 1st: " << database->cells[tracker[0]]->name() <<
                        "-----ratio: " << areas[0] << " | " << areas[1] << " | " << areas[2] << " | " << endl;

    return true;

} //END MODULE

//-------------------------------------------------------------------------------

void Partition::iter(){
    GAIN = INT_MAX;
    int GAIN_dif = INT_MAX;
    GAINS.push_back(0);
    int iteration = 0;
    while(GAIN_dif > 0){
        if (iteration == 0)
            GAIN = 0;


        init_gain();
        // move a step
        pass();
        GAIN_dif = GAIN - GAINS.back();
        GAINS.push_back(GAIN);

        // renew tracks
        freecells.assign(freecells.size(), 1);
        tracker.clear();
        gain_history.clear();

        cout << "-----------------------------------iteration: " << iteration << "| Gm: " << GAIN << "------------------------" << endl;
        cutsize();
        iteration++;
    }
} //END MODULE

//-------------------------------------------------------------------------------

void Partition::cutsize(){
    int cut = 0;
    for (db::Net* net : database->nets){
        int partition[2] = {0,0};
        for (db::Cell* cell : net->cell_list){
            partition[cell->group]++;
        }
        cut += (partition[0]*partition[1] > 0);
    }
    cout << "========cutsize: " << cut << " ============ " << endl;
} //END MODULE

//-------------------------------------------------------------------------------
