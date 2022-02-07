#include "BucketList.h"
#include "pt/Partition.h"

using namespace pt;

void VertexList::initList(){
    BucketList.resize(2);
    maxGainIndex.resize(2);
    // single list
    for (int i = 0; i<BucketList.size();i++){
        for (int j = -maxDegree; j <= maxDegree; j++) {
            BucketList[i][j] = make_shared<Vertex>("head");
        }
    }
    maxGainIndex = {0,0};
} //END MODULE

//-------------------------------------------------------------------------------

void VertexList::clear(){
    for (int i = 0; i<BucketList.size();i++){
        for (int j = -maxDegree; j <= maxDegree; j++) {
            BucketList[i][j]->next = NULL;
        }
    }
    maxGainIndex = {0,0};
} //END MODULE

//-------------------------------------------------------------------------------

void VertexList::addVertex(db::Cell* cell){
    int gain = cell->gain;
    int group = cell->group;
    // gain index
	maxGainIndex[group] = max(gain, maxGainIndex[group]);

    // head vertex
    VertexPtr header = BucketList[group][gain];
	VertexPtr tmp = header->next;

    // create vertex
    VertexPtr vertex = make_shared<Vertex>(cell->name());;
    vertex->id = cell->id;
	vertex->gain = gain;

    // insert after head
	header->next = vertex;
	vertex->next = tmp;

    // pre
    vertex->pre = header;
    if(tmp!=NULL)
        tmp->pre = vertex;

    // add to graph
    vertexList[cell->id] = vertex;
} //END MODULE

//-------------------------------------------------------------------------------

void VertexList::reVertex(db::Cell* cell){
    int gain = cell->gain;
    int group = cell->group;
    auto map = BucketList[group];
    // gain index
	maxGainIndex[group] = max(gain, maxGainIndex[group]);

    // head vertex
    VertexPtr header = BucketList[group][gain];
	VertexPtr tmp = header->next;

    // maintain vertex
    VertexPtr vertex = vertexList[cell->id];
    vertex->id = cell->id;
	vertex->gain = gain;

    // insert after head
    // insert after head
	header->next = vertex;
	vertex->next = tmp;

    // pre
    vertex->pre = header;
    if(tmp!=NULL)
        tmp->pre = vertex;
} //END MODULE

//-------------------------------------------------------------------------------

void VertexList::printBucket(){
    for (int i = 0; i<BucketList.size();i++){
        for (int j = -maxDegree; j <= maxDegree; j++) {
            log() << "----------DEGREE " << j << "---------" << endl;
            VertexPtr ptr = BucketList[i][j];
            while(ptr->next != NULL){
                log() << ptr->next->id << " | " << ptr->next->gain << endl;
                ptr = ptr->next;
            }
        }
    }
} //END MODULE

//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------

void VertexList::sizeBucket(){
    int num=0;
    for (int i = 0; i<BucketList.size();i++){
        for (int j = -maxDegree; j <= maxDegree; j++) {
            VertexPtr ptr = BucketList[i][j];
            while(ptr->next != NULL){
                num++;
                ptr = ptr->next;
            }
        }
    }
    log() << num << " cell left\n";
} //END MODULE

//-------------------------------------------------------------------------------

void VertexList::refreshmax(){
    for (int i = 0; i<BucketList.size();i++){
        for (int j = maxDegree; j >= maxDegree; j--) {
            if(BucketList[i][j]->next !=NULL){
                maxGainIndex[i] = j;
            }
        }
    }

} //END MODULE
