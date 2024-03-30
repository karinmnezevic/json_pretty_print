#include "json_parser.h"
#include <filesystem>
#include <fstream>
#include <string>

bool identical_content(std::filesystem::path file1, std::filesystem::path file2) {
    std::ifstream stream1(file1);
    std::ifstream stream2(file2);

    if (!stream1.is_open() || !stream2.is_open()) {
        std::cerr << "Error opening files." << std::endl;
        return false;
    }

    std::string content1((std::istreambuf_iterator<char>(stream1)), std::istreambuf_iterator<char>());
    std::string content2((std::istreambuf_iterator<char>(stream2)), std::istreambuf_iterator<char>());
    return content1 == content2;
}

bool run_test(std::string base) {
    std::ifstream ifile(base + "_in.txt");
    std::ofstream ofile(base + "_generated.txt");
    if (ifile.is_open()) {
        std::string test_in((std::istreambuf_iterator<char>(ifile)), std::istreambuf_iterator<char>());
        pretty_json::json_parser parser(test_in);
        std::unique_ptr<pretty_json::json_any> parsed_json = parser.parse();
        parsed_json->print(1, ofile);
    }
    ifile.close();
    ofile << std::endl;

    return identical_content(base + "_generated.txt", base + "_expected.txt");
}

std::vector<std::string> test_names = {
    "test_1", "test_2", "test_3", "test_4", "test_5", "test_6", "test_7", "test_8", "test_9"
};

int main() {

    for (auto& base_name : test_names) {
        if (!run_test("tests/" + base_name)) {
            std::cout << base_name << " FAILED" << std::endl;
            return -1;
        }
        else 
            std::cout << base_name << " PASSED" << std::endl;
    }

    return 0; 
}