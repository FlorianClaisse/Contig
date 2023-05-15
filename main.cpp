#include <iostream>
#include <vector>
#include <string>

#include "FindAll/include/parser.h"

#define FINDALL "--findall"
#define CODONCOUNT "--codoncount"
#define CONTIGDIFF "--contigdiff"

using namespace std;

void usage();

int main(int argc, char* argv[]) {
    if (argc > 1) {
        vector<string_view> args(argv + 2, argv + argc);
        if (::strcmp(argv[1], "--help") == 0 || ::strcmp(argv[1], "-h") == 0) usage();
        else if (::strcmp(argv[1], FINDALL) == 0) return find_all::parse(args);
        else if (::strcmp(argv[1], CODONCOUNT) == 0);
        else if (::strcmp(argv[1], CONTIGDIFF) == 0);
        else usage();
    } else usage();

    return EXIT_SUCCESS;
}

void usage() {
    cout << "Usage :\n"
    << "./Contig <program_name> ...\n"
    << "\t" << FINDALL << "\tProgramme qui determine quelle contigs d'un fichier est présent dans l'ensemble des fichier d'un dossier.\n"
    << "\t" << CODONCOUNT << "\tProgramme qui compte de le nombre de chaque codon pour une multitude de ficheir.\n"
    << "\t" << CONTIGDIFF << "\tPragramme qui à partir d'un dossier A et B de trouver tous les communs de A qui ne sont pas dans B.\n";
}
