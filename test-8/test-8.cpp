#include "prereqs.hpp"

#include "test-8.hpp"

#include "config.hpp"

int main(int argc, char** args) {
    (void) argc;
    (void) args;

    cerr.precision(3);
    cerr.imbue(std::locale("en_US.UTF-8"));

    cerr << config::print() << endl;

    auto t1 = std::chrono::steady_clock::now();
    auto graphs = ramseyGraphs<config::r, config::s, config::n>();
    auto t2 = std::chrono::steady_clock::now();
    auto t12 = std::chrono::duration<double>(t2 - t1).count();

    cerr << "Ramsey(" << config::r << "," << config::s << ")-graphs with " << config::n << " vertices: " << graphs.size() << endl;
    cerr << "Total time: " << std::fixed << t12 << " seconds" << endl;

    return 0;
}
