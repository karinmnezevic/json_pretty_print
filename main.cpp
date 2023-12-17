#include <iostream>
#include "json.h"

int main() {
    // Example usage
    std::string json_input = R"(
        {
            "name": "John Doe",
            "age": 30,
            "isStudent": false,
            "grades": [95, 88, 75],
            "address": {
                "city": "New York",
                "zip": "10001"
            }
        }
    )";
    
    json_parser parser(json_input);
    std::unique_ptr<json_any> parsed_json = parser.parse();
    parsed_json->print();

    return 0;
}