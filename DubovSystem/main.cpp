//
//  main.cpp
//  DubovSystem
//
//  Created by Michael Shapiro on 5/11/24.
//

// Copyright 2024 Michael Shapiro
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <iostream>
#include "trf util/trf.hpp"
#include "Tournament.hpp"
#include "trf util/rtg.hpp"
#include "fpc.hpp"
#include "csv util/csv.hpp"

/**
 * Help info
 */
void helpDisplay() {
    std::cout << "--------FIDE DUBOV SYSTEM PAIRING ENGINE--------" << std::endl;
    std::cout << "--------------------Version 2.1.2---------------" << std::endl;
    std::cout << "---------------Author: Michael Shapiro----------" << std::endl;
    std::cout << "-------------------Help Section-----------------" << std::endl;
    std::cout << "\n\n\n\n";
    std::cout << "|Argument     |Description                  |" << std::endl;
    std::cout << "|-------------|-----------------------------|" << std::endl;
    std::cout << "|--help       |Help Page                    |" << std::endl;
    std::cout << "|--pairings   |Do next round from given file|" << std::endl;
    std::cout << "|--sample     |Example Usage                |" << std::endl;
    std::cout << "|--version    |Output Version               |" << std::endl;
    std::cout << "|--rtg        |Random tournament generator  |" << std::endl;
    std::cout << "|--p_count    |(for rtg) player count       |" << std::endl;
    std::cout << "|--rtg_rounds |(for rtg) total rounds       |" << std::endl;
    std::cout << "|--fpc        |Free Pairings Checker        |" << std::endl;
    std::cout << "|--fpc_rounds |(for fpc) Round to check     |" << std::endl;
    std::cout << "|--output     |Output pairings to a file    |" << std::endl;
}

/**
 * Sample usage
 */
void exampleUsage() {
    std::cout << "EXAMPLE USAGE" << std::endl;
    std::cout << "./CPPDubovSystem --pairings \"path/to/file.trf\"" << std::endl;
    std::cout << "OR" << std::endl;
    std::cout << "./CPPDubovSystem --pairings \"path/to/file.trf\" --output \"path/to/output.csv\"" << std::endl;
    std::cout << "FILE MUST BE IN TRF16 FORMAT!\n\n";
    std::cout << "EXAMPLE USAGE FOR RANDOM TOURNAMENT GENERATOR\n";
    std::cout << "./CPPDubovSystem --rtg path/to/trf/output.trf --p_count 10 --rtg_rounds 5\n\n";
    std::cout << "EXAMPLE USAGE FOR FREE PAIRINGS CHECKER\n";
    std::cout << "./CPPDubovSystem --fpc path/to/trf/output.trf --fpc_rounds 2" << std::endl;
}

/**
 * Gets the largest player name (in length) for white and black and returns the length for both
 */
std::pair<int, int> getLargestPlayerName(const std::vector<CPPDubovSystem::Match> &pairings) {
    int white_len = 0;
    int black_len = 0;
    
    // run through pairings and take the larger of the two lengths
    for(auto i : pairings) {
        white_len = std::max(white_len, (int) i.white.getName().size());
        if(i.is_bye) {
            black_len = std::max(black_len, 3);
        } else {
            black_len = std::max(black_len, (int) i.black.getName().size());
        }
    }
    
    return std::make_pair(white_len, black_len);
}

/**
 * Outputs the pairings generated
 */
void outputPairings(const std::vector<CPPDubovSystem::Match> &pairings, int for_round) {
    std::cout << "---------PAIRINGS FOR ROUND " << for_round << "---------" << std::endl;
    // take the larger of the lengths for pairings so our output looks more reliable
    std::pair<int, int> name_lengths = getLargestPlayerName(pairings);
    std::string first_line = "|White";
    int white_loop = std::max(5, name_lengths.first);
    for(int i = 5; i < white_loop; i++)
        first_line += " ";
    first_line += "|Black";
    int black_loop = std::max(5, name_lengths.second);
    for(int i = 5; i < black_loop; i++)
        first_line += " ";
    first_line += "|";
    std::cout << first_line << std::endl;
    // also configure second line
    std::string second_line = "|";
    for(int i = 1; i <= name_lengths.first; i++)
        second_line += "-";
    second_line += "|";
    for(int i = 1; i <= name_lengths.second; i++)
        second_line += "-";
    second_line += "|";
    std::cout << second_line << std::endl;
    for(int i = 0; i < pairings.size(); i++) {
        std::cout << "|" << pairings[i].white.getName() << std::string(name_lengths.first - ((int) pairings[i].white.getName().size()), ' ') << "|";
        if(pairings[i].is_bye) {
            std::cout << "BYE" << std::string(name_lengths.second - 3, ' ') << "|" << std::endl;
            continue;
        }
        std::cout << pairings[i].black.getName() << std::string(name_lengths.second - ((int) pairings[i].black.getName().size()), ' ') << "|" << std::endl;
    }
}

/**
 * Outputs pairings to a file
 */
