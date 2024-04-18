#include "json_types.h"

namespace pretty_json {

json_object::json_object(std::vector<std::pair<std::string_view, json_type>>& key_vals) :
    _key_vals(key_vals) {}

void json_object::print(size_t level, std::ostream& os) const {
    return;
}

size_t json_object::len() const{
    return 0;
}

json_array::json_array(std::vector<json_type>& array) :
    _array(array) {}

void json_array::print(size_t level, std::ostream& os) const {
    return;
}

size_t json_array::len() const{
    return 0;
}


} // namespace pretty_json