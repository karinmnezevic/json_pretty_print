#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <iostream>

class json_any {
public:
    virtual void print() const = 0;
    virtual ~json_any() = default;
};

class json_object : public json_any {
    std::vector<std::pair<std::string, std::unique_ptr<json_any>>> _key_vals;
public:
    json_object(std::vector<std::pair<std::string, std::unique_ptr<json_any>>>& key_vals) {
        for (auto& ptr : key_vals) 
            _key_vals.push_back(std::move(ptr));
    }
    void print() const override {
        std::cout << '{' << std::endl;
        for (const auto& [key, val_ptr] : _key_vals) {
            std::cout << key << ": ";
            val_ptr->print();
            std::cout << ", ";
        }
        std::cout << '}' << std::endl;
    }
};

class json_array : public json_any {
    std::vector<std::unique_ptr<json_any>> _array;
public:
    json_array(std::vector<std::unique_ptr<json_any>>& array) {
        for (auto& ptr : array)
            _array.push_back(std::move(ptr));
    }
    void print() const override {
        std::cout << '[' << std::endl;
        for (const auto& ptr : _array) {
            ptr->print();
            std::cout << ", ";
        }
        std::cout << ']' << std::endl;
    }
};

//json_val<string> -> json_val<string_view>
template <typename T>
class json_val : public json_any {
    T _val;
public:
    json_val(T val) : _val(val) {}
    void print() const override {
        std::cout << _val;
    };
    T get() const {
        return _val;
    }
};

class json_parser {
private:
    std::string_view _expr;
    size_t _pos;

public:
    json_parser(const std::string_view expr) : _expr(expr), _pos(0) {}
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
        std::vector<std::pair<std::string, std::unique_ptr<json_any>>> result;
        while (next_char() != '}') {
            skip_spaces();
            std::string key = parse_string()->get();
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
        std::string result;
        next_char();
        while (_pos < _expr.size() && peek_next_char() != '\"') {
            // Handle escape characters
            result += next_char();
        }
        next_char();
        return std::make_unique<json_val<std::string>>(result);
    }

    std::unique_ptr<json_val<bool>> parse_bool() {
        if (peek_next_char() == 't') {
            _pos += 4;
            return std::make_unique<json_val<bool>>(true);
        }
        else {
            _pos += 5;
            return std::make_unique<json_val<bool>>(false);
        }
    }

    std::unique_ptr<json_val<std::nullptr_t>> parse_null() {
        _pos += 4;
        return std::unique_ptr<json_val<std::nullptr_t>>(nullptr);
    }

    std::unique_ptr<json_val<int>> parse_number() {
        //dodati neku podrsku za float?
        std::string number;
        while (_pos < _expr.size() && std::isdigit(peek_next_char())) 
            number += next_char();

        return std::make_unique<json_val<int>>(stoi(number));
    }
};