void outputPairingsToFile(const std::vector<CPPDubovSystem::Match> &pairings, const std::string &path) {
    // make csv data table
    PairingTable pt;
    
    // add rows
    for(int i = 0; i < pairings.size(); i++) {
        if(pairings[i].is_bye) {
            pt.addRow(std::to_string(pairings[i].white.getID()), "-1");
            continue;
        }
        pt.addRow(std::to_string(pairings[i].white.getID()), std::to_string(pairings[i].black.getID()));
    }
    
    // output to file
    pt.outputToFile(path);
}

int main(int argc, const char * argv[]) {
    if(argc == 1) {
        helpDisplay();
        return 0;
    } /*else if(argc > 3) {
        std::cout << "Too many arguments given. Run --help for a list of available commands" << std::endl;
        return 0;
    }*/
    if(argc == 2) {
        std::string passed = argv[1];
        if(passed == "--help") {
            helpDisplay();
            return 0;
        } else if(passed == "--sample") {
            exampleUsage();
            return 0;
        } else if(passed == "--pairings") {
            std::cerr << "Missing TRF file path" << std::endl;
            return -1;
        } else if(passed == "--version") {
            std::cout << "CPPDubovSystem -- Version 2.1.2" << std::endl;
            return 0;
        }
        std::cerr << "Unexpected command passed in. Execute --help command or --sample for sample usage" << std::endl;
        return 0;
    }
    // argc = 3
    std::string pair_command = argv[1];
    std::string path = argv[2];
    if(pair_command == "--rtg") {
        // expect p-count and rounds
        if(argc < 7) {
            std::cerr << "Too few arguments passed for random tournament generator. Run --sample for example usage" << std::endl;
            return 0;
        } else if(argc > 7) {
            std::cerr << "Too many arguments passed for random tournament generator. Run --sample for example usage" << std::endl;
            return 0;
        }
        std::string p1 = argv[3];
        std::string p11 = argv[4];
        std::string p2 = argv[5];
        std::string p3 = argv[6];
        if(p1 != "--p_count") {
            std::cerr << "Expected --p_count in call" << std::endl;
            return 0;
        }
        if(p2 != "--rtg_rounds") {
            std::cerr << "Expected --rtg_rounds in call" << std::endl;
            return 0;
        }
        int pc = std::stoi(p11);
        int r = std::stoi(p3);
        std::string trf_output = CPPDubovSystem::Tournament::simulateTournament(pc, r);
        TRFUtil::TRFFile file_write(path);
        file_write.write(trf_output);
        return 0;
    } else if(pair_command == "--fpc") {
        // expect fpc_rounds
        if(argc != 5) {
            std::cerr << "Invalid arguments passed for free pairings checker. Run --sample for example usage" << std::endl;
            return 0;
        }
        std::string p1 = argv[3];
        std::string p2 = argv[4];
        if(p1 != "--fpc_rounds") {
            std::cerr << "Missing --fpc_rounds in call" << std::endl;
            return 0;
        }
        int pc = std::stoi(p2);
        fpc::PairingsChecker pc_main(path, pc);
        std::cout << "Initiating Pairings Checker.\n\n";
        std::cout << pc_main.outputReport();
        return 0;
    }
    if(pair_command != "--pairings") {
        std::cout << "Unknown command passed in" << std::endl;
        return 0;
    }
    TRFUtil::TRFFile file(path);
    TRFUtil::TRFData file_read = file.read();
    
    // make sure rounds exist
    if(!file_read.tnrCodeExists()) {
        std::cerr << "Missing tournament number of rounds in TRF file" << std::endl;
        return 2;
    }
    
    int rounds_done = 0;
    
    // get tournament
    CPPDubovSystem::Tournament trfTournament = CPPDubovSystem::Tournament::makeTournament(file_read, &rounds_done);
    
    // now determine if the tournament is already complete
    // the tournament is complete when all the rounds have been played
    if(rounds_done >= file_read.getRoundsTnr()) {
        std::cerr << "The tournament is already complete!" << std::endl;
        return 3;
    }
    
    std::vector<CPPDubovSystem::Match> m;
    
    // check if acceleration was invoked
    if(file_read.isAccelerationOn()) {
        // do next round pairings with acceleration
        m = trfTournament.generatePairings(rounds_done + 1, true);
    } else {
        // do next round pairings without acceleration
        m = trfTournament.generatePairings(rounds_done + 1);
    }
    
    
    // check for errors as needed
    if(trfTournament.pairingErrorOccured()) {
        std::cout << "Pairings cannot be generated!" << std::endl;
        return -1;
    }
    
    outputPairings(m, rounds_done + 1);
    
    // check if output to file is present
    if(argc == 5) {
        std::string ot = argv[3];
        if(ot != "--output") {
            std::cerr << "Expected --output for pairings file output path. Pairings were not outputted" << std::endl;
            return -1;
        }
        // expect path
        std::string output_path = argv[4];
        // output to path
        outputPairingsToFile(m, output_path);
    }
    
    return 0;
}
