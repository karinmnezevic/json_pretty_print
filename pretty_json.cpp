#include "json_parser.h"

int main() {

    std::string json_input;
    while (2 < 3) {
        getline(std::cin, json_input);
        pretty_json::json_parser parser(json_input);
        std::unique_ptr<pretty_json::json_any> parsed_json = parser.parse();
        parsed_json->print(1, std::cout);
    }

    return 0;
}