#include "pt/Partition.h"
#include "global.h"

void signalHandler(int signum) {
    std::cout << "Signal (" << signum << ") received. Exiting...\n";

    std::exit(signum);
}

// -----------------------------------------------------------------------------

void partition(const string& file_path){
    std::cout << "partitioning" << std::endl;

    // required
	std::string cellFile = file_path;
    std::string netFile = cellFile.substr(0,cellFile.find_last_of('.')) + ".nets";

    db::Database database;

    std::cout << &database << std::endl;

    database.read(cellFile, netFile);


    pt::Partition parter(&database);

    std::cout << &parter.database << std::endl;

    parter.load();

    parter.iter();

    parter.write();

    // for (int i : parter.gain_history)
    //     cout << i << endl;

}

// -----------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    srand (time(NULL));

    std::cout << std::boolalpha;  // set std::boolalpha to std::cout
    
    std::cout << "----------------------------" << std::endl;

    string file_path = string(argv[1]);

    std::cout << file_path << std::endl;

    // vector<int> a = {-5,1,3};
    // cout << max_element(a.begin(),
    //                             a.end()) - a.begin();
    // exit(1);

    partition(file_path);

    std::cout << "----------terminated------------" << std::endl;

    return 0;
}