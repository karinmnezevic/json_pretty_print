#pragma once

#include <iomanip>
#include <vector>
#include <variant>
#include <string>

namespace pretty_json {

inline size_t indentation = 2;

//json_val<int>          represents NUMBERS
//json_val<bool>         represents BOOLS
//json_val<std::nullptr> represents NULL
//json_val<std::string>  represents STRINGS
template <typename T>
class json_val {
    std::string_view _content;
public: 
    inline json_val(char* beg, size_t len) : _content(beg, len) {}
    inline void print(size_t level, std::ostream& os, size_t val_width = 0, size_t key_width = 0) const {
        os << std::right << std::setw(std::max(_content.size(), val_width)) << _content;
    }
    inline size_t len() const{
        return _content.length();
    }
    inline bool is_basic_json() const {
        return true;
    }
    inline std::string_view val() const {
        return _content;
    }
};

class json_object;
class json_array;

using json_type = std::variant <
    json_val<int>,
    json_val<bool>,
    json_val<std::nullptr_t>,
    json_val<std::string>,
    json_object,
    json_array
>;

class json_object {
private:
    std::vector<std::pair<std::string_view, json_type>> _key_vals;
public:
    json_object(std::vector<std::pair<std::string_view, json_type>>& key_vals);
    void print(size_t level, std::ostream& os) const;
    size_t len() const;
};

class json_array {
private:
    std::vector<json_type> _array;
public:
    json_array(std::vector<json_type>& array);
    void print(size_t level, std::ostream& os) const;
    size_t len() const;
};

} // namespace pretty_json