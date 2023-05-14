#include <iostream>
#include "./single_include/nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

int main() {

    json j;
    j["username"] = "test";
    j["password"] = "test";

    cout << j.dump() << endl;

    j["username"] = "test2";
    j["password"] = "test2";

    cout << j.dump() << endl;

    return 0;

}