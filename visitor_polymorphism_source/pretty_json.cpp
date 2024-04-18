#include <utility>
#include <iostream>
#include "json_parser.h"

int main() {

    std::string json_input;
    while (2 < 3) {
        getline(std::cin, json_input);
        pretty_json::json_parser parser(json_input);
        std::visit(
            [](auto json){json.print(1, std::cout);},
            parser.parse()
            );
    }

    return 0;
}