//
// Created by Florian Claisse on 04/05/2023.
//

#include <map>
#include <iostream>
#include <fstream>

#include "../Foundation/include/fasta.h"
#include "../Foundation/include/directory.h"
#include "condo_count.h"

using namespace std;
namespace fs = std::filesystem;

void init_codon(map<string, int> &codon) {
    codon["TTT"] = 0; codon["TCT"] = 0; codon["TAT"] = 0; codon["TGT"] = 0;
    codon["TTC"] = 0; codon["TCC"] = 0; codon["TAC"] = 0; codon["TGC"] = 0;
    codon["TTA"] = 0; codon["TCA"] = 0; codon["TAA"] = 0; codon["TGA"] = 0;
    codon["TTG"] = 0; codon["TCG"] = 0; codon["TAG"] = 0; codon["TGG"] = 0;

    codon["CTT"] = 0; codon["CCT"] = 0; codon["CAT"] = 0; codon["CGT"] = 0;
    codon["CTC"] = 0; codon["CCC"] = 0; codon["CAC"] = 0; codon["CGC"] = 0;
    codon["CTA"] = 0; codon["CCA"] = 0; codon["CAA"] = 0; codon["CGA"] = 0;
    codon["CTG"] = 0; codon["CCG"] = 0; codon["CAG"] = 0; codon["CGG"] = 0;

    codon["ATT"] = 0; codon["ACT"] = 0; codon["AAT"] = 0; codon["AGT"] = 0;
    codon["ATC"] = 0; codon["ACC"] = 0; codon["AAC"] = 0; codon["AGC"] = 0;
    codon["ATA"] = 0; codon["ACA"] = 0; codon["AAA"] = 0; codon["AGA"] = 0;
    codon["ATG"] = 0; codon["ACG"] = 0; codon["AAG"] = 0; codon["AGG"] = 0;

    codon["GTT"] = 0; codon["GCT"] = 0; codon["GAT"] = 0; codon["GGT"] = 0;
    codon["GTC"] = 0; codon["GCC"] = 0; codon["GAC"] = 0; codon["GGC"] = 0;
    codon["GTA"] = 0; codon["GCA"] = 0; codon["GAA"] = 0; codon["GGA"] = 0;
    codon["GTG"] = 0; codon["GCG"] = 0; codon["GAG"] = 0; codon["GGG"] = 0;
}

int codon_count::start(program_option::CodonCount &options) {

    map<string, int> codon, total_codon;
    init_codon(codon);
    init_codon(total_codon);

    if (!is_directory(options.inputA)) {
        cout << "Path : " << options.inputA << "n'est pas un dossier" << endl;
        return EXIT_FAILURE;
    }

    if (fasta::directory_to_fasta_line(options.inputA) == EXIT_FAILURE) return EXIT_FAILURE;

    unsigned long all_total(0);
    for (const auto &currentFile: fs::directory_iterator(options.inputA)) {
        if (!directory::is_fastaline_file(currentFile)) continue;

        ifstream inputFile(currentFile.path());

        ofstream outputFile(options.output.string().append("/" + currentFile.path().stem().string() + ".txt"), ios::trunc);
        outputFile << "Contig Name\tCodon\tNumber\tPercentage\n";

        string lineRead, prot_name;
        unsigned long total(0);
        while (getline(inputFile, lineRead)) {
            if (lineRead.at(0) == '>') prot_name = lineRead;
            else {
                for (unsigned long i = 0; i < lineRead.size(); i += 3) {
                    string sub = lineRead.substr(i, 3);
                    if (codon.find(sub) != codon.end()) {
                        codon[sub]++;
                        total_codon[sub]++;
                        total++;
                        all_total++;
                    } else {
                        cout << "Contig : " << prot_name << ", codon : " << sub << " unknown\n";
                    }
                }

                for (const auto &key: codon) {
                    if (key.second == 0) continue;
                    outputFile << prot_name << "\t" << key.first << "\t" << key.second << "\t"
                               << (((double) key.second / (double) total) * 100) << "%\n";
                }
                total = 0;
                init_codon(codon);
            }
        }
    }

    ofstream total_output(options.output.string().append("/total_output.txt"), ios::trunc);
    total_output << "Codon\tNumber\tPercentage\n";

	for (const auto &key : total_codon) {
		if (key.second == 0) continue;
		total_output << key.first << "\t" << key.second << "\t" << (((double)key.second / (double) all_total) * 100) << "%\n";
	}

    return EXIT_SUCCESS;
}
