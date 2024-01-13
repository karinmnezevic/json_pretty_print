#include "json_types.h"
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>
#include <typeinfo>

namespace pretty_json {

int indentation = 2; // number of spaces used as indent

bool is_basic_val(const std::unique_ptr<json_any>& ptr) {
    return  typeid(*ptr) == typeid(json_val<int>) || 
            typeid(*ptr) == typeid(json_val<std::string>) ||
            typeid(*ptr) == typeid(json_val<bool>) ||
            typeid(*ptr) == typeid(json_val<std::nullptr_t>);
}

json_object::json_object(key_vals_t& key_vals) : _key_vals(std::move(key_vals)) {}

size_t json_object::len() const {
    int ret = 2; // {}
    for (const auto& [key, val] : _key_vals) {
        ret += key.length() + 2; // : 
        ret += val->len() + 2; // ,
    }
    return ret - 2; // no comma after last elem
}

bool json_object::is_one_liner() const {
    return len() <= 60 && _key_vals.size() <= 4;
}

size_t json_object::longest_child() const {
    size_t max_val_len = 0;
    for (const auto& [key, val] : _key_vals)
        max_val_len = std::max(max_val_len, val->len());
    return max_val_len;
}

size_t json_object::longest_key() const {
    size_t max_key_len = 0;
    for (const auto& [key, val] : _key_vals)
        max_key_len = std::max(max_key_len, key.length());
    return max_key_len;
}

bool json_object::all_children_are_basic_vals() const {
    bool ret = true;
    for (const auto& [key, val] : _key_vals)
        ret &= is_basic_val(val);
    return ret;
}

bool json_object::is_matrix() const { //{[], [], [], [], []}
    for (const auto& [key, val] : _key_vals) {
        if (is_basic_val(val)) return false; //all_children_are_containers
        if (!val->is_one_liner()) return false; //they fit in one line
        if (!val->all_children_are_basic_vals()) return false; //all_granchildren_are_basic_vals
    }
    return true;
}

size_t json_object::matrix_cell_width() const {
    size_t max_grandchild_len = 0;
    for (const auto& [key, val] : _key_vals)
        max_grandchild_len = std::max(max_grandchild_len, val->longest_child());
    return max_grandchild_len;
}

void json_object::print(int level, int width) const {
    if(is_one_liner()) {
        std::cout << '{';
        for (int i = 0; i < _key_vals.size(); i ++) {
            std::cout << _key_vals[i].first << ": ";
            _key_vals[i].second->print(level + 1, width);
            if (i != _key_vals.size() - 1)
                std::cout << ", ";
        }
        std::cout << '}';
    }
    else if(is_matrix()) {
        std::cout << '{' << std::endl;
        for (int i = 0; i < _key_vals.size(); i ++) {
            std::cout << std::string(level * indentation, ' ') << _key_vals[i].first << ": ";
            _key_vals[i].second->print(level + 1, matrix_cell_width());
            if (i != _key_vals.size() - 1)
                std::cout << ", ";
            std::cout << std::endl;
        }
        std::cout << std::string((level-1) * indentation, ' ') << '}';
    }
    else {
        std::cout << '{' << std::endl;
        for (int i = 0; i < _key_vals.size(); i ++) {
            std::cout << std::string(level * indentation, ' ');
            if (all_children_are_basic_vals()) {
                std::cout << std::setw(longest_key()) << std::left << _key_vals[i].first << ": ";
                _key_vals[i].second->print(level + 1, longest_child());
            }
            else { 
                std::cout << _key_vals[i].first << ": ";
                _key_vals[i].second->print(level + 1);
            }
            if (i != _key_vals.size() - 1)
                std::cout << ", ";
            std::cout << std::endl;
        }
        std::cout << std::string((level-1) * indentation, ' ') << '}';
    }
}

json_array::json_array(std::vector<std::unique_ptr<json_any>>& array) :
    _array(std::move(array)) {}

size_t json_array::len() const {
    int ret = 2; // []
    for (const auto& elem : _array)
        ret += elem->len() + 2;  //,
    return ret - 2; // no comma after last elem
}

bool json_array::is_one_liner() const {
    return len() <= 60 && _array.size() <= 4;
}

size_t json_array::longest_child() const {
    size_t max_elem_len = 0;
    for (const auto& elem : _array) 
        max_elem_len = std::max(max_elem_len, elem->len());
    return max_elem_len;
}

bool json_array::all_children_are_basic_vals() const {
    bool ret = true;
    for (const auto& elem : _array) 
        ret &= is_basic_val(elem);
    return ret;
}

bool json_array::is_matrix() const { //[[], [], [], [], []]
    for (const auto& elem : _array) {
        if (is_basic_val(elem)) return false; //all_children_are_containers
        if (!elem->is_one_liner()) return false; //they fit in one line
        if (!elem->all_children_are_basic_vals()) return false; //all_granchildren_are_basic_vals
    }
    return true;
}

size_t json_array::matrix_cell_width() const {
    size_t max_grandchild_len = 0;
    for (const auto& elem : _array)
        max_grandchild_len = std::max(max_grandchild_len, elem->longest_child());
    return max_grandchild_len;
}

void json_array::print(int level, int width) const {
    if (is_one_liner()) {
        std::cout << '[';
        for (int i = 0; i < _array.size(); i ++) {
            _array[i]->print(level + 1, width);
            if (i != _array.size() - 1)
                std::cout << ", ";
        }
        std::cout << ']';
    }
    else if (is_matrix()) {
        std::cout << '[' << std::endl;
        for (int i = 0; i < _array.size(); i ++) {
            std::cout << std::string(level * indentation, ' ');
            _array[i]->print(level + 1, matrix_cell_width());
            if (i != _array.size() - 1)
                std::cout << ", ";
            std::cout << std::endl;
        }
        std::cout << std::string((level-1) * indentation, ' ') << ']';
    }
    else {
        std::cout << '[' << std::endl;
        for (int i = 0; i < _array.size(); i ++) {
            std::cout << std::string(level * indentation, ' ');
            if (all_children_are_basic_vals())
                _array[i]->print(level + 1, longest_child());
            else
                _array[i]->print(level + 1);
            if (i != _array.size() - 1)
                std::cout << ", ";
            std::cout << std::endl;
        }
        std::cout << std::string((level-1) * indentation, ' ') << ']';
    }
}

} // namespace pretty_json