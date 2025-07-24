//
//  Tournament.hpp
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

#ifndef Tournament_hpp
#define Tournament_hpp

#ifndef CPPDUBOVSYSTEM_VERSION
#define CPPDUBOVSYSTEM_VERSION 2.0
#endif

#include <stdio.h>
#include <queue>
#include <vector>
#include <memory>
#include "Player.hpp"
#include "LinkedList.hpp"
#include "trf util/trf.hpp"
#include "trf util/rtg.hpp"
#include "baku.hpp"
#include "graph util/Matching.h"


/**
 * The core namespace for CPPDubovSystem
 */
namespace CPPDubovSystem {
/**
 * A simple match
 */
class Match {
public:
    /**
     * The player playing white in the match
     */
    Player white;
    /**
     * The player playing black in the match
     */
    Player black;
    /**
     * If this whole match is actually just a bye
     */
    bool is_bye;
    /**
     * Constructor for all three core values
     */
    Match(const Player &white, const Player &black, bool is_bye);
};

/**
 * For evaluating matches
 */
class MatchEval: public Match {
private:
    /**
     * If this match has a problem. In other words, the players cannot play each other for whatever reason (for instance they already played each other in the tournament before).
     */
    bool problem;
public:
    /**
     * Constructs the match evaluation with a given white player, black player, and a boolean indicating if there is a problem in the match
     */
    MatchEval(const Player &white, const Player &black, bool problem);
    
    /**
     * Sets the problem
     */
    void setProblem(bool p);
    /**
     * Checks if the board has a problem
     */
    bool isProblem() const {return problem;}
};

/**
 * This is used when we need access to a few functions globaly (not just the Tournament class). It is mainly used for the RTG and working with player data generally
 */
namespace Utils {
/**
 * For merge sort (sorting players by rating)
 */
void playersRatingMerge(std::vector<Player> *players, int const left, int const mid, int const right);
/**
 * For merge sort (sorting players by rating in rtg)
 */
void playersRatingRTGMerge(std::vector<Player> *players, int const left, int const mid, int const right);
/**
 * For sorting players by rating
 */
void sortPlayersRating(std::vector<Player> *players, int const begin, int const end);
/**
 * For sorting player in rtg
 */
void sortPlayersRatingRTG(std::vector<Player> *players, int const begin, int const end);
/**
 * A raw TRF match with read data and information
 */
class TRFMatch {
private:
    /**
     * White player in the match
     */
    Player *white;
    /**
     * Black player in the match. Can by nullptr if the match is a bye match
     */
    Player *black;
    /**
     * The target round this match belongs to
     */
    unsigned int target_round;
public:
    
    /**
     * The number of points white got from this match
     */
    double white_pts;
    /**
     * The number of points black got from this match
     */
    double black_pts;
    /**
     * An indicator if this match is a bye match
     */
    bool is_bye = false;
    
    /**
     * Constructs the full class given the data
     */
    TRFMatch(Player *white, Player *black, const double white_pts, const double black_pts, unsigned const int target_round);
    /**
     * Default empty constructor
     */
    TRFMatch();
    
    /**
     * Sets player white
     */
    void setWhite(Player *w) {white = w;}
    /**
     * Sets player black
     */
    void setBlack(Player *b) {black = b;}
    /**
     * Sets white points
     */
    void setWhitePts(double w) {white_pts = w;}
    /**
     * Sets black points
     */
    void setBlackPts(double b) {black_pts = b;}
    /**
     * Gets the target round the match is registered to
     */
    int getTargetRound() const {return target_round;}
    
    /**
     * Distributes the point information to the players
     */
    void distributePoints();
    /**
     * Distributes float info to the players
     */
    void distributeFloat();
    
