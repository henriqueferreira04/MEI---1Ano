
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <cmath>

#include "lz4.h"

#define BLOCK_MiB (512 * 1024 * 1024)



struct CityData {
    std::string name;
    double avg;
    double std;
    double min;
    double max;
};


struct Measurement {
    double value;
    int count;
};

int main(int argc, char* argv[])
{
    // Use default file ...
    const char* file = "measurements-1.cle";
    if (argc > 1){
        // ... or the first argument.
        file = argv[1];
    }
    std::ifstream fh(file, std::ios::binary);
    if (not fh.is_open()){
        return 1;
    }
    // Get the number of blocks
    int nblocks;
    fh.read((char*)&nblocks, sizeof(int));
    if (fh.gcount() != sizeof(int) || nblocks <= 0) {
        return 1;
    }

    std::map<std::string, std::vector<double>> measurements;

    // process block by block
    for (int i = 0; i < nblocks; ++i){
        int compressed_size;
        int block_size;
        fh.read((char*)&compressed_size, sizeof(int));
        fh.read((char*)&block_size, sizeof(int));
        if (compressed_size <= 0 || block_size <= 0 || block_size > BLOCK_MiB) {
            return 1;
        }
        std::vector<char> compressed(compressed_size);
        fh.read(compressed.data(), compressed_size);
        if (fh.gcount() != compressed_size) {
            return 1;
        }
        std::vector<char> decompressed(block_size);
        int decoded = LZ4_decompress_safe(
            compressed.data(),
            decompressed.data(),
            compressed_size,
            block_size
        );  
        if (decoded < 0) {
                return 1;
        }
        if (decoded != block_size) {
            return 1;
        }


        std::stringstream ss(std::string(decompressed.data(), block_size));
        std::string line;

        while (std::getline(ss, line)) {
            size_t semicolon_pos = line.find(';');
            if (semicolon_pos != std::string::npos) {
                std::string station = line.substr(0, semicolon_pos);
                double temperature = std::stod(line.substr(semicolon_pos + 1));
                
                measurements[station].push_back(temperature);
            }
        }

    }
    
    std::vector<CityData> city_stats;
    for (const auto& entry : measurements) {
        const std::string& city = entry.first;
        const std::vector<double>& temps = entry.second;

        double sum = 0.0;
        double sum_sq = 0.0;
        double min_temp = temps[0];
        double max_temp = temps[0];

        for (double temp : temps) {
            sum += temp;
            sum_sq += temp * temp;
            if (temp < min_temp) min_temp = temp;
            if (temp > max_temp) max_temp = temp;
        }

        double avg = sum / temps.size();
        double stddev = std::sqrt(sum_sq / temps.size() - avg * avg);

        city_stats.push_back({city, avg, stddev, min_temp, max_temp});
    }

    std::ofstream out("stats.json");
    out << std::fixed << std::setprecision(1);
    out << "{\n  \"cities\": [\n";
    for (size_t i = 0; i < city_stats.size(); ++i) {
        const CityData& s = city_stats[i];
        out << "    {\n";
        out << "      \"name\": \"" << s.name << "\",\n";
        out << "      \"avg\": " << s.avg << ",\n";
        out << "      \"std\": " << s.std << ",\n";
        out << "      \"min\": " << s.min << ",\n";
        out << "      \"max\": " << s.max << "\n";
        out << "    }";
        if (i + 1 < city_stats.size()) {
            out << ",";
        }
        out << "\n";
    }
    out << "  ]\n}\n";
    out.close();
    // Always close the file when done
    fh.close();

    return 0;
}

