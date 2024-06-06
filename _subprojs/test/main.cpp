#include <iostream>
#include <string>
#include <fstream>
#include <alib/autil.h>
#include <alib/aclock.h>
#include <alib/alogger.h>
#include <benchmark/benchmark.h>
#include <glm/glm.hpp>

using namespace alib::ng;




template<bool returnACopy = false> auto test(){
 if constexpr(returnACopy)return 1;
}

int main(){
    Logger logger;
    logger.setOutputFile("test.log");
    LogFactory lg("Test",logger);
}

/*
ifstream ifs,ifs2;
int init(){
    ifs.open("E:\\UnlimitedLife\\_subprojs\\test\\test.txt");
    ifs2.open("E:\\UnlimitedLife\\_subprojs\\test\\test.txt");
    return 0;
}
int a = init();

void test(benchmark::State & s){
    for(auto _ : s){
        ifs.seekg(0,std::ios::beg);
        string b = Util::io_readAll(ifs);
    }
}

void test_cmp(benchmark::State & s){
    for(auto _ : s){
        string tmp;
        string b;
        ifs2.seekg(0,std::ios::beg);
        while(!ifs2.eof()){
            getline(ifs2,tmp);
            b += tmp;
        }
    }
}

BENCHMARK(test);
BENCHMARK(test_cmp);
*/
