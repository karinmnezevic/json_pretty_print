#pragma once

#include <string_view>
#include <vector>
#include <memory>
#include <iomanip>
#include <iostream>


namespace pretty_json {

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
    inline json_val(char* beg, size_t len) : _val(beg, len) {}
    bool is_one_liner() const override {
        return true;
    }
    inline void print(int level, int width = 0) const override {
        if (width == 0)
            width = _val.size();
        std::cout << std::right << std::setw(width) << _val;
    }
    inline std::string_view get() const {
        return _val;
    }
    inline size_t len() const override {
        return _val.length();
    }
    inline bool all_children_are_basic_vals() const override {
        return true;
    }
    inline size_t longest_child() const override{
        return 0;
    }
};

using key_vals_t = std::vector<std::pair<std::string_view, std::unique_ptr<json_any>>>;
class json_object : public json_any {
    key_vals_t _key_vals;
public:
    json_object(key_vals_t& key_vals);
    size_t len() const override;
    bool is_one_liner() const override;
    size_t longest_child() const override;
    bool all_children_are_basic_vals() const override;
    void print(int level, int width) const override;
    bool is_matrix() const; //{[], [], [], [], []}
    size_t matrix_cell_width() const;
    size_t longest_key() const;
};

using array_t = std::vector<std::unique_ptr<json_any>>;
class json_array : public json_any {
    array_t _array;
public:
    json_array(array_t& array);
    size_t len() const override;
    bool is_one_liner() const override;
    size_t longest_child() const override;
    bool all_children_are_basic_vals() const override;
    void print(int level, int width) const override;
    bool is_matrix() const; //[[], [], [], [], []]
    size_t matrix_cell_width() const;
};

} // namespace pretty_json