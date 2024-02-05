#pragma once

#include <string>
#include <memory>
#include "json_types.h"

namespace pretty_json {

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
        container_t result;
        while (next_char() != '}') {
            skip_spaces();
            result.push_back(parse_key_val());
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

    std::unique_ptr<json_key_value> parse_key_val() {
        std::string_view key = parse_string()->val();
        skip_spaces();
        next_char(); // :
        skip_spaces();
        std::unique_ptr<json_any> val = parse_any();
        return std::make_unique<json_key_value>(key, std::move(val));
    }

    std::unique_ptr<json_val<std::string>> parse_string() {
        size_t beg = _pos;
        next_char();
        while (_pos < _expr.size() && peek_next_char() != '\"') {
            if (peek_next_char() == '\\')
                next_char();
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

} // namespace json_pretty