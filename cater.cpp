#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <regex>
#include <vector>
#include <chrono>
#include <thread>

namespace fs = std::filesystem;

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string detect_language(const std::string& source) {
    if (source.find("fun main") != std::string::npos ||
        source.find("fun ") != std::string::npos ||
        source.find("val ") != std::string::npos ||
        source.find("var ") != std::string::npos) {
        return "kotlin";
    }
    if (source.find("object ") != std::string::npos ||
        source.find("def ") != std::string::npos ||
        source.find("println") != std::string::npos && source.find("main") != std::string::npos) {
        return "scala";
    }
    if (source.find("class ") != std::string::npos ||
        source.find("public static void main") != std::string::npos) {
        return "java";
    }
    return "java";
}

int compile_and_run(const std::string& filename, bool gen_dex) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return 1;
    }
    std::string source((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    std::string lang = detect_language(source);
    std::string base = fs::path(filename).stem().string();
    std::string cmd;

    if (lang == "kotlin") {
        cmd = "kotlinc -include-runtime -d " + base + ".jar " + filename;
        if (system(cmd.c_str()) != 0) return 1;
        if (gen_dex) {
            cmd = "d8 --lib android.jar --output . " + base + ".jar";
            if (system(cmd.c_str()) != 0) return 1;
            std::cout << "Generated classes.dex (rename to " << base << ".dex)" << std::endl;
        } else {
            cmd = "java -jar " + base + ".jar";
            return system(cmd.c_str());
        }
    }
    else if (lang == "scala") {
        cmd = "scalac " + filename;
        if (system(cmd.c_str()) != 0) return 1;
        if (gen_dex) {
            std::cerr << "DEX generation for Scala requires manual class packaging" << std::endl;
            return 1;
        } else {
            cmd = "scala " + base;
            return system(cmd.c_str());
        }
    }
    else { // java
        cmd = "javac " + filename;
        if (system(cmd.c_str()) != 0) return 1;
        if (gen_dex) {
            cmd = "jar cfe " + base + ".jar " + base + " " + base + ".class";
            if (system(cmd.c_str()) != 0) return 1;
            cmd = "d8 --lib android.jar --output . " + base + ".jar";
            if (system(cmd.c_str()) != 0) return 1;
            std::cout << "Generated classes.dex (rename to " << base << ".dex)" << std::endl;
        } else {
            cmd = "java " + base;
            return system(cmd.c_str());
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file.jsh> [--dex]" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
    bool gen_dex = (argc >= 3 && std::string(argv[2]) == "--dex");
    return compile_and_run(filename, gen_dex);
}
