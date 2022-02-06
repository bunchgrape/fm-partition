#include "global.h"
#include "db/Database.h"

using namespace db;


class BookshelfData {
public:
    int nCells;
    int nNets;

    std::string format;
    unordered_map<string, int> cellMap;

    vector<string> cellName;
    vector<int> cellSize;
    vector<string> netName;
    vector<vector<int>> netCells;

    void clearData() {
        cellMap.clear();
        cellName.clear();
        netName.clear();
        netCells.clear();
    }
};

BookshelfData bsData;

//---------------------------------------------------------------------

bool isBookshelfSymbol(unsigned char c) {
    static char symbols[256] = {0};
    static bool inited = false;
    if (!inited) {
        symbols[(int)'('] = 1;
        symbols[(int)')'] = 1;
        // symbols[(int)'['] = 1;
        // symbols[(int)']'] = 1;
        symbols[(int)','] = 1;
        // symbols[(int)'.'] = 1;
        symbols[(int)':'] = 1;
        symbols[(int)';'] = 1;
        // symbols[(int)'/'] = 1;
        symbols[(int)'#'] = 1;
        symbols[(int)'{'] = 1;
        symbols[(int)'}'] = 1;
        symbols[(int)'*'] = 1;
        symbols[(int)'\"'] = 1;
        symbols[(int)'\\'] = 1;

        symbols[(int)' '] = 2;
        symbols[(int)'\t'] = 2;
        symbols[(int)'\n'] = 2;
        symbols[(int)'\r'] = 2;
        inited = true;
    }
    return symbols[(int)c] != 0;
}

bool readBSLine(istream& is, vector<string>& tokens) {
    tokens.clear();
    string line;
    while (is && tokens.empty()) {
        // read next line in
        getline(is, line);

        char token[1024] = {0};
        int lineLen = (int)line.size();
        int tokenLen = 0;
        for (int i = 0; i < lineLen; i++) {
            char c = line[i];
            if (c == '#') {
                break;
            }
            if (isBookshelfSymbol(c)) {
                if (tokenLen > 0) {
                    token[tokenLen] = (char)0;
                    tokens.push_back(string(token));
                    token[0] = (char)0;
                    tokenLen = 0;
                }
            } else {
                token[tokenLen++] = c;
                if (tokenLen > 1024) {
                    // TODO: unhandled error
                    tokens.clear();
                    return false;
                }
            }
        }
        // line finished, something else in token
        if (tokenLen > 0) {
            token[tokenLen] = (char)0;
            tokens.push_back(string(token));
            tokenLen = 0;
        }
    }
    return !tokens.empty();
}

//---------------------------------------------------------------------

bool Database::readNets(const std::string& file) {
    cout << "reading net" << std::endl;
    ifstream fs(file.c_str());
    if (!fs.good()) {
        printlog("cannot open file: %s", file.c_str());
        return false;
    }
    
    vector<string> tokens;
    int netID;
    while (readBSLine(fs, tokens)) {
        if (tokens[0] == "NET") {
            int netID = bsData.nNets++;
            bsData.netName.push_back(tokens[1]);
            bsData.netCells.resize(bsData.nNets);
            for (unsigned i = 2; i < tokens.size(); i++) {
                string cName = tokens[i];
                int cellID = bsData.cellMap[cName];
                bsData.netCells[netID].push_back(cellID);
            }
        }
        // else{
        //     for (unsigned i = 2; i < tokens.size(); i++) {
        //         string cName = tokens[i];
        //         int cellID = bsData.cellMap[cName];
        //         bsData.netCells[netID].push_back(cellID);
        //     }
        // }
    }

    fs.close();
    return true;
}

//---------------------------------------------------------------------

bool Database::readCells(const std::string& file) {
    cout << "reading cells" << std::endl;
    ifstream fs(file.c_str());
    if (!fs.good()) {
        printlog("cannot open file: %s", file.c_str());
        return false;
    }

    int numCells = 0;

    vector<string> tokens;
    while (readBSLine(fs, tokens)) {
        if (tokens.size() == 2) {
            int cellID = bsData.nCells++;
            string cName = tokens[0];

            bsData.cellName.push_back(cName);
            bsData.cellMap[cName] = cellID;
            bsData.cellSize.push_back(stoi(tokens[1]));
        }
    }

    fs.close();
    return true;
}

//---------------------------------------------------------------------


bool Database::read(const std::string& cellFile, const std::string& netFile) {

    readCells(cellFile);
    readNets(netFile);

    this->nCells = bsData.nCells;
    this->nNets = bsData.nNets;

    for (int i = 0; i < bsData.nCells; i++) {
        int cellID = i;
        Cell* cell = this->addCell(bsData.cellName[cellID], bsData.cellSize[cellID]);
        cell->id = cellID;
    }
    
    // net
    for (unsigned i = 0; i != bsData.nNets; ++i) {
        int netID = i;
        Net* net = this->addNet(bsData.netName[netID]);
        net->id = netID;
        int counter[2] = {0,0};
        for (unsigned j = 0; j != bsData.netCells[netID].size(); ++j) {
            int cellID = bsData.netCells[netID][j];
            Cell* cell = this->getCell(bsData.cellName[cellID]);
            
            //for graph
            net->cell_list.push_back(cell);
            cell->net_list.push_back(net);
            
            // for cut
            counter[cell->group]++;
        }
        net->cut = (counter[0]!=0)*(counter[1]!=0);
    }

    return true;
}
