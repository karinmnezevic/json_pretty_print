#pragma once

#include <string_view>
#include <vector>
#include <memory>
#include <iomanip>
#include <iostream>
#include <tuple>

namespace pretty_json {

inline int indentation = 2; // number of spaces used as indent

class json_any {
public:
    virtual size_t len() const = 0;
    virtual bool is_basic_val() const = 0;
    virtual const std::vector<std::unique_ptr<json_any>>& children() const = 0;
    virtual ~json_any() = default;
    virtual void print(int level, int val_width = 0, int key_width = 0) const = 0;

    bool all_children_are_basic_vals() const;
    bool is_one_liner() const;
    virtual void print_as_row(int level, const std::vector<size_t> keys_v = {}, const std::vector<size_t> widths_v = {}) const {} //for containers
};

using container_t = std::vector<std::unique_ptr<json_any>>;
inline container_t empty_children;

template <typename T>
class json_val : public json_any {
    std::string_view _val;
public: 
    inline json_val(char* beg, size_t len) : _val(beg, len) {}
    inline void print(int level, int val_width = 0, int key_width = 0) const override {
        if (val_width == 0)
            val_width = _val.size();
        std::cout << std::right << std::setw(val_width) << _val;
    }
    inline size_t len() const override {
        return _val.length();
    }
    inline bool is_basic_val() const override {
        return true;
    }
    inline std::string_view get() const {
        return _val;
    }
private:
    const container_t& children() const override {
        return empty_children;
    }
    bool is_one_liner() const {
        return true;
    }
};

class json_key_value : public json_any {
    std::string_view _key;
    std::unique_ptr<json_any> _val;
public:
    json_key_value(std::string_view key, std::unique_ptr<json_any> val) : _key(key), _val(std::move(val)) {}
    void print(int level, int val_width = 0, int key_width = 0) const override;
    void print_as_row(int level, const std::vector<size_t> keys_v = {}, const std::vector<size_t> widths_v = {}) const override;
    size_t len() const override;
    bool is_basic_val() const override;
    size_t key_len() const;
    size_t val_len() const;
private:
    const container_t& children() const override;
};

class json_container : public json_any {
public:
    container_t _elems;
public:
    json_container(container_t& elems);
    size_t len() const override;
    const container_t& children() const override;
    bool is_basic_val() const override;
    size_t longest_child() const;
    std::tuple<bool, std::vector<size_t>, std::vector<size_t>> check_matrix() const;
};

class json_object : public json_container {
public:
    json_object(container_t& key_vals);
    void print(int level, int val_width = 0, int key_width = 0) const override;
    void print_as_row(int level, const std::vector<size_t> keys_v = {}, const std::vector<size_t> widths_v = {}) const override;
    size_t longest_key() const;
};

class json_array : public json_container {
public:
    json_array(container_t& array);
    void print(int level, int val_width = 0, int key_width = 0) const override;
    void print_as_row(int level, const std::vector<size_t> keys_v = {}, const std::vector<size_t> widths_v = {}) const override;
};

} // namespace pretty_json