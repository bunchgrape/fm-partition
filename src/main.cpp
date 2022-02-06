#include "pt/Partition.h"
#include "global.h"

void signalHandler(int signum) {
    std::cout << "Signal (" << signum << ") received. Exiting...\n";

    std::exit(signum);
}

// -----------------------------------------------------------------------------

void partition(const string& file_path){
    std::cout << "Partitioning" << std::endl;

    // required
	std::string cellFile = file_path;
    std::string prefix = cellFile.substr(0,cellFile.find_last_of('.'));
    std::string netFile = prefix + ".nets";
    std::string design = prefix.substr(prefix.find_last_of('/'));

    db::Database database;

    database.designName = design;

    database.read(cellFile, netFile);

    pt::Partition parter(&database);

    parter.load();

    parter.iter_bucket();

    parter.write();

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


    partition(file_path);

    printlog("---------------------------------------------------------------------------");
    printlog("                               Terminated...                               ");
    printlog("---------------------------------------------------------------------------");

    return 0;
}