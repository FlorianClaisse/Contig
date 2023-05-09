//
// Created by Florian Claisse on 02/05/2023.
//

#include <filesystem>
#include <charconv>

#include "include/program_option.h"
#include "../FindAll/find_all.h"
#include "../CodonCount/condo_count.h"

using namespace std;
namespace fs = std::filesystem;

// ./Contig [program_name] ...
int program_option::parse(int argc, char **argv) {
    vector<string_view> args(argv, argv + argc);
    if (args.size() < 2) {
        cout << "La ligne de commande doit au moins contenir le nom du program à éxécuter." << endl;
        return usage();
    }

    vector<string_view> sub_args(argv + 2, argv + argc);
    if (args[1] == FINDALL) {
        return parse_find_all(sub_args);
    } else if (args[1] == CODONCOUNT) {
        return parse_codon_count(sub_args);
    }

    return usage();
}

int program_option::usage() {
    cout << "Usage :" << endl
    << "./Contig [program_name] ..." << endl
    << "\t" << FINDALL << "\tProgramme qui permet à partir d'un fichier A (type nucl ou prot), de trouver si il sont présent dans tous les fichiers du dossier B."
    << "\t" << CODONCOUNT << "\tProgramme qui permet à partir d'un fichier d'entrée A de compter le nombre de chaque codon pour chaque contig."
    << endl;

    return EXIT_SUCCESS;
}

// --inputA <path> --inputB <path> --type <nucl/prot> --output <path> [--accept <percentage>]
int program_option::parse_find_all(const vector<string_view> &argv) {
    if (argv.size() < 8 || (argv.size() % 2) != 0) return find_all_usage();
    if (argv[0] != INPUTA || argv[2] != INPUTB || argv[4] != TYPE || argv[6] != OUTPUT) return find_all_usage();
    ::printf("%d\n", __LINE__);
    int acceptValue(100);
    if (argv.size() == 10 && argv[8] == ACCEPT) {
        auto result = from_chars(argv[9].data(), argv[9].data() + argv[9].size(), acceptValue);
        if (result.ec == errc::invalid_argument) return find_all_usage();
    }

    if (argv.size() > 10) return find_all_usage();

    if (!fs::exists(argv[1])) {
        cout << "Le fichier d'entrée A n'existe pas ou n'est pas accessible." << endl;
        return EXIT_FAILURE;
    }

    if (!fs::exists(argv[3])) {
        cout << "Le dossier d'entrée B n'exsite pas ou n'est pas accessible." << endl;
        return EXIT_FAILURE;
    }

    if (string(argv[5]) != NUCLEIC && string(argv[5]) != PROTEIN) {
        cout << "Le type de fichier n'est pas valide." << endl;
        return EXIT_FAILURE;
    }

    if (!fs::exists(argv[7])) {
        cout << "Le dossier de sortie n'existe pas ou n'est pas accessible." << endl;
        return EXIT_FAILURE;
    }

    FindAll options = {string(argv[1]), string(argv[3]), string(argv[7]), acceptValue, string(argv[5]) == NUCLEIC};
    return find_all::start(options);
}


int program_option::find_all_usage() {
    cout << "Find All" << endl
    << "Usage :" << endl
    << "./Contig " << FINDALL << " " << INPUTA << " <path> " << INPUTB << " <path> " << TYPE << " nucl/prot " << OUTPUT << " <path> " << "[" << ACCEPT << " <percentage>" << "]\n\n"
    << "\t" << INPUTA << "\tChemin vers le fichiers qui contient les contigs à trouver.\n\n"
    << "\t" << INPUTB << "\tChemin vers le dossier qui contient les fichiers ou il faut trouver les contigs.\n\n"
    << "\t" << TYPE << "\t\tLe type de fichier (nucl/prot).\n\n"
    << "\t" << OUTPUT << "\tChemin vers le dossier qui va contenir le(s) fichier(s) de sortie.\n\n"
    << "\t" << ACCEPT << "\tPermet de spécifier le pourcentage minimum pour accepter un contig comme reconnu.\n\n";
    return EXIT_SUCCESS;
}

// --inputA <path> --output <path>
int program_option::parse_codon_count(const vector<string_view> &argv) {
    if (argv.size() != 4) return codon_count_usage();

    if(argv[0] != INPUTA || argv[2] != OUTPUT) return codon_count_usage();

    if (!fs::exists(argv[1])) {
        cout << "Le fichier d'entrée A n'existe pas ou n'est pas accessible." << endl;
        return EXIT_FAILURE;
    }
    if (!fs::exists(argv[3])) {
        cout << "Le dossier d'entrée B n'exsite pas ou n'est pas accessible." << endl;
        return EXIT_FAILURE;
    }

    CodonCount options = {string(argv[1]), string(argv[3])};
    return codon_count::start(options);
}

int program_option::codon_count_usage() {
    return EXIT_SUCCESS;
}

