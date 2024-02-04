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

bool test(std::string base) {
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

int main() {

    if (!test("tests/test_1"))
        return -1;
    
    if (!test("tests/test_2"))
        return -1;
    
    if (!test("tests/test_3"))
        return -1;
    
    if (!test("tests/test_4"))
        return -1;

    return 0; 
}