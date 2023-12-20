#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>
#include <typeinfo>

int indentation = 2;

class json_any {
public:
    virtual void print(int level, int width = 0) const = 0;
    virtual size_t len() const = 0;
    virtual bool is_one_liner() const = 0;
    virtual size_t longest_child() const = 0;
    virtual bool all_children_are_basic_vals() const = 0;
    virtual ~json_any() = default;
};

template <typename T>
class json_val : public json_any {
    std::string_view _val;
public:
    json_val(char * beg, size_t len) : _val(beg, len) {}
    bool is_one_liner() const override {
        return true;
    }
    void print(int level, int width = -1) const override {
        if (width == -1)
            width = _val.size();
        std::cout << std::right << std::setw(width) << _val;
    }
    std::string_view get() const {
        return _val;
    }
    size_t len() const {
        return _val.length();
    }
    bool all_children_are_basic_vals() const {
        return true;
    }
    size_t longest_child() const {
        return 0;
    }
};

bool is_basic_val(const std::unique_ptr<json_any>& ptr) {
    return  typeid(*ptr) == typeid(json_val<int>) || 
            typeid(*ptr) == typeid(json_val<std::string>) ||
            typeid(*ptr) == typeid(json_val<bool>) ||
            typeid(*ptr) == typeid(json_val<std::nullptr_t>);
}

class json_object : public json_any {
    std::vector<std::pair<std::string_view, std::unique_ptr<json_any>>> _key_vals;
public:
    json_object(std::vector<std::pair<std::string_view, std::unique_ptr<json_any>>>& key_vals) :
        _key_vals(std::move(key_vals)) {}

    size_t len() const {
        int ret = 2; // {}
        for (const auto& [key, val] : _key_vals) {
            ret += key.length() + 2; // : 
            ret += val->len() + 2; // ,
        }
        return ret - 2; // no comma after last elem
    }

    bool is_one_liner() const override {
        return len() <= 60 && _key_vals.size() <= 4;
    }

    size_t longest_child() const {
        size_t max_val_len = 0;
        for (const auto& [key, val] : _key_vals)
            max_val_len = std::max(max_val_len, val->len());
        return max_val_len;
    }

    size_t longest_key() const {
        size_t max_key_len = 0;
        for (const auto& [key, val] : _key_vals)
            max_key_len = std::max(max_key_len, key.length());
        return max_key_len;
    }

    bool all_children_are_basic_vals() const {
        bool ret = true;
        for (const auto& [key, val] : _key_vals)
            ret &= is_basic_val(val);
        return ret;
    }

    bool is_matrix() const { //{[], [], [], [], []}
        for (const auto& [key, val] : _key_vals) {
            if (is_basic_val(val)) return false; //all_children_are_containers
            if (!val->is_one_liner()) return false; //they fit in one line
            if (!val->all_children_are_basic_vals()) return false; //all_granchildren_are_basic_vals
        }
        return true;
    }

    size_t matrix_cell_width() const {
        size_t max_grandchild_len = 0;
        for (const auto& [key, val] : _key_vals)
            max_grandchild_len = std::max(max_grandchild_len, val->longest_child());
        return max_grandchild_len;
    }

    void print(int level, int width) const override {
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
};

class json_array : public json_any {
    std::vector<std::unique_ptr<json_any>> _array;
public:
    json_array(std::vector<std::unique_ptr<json_any>>& array) :
        _array(std::move(array)) {}

    size_t len() const {
        int ret = 2; // []
        for (const auto& elem : _array)
            ret += elem->len() + 2;  //,
        return ret - 2; // no comma after last elem
    }

    bool is_one_liner() const override {
        return len() <= 60 && _array.size() <= 4;
    }

    size_t longest_child() const {
        size_t max_elem_len = 0;
        for (const auto& elem : _array) 
            max_elem_len = std::max(max_elem_len, elem->len());
        return max_elem_len;
    }

    bool all_children_are_basic_vals() const {
        bool ret = true;
        for (const auto& elem : _array) 
            ret &= is_basic_val(elem);
        return ret;
    }

    bool is_matrix() const { //[[], [], [], [], []]
        for (const auto& elem : _array) {
            if (is_basic_val(elem)) return false; //all_children_are_containers
            if (!elem->is_one_liner()) return false; //they fit in one line
            if (!elem->all_children_are_basic_vals()) return false; //all_granchildren_are_basic_vals
        }
        return true;
    }

    size_t matrix_cell_width() const {
        size_t max_grandchild_len = 0;
        for (const auto& elem : _array)
            max_grandchild_len = std::max(max_grandchild_len, elem->longest_child());
        return max_grandchild_len;
    }

    void print(int level, int width) const override {
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
};

class json_parser {
private:
    std::string _expr;
    size_t _pos;

public:
    json_parser(const std::string& expr) : _expr(expr), _pos(0) {}
    std::unique_ptr<json_any> parse() {
        skip_spaces();
        return parse_any();
    }
private:
    char next_char() {
        return _expr[_pos++];
    }

    char peek_next_char() const {
        return _expr[_pos];
    }

    void skip_spaces() {
        while (_pos < _expr.size() && std::isspace(_expr[_pos]))
            _pos ++;
    }

    std::unique_ptr<json_any> parse_any() {
        char curr_char = peek_next_char();
        switch(curr_char) {
            case '{':
                return parse_object();
            case '[':
                return parse_array();
            case '\"':
                return parse_string();
            case 't':
                return parse_bool();
            case 'f':
                return parse_bool();
            case 'n':
                return parse_null();
            default:
                return parse_number();
        }
    }
    
    std::unique_ptr<json_object> parse_object() {
        std::vector<std::pair<std::string_view, std::unique_ptr<json_any>>> result;
        while (next_char() != '}') {
            skip_spaces();
            std::string_view key = parse_string()->get();
            skip_spaces();
            next_char();
            skip_spaces();
            result.push_back(std::make_pair(key, parse_any()));
            skip_spaces();
        }

        return std::make_unique<json_object>(result);
    }

    std::unique_ptr<json_array> parse_array() {
        std::vector<std::unique_ptr<json_any>> result;
        while (next_char() != ']') {
            skip_spaces();
            result.push_back(parse_any());
            skip_spaces();
        }
        return std::make_unique<json_array>(result);
    }

    std::unique_ptr<json_val<std::string>> parse_string() {
        size_t beg = _pos;
        next_char();
        while (_pos < _expr.size() && peek_next_char() != '\"')
            next_char();
        next_char();
        return std::make_unique<json_val<std::string>>(_expr.data() + beg, _pos - beg);
    }

    std::unique_ptr<json_val<bool>> parse_bool() {
        if (peek_next_char() == 't') {
            _pos += 4;
            return std::make_unique<json_val<bool>>(_expr.data() + _pos - 4, 4);
        }
        else {
            _pos += 5;
            return std::make_unique<json_val<bool>>(_expr.data() + _pos - 5, 5);
        }
    }

    std::unique_ptr<json_val<std::nullptr_t>> parse_null() {
        _pos += 4;
        return std::make_unique<json_val<std::nullptr_t>>(_expr.data() + _pos - 4, 4);
    }

    std::unique_ptr<json_val<int>> parse_number() {
        size_t beg = _pos;

        while (_pos < _expr.size() && 
                (std::isdigit(peek_next_char()) || peek_next_char() == '-' || peek_next_char() == '.' || 
                        peek_next_char() == 'e' || peek_next_char() == 'E')) 
            next_char();

        return std::make_unique<json_val<int>>(_expr.data() + beg, _pos - beg);
    }
};