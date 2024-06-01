#include <iostream>
#include <sol/sol.hpp>


int main(){
    using namespace std;
    using namespace sol;

    sol::state lua;
    lua.set_function("beep",[]{cout << "Hello Lua" << endl;});
    lua.script("beep()");
}

