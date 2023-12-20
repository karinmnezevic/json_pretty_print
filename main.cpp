#include <iostream>
#include "json.h"

int main() {
    // Example usage
    //{"name": "John Doe", "age": 30,"isStudent": false,"grades": [95, 88, 75],"address": {"city": "New York","zip": "10001"}}
    //
    //
    //{"coord": [{"x": 1.2, "y": 1.23}, {"x": 3.5432, "y": 123}, {"x": 1, "y": 123.4} , {"x": -12, "y": 1.2}, {"x": 12, "y": 969.696}]}
    //{"coord": [[1.2, 1.23], [-3.1415, 2], [0.3, 2], [12, 969.696], [1, 2, 3], [-1, -2, -3, -4]]}
    //[1, 23, 342.123, -123, 3242, 984, -23.435]
    //{"x": 1, "x": 23, "x": 342.123, "x": -123, "x": 3242, "x": 984, "x": -23.435}
    //{"x": 1, "xy": 23, "xyz": 342.123, "y": -123, "yz": 3242, "z": 984, "x": -23.435}

    /*
    
dodati using za egzoticne tipove
maknut ruzan for
dodati neku fora za indentaciju
sve prepisati u std::variant terminima


Zakljucak:
Kad propisati velicinu ispisa:
1) ispisujemo kontejner u nove redove, koji sadrzi samo jednostavne tipove
2) ispisujemo kontejner u nove redove, koji sadrzi one-line kontejnere jednostavnih tipova
*/

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