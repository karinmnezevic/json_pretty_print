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
    json_type parse() {
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

    json_type parse_any() {
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

    json_object parse_object() {
        std::vector<std::pair<std::string_view, json_type>> result;
        while (next_char() != '}') {
            skip_spaces();
            std::string_view key = parse_string().val();
            skip_spaces();
            next_char(); // :
            skip_spaces();
            result.push_back(std::make_pair(key, parse_any()));
            skip_spaces();
        }

        return json_object(result);
    }

    json_array parse_array() {
        std::vector<json_type> result;
        while (next_char() != ']') {
            skip_spaces();
            result.push_back(parse_any());
            skip_spaces();
        }
        return json_array(result);
    }

    json_val<std::string> parse_string() {
        size_t beg = _pos;
        next_char();
        while (_pos < _expr.size() && peek_next_char() != '\"') {
            if (peek_next_char() == '\\')
                next_char();
            next_char();
        }
        next_char();
        return json_val<std::string>(_expr.data() + beg, _pos - beg);
    }

    json_val<bool> parse_bool() {
        if (peek_next_char() == 't') {
            _pos += 4;
            return json_val<bool>(_expr.data() + _pos - 4, 4);
        }
        else {
            _pos += 5;
            return json_val<bool>(_expr.data() + _pos - 5, 5);
        }
    }

    json_val<std::nullptr_t> parse_null() {
        _pos += 4;
        return json_val<std::nullptr_t>(_expr.data() + _pos - 4, 4);
    }

    json_val<int> parse_number() {
        size_t beg = _pos;

        while (_pos < _expr.size() && 
                (std::isdigit(peek_next_char()) || peek_next_char() == '-' || peek_next_char() == '+' || 
                        peek_next_char() == '.' || peek_next_char() == 'e' || peek_next_char() == 'E')) 
            next_char();

        return json_val<int>(_expr.data() + beg, _pos - beg);
    }

};

} // namespace pretty_json