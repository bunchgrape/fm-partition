#include "pt/Partition.h"
#include "global.h"

void signalHandler(int signum) {
    std::cout << "Signal (" << signum << ") received. Exiting...\n";

    std::exit(signum);
}

// -----------------------------------------------------------------------------

void partition(const string& file_path){
    log()<< "Partitioning" << std::endl;

    utils::timer runtime;

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

    // runtime
    double io_time = runtime.elapsed();
    log() << "========== IO time: " << io_time << " s ==========\n\n\n";

    parter.iter();
     // runtime
    double exe_time = runtime.elapsed();
    log() << "========== Execution time: " << exe_time - io_time << " s ==========\n\n\n";
    
    parter.write();

    ofstream outfile;

    outfile.open("./output"+design+".log", ios::out);

    outfile << "cutsize " << parter.cut_size << "\n";
    outfile << "IO time: " << io_time << "\n";
    outfile << "Execution time: " << exe_time - io_time << "\n";
    
    outfile.close();
}

// -----------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    srand (time(NULL));

    std::cout << std::boolalpha;  // set std::boolalpha to std::cout
    
    log() << "---------------------------------------------------------------" << std::endl;

    string file_path = string(argv[1]);

    log() << "Reading from " << file_path << std::endl;


    partition(file_path);

    printlog("---------------------------------------------------------------------------");
    printlog("                               Terminated...                               ");
    printlog("---------------------------------------------------------------------------");

    return 0;
}