    /**
     * Gets the ID of white player
     */
    int getWID() const;
    /**
     * Gets the ID of black player
     */
    int getBID() const;
};
/**
 * For merge sort. Sorts the raw matches
 */
void mergeRawMatches(std::vector<Utils::TRFMatch> *raw_games, int const left, int const mid, int const right);
/**
 * Sorts the raw TRF matches by round
 */
void sortRawMatches(std::vector<Utils::TRFMatch> *raw_games, const int begin, const int end);
}

/**
 * A simple tournament running Dubov system pairings
 */
class Tournament {
    
private:
    /**
     * Just a shorthand notation for games to evaluate
     */
    using eval_games = std::vector<MatchEval>;
    /**
     * Players in the tournament
     */
    std::vector<Player> players;
    /**
     * Number of players in tournament
     */
    int player_count = 0;
    /**
     * Total rounds in tournament
     */
    int total_rounds;
    /**
     * Determines if a pairing error occured
     */
    bool pairing_error = false;
    /**
     * If shifters were just applied to a (pairing) group
     */
    bool applied_shifters = false;
    /**
     * Just to record the current round globaly
     */
    int current_round = 0;
    
    /**
     * Divides a given group into to separate subgroups, both being players who are due white and black
     */
    void splitGroups(std::vector<Player> *white_seekers, std::vector<Player> *black_seekers, std::vector<Player> &group);
    /**
     * Gets the next available transpostion
     */
    std::shared_ptr<std::vector<MatchEval>> nextTransposition(std::vector<Player> &white_seekers, std::vector<Player> &black_seekers, int pof);
    /**
     * EXPERIMENTAL! Another implementation for applying transpositions
     */
//    std::shared_ptr<eval_games> applyTranspositions(std::vector<Player> &white_seekers, std::vector<Player> &black_seekers);
    /**
     * Evalutes the transpositions
     */
    void evaluateTranspositions(std::vector<MatchEval> *games_eval, std::vector<Player> &white_seekers, std::vector<Player> &black_seekers, bool *error);
    /**
     * Moves players from one group to the other
     */
    [[deprecated("Use of migratePlayers has been deprecated. It has been replaced by applyStandardShifters")]]
    void migratePlayers(std::vector<Player> *g1, std::vector<Player> *g2);
    /**
     * Generates a migration queue for a given group
     */
    std::vector<int> generateMigrationQueue(const std::vector<Player> &for_group) const;
    /**
     * For merging upfloat wrappers
     */
    void mergeUpfloaterWrappers(const std::vector<Player> &wrong_colors, const std::vector<Player> &max_upfloat, std::vector<Player> *upfloaters);
    /**
     * Merges matches in the recursive call
     */
    void mergeMatches(const std::vector<Match> &m1, std::vector<Match> *main);
    /**
     * Determines which player should recieve their due color, and returns the ID of the player
     */
    int playerShouldAlternate(Player &white, Player &black);
    /**
     * Optimizes the colors of the matches to make sure that all of criteria E is met
     */
    void optimizeColors(std::vector<Match> *games);
    /**
     * Finds a list of upfloaters for a given imbalanced group
     */
    std::vector<Player> findUpfloaters(LinkedListNode &next_group, int imbalance);
    /**
     * Applies the exhcnages given the minimum number of shifters to move
     */
    void getExchangeShifters(std::vector<Player> &white_seekers, std::vector<Player> &black_seekers, std::vector<int> &w_shift, std::vector<int> &b_shift, bool &error);
    /**
     * Applies shifters to ws or bs. This is specifically when one of those two groups are larger than the other
     */
    void applyStandardShifters(std::vector<Player> &white_seekers, std::vector<Player> &black_seekers, bool &error);
    /**
     * Generates the order floaters should be selected given the next group and the current imbalance situation
     */
    std::queue<Player> generateFloatQueue(LinkedListNode &next_group, int color_imbalance) const;
    /**
     * Similar to findMultiUpfloaters, but it designed for groups with an even number of whites and blacks (i.e. white seekers size is == to black seekers size)
     */
    std::set<int> findMultiUpfloatersNP(std::vector<Player> &white_seekers, std::vector<Player> &black_seekers, LinkedListNode &next_group, Graph &g, std::vector<double> &cost, const std::map<int, int> &p_convert, const std::vector<Player> &p_reverse);
    /**
     * Finds the next set of upfloaters that can satisfy the pairing. This returns the ids of all the players who should be the upfloaters
     */
    std::set<int> findMultiUpfloaters(std::vector<Player> &white_seekers, std::vector<Player> &black_seekers, LinkedListNode &next_group, bool &error);
    /**
     * Recreates the new groups by removing all upfloaters from the lower groups
     */
    LinkedListNode makeNewGroups(const LinkedListNode &old_groups, const std::set<int> &upfloaters, std::vector<Player> *white_seekers, std::vector<Player> *black_seekers) const;
    /**
     * Converts the match eval games to normal matches
     */
    std::vector<Match> castToMatch(const std::vector<MatchEval> &games_eval);
    /**
     * Maximizes the pairings for a group
     */
    std::vector<Match> maximizePairings(std::vector<Player> &white_seekers, std::vector<Player> &black_seekers, bool *pairing_error);
    /**
     * For merge sort (sorting players by ARO)
     */
    void mergeGroupARO(std::vector<Player> *group, int const left, int const mid, int const right);
    /**
     * For merge sort (soring players by points)
     */
    static void playersPointsMerge(std::vector<Player> *players, int const left, int const mid, int const right);
    /**
     * For sorting players by points
     */
    static void sortPlayersPoints(std::vector<Player> *players, int const begin, int const end);
    /**
     * Sorts a group by ARO (average rating of opponent)
     */
    void sortGroupARO(std::vector<Player> *group, int const begin, int const end);
    /**
     * Makes round 1 pairings
     */
    std::vector<Match> makeRound1();
    /**
     * Makes the groups for the players
     */
    LinkedList makeGroups() const;
    /**
     * Makes a pairing for a group
     */
    std::vector<Match> makePairingForGroup(LinkedListNode *g, int pairing_round);
    /**
     * Initializes all due colors for the players
     */
    void initPlayers();
    /**
     * Makes subsequent round pairings
     */
    std::vector<Match> makeSubsequent(int pairing_round);
    /**
     * For getting matches of a specific round
     */
    std::vector<Utils::TRFMatch> extractedMatch;
    /**
     * Removes the bye player and returns the index of the bye player removed from
     */
    int removeByePlayer(std::vector<Player> *players, int bye_remove, Player *p_bye) const;
    /**
     * The bye stack
     */
    std::vector<int> bye_queue;
    /**
     * Gets the index of the player who should get the bye
     */
    std::vector<int> getByePlayerStack(const std::vector<Player> &selection, int current_round) const;
    /**
     * Fixes the colors for round 1 baku acceleration
     */
    std::vector<Match> fixBakuR1(const std::vector<Match> &games) const;
public:
    /**
     * Initializes the tournament with a given total number of rounds
     */
    explicit Tournament(int total_rounds);
    
