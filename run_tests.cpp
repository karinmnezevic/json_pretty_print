#include "json_parser.h"

int main() {
    pretty_json::json_parser parser(
        "{\"name\": \"John Doe\", \"age\": 30,\"isStudent\": false,\"grades\": [95, 88, 75],\"address\": {\"city\": \"New York\",\"zip\": \"10001\"}}"
    );
    std::unique_ptr<pretty_json::json_any> parsed_json = parser.parse();
    
    return 0; 
}