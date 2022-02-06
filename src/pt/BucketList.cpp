#include "BucketList.h"
#include "pt/Partition.h"

using namespace pt;

void BucketList::initList(vector<BucketPtr>& buckets){
	for (int i = 0; i < (2 * maxDegree + 1); i++) {
		buckets.push_back(make_shared<Bucket>());
	}
} //END MODULE

//-------------------------------------------------------------------------------

void BucketList::SortList(int vertexIndex, int gain, vector<BucketPtr>& buckets, const vector<BucketPtr>& vertices, bool side){
    int* maxGainIndex;
	if (side) {
		maxGainIndex = &maxLeftGainIndex;
	}
	else {
		maxGainIndex = &maxRightGainIndex;
	}
	int index;
	if (gain != 0) {
		index = maxDegree + gain;
		*maxGainIndex = max(index, *maxGainIndex);
	}
	else {
		index = maxDegree;
		*maxGainIndex = max(index, *maxGainIndex);
	}
	BucketPtr v = buckets.at(index);
	BucketPtr temp = v->next;
	v->next = vertices.at(vertexIndex);
	v->next->gain = gain;
	v->next->next = temp;
} //END MODULE

//-------------------------------------------------------------------------------

void Partition::init_bucket(){
    BucketListA.clear();
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
        BucketListA[gain].push_back(cell->id);
        gain_list_bucket[i] = gain;
        // cout << gain << " | " << BucketListA.end()->first << " | " << BucketListA.begin()->first << endl;
    }
} //END MODULE

//-------------------------------------------------------------------------------
