//
// Created by Florian Claisse on 02/05/2023.
//

#include <fstream>
#include <filesystem>
#include <omp.h>
#include "../Foundation/include/fasta.h"
#include "../Foundation/include/directory.h"
#include "find_all.h"

using namespace std;
namespace fs = std::filesystem;

int check_options(const program_option::FindAll &options) {
    if (!fasta::is_fasta_file(options.inputA)) {
        cout << "Le fichier : " << options.inputA <<  "n'est pas un fichier fasta." << endl;
        return EXIT_FAILURE;
    }

    if (!fs::is_directory(options.inputB)) {
        cout << "Path : " << options.inputB << "n'est pas un dossier" << endl;
        return EXIT_FAILURE;
    }

    if (!fs::is_directory(options.output)) {
        cout << "Path : " << options.output << "n'est pas un dossier" << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int convert_all_to_fastaline(const program_option::FindAll &options) {
    if (fasta::to_fasta_line(options.inputA) != EXIT_SUCCESS) return EXIT_FAILURE;

    // Convert directory fasta to fastaline
    for (const auto &currentFile : fs::directory_iterator(options.inputB)) {
        if (!fasta::is_fasta_file(currentFile)) continue;
        if (fasta::is_result_file(currentFile)) continue;
        if (fasta::to_fasta_line(currentFile) != EXIT_SUCCESS) return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// Format : >ContigNameB -> ContigNameA -> Percentage
int generate_output(const program_option::FindAll &options) {
    ofstream outputFile(options.output.string().append("/output.txt"), ios::trunc);
    outputFile << "Filename\n";
    string lineRead;
    for (const auto &currentPath : fs::directory_iterator(options.output)) {
        if (!fasta::is_result_file(currentPath)) continue;
        outputFile << currentPath.path().filename() << "\t";
        ifstream currentFile(currentPath);
        while(getline(currentFile, lineRead)) {
            if (lineRead.empty()) continue;
            if (lineRead.at(0) == '>') {
                size_t pos, second_pos;
                if ((pos = lineRead.find("->")) != string::npos) {
                    second_pos = lineRead.find_last_of('>');
                    cout << "Line : " << lineRead << ", pos : " << pos << ", last : " << second_pos;
                    if (second_pos == (pos + 3)) outputFile << lineRead.substr(pos + 3);
                    else outputFile << lineRead.substr(pos + 3, second_pos);
                } else return EXIT_FAILURE;
            }
        }

        outputFile << "\n";
    }

    outputFile.close();

    return EXIT_SUCCESS;
}

int find_all::start(const program_option::FindAll &options) {
    if (check_options(options) != EXIT_SUCCESS) return EXIT_FAILURE;

    if (convert_all_to_fastaline(options) != EXIT_SUCCESS) return EXIT_FAILURE;

    // Stocker les contigs du fichier de test dans un tableau.
    ifstream inputFile(directory::removeExtension(options.inputA).append(".fastaline"));

    string name, value;
    map<string, string> contigs;
    while(!inputFile.eof()) {
        getline(inputFile, name);
        getline(inputFile, value);
        contigs[name] = value;
    }
    inputFile.close();

    for (const auto &contig_value: contigs) {
        cout << "Value : " << contig_value.first << ", name : " << contig_value.second << endl << endl;
    }

    long nb_files = distance(fs::directory_iterator(options.inputB), fs::directory_iterator{}); // C++17
    cout << "Nb files : " << nb_files << endl;

#pragma omp parallel for default(none) shared(nb_files, contigs, options)
    for (long i = 0; i < nb_files; i++) {
        fs::directory_entry file = *next(fs::directory_iterator(options.inputB), i);
        if (!fasta::is_fastaline_file(file)) continue;
        if (fasta::is_result_file(file)) continue;

        printf("File : %s, Threads = %d, i = %ld\n", file.path().c_str(), omp_get_thread_num(), i);

        string fileNameWithoutExtension(directory::fileNameWithoutExtension(file.path()));
        ofstream currentOutputResult(options.output.string().append("/" + fileNameWithoutExtension + "-result.fasta"), ios::trunc);

        if (options.accept == 100) {
            fasta::find_contig(file.path(), contigs, options.nucl, [&currentOutputResult](const string &nameA, const string &nameB, const string &value) -> void {
                currentOutputResult << nameB << " -> " << nameA << endl << value << endl;
            });
        }
        else {
            fasta::find_contigs(file.path(), contigs, (100 - options.accept), options.nucl, [&currentOutputResult](const string &nameA, const string &nameB, const string &value, double percentage) -> void {
                currentOutputResult << nameB << " -> " << nameA << " -> " << (100.0 - percentage) << "%" << endl << value << endl;
            });
        }
        currentOutputResult.close();
    }

    return generate_output(options);
}