    /**
     * Adds a player to the list of players in the tournament
     */
    void addPlayer(const Player &p);
    /**
     * Adds a player (through std::move) to the list of players in the tournament
     */
    void addPlayer(Player &&p);
    /**
     * Gets all registered players in the tournament
     */
    std::vector<Player> getPlayers() const {return players;}
    /**
     * A simple getter for pairing\_error
     */
    bool pairingErrorOccured() const {return pairing_error;}
    /**
     * A simple getter for player count
     */
    int getPlayerCount() const {return player_count;}
    /**
     * Generates pairings for a given round
     */
    std::vector<Match> generatePairings(int r);
    /**
     * Generates pairings for a given round with baku acceleration
     */
    std::vector<Match> generatePairings(int r, bool baku_acceleration);
    /**
     * Gets the raw matches extracted from a particular round
     */
    std::vector<Utils::TRFMatch> getRawMatches() const {return extractedMatch;}
    /**
     * Sets the raw matches
     */
    void setRawMatch(const std::vector<Utils::TRFMatch> &s) {extractedMatch = s;}
    /**
     * Creates a tournament from TRF data
     */
    static Tournament makeTournament(const TRFUtil::TRFData &from_data, int *next_round, int stop_read = -1);
    /**
     * Simulates a tournament (random tournament generator)
     */
    static std::string simulateTournament(int p_count, int max_rounds);
};
}

#endif /* Tournament_hpp */
