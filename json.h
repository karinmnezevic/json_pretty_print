#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <iostream>

int indentation = 2;

class json_any {
public:
    virtual void print(int level) const = 0;
    virtual size_t len() const = 0;
    virtual ~json_any() = default;
};

class json_object : public json_any {
    std::vector<std::pair<std::string_view, std::unique_ptr<json_any>>> _key_vals;
public:
    json_object(std::vector<std::pair<std::string_view, std::unique_ptr<json_any>>>& key_vals) :
        _key_vals(std::move(key_vals)) {}

    void print(int level) const override {
        bool new_lines = len() > 60 || _key_vals.size() > 4;
        std::cout << '{';
        if (new_lines)
            std::cout << std::endl;

        for (int i = 0; i < _key_vals.size(); i ++) {
            if (new_lines)
                std::cout << std::string(level * indentation, ' ');
            std::cout << _key_vals[i].first << ": ";
            _key_vals[i].second->print(level + 1);
            if (i != _key_vals.size() - 1)
                std::cout << ", ";
            if (new_lines)
                std::cout << std::endl;
        }
        if (new_lines)
            std::cout << std::string((level-1) * indentation, ' ');
        std::cout << '}';
    }
    size_t len() const {
        int ret = 2; // {}
        for (const auto& [key, val] : _key_vals) {
            ret += key.length() + 2; // : 
            ret += val->len() + 2; // ,
        }
        return ret - 2; // no comma after last elem
    }
};

class json_array : public json_any {
    std::vector<std::unique_ptr<json_any>> _array;
public:
    json_array(std::vector<std::unique_ptr<json_any>>& array) :
        _array(std::move(array)) {}

    void print(int level) const override {
        bool new_lines = len() > 60 || _array.size() > 4;
        std::cout << '[';
        if (new_lines)
            std::cout << std::endl;

        for (int i = 0; i < _array.size(); i ++) {
            if (new_lines)
                std::cout << std::string(level * indentation, ' ');
            _array[i]->print(level + 1);
            if (i != _array.size() - 1)
                std::cout << ", ";
            if (new_lines)
                std::cout << std::endl;
        }
        if (new_lines)
            std::cout << std::string((level-1) * indentation, ' ');
        std::cout << ']';
    }
    size_t len() const {
        int ret = 2; // []
        for (const auto& elem : _array)
            ret += elem->len() + 2;  //,
        return ret - 2; // no comma after last elem
    }
};

template <typename T>
class json_val : public json_any {
    std::string_view _val;
public:
    json_val(char * beg, size_t len) : _val(beg, len) {}
    void print(int) const override {
        std::cout << _val;
    };
    std::string_view get() const {
        return _val;
    }
    size_t len() const {
        return _val.length();
    }
};

class json_parser {
private:
    std::string _expr;
    size_t _pos;

public:
    json_parser(const std::string expr) : _expr(expr), _pos(0) {}
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
        while (_pos < _expr.size() && std::isspace(_expr[_pos])) {
            _pos ++;
        }
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
        while (_pos < _expr.size() && peek_next_char() != '\"') {
            // Handle escape characters
            next_char();
        }
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
