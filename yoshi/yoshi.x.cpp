#include "yoshi.hpp"

#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    benchmark::Initialize(&argc, argv);
    bool help = false;
    bool short_run = false;
    try
    {
        po::options_description desc{"Options"};
        desc.add_options()
        ("help,h", "Help screen")
        ("short,s", po::bool_switch()->default_value(false), "Short mode");

        po::variables_map vm;
        po::store(parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        bool help = vm.count("help");
        short_run = vm["short"].as<bool>();
        if (help)
        {
            std::cout << desc << "\n";
        }
    }
    catch (const po::error &ex)
    {
        std::cerr << ex.what() << '\n';
        return 1;
    }
    const auto& benchmarks = yoshi::internal::Benchmarks::instance()->get();
    for(auto& b : benchmarks)
    {
        auto bench = benchmark::RegisterBenchmark(b->name.c_str(), b->benchmark);
        if (short_run)
        {
            if (b->shortArgs)
            {
                bench->Arg(1024)->Arg(8192);
            }
            else
            {
                bench->Arg(10000)->Arg(250000);
            }
        }
        else
        {
            if (b->shortArgs)
            {
                bench->Arg(1000)->Arg(5000)->Arg(10000)->Arg(25000)->Arg(50000)->Arg(75000)->Arg(100000);
            }
            else
            {
                bench->Arg(1000)->Arg(10000)->Arg(50000)->Arg(100000)->Arg(250000)->Arg(500000)->Arg(750000)->Arg(1000000);
            }
        }
    }
    benchmark::RunSpecifiedBenchmarks();
}
