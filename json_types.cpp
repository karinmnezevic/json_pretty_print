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
    return len() <= 60;
}

json_key_value::json_key_value(std::string_view key, std::unique_ptr<json_any> val) : 
    _key(key), _val(std::move(val)) {}

void json_key_value::print(int level, int val_width, int key_width) const {
    if (key_width == 0) 
        key_width = _key.size();
    std::cout << std::left << std::setw(key_width) << _key << ": ";
    _val->print(level, val_width, key_width);
}

size_t json_key_value::len() const {
    return _key.size() + 2 + _val->len();
}

bool json_key_value::is_basic_val() const {
    return _val->is_basic_val();
}

std::string_view json_key_value::key() const {
    return _key;
}

const std::unique_ptr<json_any>& json_key_value::val() const {
    return _val;
}

json_container::json_container(container_t& elems) : _elems(std::move(elems)) {}

std::pair<size_t, size_t> json_container::calc_widths() const {
    size_t val_width = 0;
    size_t key_width = 0;
    for (const auto& elem : _elems) {
        if (!elem->is_basic_val())
            return std::make_pair(0,0);
        if (const auto* key_value = dynamic_cast<const json_key_value*>(elem.get())) {
            val_width = std::max(val_width, key_value->val()->len());
            key_width = std::max(key_width, key_value->key().size());
        }
        else
            val_width = std::max(val_width, elem->len());
    }
    return std::make_pair(val_width, key_width);
}

bool json_container::is_one_liner() const {
    return len() <= 60 && _elems.size() <= 4;
}

size_t json_container::len() const {
    size_t ret = 2;
    for (const auto& elem : _elems)
        ret += elem->len() + 2; // comma
    return ret - 2; // no comma after last elem
}

const container_t& json_container::elems() const {
    return _elems;
}

bool json_container::is_basic_val() const {
    return false;
}

std::tuple<bool, std::vector<size_t>, std::vector<size_t>> json_container::check_matrix() const {
    bool is_matrix = true;
    std::vector<size_t> max_column_key_len;
    std::vector<size_t> max_column_val_len;
    for (const auto& elem : _elems) {
        if (const auto* container = dynamic_cast<const json_container*>(elem.get())) {
            is_matrix &= container->is_one_liner();
            for (size_t col = 0; col < container->elems().size(); col ++) {
                is_matrix &= container->elems()[col]->is_basic_val();
                if (max_column_val_len.size() <= col) {
                    max_column_key_len.push_back(0);
                    max_column_val_len.push_back(0);
                }
                if (const auto* key_value = dynamic_cast<const json_key_value*>(container->elems()[col].get())) {
                    max_column_key_len[col] = std::max(max_column_key_len[col], key_value->key().size());
                    max_column_val_len[col] = std::max(max_column_val_len[col], key_value->val()->len());                    
                }
                else
                    max_column_val_len[col] = std::max(max_column_val_len[col], container->elems()[col]->len());
            }
        }
        else {
            is_matrix = false;
            break;
        }
    }
    return std::make_tuple(is_matrix, max_column_key_len, max_column_val_len);
}

void json_container::print_single_line(int level, std::vector<size_t> keys_v, std::vector<size_t> vals_v) const {
    if (vals_v.empty()) vals_v.assign(_elems.size(), 0);
    if (keys_v.empty()) keys_v.assign(_elems.size(), 0);
    for (int i = 0; i < _elems.size(); i ++) {
        _elems[i]   ->print(level + 1, vals_v[i], keys_v[i]);
        if (i != _elems.size() - 1)
            std::cout << ", ";
    }
}

void json_container::print_multiline(int level, size_t val_width, size_t key_width) const {
    std::cout << std::endl;
    for (int i = 0; i < _elems.size(); i ++) {
        std::cout << std::string(level * indentation, ' ');
        _elems[i]->print(level + 1, val_width, key_width);
        if (i != _elems.size() - 1)
            std::cout << ", ";
        std::cout << std::endl;
    }
    std::cout << std::string((level-1) * indentation, ' ');
}

void json_container::print_multiline_matrix(
    int level, std::vector<size_t> col_key_widths, std::vector<size_t> col_val_widths) const {
    std::cout << std::endl;
    for (int i = 0; i < _elems.size(); i ++) {
        std::cout << std::string(level * indentation, ' ');
        if (const auto* container = dynamic_cast<const json_container*>(_elems[i].get())) //ovo uvijek mora vrijediti
            container->print_single_line(level + 1, col_key_widths, col_val_widths);
        if (i != _elems.size() - 1)
            std::cout << ", ";
        std::cout << std::endl;
    }
    std::cout << std::string((level - 1) * indentation, ' ');
}

void json_container::print(int level, int val_width, int key_width) const{
    std::cout << _begin;
    if (is_one_liner()) {
        print_single_line(level);
        std::cout << _end;
        return;
    }

    auto [is_matrix, max_keys, max_vals] = check_matrix();
    if (is_matrix) {
        print_multiline_matrix(level, max_keys, max_vals);
        std::cout << _end;
        return;
    }
    
    auto [val_w, key_w] = calc_widths();
    print_multiline(level, val_w, key_w);
    std::cout << _end;
}

json_object::json_object(container_t& key_vals) : 
    json_container::json_container(key_vals) {
        _begin = '{';
        _end = '}';
    }

json_array::json_array(container_t& array) : json_container::json_container(array) {
    _begin = '[';
    _end = ']';
}

} // namespace pretty_json