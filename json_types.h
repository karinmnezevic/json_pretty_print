#pragma once

#include <string_view>
#include <vector>
#include <memory>
#include <iomanip>
#include <iostream>
#include <tuple>

namespace pretty_json {

inline size_t indentation = 2; // number of spaces used as indent

class json_any {
public:
    virtual size_t len() const = 0;
    virtual bool is_one_liner() const;
    virtual bool is_basic_val() const = 0;
    virtual void print(size_t level, std::ostream& os, size_t val_width = 0, size_t key_width = 0) const = 0;
    virtual ~json_any() = default;
};

using container_t = std::vector<std::unique_ptr<json_any>>;

template <typename T>
class json_val : public json_any {
    std::string_view _content;
public: 
    inline json_val(char* beg, size_t len) : _content(beg, len) {}
    inline void print(size_t level, std::ostream& os, size_t val_width = 0, size_t key_width = 0) const override {
        std::cout << std::right << std::setw(std::max(_content.size(), val_width)) << _content;
    }
    inline size_t len() const override {
        return _content.length();
    }
    inline bool is_basic_val() const override {
        return true;
    }
    inline std::string_view val() const {
        return _content;
    }
};

class json_key_value : public json_any {
    std::string_view _key;
    std::unique_ptr<json_any> _val;
public:
    json_key_value(std::string_view key, std::unique_ptr<json_any> val);
    void print(size_t level, std::ostream& os, size_t val_width = 0, size_t key_width = 0) const override;
    size_t len() const override;
    bool is_basic_val() const override;
    
    std::string_view key() const;
    const std::unique_ptr<json_any>& val() const;
};

class json_container : public json_any {
public:
    container_t _elems;
    char _begin, _end;
public:
    json_container(container_t& elems);
    size_t len() const override;
    bool is_basic_val() const override;
    bool is_one_liner() const override;

    std::pair<size_t, size_t> calc_widths() const;
    std::tuple<bool, std::vector<size_t>, std::vector<size_t>> check_matrix() const;
    const container_t& elems() const;

    void print(size_t level, std::ostream& os, size_t val_width = 0, size_t key_width = 0) const override;
    void print_single_line(size_t level, std::ostream& os, const std::vector<size_t> keys_v = {}, const std::vector<size_t> widths_v = {}) const;
    void print_multiline_matrix(size_t level, std::ostream& os, std::vector<size_t> col_key_widths, std::vector<size_t> col_val_widths) const;
    void print_multiline(size_t level, std::ostream& os, size_t val_width, size_t key_width) const;
};

class json_object : public json_container {
public:
    json_object(container_t& key_vals);
};

class json_array : public json_container {
public:
    json_array(container_t& array);
};

} // namespace pretty_json