#include <iostream>
#include "json.h"

int main() {
    // Example usage
    //{"name": "John Doe", "age": 30,"isStudent": false,"grades": [95, 88, 75],"address": {"city": "New York","zip": "10001"}}
    //{"coord": [{"x": 1.2, "y": 1.23}, {"x": 3.5432, "y": 123}, {"x": 1, "y": 123.4} , {"x": -12, "y": 1.2}, {"x": 12, "y": 969.696}]}
    //{"coord": [[1.2, 1.23], [-3.1415, 2], [0.3, 2], [12, 969.696], [1, 2, 3], [-1, -2, -3, -4]]}
    std::string json_input;
    while (2 < 3) {
        getline(std::cin, json_input);
        json_parser parser(json_input);
        std::unique_ptr<json_any> parsed_json = parser.parse();
        parsed_json->print(1);
        std::cout << parsed_json->len() << std::endl;
    }
    return 0;
}