#include <iostream>
#include <sol/sol.hpp>
#include <benchmark/benchmark.h>

unsigned int i = 0;

void rmain(benchmark::State & st){
    for(auto _ : st){
        using namespace std;
        using namespace sol;

        sol::state lua;
        lua.set_function("beep",[&]{++i;});
        lua.script("beep()");
    }
}

BENCHMARK(rmain);

