#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>
#include <typeinfo>

#include "json_types.h"

namespace pretty_json {

bool json_any::is_one_liner() const {
    return len() <= 60 && children().size() <= 4;
}

bool json_any::all_children_are_basic_vals() const {
    bool ret = true;
    for (const auto& child : children()) 
        ret &= child->is_basic_val();
    return ret;
}

void json_key_value::print(int level, int val_width, int key_width) const {
    if (key_width == 0) 
        key_width = _key.size();
    std::cout << std::left << std::setw(key_width) << _key << ": ";
    _val->print(level, val_width, key_width);
}

void json_key_value::print_as_row(int level, const std::vector<size_t> keys_v, const std::vector<size_t> widths_v) const {
    //longest key nekako vrag ga odnio
    std::cout << _key << ": ";
    _val->print_as_row(level, keys_v, widths_v);
}

size_t json_key_value::len() const {
    return _key.size() + 2 + _val->len();
}

bool json_key_value::is_basic_val() const {
    return _val->is_basic_val();
}

const container_t& json_key_value::children() const {
    return _val->children();
}

size_t json_key_value::key_len() const {
    return _key.size();
}

size_t json_key_value::val_len() const {
    return _val->len();
}

json_container::json_container(container_t& elems) : _elems(std::move(elems)) {}

size_t json_container::len() const {
    size_t ret = 2;
    for (const auto& elem : _elems)
        ret += elem->len() + 2; // comma
    return ret - 2; // no comma after last elem
}

const container_t& json_container::children() const {
    return _elems;
}

bool json_container::is_basic_val() const {
    return false;
}

size_t json_container::longest_child() const {
    size_t max_len = 0;
    for (const auto& elem : _elems)
        max_len = std::max(max_len, elem->len());
    return max_len;
}

std::tuple<bool, std::vector<size_t>, std::vector<size_t>> json_container::check_matrix() const {
    bool is_matrix = true;
    std::vector<size_t> max_column_key_len;
    std::vector<size_t> max_column_val_len;
        for (const auto& elem : _elems) {
        is_matrix &= !elem->is_basic_val(); //all_children_are_containers
        is_matrix &= elem->is_one_liner(); //they fit in one line
        is_matrix &= elem->all_children_are_basic_vals(); //all_granchildren_are_basic_vals
        if (!is_matrix) break;
        int column = 0;
        for (const auto& grandchild : elem->children()) {
            if (const auto* key_value = dynamic_cast<const json_key_value*>(grandchild.get())) {
                if (max_column_val_len.size() <= column) {
                    max_column_key_len.push_back(key_value->key_len());
                    max_column_val_len.push_back(key_value->val_len());
                }
                else {
                    max_column_key_len[column] = std::max(max_column_key_len[column], key_value->key_len());
                    max_column_val_len[column] = std::max(max_column_val_len[column], key_value->val_len());
                }
            }
            else {
                if (max_column_val_len.size() <= column) {
                    //pretpostavljam da su u ovom slucaju svi basic_val
                    max_column_val_len.push_back(grandchild->len());
                }
                else {
                    max_column_val_len[column] = std::max(max_column_val_len[column], grandchild->len());
                }
            }
            column ++;
        }
    }
    return std::make_tuple(is_matrix, max_column_key_len, max_column_val_len);
}

json_object::json_object(container_t& key_vals) : json_container::json_container(key_vals) {}

size_t json_object::longest_key() const {
    size_t max_key_len = 0;
    for (const auto& elem : _elems) {
        if (const auto* key_value = dynamic_cast<const json_key_value*>(elem.get())) {
            max_key_len = std::max(max_key_len, key_value->key_len()); //mozda se mozes i bez ifa izvuc
        }
    }
    return max_key_len;
}

void json_object::print(int level, int val_width, int key_width) const {
    if(is_one_liner()) {
        std::cout << '{';
        for (int i = 0; i < _elems.size(); i ++) {
            _elems[i]->print(level + 1, val_width, key_width);
            if (i != _elems.size() - 1)
                std::cout << ", ";
        }
        std::cout << '}';
    }
    else {
        auto [is_matrix, max_keys, max_vals] = check_matrix();
        if (is_matrix) {
            std::cout << '{' << std::endl;
            for (int i = 0; i < _elems.size(); i ++) {
                std::cout << std::string(level * indentation, ' '); 
                _elems[i]->print_as_row(level + 1, max_keys, max_vals);
                if (i != _elems.size() - 1)
                    std::cout << ", ";
                std::cout << std::endl;
            }
            std::cout << std::string((level - 1) * indentation, ' ') << '}';
        }
        else {
            std::cout << '{' << std::endl;
            for (int i = 0; i < _elems.size(); i ++) {
                std::cout << std::string(level * indentation, ' ');
                if (all_children_are_basic_vals()) {
                    _elems[i]->print(level + 1, longest_child(), longest_key());
                }
                else {
                    _elems[i]->print(level + 1, 0, 0);
                }
                if (i != _elems.size() - 1)
                    std::cout << ", ";
                std::cout << std::endl;
            }
            std::cout << std::string((level - 1) * indentation, ' ') << '}';
        }
    }
}

void json_object::print_as_row(int level, const std::vector<size_t> keys_v, const std::vector<size_t> vals_v) const {
    std::cout << '{';
    for (int i = 0; i < _elems.size(); i ++) {
        _elems[i]->print(level + 1, vals_v[i], keys_v[i]);
        if (i != _elems.size() - 1)
            std::cout << ", ";
    }
    std::cout << '}';
}

json_array::json_array(container_t& array) : json_container::json_container(array) {}

void json_array::print(int level, int val_width, int key_width) const {
    if(is_one_liner()) {
        std::cout << '[';
        for (int i = 0; i < _elems.size(); i ++) {
            _elems[i]->print(level + 1, val_width, key_width);
            if (i != _elems.size() - 1)
                std::cout << ", ";
        }
        std::cout << ']';
    }
    else {
        auto [is_matrix, max_keys, max_vals] = check_matrix();
        if(is_matrix) {
            std::cout << '[' << std::endl;
            for (int i = 0; i < _elems.size(); i ++) {
                std::cout << std::string(level * indentation, ' ');
                _elems[i]->print_as_row(level + 1, max_keys, max_vals);
                if (i != _elems.size() - 1)
                    std::cout << ", ";
                std::cout << std::endl;
            }
            std::cout << std::string((level - 1) * indentation, ' ') << ']';
        }
        else {
            std::cout << '[' << std::endl;
            for (int i = 0; i < _elems.size(); i ++) {
                std::cout << std::string(level * indentation, ' ');
                if (all_children_are_basic_vals()) {
                    _elems[i]->print(level + 1, longest_child());
                }
                else {
                    _elems[i]->print(level + 1, 0, 0);
                }
                if (i != _elems.size() - 1)
                    std::cout << ", ";
                std::cout << std::endl;
            }
            std::cout << std::string((level-1) * indentation, ' ') << ']';
        }
    }
}

void json_array::print_as_row(int level, const std::vector<size_t> keys_v, const std::vector<size_t> vals_v) const {
    std::cout << '[';
    for (int i = 0; i < _elems.size(); i ++) {
        _elems[i]->print(level + 1, vals_v[i]);
        if (i != _elems.size() - 1)
            std::cout << ", ";
    }
    std::cout << ']';
}


} // namespace pretty_json