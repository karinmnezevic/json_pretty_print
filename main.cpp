#include <iostream>
#include "json_parser.h"

int main() {
    // Example usage
    //{"name": "John Doe", "age": 30,"isStudent": false,"grades": [95, 88, 75],"address": {"city": "New York","zip": "10001"}}
    //{"employees":[{"firstName":"John", "lastName":"Doe"},{"firstName":"Anna", "lastName":"Smith"},{"firstName":"Peter", "lastName":"Jones"}]}
    //[2324.24, -123.37, 0.23, 13.18, 5757.49, -73.23]
    //[2324.4, -123.7, null, 13.1, 5757.4, false]
    //{"x": 1, "x": 23, "x": 342.123, "x": -123, "x": 3242, "x": 984, "x": -23.435}
    //{"x": 2324.24, "xy": -123.37, "xyz": 0.23, "y":13.18, "yz": 5757.49, "z": -73.23, "x": -23.43}
    //{"coord": {"x": 1.2, "y": 1.23, "z": -13}, "coord": {"x": -13.845, "y": 1234, "z": 0}, "coord": {"x": 4563, "y": -12, "z": 0} , "coord": {"x": 1.2, "y": 1.23, "z": -12}}
    //{"coord": [{"x": 1.2, "y": 1.23}, {"x": 3.5432, "y": 123}, {"x": 1, "y": 123.4} , {"x": -12, "y": 1.2}, {"x": 12, "y": 969.696}]}
    //[[12.34, -1.24, 4859.13, 10.19], [321.32, -2342.12, 4.13, 21.23],[-12.33, -123.33, 23.33, 5435.33], [111.11, 22.22, 3.33, -4.44]]
    //[[123123214, 232, -12313], [1233, 24242, 1232], [211, -1, 0.32]]

// napravit github workflow s testiranjem ovih kiflica
//is_basic_val -> is_basic_json
//using ptr -> std::unique_ptr
//_elems public koji kurac, generalno public/private brija sredit
//prvo val_w onda key_w uvijek u funkcijama
// dodat parsanje esc chara u stringovima
// pitat chatgpt za neke savjete
// promjenit ime u json_pretty_print

// napisat neke formalne testove
//test matrica s malo normalnim, malo key_val
//test matrice s razlicitim brojem stupaca
//test [ vise objekata koji ne stanu u isti red ]

// mozda brojke poravnavat s desna, a stringove slijeva
    std::string json_input;
    while (2 < 3) {
        getline(std::cin, json_input);
        pretty_json::json_parser parser(json_input);
        std::unique_ptr<pretty_json::json_any> parsed_json = parser.parse();
        parsed_json->print(1, std::cout);
    }

    return 0;
}       