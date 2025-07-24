//
//  fpc.hpp
//  DubovSystem
//
//  Created by Michael Shapiro on 5/23/24.
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

#ifndef fpc_hpp
#define fpc_hpp

#include <stdio.h>
#include <vector>
#include <memory>
#include "trf util/trf.hpp"
#include "Tournament.hpp"
#include "Player.hpp"

/// Free Pairings Checker
/// FIDE requires this for endorsement
/// The function of this is to simply validate some pairings input

/**
 * Free pairings checker
 */
namespace fpc {
/**
 * This is the core pairings checker class
 */
class PairingsChecker {
private:
    /**
     * This is the string data of the tournament
     */
    std::string rawTournamentData;
    /**
     * The round to check pairings for
     */
    int target_round;
    /**
     * All the games (completely processed) to compare. These are the pairings derived from the file
     */
    std::vector<CPPDubovSystem::Utils::TRFMatch> match_check;
    /**
     * The games we expect (checker pairings)
     */
    std::vector<CPPDubovSystem::Match> games_expected;
    /**
     * Any pairings that match\_check didn't have that existed in games\_expected
     */
    std::vector<CPPDubovSystem::Match> games_no_found;
    /**
     * Just a helper variable for quickly getting a player
     */
    std::map<int, CPPDubovSystem::Player> players_scaned;
    /**
     * Number of pairings (tournament pairings) that violated C1 (Two players shall not play each other more than once)
     */
    int bad_opp = 0;
    /**
     * Number of pairings (tournament pairings) that violated C3 (Two players with the same absolute color preference shall not meet)
     */
    int bad_color = 0;
    /**
     * For evaluating the raw matchs
     */
    void scan();
public:
    /**
     * We initiate the raw tournament read from the TRF file, and we accept the parameter for the target round to check pairings for
     */
    PairingsChecker(const std::string &rawTournament, int targetRound);
    /**
     * Copy not allowed
     */
    PairingsChecker(const PairingsChecker &pc) = delete;
    /**
     * Move not allowed
     */
    PairingsChecker(PairingsChecker &&pc) = delete;
    /**
     * Assignment not allowed
     */
    PairingsChecker &operator=(const PairingsChecker &pc) = delete;
    /**
     * Move assignment not allowed
     */
    PairingsChecker &operator=(PairingsChecker &&pc) = delete;
    /**
     * Returns the string representation of the report
     */
    std::string outputReport();
};
}

#endif /* fpc_hpp */
