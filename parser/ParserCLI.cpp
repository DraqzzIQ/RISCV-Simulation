#include <bitset>
#include <fstream>
#include <iostream>

#include "Parser.h"

using std::cout;

int main(const int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <in> [out]" << std::endl;
        return 1;
    }

    const string in = argv[1];
    string out;

    if (argc >= 3) {
        out = argv[2];
    }

    std::ifstream inputFile(in);
    if (!inputFile) {
        std::cerr << "Error opening file: " << in << std::endl;
        return 1;
    }

    vector<std::string> lines;
    string line;
    while (std::getline(inputFile, line)) {
        lines.push_back(line);
    }
    inputFile.close();

    ParsingResult result = Parser::Parse(lines);
    if (!result.success) {
        cout << "Line " << result.errorLine << ": Error while parsing instruction: " << lines[result.errorLine]
             << std::endl;
        cout << "Error: " << result.errorType << std::endl;
        return 1;
    }

    if (out.empty()) {
        for (const auto& instruction : result.instructions) {
            cout << std::bitset<32>(instruction) << std::endl;
        }
    }
    else {
        std::ofstream outputFile(out);
        if (!outputFile) {
            std::cerr << "Error opening file: " << out << std::endl;
            return 1;
        }

        for (const auto& instruction : result.instructions) {
            outputFile << std::bitset<32>(instruction) << std::endl;
        }
        outputFile.close();
    }

    return 0;
}
