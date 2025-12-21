//
//  Tournament.cpp
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

#include "Tournament.hpp"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <random>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include "assertm.h"

CPPDubovSystem::MatchEval::MatchEval(const Player &white, const Player &black, bool problem): Match(white, black, false) {
    this->problem = problem;
}

void CPPDubovSystem::MatchEval::setProblem(bool p) {
    this->problem = p;
}

CPPDubovSystem::Utils::TRFMatch::TRFMatch(Player *white, Player *black, const double white_pts, const double black_pts, unsigned const int target_round) {
    this->white = white;
    this->black = black;
    this->white_pts = white_pts;
    this->black_pts = black_pts;
    this->target_round = target_round;
}

CPPDubovSystem::Utils::TRFMatch::TRFMatch() {
    this->white = nullptr;
    this->black = nullptr;
    this->white_pts = 0.0;
    this->black_pts = 0.0;
    this->target_round = -1;
}

void CPPDubovSystem::Utils::TRFMatch::distributePoints() {
    this->white->trf_pts += this->white_pts;
    this->black->trf_pts += this->black_pts;
}

void CPPDubovSystem::Utils::TRFMatch::distributeFloat() {
    if(this->black != nullptr) {
        if(this->white->trf_pts > this->black->trf_pts) {
            this->black->incrementUpfloat();
            this->black->setUpfloatPrevStatus(true);
        } else if(this->white->trf_pts < this->black->trf_pts) {
            this->white->incrementUpfloat();
            this->white->setUpfloatPrevStatus(true);
        } else {
            this->white->setUpfloatPrevStatus(false);
            this->black->setUpfloatPrevStatus(false);
        }
    }
}

int CPPDubovSystem::Utils::TRFMatch::getWID() const {
    if(this->white == nullptr) {
        return -1;
    }
    return this->white->getID();
}

int CPPDubovSystem::Utils::TRFMatch::getBID() const {
    if(this->black == nullptr) {
        return -1;
    }
    return this->black->getID();
}

//MARK: SORTING ALGORITHMS TO USE FOR PAIRING
/// Merge sort is the core sorting algorithm here (for all sorts)

void CPPDubovSystem::Tournament::mergeGroupARO(std::vector<Player> *group, const int left, const int mid, const int right) {
    int const subVectorOne = mid - left + 1;
    int const subVectorTwo = right - mid;
    
    auto *leftVector = new std::vector<Player>(), *rightVector = new std::vector<Player>();
    leftVector->resize(subVectorOne);
    rightVector->resize(subVectorTwo);
    
    for(auto i = 0; i < subVectorOne; i++) {
        (*leftVector)[i] = (*group)[left + i];
    }
    for(auto i = 0; i < subVectorTwo; i++) {
        (*rightVector)[i] = (*group)[mid + 1 + i];
    }
    
    auto indexOfVectorOne = 0, indexOfVectorTwo = 0;
    int indexOfMergedVector = left;
    
    while(indexOfVectorOne < subVectorOne && indexOfVectorTwo < subVectorTwo) {
        // if aro is equal, get the lower pairing num of the two
        if((*leftVector)[indexOfVectorOne].getARO() <= (*rightVector)[indexOfVectorTwo].getARO()) {
            if((*leftVector)[indexOfVectorOne].getARO() == (*rightVector)[indexOfVectorTwo].getARO()) {
//                if((*leftVector)[indexOfVectorOne].getRating() < (*rightVector)[indexOfVectorTwo].getRating()) {
                if((*leftVector)[indexOfVectorOne].getID() < (*rightVector)[indexOfVectorTwo].getID()) {
                    (*group)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
                    indexOfVectorOne += 1;
                } else {
                    (*group)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
                    indexOfVectorTwo += 1;
                }
            } else {
                (*group)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
                indexOfVectorOne += 1;
            }
        } else {
            (*group)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
            indexOfVectorTwo += 1;
        }
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorOne < subVectorOne) {
        (*group)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
        indexOfVectorOne += 1;
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorTwo < subVectorTwo) {
        (*group)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
        indexOfVectorTwo += 1;
        indexOfMergedVector += 1;
    }
    
    delete leftVector;
    delete rightVector;
}

void CPPDubovSystem::Utils::mergeRawMatches(std::vector<Utils::TRFMatch> *raw_games, const int left, const int mid, const int right) {
    int const subVectorOne = mid - left + 1;
    int const subVectorTwo = right - mid;
    
    auto *leftVector = new std::vector<Utils::TRFMatch>(), *rightVector = new std::vector<Utils::TRFMatch>();
    leftVector->resize(subVectorOne);
    rightVector->resize(subVectorTwo);
    
    for(auto i = 0; i < subVectorOne; i++) {
        (*leftVector)[i] = (*raw_games)[left + i];
    }
    for(auto i = 0; i < subVectorTwo; i++) {
        (*rightVector)[i] = (*raw_games)[mid + 1 + i];
    }
    
    auto indexOfVectorOne = 0, indexOfVectorTwo = 0;
    int indexOfMergedVector = left;
    
    while(indexOfVectorOne < subVectorOne && indexOfVectorTwo < subVectorTwo) {
        if((*leftVector)[indexOfVectorOne].getTargetRound() <= (*rightVector)[indexOfVectorTwo].getTargetRound()) {
            (*raw_games)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
            indexOfVectorOne += 1;
        } else {
            (*raw_games)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
            indexOfVectorTwo += 1;
        }
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorOne < subVectorOne) {
        (*raw_games)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
        indexOfVectorOne += 1;
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorTwo < subVectorTwo) {
        (*raw_games)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
        indexOfVectorTwo += 1;
        indexOfMergedVector += 1;
    }
    
    delete leftVector;
    delete rightVector;
}

void CPPDubovSystem::Tournament::sortGroupARO(std::vector<Player> *group, const int begin, const int end) {
    if(begin >= end) {
        return;
    }
    
    int mid = begin + (end - begin) / 2;
    this->sortGroupARO(group, begin, mid);
    this->sortGroupARO(group, mid + 1, end);
    this->mergeGroupARO(group, begin, mid, end);
}

void CPPDubovSystem::Utils::sortRawMatches(std::vector<Utils::TRFMatch> *raw_games, const int begin, const int end) {
    if(begin >= end) {
        return;
    }
    
    int mid = begin + (end - begin) / 2;
    Utils::sortRawMatches(raw_games, begin, mid);
    Utils::sortRawMatches(raw_games, mid + 1, end);
    Utils::mergeRawMatches(raw_games, begin, mid, end);
}

//MARK: MERGE SORT IMPLEMENTATION END

void CPPDubovSystem::Tournament::splitGroups(std::vector<Player> *white_seekers, std::vector<Player> *black_seekers, std::vector<Player> &group) {
    // put all white seekers into the white seekers container
    // likewise all black seekers should go into the black seekers container
    for(int i = 0; i < group.size(); i++) {
        if(group[i].getDueColor() == Color::WHITE) {
            white_seekers->push_back(group[i]);
        } else {
            black_seekers->push_back(group[i]);
        }
    }
}

void CPPDubovSystem::Tournament::mergeMatches(const std::vector<Match> &m1, std::vector<Match> *main) {
    // merge both vectors into main
    for(int i = 0; i < m1.size(); i++) {
        main->push_back(m1[i]);
    }
}

void CPPDubovSystem::Tournament::mergeUpfloaterWrappers(const std::vector<Player> &wrong_colors, const std::vector<Player> &max_upfloat, std::vector<Player> *upfloaters) {
    // simply combine all the different sets of upfloaters into one bin of upfloaters
    for(int i = 0; i < wrong_colors.size(); i++) {
        upfloaters->push_back(wrong_colors[i]);
    }
    
    for(int i = 0; i < max_upfloat.size(); i++) {
        upfloaters->push_back(max_upfloat[i]);
    }
}

std::vector<CPPDubovSystem::Player> CPPDubovSystem::Tournament::findUpfloaters(LinkedListNode &next_group, int imbalance) {
    std::vector<Player> upfloaters;
    
    // look through sorted partition and evaluate with given score
    LinkedListNode* curr = &next_group;
    std::string logger_curr_group = "### Looking For Upfloaters";
    while(curr != nullptr) {
        logger_curr_group += "\n\nLooking into next score group and picking all possible upfloaters...";
        std::vector<Player> upfloater_max;
        std::vector<Player> wrong_color; // C.7 minimize players who don't get their color preference
        std::vector<Player> upfloated_previous; // C.10 minimize upfloaters who upfloated previously
        for(int i = 0; i < curr->data.size(); i++) {
            // make sure this player is not marked as an upfloater already
            // we can ignore all the float history if it is the last round
            if(!curr->data[i].canUpfloat(this->total_rounds) && this->current_round != this->total_rounds) {
                upfloater_max.push_back(curr->data[i]);
                logger_curr_group += "\nPlayer with ID " + std::to_string(curr->data[i].getID()) + "(" + curr->data[i].getName() + ") is a max upfloater, and this round is not the last round. Putting this player as one who should receive less priority for being an upfloater...";
                continue;
            }
            // C.10 minimize upfloaters who upfloated previously
            if(curr->data[i].upfloatedPreviously() && this->current_round != this->total_rounds) {
                upfloated_previous.push_back(curr->data[i]);
                logger_curr_group += "\nPlayer with ID " + std::to_string(curr->data[i].getID()) + "(" + curr->data[i].getName() + ") upfloated previously, and this round is not the last round. Putting this player as one who should receive less priority for being an upfloater (but more over the max upfloater)...";
                continue;
            }
//            if(imbalance == 1) {
            if(imbalance > 0) {
                if(curr->data[i].getDueColor() == Color::WHITE) {
                    wrong_color.push_back(curr->data[i]);
                    logger_curr_group += "\nPlayer with ID " + std::to_string(curr->data[i].getID()) + "(" + curr->data[i].getName() + ") has the wrong due color (the majority of players in the score group requiring upfloaters have more Black Seekers than White). Putting this player as one who should receive less priority for being an upfloater...";
                    continue;
                }
                upfloaters.push_back(curr->data[i]);
                logger_curr_group += "\nPlayer with ID " + std::to_string(curr->data[i].getID()) + "(" + curr->data[i].getName() + ") is a possible upfloater with greater priority for becoming an upfloater. Putting this player closer to the top...";
            } else {
                if(curr->data[i].getDueColor() == Color::BLACK || curr->data[i].getDueColor() == Color::NO_COLOR) {
                    wrong_color.push_back(curr->data[i]);
                    logger_curr_group += "\nPlayer with ID " + std::to_string(curr->data[i].getID()) + "(" + curr->data[i].getName() + ") has the wrong due color (the majority of players in the score group requiring upfloaters have more White Seekers than Black). Putting this player as one who should receive less priority for being an upfloater...";
                    continue;
                }
                upfloaters.push_back(curr->data[i]);
                logger_curr_group += "\nPlayer with ID " + std::to_string(curr->data[i].getID()) + "(" + curr->data[i].getName() + ") is a possible upfloater with greater priority for becoming an upfloater. Putting this player closer to the top...";
            }
        }
        
        // merge wrong color and max upfloaters with upfloaters
        this->mergeUpfloaterWrappers(wrong_color, upfloater_max, &upfloaters);
        // also merge float prev
        this->mergeUpfloaterWrappers(upfloated_previous, std::vector<Player>(), &upfloaters);
        
        // shift node
        curr = curr->next;
        
        // C.6 minimize score difference
    }
    
    return upfloaters;
}

void CPPDubovSystem::Tournament::getExchangeShifters(std::vector<Player> &white_seekers, std::vector<Player> &black_seekers, std::vector<int> &w_shift, std::vector<int> &b_shift, bool &error) {
    // first generate a migration queue for both sides
    std::vector<int> w_migration = this->generateMigrationQueue(white_seekers);
    std::vector<int> b_migration = this->generateMigrationQueue(black_seekers);
    
    // quickly add vertecies to our graph
    Graph g_main(((int) white_seekers.size()) + ((int) black_seekers.size()));
    std::map<int, int> p_map;
    std::vector<Player> p_equiv(((int) white_seekers.size()) + ((int) black_seekers.size()));
    std::vector<std::pair<int, int>> move_priority(p_equiv.size()); // first is priority, second is index
    int vertex_id = 0;
    for(int i = 0; i < white_seekers.size(); i++) {
        p_map[white_seekers[i].getID()] = vertex_id;
        p_equiv[vertex_id] = white_seekers[i];
        move_priority[vertex_id] = std::make_pair(0, i);
        vertex_id += 1;
    }
    for(int i = 0; i < black_seekers.size(); i++) {
        p_map[black_seekers[i].getID()] = vertex_id;
        p_equiv[vertex_id] = black_seekers[i];
        move_priority[vertex_id] = std::make_pair(0, i);
        vertex_id += 1;
    }
    
    // now to initialize weights
    // we are only using minimum weight matching to determine who to move
    // so weights shouldn't matter too much when pairing players of opposite colors
    std::vector<double> cost;
    for(int i = 0; i < white_seekers.size(); i++) {
        for(int z = 0; z < black_seekers.size(); z++) {
            // make sure players are compatible
            if(white_seekers[i].canPlayOpp(black_seekers[z])) {
                g_main.AddEdge(p_map[white_seekers[i].getID()], p_map[black_seekers[z].getID()]);
                cost.push_back(0); // again, we are only using the graph for optimal selection, so the weight doesn't matter, it just has to be lower then everything else
            }
        }
    }
    
    // now to pair groups against themseleves
    // we will use the migration queue generated to make those edge weights
    std::vector<bool> migration_w_done(white_seekers.size());
    int min_weight = 1;
    for(int i = 0; i < w_migration.size(); i++) {
        // pair against group
        int index_check = w_migration[i];
        move_priority[p_map[white_seekers[index_check].getID()]].first = min_weight;
        for(int z = 0; z < white_seekers.size(); z++) {
            // make sure index_check is not z
            if(index_check == z) continue;
            
            // also make sure z has not been completely paired
            if(migration_w_done[z]) continue;
            
            // now compare
            if(white_seekers[index_check].canPlayOpp(white_seekers[z])) {
                g_main.AddEdge(p_map[white_seekers[index_check].getID()], p_map[white_seekers[z].getID()]);
                cost.push_back(min_weight);
            }
        }
        
        // mark index check as completely paired
        migration_w_done[index_check] = true;
        // and increment min_weight
        min_weight += 1;
    }
    
    // we will do the same operation on black
    migration_w_done.clear();
    migration_w_done.resize(black_seekers.size());
    min_weight = 1;
    for(int i = 0; i < b_migration.size(); i++) {
        // pair against group
        int index_check = b_migration[i];
        move_priority[p_map[black_seekers[index_check].getID()]].first = min_weight;
        for(int z = 0; z < black_seekers.size(); z++) {
            // make sure index_check is not z
            if(index_check == z) continue;
            
            // also make sure z has not been completely paired
            if(migration_w_done[z]) continue;
            
            // now compare
            if(black_seekers[index_check].canPlayOpp(black_seekers[z])) {
                g_main.AddEdge(p_map[black_seekers[index_check].getID()], p_map[black_seekers[z].getID()]);
                cost.push_back(min_weight);
            }
        }
        
        // mark index check as completely paired
        migration_w_done[index_check] = true;
        // and increment min_weight
        min_weight += 1;
    }
    
    // now set up the matching
    Matching matching(g_main);
    
    // compute matching
    std::pair<std::list<int>, double> matched = matching.SolveMinimumCostPerfectMatching(cost);
    
    // make sure matching exists
    if(matched.first.size() == 0) {
        error = true;
        return;
    }
    
    // do some logging before we even output who is moved
    if(this->enable_logging) {
        // generate migration queue
        this->logExplanation("Generating order of shifters to move...");
        auto logger = [](const std::vector<int> &migration, const std::vector<Player> &group) {
            std::string migration_txt;
            for(int i = 0; i < migration.size(); i++) {
                migration_txt += "\n" + std::to_string(i + 1) + ". " + group[migration[i]].getName() + " (ID = " + std::to_string(group[migration[i]].getID()) + ")";
            }
            return migration_txt;
        };
        if(white_seekers.size() % 2 > 0 && !white_seekers.empty()) {
            this->logExplanation("White Seekers group has an odd number of players. We pick the players to move as follows: take the middle player (this has largest priority to move), the player left of that middle player (this is second priority to move), the player right of that middle player (this is third priority to move), the player left of that left player (this is fourth priority), etc.");
        } else {
            this->logExplanation("White Seekers group has an even number of players. We pick the players to move as follows: take the middle player. The player to the left of this player as the largest priority to move, the middle player has the second priority to move, the player to the left of the left player has the third priority to move, the player to the right of the middle player has the fourth priority to move, etc.");
        }
        this->logExplanation(logger(w_migration, white_seekers));
        if(black_seekers.size() % 2 > 0 && !black_seekers.empty()) {
            this->logExplanation("Black Seekers group has an odd number of players. We pick the players to move as follows: take the middle player (this has largest priority to move), the player left of that middle player (this is second priority to move), the player right of that middle player (this is third priority to move), the player left of that left player (this is fourth priority), etc.");
        } else {
            this->logExplanation("Black Seekers group has an even number of players. We pick the players to move as follows: take the middle player. The player to the left of this player as the largest priority to move, the middle player has the second priority to move, the player to the left of the left player has the third priority to move, the player to the right of the middle player has the fourth priority to move, etc.");
        }
        this->logExplanation(logger(b_migration, black_seekers));
        this->logExplanation("Try to swap players of similar priorities. For example, player with the highest priority in White Seekers should be swapped with the player with the highest priority in Black Seekers.");
    }
    
    for(std::list<int>::iterator it = matched.first.begin(); it != matched.first.end(); it++) {
        std::pair<int, int> e = g_main.GetEdge(*it);
        Color e1_col = p_equiv[e.first].getDueColor();
        Color e2_col = p_equiv[e.second].getDueColor();
        
        // we have a shifter if both players of the same due color end up playing each other
        if(e1_col == e2_col) {
            // shifter found!
            // pick the one with the smaller priority to move
            if(move_priority[e.first].first < move_priority[e.second].first) {
                if(e1_col == Color::WHITE) {
                    w_shift.push_back(move_priority[e.first].second);
                } else {
                    b_shift.push_back(move_priority[e.first].second);
                }
            } else {
                if(e1_col == Color::WHITE) {
                    w_shift.push_back(move_priority[e.second].second);
                } else {
                    b_shift.push_back(move_priority[e.second].second);
                }
            }
        }
    }
}

/*void Tournament::getExchangeShifters(std::vector<Player> &white_seekers, std::vector<Player> &black_seekers, std::vector<int> *w_shift, std::vector<int> *b_shift, bool *error) {
    // apply minimum weight matching
    CPPDSGraphUtil::Graph g_main;
    // add players as vertecies
    // also set up indecies for each spot in migration queue
    std::vector<int> migration_queue = Utils::generateMigrationQueue(white_seekers);
    std::map<int, int> migration_order;
    std::set<int> wp; // white seekers ID
    std::set<int> bp;
    std::map<int, int> pos;
    std::map<int, int> m_pos;
    for(int i = 0; i < white_seekers.size(); i++) {
        g_main.AddVertex(white_seekers[i].getID());
        g_main.AddVertex(black_seekers[i].getID());
        wp.insert(white_seekers[i].getID());
        bp.insert(black_seekers[i].getID());
        pos[white_seekers[i].getID()] = i;
        pos[black_seekers[i].getID()] = i;
        // i at position WS and BS should have roughly the same migration queue spot
        for(int z = 0; z < migration_queue.size(); z++) {
            if(migration_queue[z] == i) {
                migration_order[i] = z;
                m_pos[white_seekers[i].getID()] = z;
                m_pos[black_seekers[i].getID()] = z;
                break;
            }
        }
    }
    
    // now set up the edges for each vertex
    // add edges to compatible pairings
    // that is both players can play each other without violating the absolute criteria
    for(int i = 0; i < white_seekers.size(); i++) {
        // check if adjacent vertex for BS is compatible
        if(white_seekers[i].canPlayOpp(black_seekers[i])) {
            g_main.AddEdge(white_seekers[i].getID(), black_seekers[i].getID(), 0);
        }
        // check lower players
        for(int z = i + 1; z < white_seekers.size(); z++) {
            // compare WS[i] to WS[z] and BS[z]
            if(white_seekers[i].canPlayOpp(white_seekers[z])) {
                // edge weight should depend on both players positions in migration queue
                g_main.AddEdge(white_seekers[i].getID(), white_seekers[z].getID(), migration_order[i] + migration_order[z] + 100); // add 100 to minimize the number of players which don't get their due color
            }
            if(white_seekers[i].canPlayOpp(black_seekers[z])) {
                g_main.AddEdge(white_seekers[i].getID(), black_seekers[z].getID(), 1); // edge weight surprisingly doesn't matter here as much since both players get due colors granted
                // we simply use the minimum weighted matching algorithm only to determine the shifters
                // as long as players with the same due color have a edge weight significantly higher than ones with opposite due colors, we are more likely to gaurantee who our shifters are
            }
            
            // also compare BS[i] to WS[z] and BS[z]
            if(black_seekers[i].canPlayOpp(white_seekers[z])) {
                g_main.AddEdge(white_seekers[z].getID(), black_seekers[i].getID(), 1);
            }
            if(black_seekers[i].canPlayOpp(black_seekers[z])) {
                g_main.AddEdge(black_seekers[i].getID(), black_seekers[z].getID(), migration_order[i] + migration_order[z] + 100);
            }
        }
    }
    
    // now make matching
    Matching m_main(g_main);
    
    // compute matching
    std::vector<std::pair<int, int>> mwm = m_main.computeMinimumWeightMatching();
    
    // make sure a perfect matching was found
    if(mwm.size() < white_seekers.size()) {
        *error = true; // a valid matching doesn't exist
        return;
    }
    
    std::vector<int> w_shifters;
    std::vector<int> b_shifters;
    
    // now run through the entire minimum weight matching and identify which players were the shifters
    for(int i = 0; i < mwm.size(); i++) {
        if(wp.contains(mwm[i].first) && wp.contains(mwm[i].second)) {
            // shifter found!
            // player closer to the middle is our shifter
            if(m_pos[mwm[i].first] < m_pos[mwm[i].second]) {
                w_shifters.push_back(pos[mwm[i].first]);
            } else {
                w_shifters.push_back(pos[mwm[i].second]);
            }
        }
        if(bp.contains(mwm[i].first) && bp.contains(mwm[i].second)) {
            // shifter found!
            // same operation as pervious if statement
            if(m_pos[mwm[i].first] < m_pos[mwm[i].second]) {
                b_shifters.push_back(pos[mwm[i].first]);
            } else {
                b_shifters.push_back(pos[mwm[i].second]);
            }
        }
    }
    
    // set variables to pointers
    *w_shift = w_shifters;
    *b_shift = b_shifters;
}*/

void CPPDubovSystem::Tournament::applyStandardShifters(std::vector<Player> &white_seekers, std::vector<Player> &black_seekers, bool &error) {
    
    // the process here should be similar to getExchangeShifters
    // just the weights need to be set up slightly differently
    // we first need to initilaize our graph
    Graph g_main(((int) white_seekers.size()) + ((int) black_seekers.size()));
    
    std::map<int, int> p_map;
    std::vector<Player> p_equiv(((int) white_seekers.size()) + ((int) black_seekers.size()));
    std::vector<std::pair<int, int>> move_priority(p_equiv.size()); // first is priority, second is index
    int vertex_id = 0;
    for(int i = 0; i < white_seekers.size(); i++) {
        p_map[white_seekers[i].getID()] = vertex_id;
        p_equiv[vertex_id] = white_seekers[i];
        move_priority[vertex_id] = std::make_pair(0, i);
        vertex_id += 1;
    }
    for(int i = 0; i < black_seekers.size(); i++) {
        p_map[black_seekers[i].getID()] = vertex_id;
        p_equiv[vertex_id] = black_seekers[i];
        move_priority[vertex_id] = std::make_pair(0, i);
        vertex_id += 1;
    }
    
    std::vector<double> cost;
    
    // first set all pairing of opposite due color to 0 weight
    for(int i = 0; i < white_seekers.size(); i++) {
        for(int z = 0; z < black_seekers.size(); z++) {
            // make sure players are compatible
            if(white_seekers[i].canPlayOpp(black_seekers[z])) {
                g_main.AddEdge(p_map[white_seekers[i].getID()], p_map[black_seekers[z].getID()]);
                cost.push_back(0); // again, we are only using the graph for optimal selection, so the weight doesn't matter, it just has to be lower then everything else
            }
        }
    }
    
    // now for determining weights on the group itself
    // the larger group should have the smaller weights
    int min_weight = 1;
    std::vector<Player> &larger_group = white_seekers.size() > black_seekers.size() ? white_seekers : black_seekers;
    std::vector<Player> &smaller_group = white_seekers.size() < black_seekers.size() ? white_seekers : black_seekers;
    std::vector<int> larger_migration = this->generateMigrationQueue(white_seekers.size() > black_seekers.size() ? white_seekers : black_seekers);
    std::vector<int> smaller_migration = this->generateMigrationQueue(white_seekers.size() < black_seekers.size() ? white_seekers : black_seekers);
    std::vector<bool> migration_w_done(std::max(white_seekers.size(), black_seekers.size()));
    
    ASSERT(larger_group.size() == larger_migration.size(), "shifter migration vector was not equal to the size of the group. This is a bug and should be very unlikely to happen");
    ASSERT(smaller_group.size() == smaller_migration.size(), "shifter migration vector was not equal to the size of the group. This is a bug and should be very unlikely to happen");
    
    for(int i = 0; i < larger_group.size(); i++) {
        // pair against group
        int index_check = larger_migration[i];
        move_priority[p_map[larger_group[index_check].getID()]].first = min_weight;
        for(int z = 0; z < larger_group.size(); z++) {
            // make sure index_check is not z
            if(index_check == z) continue;
            
            // also make sure z has not been completely paired
            if(migration_w_done[z]) continue;
            
            // now compare
            if(larger_group[index_check].canPlayOpp(larger_group[z])) {
                g_main.AddEdge(p_map[larger_group[index_check].getID()], p_map[larger_group[z].getID()]);
                cost.push_back(min_weight);
            }
        }
        
        // mark index check as completely paired
        migration_w_done[index_check] = true;
        // and increment min_weight
        min_weight += 1;
    }
    
    migration_w_done.clear();
    migration_w_done.resize(std::min(white_seekers.size(), black_seekers.size()));
    for(int i = 0; i < smaller_group.size(); i++) {
        // pair against group
        int index_check = smaller_migration[i];
        move_priority[p_map[smaller_group[index_check].getID()]].first = min_weight;
        for(int z = 0; z < smaller_group.size(); z++) {
            // make sure index_check is not z
            if(index_check == z) continue;
            
            // also make sure z has not been completely paired
            if(migration_w_done[z]) continue;
            
            // now compare
            if(smaller_group[index_check].canPlayOpp(smaller_group[z])) {
                g_main.AddEdge(p_map[smaller_group[index_check].getID()], p_map[smaller_group[z].getID()]);
                cost.push_back(min_weight);
            }
        }
        
        // mark index check as completely paired
        migration_w_done[index_check] = true;
        // and increment min_weight
        min_weight += 1;
    }
    
    // finally we can do the actual matching
    Matching matching(g_main);
    
    std::pair<std::list<int>, double> matched = matching.SolveMinimumCostPerfectMatching(cost);
    
    // make sure matching exists
    if(matched.first.size() == 0) {
        error = true;
        return;
    }
    
    std::vector<Player> reconstructed_black_seekers;
    std::vector<Player> reconstructed_white_seekers;
    
    std::set<int> w_remove;
    std::set<int> b_remove;
    
    // do some logging before we even output who is moved
    if(this->enable_logging) {
        // generate migration queue
        this->logExplanation("Generating order of shifters to move...");
        std::vector<int> temp = this->generateMigrationQueue(larger_group);
        auto logger = [](const std::vector<int> &migration, const std::vector<Player> &group) {
            std::string migration_txt;
            for(int i = 0; i < migration.size(); i++) {
                migration_txt += "\n" + std::to_string(i + 1) + ". " + group[migration[i]].getName() + " (ID = " + std::to_string(group[migration[i]].getID()) + ")";
            }
            return migration_txt;
        };
        if(larger_group.size() % 2 > 0) {
            this->logExplanation("Larger group has an odd number of players. We pick the players to move as follows: take the middle player (this has largest priority to move), the player left of that middle player (this is second priority to move), the player right of that middle player (this is third priority to move), the player left of that left player (this is fourth priority), etc.");
        } else {
            this->logExplanation("Larger group has an even number of players. We pick the players to move as follows: take the middle player. The player to the left of this player as the largest priority to move, the middle player has the second priority to move, the player to the left of the left player has the third priority to move, the player to the right of the middle player has the fourth priority to move, etc.");
        }
        this->logExplanation(logger(temp, larger_group));
        temp = this->generateMigrationQueue(smaller_group);
        if(larger_group.size() % 2 > 0) {
            this->logExplanation("Smaller group has an odd number of players. We pick the players to move as follows: take the middle player (this has largest priority to move), the player left of that middle player (this is second priority to move), the player right of that middle player (this is third priority to move), the player left of that left player (this is fourth priority), etc.");
        } else {
            this->logExplanation("Smaller group has an even number of players. We pick the players to move as follows: take the middle player. The player to the left of this player as the largest priority to move, the middle player has the second priority to move, the player to the left of the left player has the third priority to move, the player to the right of the middle player has the fourth priority to move, etc.");
        }
        this->logExplanation(logger(temp, smaller_group));
        this->logExplanation("Move priorities all set up. First, try moving players from the larger group (given their move priority) to the smaller. If this cannot be accomplished, there is a chance that some players from the smaller group have to also be moved. But avoid this from happening.");
    }
    
    // else find the players who we move, and do the move
    for(std::list<int>::iterator it = matched.first.begin(); it != matched.first.end(); it++) {
        std::pair<int, int> e = g_main.GetEdge(*it);
        Color e1_col = p_equiv[e.first].getDueColor();
        Color e2_col = p_equiv[e.second].getDueColor();
        
        // we have a shifter if both players of the same due color end up playing each other
        if(e1_col == e2_col || (e1_col == Color::BLACK && e2_col == Color::NO_COLOR) || (e1_col == Color::NO_COLOR && e2_col == Color::BLACK)) {
            // shifter found!
            // pick the one with the smaller priority to move
            if(move_priority[e.first].first < move_priority[e.second].first) {
                if(e1_col == Color::WHITE) {
                    this->logExplanation("Shifter found! Shifting " + white_seekers[move_priority[e.first].second].getName() + " (ID = " + std::to_string(white_seekers[move_priority[e.first].second].getID()) + ") from White Seekers to Black Seekers.");
                    black_seekers.push_back(white_seekers[move_priority[e.first].second]);
                    w_remove.insert(move_priority[e.first].second);
                } else {
                    this->logExplanation("Shifter found! Shifting " + black_seekers[move_priority[e.first].second].getName() + " (ID = " + std::to_string(black_seekers[move_priority[e.first].second].getID()) + ") from Black Seekers to White Seekers.");
                    white_seekers.push_back(black_seekers[move_priority[e.first].second]);
                    b_remove.insert(move_priority[e.first].second);
                }
            } else {
                if(e1_col == Color::WHITE) {
                    this->logExplanation("Shifter found! Shifting " + white_seekers[move_priority[e.first].second].getName() + " (ID = " + std::to_string(white_seekers[move_priority[e.first].second].getID()) + ") from White Seekers to Black Seekers.");
                    black_seekers.push_back(white_seekers[move_priority[e.second].second]);
                    w_remove.insert(move_priority[e.second].second);
                } else {
                    this->logExplanation("Shifter found! Shifting " + black_seekers[move_priority[e.first].second].getName() + " (ID = " + std::to_string(black_seekers[move_priority[e.first].second].getID()) + ") from Black Seekers to White Seekers.");
                    white_seekers.push_back(black_seekers[move_priority[e.second].second]);
                    b_remove.insert(move_priority[e.second].second);
                }
            }
        }
    }
    
    // reconstruct white seekers and black seekers
    for(int i = 0; i < white_seekers.size(); i++) {
        if(w_remove.contains(i)) continue;
        
        reconstructed_white_seekers.push_back(white_seekers[i]);
    }
    for(int i = 0; i < black_seekers.size(); i++) {
        if(b_remove.contains(i)) continue;
        
        reconstructed_black_seekers.push_back(black_seekers[i]);
    }
    
    white_seekers = reconstructed_white_seekers;
    black_seekers = reconstructed_black_seekers;
    
    // assert that groups are of equal length
    ASSERT(white_seekers.size() == black_seekers.size(), "white_seekers and black_seekers are not of equal length after shifters have been moved. This is a bug and should be unlikely to happen");
    
    // now resort the groups
    this->logExplanation("Sorting White Seekers and Black Seekers again...");
    this->sortGroupARO(&white_seekers, 0, ((int) white_seekers.size()) - 1);
//    Utils::sortPlayersRating(&black_seekers, 0, ((int) black_seekers.size()) - 1);
    std::sort(black_seekers.begin(), black_seekers.end(), [](const Player &a, const Player &b) {
        return a.getID() < b.getID();
    });
    
    this->logPlayers(white_seekers, "White Seekers after sorting by ARO:");
    this->logPlayers(black_seekers, "Black Seekers after sorting by pairing number:");
}

std::shared_ptr<CPPDubovSystem::Tournament::eval_games> CPPDubovSystem::Tournament::nextTransposition(std::vector<Player> &white_seekers, std::vector<Player> &black_seekers, int pof) {
    // get pivot point
    Player pivot = black_seekers[pof];
    // impose temporary pairing restriction to avoid infinite loops
//    white_seekers[pof].addPairingRestriction(pivot.getID());
    // construct black seekers up until pivot
    std::vector<Player> black_seekers_new;
    for(int i = 0; i < pof; i++) {
        black_seekers_new.push_back(black_seekers[i]);
    }
    
    // make sorted set of pivot point included
    std::vector<Player> pivot_include;
    for(int i = pof; i < black_seekers.size(); i++) {
        pivot_include.push_back(black_seekers[i]);
    }
    
    // make sure pivot_include is sorted properly
//    Utils::sortPlayersRating(&pivot_include, 0, ((int) pivot_include.size()) - 1);
    std::sort(pivot_include.begin(), pivot_include.end(), [](const Player &a, const Player &b){
        return a.getID() < b.getID();
    });
    bool touched_pivot = false;
    // pick first element in pivot_include which white seeker at element pof can player
    int works = -1;
    for(int i = 0; i < pivot_include.size(); i++) {
        if(!touched_pivot) {
            if(pivot_include[i].getID() == pivot.getID()) {
                touched_pivot = true;
            }
            continue;
        }
        if(white_seekers[pof].canPlayOpp(pivot_include[i]) && pivot_include[i].getID() != pivot.getID()) {
            // works
            works = i;
            break;
        }
    }
    
    // check if we found a useful player
    if(works == -1) {
        if(pof == 0) {
            // no more useful transpositions
            return nullptr;
        }
        // continue to search for a useful transposition
        return this->nextTransposition(white_seekers, black_seekers, pof - 1);
    }
    
    // try pairing and check for any other pairing errors
    // this can be accomplished by recreating the black seekers
    black_seekers_new.push_back(pivot_include[works]);
    // add all elements in pivot
    for(int i = 0; i < pivot_include.size(); i++) {
        if(i == works) {
            continue;
        }
        black_seekers_new.push_back(pivot_include[i]);
    }
    // try pairing bracket
    int first_problem = -1;
    eval_games transposition_eval;
    for(int i = 0; i < white_seekers.size(); i++) {
        if(!white_seekers[i].canPlayOpp(black_seekers_new[i])) {
            if(first_problem == -1) {
                first_problem = i;
            }
            transposition_eval.push_back(MatchEval(white_seekers[i], black_seekers_new[i], true));
        } else {
            transposition_eval.push_back(MatchEval(white_seekers[i], black_seekers_new[i], false));
        }
    }
    
    // check for problems
    if(first_problem == -1) {
        // no problem
        return std::make_shared<eval_games>(transposition_eval);
    }
    
    // try another transposition
    return this->nextTransposition(white_seekers, black_seekers_new, first_problem);
}

/*std::shared_ptr<CPPDubovSystem::Tournament::eval_games> CPPDubovSystem::Tournament::applyTranspositions(std::vector<Player> &white_seekers, std::vector<Player> &black_seekers) {
    //NOTE: THIS IS AN EXPERIMENTAL IMPLEMENTATION!
    // it is assumed that white_seekers and black_seekers are of equal length
    // set up a graph
    Graph g_main;
    std::vector<Player> p_get(white_seekers.size() + black_seekers.size());
    std::map<int, int> p_get_index;
    
    int vertex_id_white = 0;
    int vertex_id_black = ((int) white_seekers.size());
    
    std::vector<double> cost;
    
    int black_start_index = -1;
    std::vector<int> problems; // indecies of pairings that have a problem
    std::stack<int> no_problem;
    
    // first add all vertecies
    for(int i = 0; i < white_seekers.size(); i++) {
        g_main.AddVertex();
        g_main.AddVertex();
        p_get_index[white_seekers[i].getID()] = vertex_id_white;
        p_get[vertex_id_white] = white_seekers[i];
        p_get_index[black_seekers[i].getID()] = vertex_id_black;
        p_get[vertex_id_black] = black_seekers[i];
        
        vertex_id_black += 1;
        vertex_id_white += 1;
        
        if(!white_seekers[i].canPlayOpp(black_seekers[i])) {
            problems.push_back(i);
        } else {
            no_problem.push(i);
        }
    }
    
    std::stack<int> p;
    while(!no_problem.empty()) {
        p.push(no_problem.top());
        no_problem.pop();
    }
    
    no_problem = p;
    
    for(auto i : problems)
        no_problem.push(i);
    
    std::map<std::string, int> priorities_set; // all preset priorities
    
    int back_id = (int) black_seekers.size();
    
    // loop over all problems and set the priorities
    while(!no_problem.empty()) {
        int problem = no_problem.top();
        no_problem.pop();
        int priority_level = 1;
        
        // if current pairing is compatible, priority is 0
        if(white_seekers[problem].canPlayOpp(black_seekers[problem])) {
            g_main.AddEdge(p_get_index[white_seekers[problem].getID()], p_get_index[black_seekers[problem].getID()]);
            cost.push_back(0);
        }
        
        // go until end of pairing list
        for(int i = problem + 1; i < black_seekers.size(); i++) {
            // if the players are compatible, set the priority weight
            if(white_seekers[problem].canPlayOpp(black_seekers[i])) {
                g_main.AddEdge(p_get_index[white_seekers[problem].getID()], p_get_index[black_seekers[i].getID()]);
                // check the priority matching
                std::string priority_matching = std::to_string(white_seekers[problem].getID()) + "_" + std::to_string(black_seekers[i].getID());
                if(priorities_set[priority_matching] > 0 && priorities_set[priority_matching] < back_id) {
                    // insert that
                    cost.push_back(priorities_set[priority_matching]);
                    priority_level = std::max(priority_level, priorities_set[priority_matching]);
                } else {
                    cost.push_back(priority_level);
                    // create new priority
                    std::string priority_matching_set = std::to_string(white_seekers[i].getID()) + "_" + std::to_string(black_seekers[problem].getID());
                    priorities_set[priority_matching_set] = priority_level;
                }
            }
            
            // increment the priority level
            priority_level += 1;
        }
        
        // now go until the top
        for(int i = problem - 1; i >= 0; i--) {
            // do the same operation
            if(white_seekers[problem].canPlayOpp(black_seekers[i])) {
                g_main.AddEdge(p_get_index[white_seekers[problem].getID()], p_get_index[black_seekers[i].getID()]);
                std::string priority_matching = std::to_string(white_seekers[problem].getID()) + "_" + std::to_string(black_seekers[i].getID());
                if(priorities_set[priority_matching] > 0 && priorities_set[priority_matching] < back_id) {
                    // insert that
                    cost.push_back(priorities_set[priority_matching]);
                    priority_level = std::max(priority_level, priorities_set[priority_matching]);
                } else {
                    cost.push_back(priority_level);
                    // create new priority
                    std::string priority_matching_set = std::to_string(white_seekers[i].getID()) + "_" + std::to_string(black_seekers[problem].getID());
                    priorities_set[priority_matching_set] = priority_level;
                }
            }
            
            // increment the priority level
            priority_level += 1;
        }
    }
    
    // now that edge weights are all set, we do the matching
    Matching m(g_main);
    
    std::pair<std::list<int>, double> matched = m.SolveMinimumCostPerfectMatching(cost);
    
    // make sure matching exists
    if(matched.first.size() == 0) return nullptr;
    
    // go through matchings and add them to the return
    eval_games matchings;
    for(std::list<int>::iterator it = matched.first.begin(); it != matched.first.end(); it++) {
        std::pair<int, int> e = g_main.GetEdge(*it);
        bool is_white_first = e.first < black_start_index;
        
        //TODO: DO WE EVEN NEED THIS IF STATEMENT? THE CODE SEEMS TO WORK PERFECTLY FINE WITHOUT IT
        if(is_white_first) {
            matchings.push_back(MatchEval(p_get[e.first], p_get[e.second], false));
        } else {
            matchings.push_back(MatchEval(p_get[e.first], p_get[e.second], false));
            // no need for else statement, because this is simple bipartite matching
        }
    }
    
    return std::make_shared<eval_games>(matchings);
}*/

void CPPDubovSystem::Tournament::evaluateTranspositions(std::vector<MatchEval> *games_eval, std::vector<Player> &white_seekers, std::vector<Player> &black_seekers, bool *error) {
    // try matching
//    std::shared_ptr<eval_games> t2 = this->applyTranspositions(white_seekers, black_seekers);
//    if(t2 == nullptr) {
//        *error = true;
//        return;
//    } else {
//        *games_eval = *t2.get();
//    }
    // first find all the problem boards
    int problem_board = -1;
    for(int i = 0; i < games_eval->size(); i++) {
        if((*games_eval)[i].isProblem()) {
            problem_board = std::max(problem_board, i);
        }
    }
    
    // make sure there is at least one problem board
    if(problem_board == -1) {
        return;
    }
    
    // check how many boards we are at
    if(((int) games_eval->size()) == 1) {
        // we can't do anything with just one board!
        *error = true;
        return;
    }
    
    // apply transpositions in s2
    
//    bool error_occured = false;
    
//    std::vector<Player> ws_copy(white_seekers);
//    std::vector<Player> bs_copy(black_seekers);
    
    std::shared_ptr<std::vector<MatchEval>> tr = this->nextTransposition(white_seekers, black_seekers, problem_board);
    
    // check if transposition was found
    if(tr == nullptr) {
        // no transpositions available
        *error = true;
        return;
    }
    
    // set new games eval and recursively keep going until all boards have been satisfied
    *games_eval = *tr.get();
    
    //TODO: DO WE EVEN NEED TO DO THIS?
//    int failure = -1;
//    std::vector<Player> black_seekers_new;
//    for(int i = 0; i < games_eval->size(); i++) {
//        if((*games_eval)[i].isProblem()) {
//            if(failure == -1) {
//                failure = i;
//            }
//        }
//        black_seekers_new.push_back((*games_eval)[i].black);
//    }
//    if(failure > -1) {
//        this->evaluateTranspositions(games_eval, ws_copy, black_seekers_new, error);
//    }
}

std::vector<CPPDubovSystem::Match> CPPDubovSystem::Tournament::castToMatch(const std::vector<MatchEval> &games_eval) {
    std::vector<Match> m;
    // convert all the evaluated matches to the super class
    for(int i = 0; i < games_eval.size(); i++) {
        m.push_back(games_eval[i]);
    }
    
    // also perform optimization
    this->optimizeColors(&m);
    
    this->logMatches(m, "Games after optimizing colors:");
    
    return m;
}

std::vector<int> CPPDubovSystem::Tournament::generateMigrationQueue(const std::vector<Player> &for_group) const {
    std::vector<int> pq; // the queue
    
    if(for_group.size() == 0) return pq;
    
    // check group size
    if(for_group.size() % 2 == 0) {
        // group is even
        // we start from the middle and the priority increases from left to right
        int mid = ((int) for_group.size()) / 2;
        int left = mid - 1;
//        int right = mid + 1;
        int right = mid + 0;
        pq.push_back(left);
        pq.push_back(right);
        left -= 1;
        right += 1;
        while(left >= 0 && right < for_group.size()) {
            pq.push_back(left);
            pq.push_back(right);
            left -= 1;
            right += 1;
        }
    } else {
        // priority begins in the middle
        int n = (int) for_group.size();
        int mid = (int) floor(((double) n) / 2.0);
        pq.push_back(mid);
        // then we go left-right
        int left = mid - 1;
        int right = mid + 1;
        while(left >= 0 && right < n) {
            pq.push_back(left);
            pq.push_back(right);
            left -= 1;
            right += 1;
        }
    }
    
    return pq;
}

void CPPDubovSystem::Tournament::migratePlayers(std::vector<Player> *g1, std::vector<Player> *g2) {
    //NOTE: THIS FUNCTION HAS BEEN DEPRECATED! IT IS STILL HERE TEST HELP MAKE SOME TEST CASES
    // it is assumed that g1 and g2 are all properly sorted (wether by ARO or R)
    // 4.3.3 assign sequence numbers
    std::vector<int> sequence_nums = this->generateMigrationQueue(*g2);
    // keep moving players from g2 to g1 in the order given by sequence nums until groups are of equal size
    std::vector<Player> p_copy; // make a copy in case things don't work out well
    std::set<int> no_include;
    int n = (int) g2->size();
    while(n != g1->size() && sequence_nums.size() > 0) {
        int move = sequence_nums[0];
        sequence_nums.erase(sequence_nums.begin());
        Player p_move = (*g2)[move];
        p_copy.push_back(p_move);
        g1->push_back(p_move);
        n -= 1;
        no_include.insert(move);
    }
    // re-create g2
    std::vector<Player> g2_fixed;
    for(int i = 0; i < g2->size(); i++) {
        if(no_include.contains(i)) {
            continue;
        }
        g2_fixed.push_back((*g2)[i]);
    }
    *g2 = g2_fixed;
}

std::queue<CPPDubovSystem::Player> CPPDubovSystem::Tournament::generateFloatQueue(LinkedListNode &next_group, int color_imbalance) const {
    std::queue<Player> pq;
    
    LinkedListNode *temp = &next_group;
    std::queue<Player> pq_temp;
    std::queue<Player> max_upfloat;
    std::queue<Player> float_prev;
    
    while(temp != nullptr) {
        // first what is the color situation like?
        if(color_imbalance == 0) {
            // it seems like colors are completely balanced
            // so the whole group is added to the queue in the order we enter it
            for(auto i : temp->data) {
                if(i.upfloatedPreviously() && this->current_round != this->total_rounds) {
                    float_prev.push(i);
                    continue;
                } else if(!i.canUpfloat(this->current_round) && this->current_round != this->total_rounds) {
                    max_upfloat.push(i);
                    continue;
                }
                pq.push(i);
            }
            
            // insert max_upfloat first
            while(!max_upfloat.empty()) {
                pq.push(max_upfloat.front());
                max_upfloat.pop();
            }
            
            // then float_prev
            while(!float_prev.empty()) {
                pq.push(float_prev.front());
                float_prev.pop();
            }
            
            temp = temp->next;
            continue;
        }
        
        // we need to select floaters in a way where the group that needs the color (to balance out with the other group) has the priority of selection
        if(color_imbalance == 1) {
            // there are more whites
            for(auto i : temp->data) {
                if(i.getDueColor() == Color::WHITE) {
                    pq_temp.push(i);
                    continue;
                } else if(i.upfloatedPreviously() && this->current_round != this->total_rounds) {
                    float_prev.push(i);
                    continue;
                } else if(!i.canUpfloat(this->current_round) && this->current_round != this->total_rounds) {
                    max_upfloat.push(i);
                    continue;
                }
                pq.push(i);
            }
            
            // now add anything in the other queue
            while(!pq_temp.empty()) {
                pq.push(pq_temp.front());
                pq_temp.pop();
            }
        } else {
            // there are more blacks
            for(auto i : temp->data) {
                if(i.getDueColor() == Color::BLACK) {
                    pq_temp.push(i);
                    continue;
                } else if(i.upfloatedPreviously() && this->current_round != this->total_rounds) {
                    float_prev.push(i);
                    continue;
                } else if(!i.canUpfloat(this->current_round) && this->current_round != this->total_rounds) {
                    max_upfloat.push(i);
                    continue;
                }
                pq.push(i);
            }
            
            // now add anything in the other queue
            while(!pq_temp.empty()) {
                pq.push(pq_temp.front());
                pq_temp.pop();
            }
        }
        
        // insert max_upfloat first
        while(!max_upfloat.empty()) {
            pq.push(max_upfloat.front());
            max_upfloat.pop();
        }
        
        // then float_prev
        while(!float_prev.empty()) {
            pq.push(float_prev.front());
            float_prev.pop();
        }
        
        // continue to the next group
        temp = temp->next;
    }
    
    return pq;
}

/*std::set<int> Tournament::findMultiUpfloaters(std::vector<Player> &white_seekers, std::vector<Player> &black_seekers, LinkedListNode next_group, bool *error) {
    // there could be a number of possible upfloaters
    // we might need 2, 4, or just upfloat an entire group just to make sure a group can be paired
    // instead of trying every single possible combination of upfloaters, we are going to have to make use of minimum weight matching
    // so just like when working with exchanges, the goal isn't to get exact pairings through the minimum weight matching
    // but rather get an idea of whome to upfloat (use it to find the optimal selection)
    CPPDSGraphUtil::Graph g_main;
    int max_p = ((int) white_seekers.size()) + ((int) black_seekers.size());
//    int max_start = max_p + 0;
//    std::map<int, int> p_map;
//    std::map<int, int> p_mapr;
//    int loc = 0;
    // add vertecies for all players
    std::vector<int> kv;
    for(int i = 0; i < white_seekers.size(); i++) {
        g_main.AddVertex(white_seekers[i].getID());
        kv.push_back(white_seekers[i].getID());
//        p_map[white_seekers[i].getID()] = loc;
//        p_mapr[loc] = white_seekers[i].getID();
//        loc += 1;
    }
    // add black seekers vertecies separately because we cannot assume that white_seekers and black_seekers are of equal size
    for(int i = 0; i < black_seekers.size(); i++) {
        g_main.AddVertex(black_seekers[i].getID());
        kv.push_back(black_seekers[i].getID());
//        p_map[black_seekers[i].getID()] = loc;
//        p_mapr[loc] = black_seekers[i].getID();
//        loc += 1;
    }
    
    // add lower groups
    LinkedListNode ng_copy(next_group); // make copy as we will need to reuse this again
    LinkedListNode *ng_copy_next = &ng_copy;
    while(ng_copy_next != nullptr) {
        for(int i = 0; i < ng_copy_next->data.size(); i++) {
            g_main.AddVertex(ng_copy_next->data[i].getID());
            max_p += 1;
//            p_map[ng_copy_next->data[i].getID()] = loc;
//            p_mapr[loc] = ng_copy_next->data[i].getID();
//            loc += 1;
        }
        ng_copy_next = ng_copy_next->next;
    }
    
    Graph2 gg(max_p);
//    int temp1 = 0;
//    while(temp1 < max_p) {
//        gg.AddVertex();
//        temp1 += 1;
//    }
    
    // now to assign edge weights
    // the trick here is straight forward
    // white_seekers and black_seekers should have the smallest edge weight
    // their edge weight doesn't matter too much entirely, as long as they have the smallest edge weight than all the other groups (to encourage those players to get paired up)
    // players with opposite due colors should have smaller edge weights than larger ones
    // lastly, minimize the pairing score difference
    
    // first set the edge weights for the group
    int max_weight = 0;
    std::set<int> original;
//    std::vector<double> cost(max_p);
    for(int i = 0; i < white_seekers.size(); i++) {
        for(int z = 0; z < black_seekers.size(); z++) {
            // make sure both players can play each other
            if(!white_seekers[i].canPlayOpp(black_seekers[i])) {
                continue;
            }
            g_main.AddEdge(white_seekers[i].getID(), black_seekers[i].getID(), 0); // once again, edge weight here doesn't matter too much, we just want to make sure it it lower than other groups
//            gg.AddEdge(p_map[white_seekers[i].getID()], p_map[black_seekers[i].getID()]);
//            cost[gg.GetEdgeIndex(p_map[white_seekers[i].getID()], p_map[black_seekers[i].getID()])] = 0;
            max_weight = std::max(max_weight, i + z);
        }
    }
    for(int i = 0; i < white_seekers.size(); i++) {
        for(int z = i + 1; z < white_seekers.size(); z++) {
            if(!white_seekers[i].canPlayOpp(white_seekers[z])) {
                continue;
            }
            g_main.AddEdge(white_seekers[i].getID(), white_seekers[z].getID(), 0);
//            gg.AddEdge(p_map[white_seekers[i].getID()], p_map[white_seekers[z].getID()]);
//            cost[gg.GetEdgeIndex(p_map[white_seekers[i].getID()], p_map[white_seekers[z].getID()])] = 0;
            max_weight = std::max(max_weight, i + z + max_weight);
        }
        original.insert(white_seekers[i].getID());
    }
    for(int i = 0; i < black_seekers.size(); i++) {
        for(int z = i + 1; z < black_seekers.size(); z++) {
            if(!black_seekers[i].canPlayOpp(black_seekers[z])) {
                continue;
            }
            g_main.AddEdge(black_seekers[i].getID(), black_seekers[z].getID(), 0);
//            gg.AddEdge(p_map[black_seekers[i].getID()], p_map[black_seekers[z].getID()]);
//            cost[gg.GetEdgeIndex(p_map[black_seekers[i].getID()], p_map[black_seekers[z].getID()])] = 0;
            max_weight = std::max(max_weight, i + z + max_weight);
        }
        original.insert(black_seekers[i].getID());
    }
    
    // now do the lower groups
    LinkedListNode mt(next_group);
    LinkedListNode* temp = &mt;
    
    // time complexity: s * n^2 where is is the number of remaining groups and n is the number of players in each group
    while(temp != nullptr) {
        // increment max_weight
        max_weight += 100;
        // we must minimize the number of players who don't get their due color
        // so split up the groups
        std::vector<Player> lws;
        std::vector<Player> lbs;
        Utils::splitGroups(&lws, &lbs, temp->data);
        
        // once again, the lower group seekers could be different sizes
        for(int i = 0; i < white_seekers.size(); i++) {
            for(int z = 0; z < lbs.size(); z++) {
                if(!white_seekers[i].canPlayOpp(lbs[z])) {
                    continue;
                }
                g_main.AddEdge(white_seekers[i].getID(), lbs[z].getID(), max_weight + z);
//                gg.AddEdge(p_map[white_seekers[i].getID()], p_map[lbs[z].getID()]);
//                cost[gg.GetEdgeIndex(p_map[white_seekers[i].getID()], p_map[lbs[z].getID()])] = max_weight + z;
                max_weight += 10;
//                max_weight += z + 10;
            }
            for(int z = 0; z < lws.size(); z++) {
                if(!white_seekers[i].canPlayOpp(lws[z])) {
                    continue;
                }
                g_main.AddEdge(white_seekers[i].getID(), lws[z].getID(), max_weight + z + 1);
//                gg.AddEdge(p_map[white_seekers[i].getID()], p_map[lws[z].getID()]);
//                cost[gg.GetEdgeIndex(p_map[white_seekers[i].getID()], p_map[lws[z].getID()])] = max_weight + z + 1;
                max_weight = std::max(max_weight, max_weight + 20);
//                max_weight += z + 20;
            }
        }
        for(int i = 0; i < black_seekers.size(); i++) {
            for(int z = 0; z < lws.size(); z++) {
                if(!black_seekers[i].canPlayOpp(lws[z])) {
                    continue;
                }
                g_main.AddEdge(black_seekers[i].getID(), lws[z].getID(), max_weight + z);
//                gg.AddEdge(p_map[black_seekers[i].getID()], p_map[lws[z].getID()]);
//                cost[gg.GetEdgeIndex(p_map[black_seekers[i].getID()], p_map[lws[z].getID()])] = max_weight + z;
                max_weight = std::max(max_weight, max_weight + 10);
//                max_weight += z + 10;
            }
            for(int z = 0; z < lbs.size(); z++) {
                if(!black_seekers[i].canPlayOpp(lbs[z])) {
                    continue;
                }
                g_main.AddEdge(black_seekers[i].getID(), lbs[z].getID(), max_weight + z + 1);
//                gg.AddEdge(p_map[black_seekers[i].getID()], p_map[lbs[z].getID()]);
//                cost[gg.GetEdgeIndex(p_map[black_seekers[i].getID()], p_map[lbs[z].getID()])] = max_weight + z + 1;
                max_weight = std::max(max_weight, max_weight + 20);
//                max_weight += z + 20;
            }
        }
        
        int ow = 0;
        
        // we lastly need to set up edge weights for this group
        for(int i = 0; i < lws.size(); i++) {
            for(int z = 0; z < lbs.size(); z++) {
                if(!lws[i].canPlayOpp(lbs[z])) {
                    continue;
                }
                g_main.AddEdge(lws[i].getID(), lbs[z].getID(), 0);
//                gg.AddEdge(p_map[lws[i].getID()], p_map[lbs[z].getID()]);
//                cost[gg.GetEdgeIndex(p_map[lws[i].getID()], p_map[lbs[z].getID()])] = 0;
                ow = std::max(ow, i + z);
//                max_weight += z;
                max_weight += i + z;
            }
            for(int z = i + 1; z < lws.size(); z++) {
                if(!lws[i].canPlayOpp(lws[z])) {
                    continue;
                }
                g_main.AddEdge(lws[i].getID(), lws[z].getID(), 0);
//                gg.AddEdge(p_map[lws[i].getID()], p_map[lws[z].getID()]);
//                cost[gg.GetEdgeIndex(p_map[lws[i].getID()], p_map[lws[z].getID()])] = max_weight + 0;
                ow = std::max(ow, ow + i + z);
                max_weight += ow + i + z;
            }
            // add to ws to pair in lower brackets
            white_seekers.push_back(lws[i]);
        }
        for(int i = 0; i< lbs.size(); i++) {
            for(int z = i + 1; z < lbs.size(); z++) {
                if(!lbs[i].canPlayOpp(lbs[z])) {
                    continue;
                }
                g_main.AddEdge(lbs[i].getID(), lbs[z].getID(), 0);
//                gg.AddEdge(p_map[lbs[i].getID()], p_map[lbs[z].getID()]);
//                cost[gg.GetEdgeIndex(p_map[lbs[i].getID()], p_map[lbs[z].getID()])] = 0;
                ow = std::max(ow, ow + i + z);
                max_weight += ow + i + z;
            }
            black_seekers.push_back(lbs[i]);
        }
        // go to next group
        temp = temp->next;
    }
    
    // now apply minimum weight matching
    Matching mm(g_main);
    
    // add key vertecies
    for(auto i : kv) {
        mm.addKeyVertex(i);
    }
    
    std::vector<std::pair<int, int>> matched = mm.computeMinimumWeightMatching(true);
//    Matching2 mm(gg);
//    std::pair<std::list<int>, double> matched = mm.computeMinimumWeightMatching(cost);
    
    // make sure a matching exists
    int n = ((int) white_seekers.size()) + ((int) black_seekers.size());
    int nn = (int) matched.size();
    if(nn != (n / 2)) {
        // no matching exists
        *error = true;
        return std::set<int>();
    }
    
    std::set<int> upfloaters;
    
    // run through all the match ups and note down all match ups which contain upfloaters
    int num_in = 0;
    for(int i = 0; i < matched.size(); i++) {
        if(original.contains(matched[i].first) && !original.contains(matched[i].second)) {
            // upfloater found!
            num_in += 1;
            upfloaters.insert(matched[i].second);
        } else if(!original.contains(matched[i].first) && original.contains(matched[i].second)) {
            // upfloater found!
            num_in += 1;
            upfloaters.insert(matched[i].first);
        } else if(original.contains(matched[i].first) && original.contains(matched[i].second)) {
            num_in += 2;
        }
        // check if the whole group was paired
        if(num_in == n) {
            // no reason to waist more operations checking the rest of the matched vector
            break;
        }
    }
    
    return upfloaters;
}*/

std::set<int> CPPDubovSystem::Tournament::findMultiUpfloatersNP(std::vector<Player> &white_seekers, std::vector<Player> &black_seekers, LinkedListNode &next_group, Graph &g, std::vector<double> &cost, const std::map<int, int> &p_convert, const std::vector<Player> &p_reverse) {
    // it is assumed that white_seekers and black_seekers are of equal length
    // for each group, split up into white seekers and black seekers
    
    // do lower groups
    LinkedListNode *temp = &next_group;
    
    int max_priority = 1;
    
    std::vector<Player> merged;
    std::set<int> g_in;
    bool is_first = true;
    
    while(temp != nullptr) {
        // divide into ws and bs
        std::vector<Player> ws;
        std::vector<Player> bs;
        this->splitGroups(&ws, &bs, temp->data);
        
        int priority = max_priority + 0;
        int second_priority = priority + ((int) bs.size());
        // white priority on black should be what priority is
        // and on same color should be higher
        for(auto i : white_seekers) {
            if(is_first)
                g_in.insert(i.getID());
            for(auto z : bs) {
                if(i.canPlayOpp(z)) {
                    g.AddEdge(p_convert.at(i.getID()), p_convert.at(z.getID()));
                    // for determining cost, check if z has upfloated previosly or is max upfloater
                    if(z.upfloatedPreviously() || !z.canUpfloat(this->total_rounds)) {
                        cost.push_back(second_priority);
                        second_priority += 1;
                    } else {
                        cost.push_back(priority);
                    }
                }
                merged.push_back(z);
                priority += 1;
            }
            priority = std::max(priority, second_priority);
            second_priority = priority + ((int) ws.size());
            for(auto z : ws) {
                if(i.canPlayOpp(z)) {
                    g.AddEdge(p_convert.at(i.getID()), p_convert.at(z.getID()));
                    if(z.upfloatedPreviously() || !z.canUpfloat(this->total_rounds)) {
                        cost.push_back(second_priority);
                        second_priority += 1;
                    } else {
                        cost.push_back(priority);
                    }
                }
                merged.push_back(z);
                priority += 1;
            }
        }
        
        priority = max_priority + 0;
        second_priority = priority + ((int) ws.size());
        // no do black_seekers
        for(auto i : black_seekers) {
            if(is_first)
                g_in.insert(i.getID());
            for(auto z : ws) {
                if(i.canPlayOpp(z)) {
                    g.AddEdge(p_convert.at(i.getID()), p_convert.at(z.getID()));
                    if(z.upfloatedPreviously() || !z.canUpfloat(this->total_rounds)) {
                        cost.push_back(second_priority);
                        second_priority += 1;
                    } else {
                        cost.push_back(priority);
                    }
                }
                priority += 1;
            }
            priority = std::max(priority, second_priority);
            second_priority = priority + ((int) bs.size());
            for(auto z : bs) {
                if(i.canPlayOpp(z)) {
                    g.AddEdge(p_convert.at(i.getID()), p_convert.at(z.getID()));
                    if(z.upfloatedPreviously() || !z.canUpfloat(this->total_rounds)) {
                        cost.push_back(second_priority);
                        second_priority += 1;
                    } else {
                        cost.push_back(priority);
                    }
                }
                priority += 1;
            }
        }
        
        is_first = false;
        
        // set new max priority
        max_priority = priority + 1;
        
        // shift temp
        temp = temp->next;
    }
    
    // do rest of players
    for(int i = 0; i < merged.size(); i++) {
        for(int z = i + 1; z < merged.size(); z++) {
            if(merged[i].canPlayOpp(merged[z])) {
                g.AddEdge(p_convert.at(merged[i].getID()), p_convert.at(merged[z].getID()));
                cost.push_back(0);
            }
        }
    }
    
    // we don't need vec anymore
    merged.clear();
    
    // do matching
    Matching m(g);
    
    std::pair<std::list<int>, double> matched = m.SolveMinimumCostPerfectMatching(cost);
    
    if(matched.first.size() == 0) return std::set<int>(); // failure
    
    // loop over matchings
    int num_in = 0;
    int complete = ((int) white_seekers.size()) * 2;
    
    std::set<int> upfloaters;
    // pick up upfloaters
    for(std::list<int>::iterator it = matched.first.begin(); it != matched.first.end(); it++) {
        std::pair<int, int> e = g.GetEdge(*it);
        if(g_in.contains(p_reverse[e.first].getID()) && !g_in.contains(p_reverse[e.second].getID())) {
            // upfloater found!
            num_in += 1;
            upfloaters.insert(p_reverse[e.second].getID());
        } else if(!g_in.contains(p_reverse[e.first].getID()) && g_in.contains(p_reverse[e.second].getID())) {
            // upfloater found!
            num_in += 1;
            upfloaters.insert(p_reverse[e.first].getID());
        } else if(g_in.contains(p_reverse[e.first].getID()) && g_in.contains(p_reverse[e.second].getID())) {
            num_in += 2;
        }
        // check if the whole group was paired
        if(num_in == complete) {
            // no reason to waist more operations checking the rest of the matched vector
            break;
        }
    }
    
    return upfloaters;
}

std::set<int> CPPDubovSystem::Tournament::findMultiUpfloaters(std::vector<Player> &white_seekers, std::vector<Player> &black_seekers, LinkedListNode &next_group, bool &error) {
    // first initialize the graph
    Graph g_main(0);
    std::map<int, int> p_convert;
    std::vector<Player> p_reverse;
    std::set<int> g_in;
    
    int current_spot = 0;
    
    // add white seekers to graph
    for(int i = 0; i < white_seekers.size(); i++) {
        g_main.AddVertex();
        p_convert[white_seekers[i].getID()] = current_spot;
        p_reverse.push_back(white_seekers[i]);
        current_spot += 1;
        g_in.insert(white_seekers[i].getID());
    }
    
    // add black seekers to graph
    for(int i = 0; i < black_seekers.size(); i++) {
        g_main.AddVertex();
        p_convert[black_seekers[i].getID()] = current_spot;
        p_reverse.push_back(black_seekers[i]);
        current_spot += 1;
        g_in.insert(black_seekers[i].getID());
    }
    
    // also add subsequent players
    LinkedListNode *temp1 = &next_group;
    while(temp1 != nullptr) {
        for(int i = 0; i < temp1->data.size(); i++) {
            g_main.AddVertex();
            p_convert[temp1->data[i].getID()] = current_spot;
            p_reverse.push_back(temp1->data[i]);
            current_spot += 1;
        }
        
        temp1 = temp1->next;
    }
    
    // first set up edge weights to current group
    std::vector<double> cost;
    std::vector<Player> merged;
//    std::merge(white_seekers.begin(), white_seekers.end(), black_seekers.begin(), black_seekers.end(), merged.begin());
    for(auto i : white_seekers)
        merged.push_back(i);
    for(auto i : black_seekers)
        merged.push_back(i);
    
    // for any players within the current group which are elgible to play each other, give them an edge
    for(int i = 0; i < merged.size(); i++) {
        for(int z = i + 1; z < merged.size(); z++) {
            // make sure both players are compatible
            if(merged[i].canPlayOpp(merged[z])) {
                g_main.AddEdge(p_convert[merged[i].getID()], p_convert[merged[z].getID()]);
                cost.push_back(0); // edge weight here doesn't matter too much. We aren't trying to get exact pairings, just get an idea of who to upfloat
            }
        }
    }
    
    // get imbalance of the group
    int imbalance = 0;
    if(white_seekers.size() > black_seekers.size()) imbalance = 1;
    else if(white_seekers.size() < black_seekers.size()) imbalance = -1;
    
    if(imbalance == 0) {
        // have helper function take care of this
        std::set<int> res = this->findMultiUpfloatersNP(white_seekers, black_seekers, next_group, g_main, cost, p_convert, p_reverse);
        
        if(res.empty()) {
            // failure
            error = true;
            return res;
        }
        
        return res; // success
    }
    
    // now do the lower groups
    std::queue<Player> floater_queue = this->generateFloatQueue(next_group, imbalance);
    std::vector<Player> q_as_vec;
    
    int current_weight = 0;
    // okay now make the edge weights for this group
    while(!floater_queue.empty()) {
        Player p_check = floater_queue.front();
        q_as_vec.push_back(p_check);
        
        for(int i = 0; i < merged.size(); i++) {
            // make sure players are compatible
            if(merged[i].canPlayOpp(p_check)) {
                g_main.AddEdge(p_convert[p_check.getID()], p_convert[merged[i].getID()]);
                cost.push_back(current_weight);
            }
        }
        
        // increment current weight
        current_weight += 1;
        floater_queue.pop();
    }
    
    // finally try making edges to the players itself
    for(int i = 0; i < q_as_vec.size(); i++) {
        for(int z = i + 1; z < q_as_vec.size(); z++) {
            if(q_as_vec[i].canPlayOpp(q_as_vec[z])) {
                g_main.AddEdge(p_convert[q_as_vec[i].getID()], p_convert[q_as_vec[z].getID()]);
                cost.push_back(0); // once again, edge weight here doesn't matter what so ever
            }
        }
    }
    
    // okay now make the matching
    Matching matching(g_main);
    std::pair<std::list<int>, double> matched = matching.SolveMinimumCostPerfectMatching(cost);
    
    // check if matching was a success
    if(matched.first.size() == 0) {
        // failed
        error = true;
        return std::set<int>();
    }
    
    int num_in = 0;
    
    std::set<int> upfloaters;
    // pick up upfloaters
    std::string possible_upfloaters = "Found upfloaters that satisfy legal pairings! From the given list, after trying different upfloaters (in the given order from the list), the following produces the best pairings:";
    for(std::list<int>::iterator it = matched.first.begin(); it != matched.first.end(); it++) {
        std::pair<int, int> e = g_main.GetEdge(*it);
        if(g_in.contains(p_reverse[e.first].getID()) && !g_in.contains(p_reverse[e.second].getID())) {
            // upfloater found!
            num_in += 1;
            upfloaters.insert(p_reverse[e.second].getID());
            possible_upfloaters += "\n- Player with ID = " + std::to_string(p_reverse[e.second].getID()) + ", Name = " + p_reverse[e.second].getName() + ", Points = " + std::to_string(p_reverse[e.second].getPoints());
        } else if(!g_in.contains(p_reverse[e.first].getID()) && g_in.contains(p_reverse[e.second].getID())) {
            // upfloater found!
            num_in += 1;
            upfloaters.insert(p_reverse[e.first].getID());
            possible_upfloaters += "\n- Player with ID = " + std::to_string(p_reverse[e.second].getID()) + ", Name = " + p_reverse[e.second].getName() + ", Points = " + std::to_string(p_reverse[e.second].getPoints());
        } else if(g_in.contains(p_reverse[e.first].getID()) && g_in.contains(p_reverse[e.second].getID())) {
            num_in += 2;
        }
        // check if the whole group was paired
        if(num_in == ((int) merged.size())) {
            // no reason to waist more operations checking the rest of the matched vector
            break;
        }
    }
    
    this->logExplanation(possible_upfloaters);
    
    return upfloaters;
}

std::set<int> CPPDubovSystem::Tournament::findOptimalUpfloaters(std::vector<Player> &white_seekers, std::vector<Player> &black_seekers, LinkedListNode &next_group, bool &error) {
    // initialize all vertecies in the graph
    Graph g_main(0);
    
    // a way to identify all players
    // the key is the id of the player
    // the value is the vertex id (or index)
    std::unordered_map<int, int> p_convert;
    // all players a part of the current score group
    // this would be all players in ws and bs
    std::unordered_set<int> in_scoregroup;
    // where all players are
    // this is used to just get info
    std::vector<Player> all_players;
    // needed later
    int scoregroup_size = ((int) white_seekers.size()) + ((int) black_seekers.size());
    
    // first add players in ws and bs
    int valid_vertex_id = 0;
    
    for(int i = 0; i < white_seekers.size(); i++) {
        // add vertex
        g_main.AddVertex();
        // set the id
        p_convert[white_seekers[i].getID()] = valid_vertex_id;
        // mark this player as one in the score group
        in_scoregroup.insert(white_seekers[i].getID());
        // add to all players
        // what's nice is that the vertex id can identify the index here too!
        all_players.push_back(white_seekers[i]);
        
        // increment valid vertex id as needed
        valid_vertex_id++;
    }
    
    for(int i = 0; i < black_seekers.size(); i++) {
        g_main.AddVertex();
        // same operation as ws
        p_convert[black_seekers[i].getID()] = valid_vertex_id;
        in_scoregroup.insert(black_seekers[i].getID());
        all_players.push_back(black_seekers[i]);
        valid_vertex_id++;
    }
    
    // now add everyone in the lower score groups
    LinkedListNode *temp1 = &next_group;
    int begin_index = -1; // the index in all_players where non-residents of the current scoregroup are
    while(temp1 != nullptr) {
        // go through all players
        for(int i = 0; i < temp1->data.size(); i++) {
            // add vertex
            g_main.AddVertex();
            
            // add id
            p_convert[temp1->data[i].getID()] = valid_vertex_id;
            
            // add to all players
            all_players.push_back(temp1->data[i]);
            
            // set begin index as needed
            if(begin_index == -1) begin_index = valid_vertex_id;
            
            // increment valid id
            valid_vertex_id++;
        }
        
        // shift to next score group
        temp1 = temp1->next;
    }
    
    // set up the edge weights
    
    std::vector<double> cost; // edge weights
    
    // add all the internal edges
    // that is set up edges within the current score group
    for(int i = 0; i < white_seekers.size(); i++) {
        for(int z = 0; z < black_seekers.size(); z++) {
            // are these two players compatible?
            if(white_seekers[i].canPlayOpp(black_seekers[z])) {
                g_main.AddEdge(p_convert[white_seekers[i].getID()], p_convert[black_seekers[z].getID()]);
                cost.push_back(0); // make this pairing seem very favorable
            }
        }
        
        // also pair against self
        for(int z = i + 1; z < white_seekers.size(); z++) {
            if(white_seekers[i].canPlayOpp(white_seekers[z])) {
                g_main.AddEdge(p_convert[white_seekers[i].getID()], p_convert[white_seekers[z].getID()]);
                cost.push_back(1); // just a small lift to make the pairing seem less favorable
            }
        }
    }
    
    for(int i = 0; i < black_seekers.size(); i++) {
        for(int z = i + 1; z < black_seekers.size(); z++) {
            if(black_seekers[i].canPlayOpp(black_seekers[z])) {
                g_main.AddEdge(p_convert[black_seekers[i].getID()], p_convert[black_seekers[z].getID()]);
                cost.push_back(1);
            }
        }
    }
    
    // now do the lower scoregroups
    // first try pairing all lower players against the current score group
    for(int i = 0; i < begin_index; i++) { // this captures all players in current score group
        for(int z = begin_index; z < all_players.size(); z++) {
            // are both players compatible?
            if(all_players[i].canPlayOpp(all_players[z])) {
                g_main.AddEdge(p_convert[all_players[i].getID()], p_convert[all_players[z].getID()]);
                
                // now to determine edge weight
                double edge_weight = 0;
                
                // 2.3.6 [C10] minimize upfloaters who upfloated in the prev round (unless it is the last round of course)
                if(all_players[z].upfloatedPreviously() && this->current_round != this->total_rounds) {
                    edge_weight  = 1; // make less favorable
                }
                
                // 2.3.5 [C9] minimize number of max upfloaters who upfloated while being a max upfloater (unless it is the last round)
                if(!all_players[z].canUpfloat(this->total_rounds) && this->current_round != this->total_rounds) {
                    edge_weight += all_players[z].getNumUpfloatedIfMaxUpfloater(this->total_rounds); // make less favorable
                }
                
                // 2.3.4 [C8] minimize number of max upfloaters (unless it is the last round)
                if(!all_players[z].canUpfloat(this->total_rounds) && this->current_round != this->total_rounds) {
                    edge_weight += 2; // make less favorable
                }
                
                // 2.3.3 [C7] minimize players who don't get their due color
                if(all_players[i].getDueColor() == all_players[z].getDueColor() || (all_players[i].getDueColor() == Color::BLACK && all_players[z].getDueColor() == Color::NO_COLOR)) {
                    edge_weight += 10;
                }
                
                // 2.3.2 [C6] minimize score differences
                double score_diff = all_players[i].getPoints() - all_players[z].getPoints();
                edge_weight += score_diff * 10;
                
                // 2.3.1 [C5] (minimize upfloaters) is already handled
                edge_weight += ((double) z) / 10.0; // this is our tiebreak so players with a smaller TPN are likely to be chosen
                
                cost.push_back(edge_weight);
            }
        }
    }
    
    // now set edge weights for lower score groups
    for(int i = begin_index; i < all_players.size(); i++) {
        for(int z = i + 1; z < all_players.size(); z++) {
            if(all_players[i].canPlayOpp(all_players[z])) {
                g_main.AddEdge(p_convert[all_players[i].getID()], p_convert[all_players[z].getID()]);
                cost.push_back(0);
            }
        }
    }
    
    // apply the matching
    Matching matching(g_main);
    std::pair<std::list<int>, double> matched = matching.SolveMinimumCostPerfectMatching(cost);
    
    // were we successful?
    if(matched.first.empty()) {
        error = true;
        return std::set<int>();
    }
    
    // extract the matching
    // and return the upfloater ids
    int num_in = 0; // easy convenient way of being able to tell if we got all players in the score group paired
    
    std::set<int> upfloaters;
    for(std::list<int>::iterator it = matched.first.begin(); it != matched.first.end(); it++) {
        // get the edge
        std::pair<int, int> e = g_main.GetEdge(*it);
        
        // check if either item is in the score group
        if(in_scoregroup.contains(all_players[e.first].getID()) && !in_scoregroup.contains(all_players[e.second].getID())) {
            // upfloater found!
            num_in++; // only one score group member found
            // add id of upfloater
            upfloaters.insert(all_players[e.second].getID());
        } else if(!in_scoregroup.contains(all_players[e.first].getID()) && in_scoregroup.contains(all_players[e.second].getID())) {
            // upfloater found!
            num_in++; // only one score group member found
            // add id of upfloater
            upfloaters.insert(all_players[e.first].getID());
        } else if(in_scoregroup.contains(all_players[e.first].getID()) && in_scoregroup.contains(all_players[e.second].getID())) {
            // both players are part of the score group!
            num_in += 2; // since both players are in the score group
        }
        
        if(num_in == scoregroup_size) break; // we got all players in score group
    }
    
    return upfloaters;
}

//MARK: LOGGING FUNCTIONS
void CPPDubovSystem::Tournament::logExplanation(const std::string &explanation) {
    // only log if enabled
    if(this->enable_logging) this->logger.log(explanation);
}

void CPPDubovSystem::Tournament::logPlayers(const std::vector<Player> &players, const std::string &extra_explanation) {
    // if not applicable, then skip
    if(!this->enable_logging) return;
    std::string p_list; // where all players will be stored
    
    // add extra explanation as needed
    p_list += extra_explanation;
    
    auto player_representer = [](Player &p) {
        std::string due_color_str = "White";
        std::string strength = "No Preference";
        if(p.getDueColor() == Color::BLACK)
            due_color_str = "Black";
        else if(p.getDueColor() == Color::NO_COLOR)
            due_color_str = "No Due Color";
        if(p.getPreferenceStrength() == ColorPreference::ALTERNATION)
            strength = "Alternation";
        else if(p.getPreferenceStrength() == ColorPreference::MILD)
            strength = "Mild";
        else if(p.getPreferenceStrength() == ColorPreference::ABSOLUTE)
            strength = "Absolute";
        return p.getName() + " (ID = " + std::to_string(p.getID()) + ") -- Points = " + std::to_string(p.getPoints()) + " -- ARO = " + std::to_string(p.getARO()) + " -- Rating = " + std::to_string(p.getRating()) + " -- Due Color: " + due_color_str + " -- Due Color Strength: " + strength;
    };
    
    if(!players.empty()) {
        Player p1 = players[0];
        if(!p_list.empty())
            p_list += "\n1. " + player_representer(p1);
        else
            p_list = "1. " + player_representer(p1);
        for(int i = 1; i < players.size(); i++) {
            p1 = players[i];
            p_list += "\n" + std::to_string(i + 1) + ". " + player_representer(p1);
        }
        
        // finally log
        this->logExplanation(p_list);
    }
}

void CPPDubovSystem::Tournament::logMatches(const std::vector<Match> &games, const std::string &heading) {
    // make sure we are logging
    if(!this->enable_logging) return;
    
    // make sure we have something to log
    if(games.empty()) return;
    
    // go through and log all players in the game
    
    std::string games_to_log = heading + "";
    
    for(auto i : games)
        if(i.is_bye)
            games_to_log += "\n" + i.white.getName() + " (ID = " + std::to_string(i.white.getID()) + ") - BYE";
        else
            games_to_log += "\n" + i.white.getName() + " (ID = " + std::to_string(i.white.getID()) + ") (white) - " + i.black.getName() + "(ID = " + std::to_string(i.black.getID()) + ")";
    
    this->logExplanation(games_to_log);
}

void CPPDubovSystem::Tournament::logMatches(const std::vector<MatchEval> &games, const std::string &heading) {
    // make sure we are logging
    if(!this->enable_logging) return;
    
    // make sure we have something to log
    if(games.empty()) return;
    
    // go through and log all players in the game
    
    std::string games_to_log = heading + "";
    
    for(auto i : games)
        if(i.is_bye)
            games_to_log += "\n" + i.white.getName() + " (ID = " + std::to_string(i.white.getID()) + ") - BYE -> OK";
        else
            games_to_log += "\n" + i.white.getName() + " (ID = " + std::to_string(i.white.getID()) + ") (white) - " + i.black.getName() + "(ID = " + std::to_string(i.black.getID()) + ") -> " + (i.isProblem() ? "Pairing not compatible!" : "OK");
    
    this->logExplanation(games_to_log);
}

void CPPDubovSystem::Tournament::logScoreGroup(std::vector<Player> &white_seekers, std::vector<Player> &black_seekers, const std::string &heading) {
    // if logging is disabled, don't do anything
    if(!this->enable_logging) return;
    // if ws and bs are empty, do nothing
    if(white_seekers.empty() && black_seekers.empty()) return;
    // set up the entire markdown table
    int max_white_len = 13; // maximum length of one white seeker row
    int max_black_len = 13; // maximum length of one black seeker row
    std::vector<std::string> markdown_ws(white_seekers.size());
    std::vector<std::string> markdown_bs(black_seekers.size());
    int largest = std::max((int) white_seekers.size(), (int) black_seekers.size());
    
    int z = 0;
    
    // go through all white seekers
    for(auto i : white_seekers) {
        // create the full markdown for this player
        std::string complete = i.getName() + " (ARO = " + std::to_string(i.getARO()) + ", ID = " + std::to_string(i.getID()) + ")";
        max_white_len = std::max(max_white_len, (int) complete.size());
        markdown_ws[z] = complete;
        z += 1;
    }
    
    // reset
    z = 0;
    
    // now go through all black seekers
    for(auto i : black_seekers) {
        // create the full markdown for this player
        std::string complete = i.getName() + " (ID = " + std::to_string(i.getID()) + ")";
        max_black_len = std::max(max_black_len, (int) complete.size());
        markdown_bs[z] = complete;
        z += 1;
    }
    
    // now complete the markdown table
    std::string markdown_table = "|White Seekers";
    for(int j = 13; j < max_white_len; j++)
        markdown_table += " "; // just add a space for padding
    markdown_table += "|Black Seekers";
    for(int j = 13; j < max_black_len; j++)
        markdown_table += " ";
    markdown_table += "|";
    
    // now add the players from both groups
    for(int i = 0; i < largest; i++) {
        // add new line
        markdown_table += "\n|";
        // if white seeker has an empty spot, we add an empty string
        if(i >= markdown_ws.size())
            markdown_table += std::string(max_white_len, ' ') + "|";
        else
            markdown_table += markdown_ws[i] + std::string(max_white_len - ((int) markdown_ws[i].size()), ' ') + "|";
        
        // now do same for black seeker
        if(i >= markdown_bs.size())
            markdown_table += std::string(max_black_len, ' ') + "|";
        else
            markdown_table += markdown_bs[i] + std::string(max_black_len - ((int) markdown_bs[i].size()), ' ') + "|";
    }
    
    // finally log the table!
    this->logger.log(heading + "\n\n" + markdown_table);
}

//MARK: END LOGGING FUNCTIONS

CPPDubovSystem::LinkedListNode CPPDubovSystem::Tournament::makeNewGroups(const LinkedListNode &old_groups, const std::set<int> &upfloaters, std::vector<Player> *white_seekers, std::vector<Player> *black_seekers) const {
    // go through each groups and remove them
    // add them to ws or bs respectively
    LinkedListNode oold_groups(old_groups);
    LinkedListNode *temp = &oold_groups;
    
    while(temp != nullptr) {
        for(int i = 0; i < temp->data.size(); i++) {
            if(upfloaters.contains(temp->data[i].getID())) {
                if(temp->data[i].getDueColor() == Color::WHITE) {
                    white_seekers->push_back(temp->data[i]);
                } else {
                    black_seekers->push_back(temp->data[i]);
                }
                temp->data.erase(std::next(temp->data.begin(), i));
                i -= 1;
            }
        }
        temp = temp->next;
    }
    
    return oold_groups;
}

std::vector<CPPDubovSystem::Match> CPPDubovSystem::Tournament::maximizePairings(std::vector<Player> &white_seekers, std::vector<Player> &black_seekers, bool *pairing_error) {
    eval_games games;
    std::vector<int> board_problems;
    
    bool problem = false;
    
    // pair players while also keeping track of problems
    if(white_seekers.size() == black_seekers.size()) {
        this->logExplanation("White Seekers and Black Seekers groups are of even size. Attempting to pair them as follows: 1st player in White Seekers plays 1st player in Black Seekers, 2nd player in White Seekers plays 2nd player in Black Seekers, etc.");
        for(int i = 0; i < white_seekers.size(); i++) {
//            games.push_back(Match(white_seekers[i], black_seekers[i], false));
            if(!white_seekers[i].canPlayOpp(black_seekers[i])) {
                problem = true;
                games.push_back(MatchEval(white_seekers[i], black_seekers[i], true));
                board_problems.push_back(((int) games.size()) - 1);
                this->logExplanation(white_seekers[i].getName() + " (ID = " + std::to_string(white_seekers[i].getID()) + ") (white) vs. " + black_seekers[i].getName() + " (ID = " + std::to_string(black_seekers[i].getID()) + ") (black) - Pairing incompatible!");
            } else {
                games.push_back(MatchEval(white_seekers[i], black_seekers[i], false));
                this->logExplanation(white_seekers[i].getName() + "(ID = " + std::to_string(white_seekers[i].getID()) + ") (white) vs. " + black_seekers[i].getName() + " (ID = " + std::to_string(black_seekers[i].getID()) + ") (black) - OK");
            }
        }
        
        if(!problem) {
            this->logExplanation("No problems in pairings. Correcting pairings to give due colors to correct players...");
            // no problems we are good
            return this->castToMatch(games);
        }
        
        // reset games for transpositions
        bool error_occured = false;
        this->logExplanation("Problem exists in pairing! Attempting to fix it using transpositions (rule 4.4)...");
        this->evaluateTranspositions(&games, white_seekers, black_seekers, &error_occured);
        if(error_occured) {
            this->logExplanation("No good transpositions available!");
            
            // check if shifters were recently applied
            if(this->applied_shifters) {
                this->logExplanation("Already tried applying shifters, and no good pairing exists.");
                // simply apply different shifters
                *pairing_error = true;
                return std::vector<Match>();
            }
            
            // group sizes must have been equal originally
            // since there could be many possible combinations of shifters from both sides,
            // first try minimum weight matching to see if there even is a valid pairing
            // get all the shifters for this
            std::vector<int> w_shifters;
            std::vector<int> b_shifters;
            bool shifter_error = false;
            this->logExplanation("Attempting to move shifters (swap players between White Seekers and Black Seekers until we could get a pairing working) following rule 4.3.");
            this->getExchangeShifters(white_seekers, black_seekers, w_shifters, b_shifters, shifter_error);
            
            // check for errors
            if(shifter_error) {
                this->logExplanation("No shifters available to make a legal pairing in this score group!");
                // nothing more can be done since even the graph couldn't find a way to maximize the pairings
                *pairing_error = true;
                return std::vector<Match>();
            }
            
            // it is gauranteed that these shifters make the possible matches
            // it is also known that out of these shifters, w_shifters and b_shifters have to be of equal length
            // in fact this should be asserted
            ASSERT(w_shifters.size() == b_shifters.size(), "shifters for white seekers and black seekers were not equal. This is a bug, and should be very unlikely to happen.");
            
            // now create the swaps and recursively call this function again
            this->logExplanation("Found set of shifters to exchange to make a legal pairing! Making swaps...");
            for(int i = 0; i < w_shifters.size(); i++) {
                Player wc = white_seekers[w_shifters[i]];
                this->logExplanation(wc.getName() + " (ID = " + std::to_string(wc.getID()) + ", current White Seeker) exchanges with " + black_seekers[b_shifters[i]].getName() + " (ID = " + std::to_string(black_seekers[b_shifters[i]].getID()) + ", current Black Seeker)");
                white_seekers[w_shifters[i]] = black_seekers[b_shifters[i]];
                black_seekers[b_shifters[i]] = wc;
            }
            
            // lastly, apply the re-sorting
            this->logExplanation("Sorting both White Seekers and Black Seekers again...");
//            Utils::sortPlayersRating(&black_seekers, 0, ((int) black_seekers.size()) - 1);
            std::sort(black_seekers.begin(), black_seekers.end(), [](const Player &a, const Player &b){
                return a.getID() < b.getID();
            });
            this->sortGroupARO(&white_seekers, 0, ((int) white_seekers.size()) - 1);
            
            this->logPlayers(white_seekers, "White Seekers after sorting by ARO:");
            this->logPlayers(black_seekers, "Black Seekers after sorting by pairing number:");
            this->logScoreGroup(white_seekers, black_seekers, "### New set up for the pairing table");
            
            // now return a newly improved pairing
            this->logExplanation("Try pairing players again...");
            return this->maximizePairings(white_seekers, black_seekers, pairing_error);
        }
    } else {
        // we must attempt to move players from the bigger group to the smaller group
        // before migrating, check if we can migrate any players with no due colors
        bool error_on_move = false;
        /*std::vector<Player> ws_copy(white_seekers);
        std::vector<Player> bs_copy(black_seekers);
        if(white_seekers.size() > black_seekers.size()) {
            if(white_seekers.size() > black_seekers.size()) {
                // we still need to move some players
                Utils::migratePlayers(&black_seekers, &white_seekers, &error_on_move);
            }
            // re-sort as needed
            Utils::sortPlayersRating(&black_seekers, 0, ((int) black_seekers.size()) - 1);
        } else {
            if(black_seekers.size() > white_seekers.size()) {
                Utils::migratePlayers(&white_seekers, &black_seekers, &error_on_move);
            }
            // re-sort as needed
            Utils::sortGroupARO(&white_seekers, 0, ((int) white_seekers.size()) - 1);
            Utils::sortGroupARO(&white_seekers, 0, ((int) white_seekers.size()) - 1);
            Utils::sortPlayersRating(&black_seekers, 0, ((int) black_seekers.size()) - 1);
        }*/
        this->logExplanation("White Seekers group (" + std::to_string(white_seekers.size()) + " players), Black Seekers group (" + std::to_string(black_seekers.size()) + " players) are not the same size! Attempting to find optimal shifters to move (hopefully moving from the bigger group to the smaller, but if this is impossible, we will move some from both groups until the group can be paired). The players to be chosen to be moved from the larger group to the smaller group will be chosen as described in rule 4.3.3");
        this->applyStandardShifters(white_seekers, black_seekers, error_on_move);
        
        // check for errors
        if(error_on_move) {
            this->logExplanation("No good set of shifters found! Group cannot be paired.");
            // we can do no more from here
            *pairing_error = true;
            return this->castToMatch(games);
        }
        
        this->logScoreGroup(white_seekers, black_seekers, "### New White Seekers and Black Seekers for Pairing");
        
        // mark that shifters were applied
        this->applied_shifters = true;
        
        // now that sizes are equal, we can re-evaluate
        // we can call this function recursively since the base case was touched
        bool check = false;
        this->logExplanation("Both group sizes are now even. Attempting to pair them...");
        std::vector<Match> output = this->maximizePairings(white_seekers, black_seekers, &check);
        
        /*if(check) {
            //TODO: WILL THIS EVER BE REACHED?
            // try another set of shifters
            std::vector<int> w_shifters;
            std::vector<int> b_shifters;
            bool shift_err = false;
            this->getExchangeShifters(white_seekers, black_seekers, &w_shifters, &b_shifters, &shift_err);
//            this->getExchangeShifters2(white_seekers, black_seekers, w_shifters, b_shifters, shift_err);
            if(shift_err) {
                // nothing more can be done
                *pairing_error = true;
                return std::vector<Match>();
            }
            
            // assert sizes are equal
            ASSERT(w_shifters.size() == b_shifters.size(), "shifters for white seekers and black seekers were not equal. This is a bug, and should be very unlikely to happen.");
            
            // now create the swaps and recursively call this function again
            for(int i = 0; i < w_shifters.size(); i++) {
                Player wc = white_seekers[w_shifters[i]];
                white_seekers[w_shifters[i]] = black_seekers[b_shifters[i]];
                black_seekers[b_shifters[i]] = wc;
            }
            
            // lastly, apply the re-sorting
            Utils::sortPlayersRating(&black_seekers, 0, ((int) black_seekers.size()) - 1);
            Utils::sortGroupARO(&white_seekers, 0, ((int) white_seekers.size()) - 1);
            
            // now return a newly improved pairing
            output = this->maximizePairings(white_seekers, black_seekers, pairing_error);
        }*/
        
        // reset applied shifters
        this->applied_shifters = false;
        
        return output;
    }
    
    return this->castToMatch(games);
}

int CPPDubovSystem::Tournament::removeByePlayer(std::vector<Player> *players, int bye_remove, Player *p_bye) const {
    int r = -1;
    // find where the bye player is and remove the player from the list
    for(int i = 0; i < players->size(); i++) {
        if((*players)[i].getID() == bye_remove) {
            *p_bye = (*players)[i];
            players->erase(std::next(players->begin(), i));
            r = i;
            break;
        }
    }
    return r;
}

int CPPDubovSystem::Tournament::playerShouldAlternate(Player &white, Player &black) {
    //TODO: THIS FUNCTION IS STILL UNDER DEVELOPMENT. THE GOAL IS SIMPLE, TO DETERMINE WHICH OF THE TWO PLAYERS (WHO ARE COMPATIBLE FOR A MATCH AND HAVE THE SAME DUE COLOR) SHOULD RECEIEVE THE DUE COLOR
    // it is assumed that both players have the same due color
    // it is also assumed that both players are compatible for pairing
    
    // lets first try checking the preference stregnths and applying 5.2.3 (grant the stronger due color)
    ColorPreference white_strength = white.getPreferenceStrength();
    ColorPreference black_strength = black.getPreferenceStrength();
    
    if(white_strength == ColorPreference::ABSOLUTE) {
        // blacks strength must not be absolute since we assume that this pairing is already valid
        return white.getID();
    } else if(black_strength == ColorPreference::ABSOLUTE) {
        return black.getID();
    }
    
    if(white_strength == black_strength) {
        // strengths are equal!
        // are the color histories equal?
        if(white.isColorHistEqual(black)) {
            // then we apply rule 5.2.5 (give due color to higher ranked player)
            if(white > black)
                return white.getID();
            
            return black.getID();
        }
        // now try applying 5.2.4
        if(white.getDueColor() == Color::WHITE) {
            if(white.shouldAlternate(black)) {
                return black.getID();
            }
            return white.getID();
        } else {
            if(black.shouldAlternate(white)) {
                return white.getID();
            }
        }
        
        return black.getID();
    }
    
    // since we know that strengths are not equal, this is a simple check
    if(white_strength == ColorPreference::MILD) return white.getID();
    else if(black_strength == ColorPreference::MILD) return black.getID();
    
    if(white_strength == ColorPreference::ALTERNATION) return white.getID();
    
    return black.getID(); // we know that the strengths are not the same, so no need to compare anything else
}

void CPPDubovSystem::Tournament::optimizeColors(std::vector<Match> *games) {
    // since the program already makes sure that absolute criteria is met, just make sure equalization is preferred to alternation
    for(int i = 0; i < games->size(); i++) {
        if((*games)[i].is_bye) {
            continue;
        }
        Color gc = (*games)[i].white.getDueColor();
        if(gc == (*games)[i].black.getDueColor()) {
            //TODO: A FEW TEST CASES STILL YIELD SOME WEIRD INVALID COLORS BECAUSE OF RULE 5.2.4. SOME MORE TESTING FOR THIS RULE IN PARTICULAR IS NEEDED
            ColorPreference wp = (*games)[i].white.getPreferenceStrength();
            ColorPreference bp = (*games)[i].black.getPreferenceStrength();
            /*if((wp == ColorPreference::ALTERNATION && bp == ColorPreference::MILD && gc == Color::WHITE) || (gc == Color::WHITE && bp == ColorPreference::ABSOLUTE)) {
                // bp has higher priority
                (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
            } else if((wp == ColorPreference::MILD && bp == ColorPreference::ALTERNATION && gc == Color::BLACK) || (gc == Color::BLACK && wp == ColorPreference::ABSOLUTE)) {
                (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
            } else {
                // check 5.2.5 - grant the color preference of the higher ranked player
                // make sure to also follow 5.2.3 - grant the stronger color preference
                if(gc == Color::WHITE && wp == ColorPreference::MILD && bp == ColorPreference::ALTERNATION) {
                    continue;
                } else if(gc == Color::BLACK && wp == ColorPreference::ALTERNATION && bp == ColorPreference::MILD) {
                    continue;
                }
                if(gc == Color::BLACK && bp != ColorPreference::ABSOLUTE) {
                    if((*games)[i].white.getID() < (*games)[i].black.getID()) {
                        (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
                    }
                } else {
                    if((*games)[i].white.getID() > (*games)[i].black.getID()) {
                        (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
                    }
                }
            }*/
            std::map<ColorPreference, int> pref_equiv;
            pref_equiv[ColorPreference::ABSOLUTE] = 3;
            pref_equiv[ColorPreference::MILD] = 2;
            pref_equiv[ColorPreference::ALTERNATION] = 1;
            pref_equiv[ColorPreference::NO_PREFERENCE] = 0;
            if(gc == Color::WHITE) {
                if(pref_equiv[bp] > pref_equiv[wp]) {
                    this->logExplanation((*games)[i].black.getName() + " (ID = " + std::to_string((*games)[i].black.getID()) + ") has greater priority for due color White over: " + (*games)[i].white.getName() + " (ID = " + std::to_string((*games)[i].white.getID()) + ". Swapping colors for these two players...");
                    (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
                } else if(pref_equiv[bp] == pref_equiv[wp]) {
                    // check if black should be swapped
                    if((*games)[i].black.shouldAlternate((*games)[i].white)) {
                        this->logExplanation((*games)[i].black.getName() + " (ID = " + std::to_string((*games)[i].black.getID()) + ") has greater priority for due color White over: " + (*games)[i].white.getName() + " (ID = " + std::to_string((*games)[i].white.getID()) + ". Swapping colors for these two players...");
                        (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
                    }
                }
//                if(this->playerShouldAlternate((*games)[i].white, (*games)[i].black) == (*games)[i].black.getID()) {
//                    (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
//                }
            } else if(gc == Color::BLACK) {
                if(pref_equiv[wp] > pref_equiv[bp]) {
                    this->logExplanation((*games)[i].white.getName() + " (ID = " + std::to_string((*games)[i].white.getID()) + ") has greater priority for due color Black over: " + (*games)[i].black.getName() + " (ID = " + std::to_string((*games)[i].black.getID()) + ". Swapping colors for these two players...");
                    (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
                } else if(pref_equiv[wp] == pref_equiv[bp]) {
                    // check if white should be swapped
                    if((*games)[i].white.shouldAlternate((*games)[i].black)) {
                        this->logExplanation((*games)[i].white.getName() + " (ID = " + std::to_string((*games)[i].white.getID()) + ") has greater priority for due color Black over: " + (*games)[i].black.getName() + " (ID = " + std::to_string((*games)[i].black.getID()) + ". Swapping colors for these two players...");
                        (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
                    }
                }
//                if(this->playerShouldAlternate((*games)[i].white, (*games)[i].black) == (*games)[i].white.getID()) {
//                    (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
//                }
            } else {
                // 5.2.1 -> initial color goes to odd numbered player
//                if((*games)[i].white.getPoints() == (*games)[i].black.getPoints()) {
//                    if((*games)[i].white.getID() < (*games)[i].black.getID() && this->inital_color == Color::BLACK && (*games)[i].white.getID() % 2 > 0) {
//                        (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
//                    } else if((*games)[i].black.getID() < (*games)[i].white.getID() && this->inital_color == Color::BLACK && (*games)[i].black.getID() % 2 == 0) {
//                        (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
//                    }
//                } else if((*games)[i].white.getPoints() > (*games)[i].black.getPoints()) {
//                    if((*games)[i].white.getID() % 2 > 0 && this->inital_color == Color::BLACK) {
//                        (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
//                    }
//                } else {
//                    if((*games)[i].black.getID() % 2 > 0 && this->inital_color == Color::WHITE) {
//                        (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
//                    }
//                }
                if((*games)[i].white < (*games)[i].black && (*games)[i].black.getID() % 2 == 0) {
                    // we do the swap!
                    this->logExplanation((*games)[i].white.getName() + " (ID = " + std::to_string((*games)[i].white.getID()) + ") has greater priority for due color Black over (5.2.1 -> initial color goes to odd numbered player): " + (*games)[i].black.getName() + " (ID = " + std::to_string((*games)[i].black.getID()) + ". Swapping colors for these two players...");
                    (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
                } else if((*games)[i].white > (*games)[i].black && (*games)[i].white.getID() % 2 == 0) {
                    // we also do the swap!
                    this->logExplanation((*games)[i].white.getName() + " (ID = " + std::to_string((*games)[i].white.getID()) + ") has greater priority for due color Black over (5.2.1 -> initial color goes to odd numbered player): " + (*games)[i].black.getName() + " (ID = " + std::to_string((*games)[i].black.getID()) + ". Swapping colors for these two players...");
                    (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
                }
            }
        } else {
            // check if one player doesn't have a color preference
            if(gc == Color::NO_COLOR) {
                // check if black has a preference for white
                if((*games)[i].black.getDueColor() == Color::WHITE) {
                    this->logExplanation((*games)[i].white.getName() + " (ID = " + std::to_string((*games)[i].white.getID()) + ") has no due color! Black, " + (*games)[i].black.getName() + " (ID = " + std::to_string((*games)[i].black.getID()) + ", has the due color of White though. Swapping colors for these two players...");
                    (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
                }
            } else {
                // check if black player has no preference and white wants black
                if((*games)[i].black.getDueColor() == Color::NO_COLOR && gc == Color::BLACK) {
                    this->logExplanation((*games)[i].white.getName() + " (ID = " + std::to_string((*games)[i].white.getID()) + ") has a due color while Black has none, " + (*games)[i].black.getName() + " (ID = " + std::to_string((*games)[i].black.getID()) + ". Swapping colors for these two players...");
                    (*games)[i] = Match((*games)[i].black, (*games)[i].white, false);
                }
            }
        }
    }
}

CPPDubovSystem::Tournament::Tournament(int total_rounds) {
    this->total_rounds = total_rounds;
}

void CPPDubovSystem::Tournament::addPlayer(const Player &p) {
    this->player_count += 1; // increment player count as needed
    this->players.push_back(p);
}

void CPPDubovSystem::Tournament::addPlayer(Player &&p) {
    this->player_count += 1;
    this->players.push_back(p);
}

void CPPDubovSystem::Tournament::setRound1Color(bool make_white) {
    this->round1_top_board_white = make_white;
}

CPPDubovSystem::Match::Match(const Player &white, const Player &black, bool is_bye) {
    this->is_bye = is_bye;
    this->white = white;
    this->black = black;
}

void CPPDubovSystem::Utils::playersRatingMerge(std::vector<Player> *players, const int left, const int mid, const int right) {
    int const subVectorOne = mid - left + 1;
    int const subVectorTwo = right - mid;
    
    auto *leftVector = new std::vector<Player>(), *rightVector = new std::vector<Player>();
//    leftVector->reserve(subVectorOne);
    leftVector->resize(subVectorOne);
    rightVector->resize(subVectorTwo);
    
    for(auto i = 0; i < subVectorOne; i++) {
        (*leftVector)[i] = (*players)[left + i];
    }
    for(auto i = 0; i < subVectorTwo; i++) {
        (*rightVector)[i] = (*players)[mid + 1 + i];
    }
    
    auto indexOfVectorOne = 0, indexOfVectorTwo = 0;
    int indexOfMergedVector = left;
    
    while(indexOfVectorOne < subVectorOne && indexOfVectorTwo < subVectorTwo) {
        /*if((*leftVector)[indexOfVectorOne].getRating() >= (*rightVector)[indexOfVectorTwo].getRating()) {
            // if ratings are equal, choose the higher starting rank
            if((*leftVector)[indexOfVectorOne].getRating() == (*rightVector)[indexOfVectorTwo].getRating()) {
                if((*leftVector)[indexOfVectorOne].getID() < (*rightVector)[indexOfVectorTwo].getID()) {
                    (*players)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
                    indexOfVectorOne += 1;
                } else {
                    (*players)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
                    indexOfVectorTwo += 1;
                }
            } else {
                (*players)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
                indexOfVectorOne += 1;
            }
        } else {
            (*players)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
            indexOfVectorTwo += 1;
        }*/
        if((*leftVector)[indexOfVectorOne].getID() < (*rightVector)[indexOfVectorTwo].getID()) {
            (*players)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
            indexOfVectorOne += 1;
        } else {
            (*players)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
            indexOfVectorTwo += 1;
        }
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorOne < subVectorOne) {
        (*players)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
        indexOfVectorOne += 1;
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorTwo < subVectorTwo) {
        (*players)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
        indexOfVectorTwo += 1;
        indexOfMergedVector += 1;
    }
    
    delete leftVector;
    delete rightVector;
}

void CPPDubovSystem::Utils::playersRatingRTGMerge(std::vector<Player> *players, const int left, const int mid, const int right) {
    int const subVectorOne = mid - left + 1;
    int const subVectorTwo = right - mid;
    
    auto *leftVector = new std::vector<Player>(), *rightVector = new std::vector<Player>();
//    leftVector->reserve(subVectorOne);
    leftVector->resize(subVectorOne);
    rightVector->resize(subVectorTwo);
    
    for(auto i = 0; i < subVectorOne; i++) {
        (*leftVector)[i] = (*players)[left + i];
    }
    for(auto i = 0; i < subVectorTwo; i++) {
        (*rightVector)[i] = (*players)[mid + 1 + i];
    }
    
    auto indexOfVectorOne = 0, indexOfVectorTwo = 0;
    int indexOfMergedVector = left;
    
    while(indexOfVectorOne < subVectorOne && indexOfVectorTwo < subVectorTwo) {
        if((*leftVector)[indexOfVectorOne].getRating() >= (*rightVector)[indexOfVectorTwo].getRating()) {
            // if ratings are equal, choose the higher starting rank
            if((*leftVector)[indexOfVectorOne].getRating() == (*rightVector)[indexOfVectorTwo].getRating()) {
                if((*leftVector)[indexOfVectorOne].getID() < (*rightVector)[indexOfVectorTwo].getID()) {
                    (*players)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
                    indexOfVectorOne += 1;
                } else {
                    (*players)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
                    indexOfVectorTwo += 1;
                }
            } else {
                (*players)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
                indexOfVectorOne += 1;
            }
        } else {
            (*players)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
            indexOfVectorTwo += 1;
        }
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorOne < subVectorOne) {
        (*players)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
        indexOfVectorOne += 1;
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorTwo < subVectorTwo) {
        (*players)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
        indexOfVectorTwo += 1;
        indexOfMergedVector += 1;
    }
    
    delete leftVector;
    delete rightVector;
}

void CPPDubovSystem::Tournament::playersPointsMerge(std::vector<Player> *players, const int left, const int mid, const int right) {
    int const subVectorOne = mid - left + 1;
    int const subVectorTwo = right - mid;
    
    auto *leftVector = new std::vector<Player>(), *rightVector = new std::vector<Player>();
//    leftVector->reserve(subVectorOne);
    leftVector->resize(subVectorOne);
    rightVector->resize(subVectorTwo);
    
    for(auto i = 0; i < subVectorOne; i++) {
        (*leftVector)[i] = (*players)[left + i];
    }
    for(auto i = 0; i < subVectorTwo; i++) {
        (*rightVector)[i] = (*players)[mid + 1 + i];
    }
    
    auto indexOfVectorOne = 0, indexOfVectorTwo = 0;
    int indexOfMergedVector = left;
    
    while(indexOfVectorOne < subVectorOne && indexOfVectorTwo < subVectorTwo) {
        if((*leftVector)[indexOfVectorOne].getPoints() >= (*rightVector)[indexOfVectorTwo].getPoints()) {
            (*players)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
            indexOfVectorOne += 1;
        } else {
            (*players)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
            indexOfVectorTwo += 1;
        }
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorOne < subVectorOne) {
        (*players)[indexOfMergedVector] = (*leftVector)[indexOfVectorOne];
        indexOfVectorOne += 1;
        indexOfMergedVector += 1;
    }
    
    while(indexOfVectorTwo < subVectorTwo) {
        (*players)[indexOfMergedVector] = (*rightVector)[indexOfVectorTwo];
        indexOfVectorTwo += 1;
        indexOfMergedVector += 1;
    }
    
    delete leftVector;
    delete rightVector;
}

void CPPDubovSystem::Utils::sortPlayersRating(std::vector<Player> *players, const int begin, const int end) {
    if(begin >= end) {
        return;
    }
    
    int mid = begin + (end - begin) / 2;
    Utils::sortPlayersRating(players, begin, mid);
    Utils::sortPlayersRating(players, mid + 1, end);
    Utils::playersRatingMerge(players, begin, mid, end);
}

void CPPDubovSystem::Utils::sortPlayersRatingRTG(std::vector<Player> *players, const int begin, const int end) {
    if(begin >= end) {
        return;
    }
    
    int mid = begin + (end - begin) / 2;
    Utils::sortPlayersRatingRTG(players, begin, mid);
    Utils::sortPlayersRatingRTG(players, mid + 1, end);
    Utils::playersRatingRTGMerge(players, begin, mid, end);
}

void CPPDubovSystem::Tournament::sortPlayersPoints(std::vector<Player> *players, const int begin, const int end) {
    if(begin >= end) {
        return;
    }
    
    int mid = begin + (end - begin) / 2;
//    this->sortPlayersPoints(players, begin, mid);
//    this->sortPlayersPoints(players, mid + 1, end);
//    this->playersPointsMerge(players, begin, mid, end);
    sortPlayersPoints(players, begin, mid);
    sortPlayersPoints(players, mid + 1, end);
    playersPointsMerge(players, begin, mid, end);
}

std::vector<int> CPPDubovSystem::Tournament::getByePlayerStack(const std::vector<Player> &selection, int current_round) const {
    // find the player with the lowest rank, played the highest number of games, and has not scored a forfeit win or gotten a bye before
    int lowest = ((int) selection.size()) - 1;
    std::vector<int> candidate_player;
    std::vector<int> bye_queue;
    std::priority_queue<std::pair<int, int>> next_candidate;
    double prev_points = -1.0;
    while(lowest >= 0) {
        if(prev_points == -1.0) {
            prev_points = selection[lowest].getPoints();
        }
        // make sure player hasn't scored forfeit win and hasn't gotten bye before
        if(selection[lowest].hasReceievedBye()) {
            // invalid
            lowest -= 1;
            continue;
        }
        
        // compare points
        if(selection[lowest].getPoints() != prev_points) {
            prev_points = selection[lowest].getPoints();
            // add all in candidate
            while(!next_candidate.empty()) {
                bye_queue.push_back(next_candidate.top().second);
                next_candidate.pop();
            }
        }
        
        // 2.1.4 player should have played highest number of games
        if(selection[lowest].getOppCount() < current_round - 1) {
            // hasn't played highest number of games, but is a candidate
            candidate_player.push_back(selection[lowest].getID());
            next_candidate.push(std::make_pair(selection[lowest].getOppCount(), selection[lowest].getID()));
            lowest -= 1;
            continue;
        }
        
        // compare points
//        if(selection[lowest].getPoints() != prev_points) {
//            prev_points = selection[lowest].getPoints();
//            for(int i = 0; i < candidate_player.size(); i++) {
//                bye_queue.push_back(candidate_player[i]);
//            }
//            candidate_player.clear();
//        }
        
        // player can get bye
        bye_queue.push_back(selection[lowest].getID());
        lowest -= 1;
        // 2.1.3 player should have lowest score
    }
    
    // merge candidate players
    while(((int) candidate_player.size()) > 0) {
        bye_queue.push_back(candidate_player[0]);
        candidate_player.erase(candidate_player.begin());
    }
    
    // check sizes
    if(bye_queue.size() == 0) {
        // no player can get a bye
        throw std::invalid_argument("all players in tournament are not compatible for bye");
    }
    
    return bye_queue;
}

std::vector<CPPDubovSystem::Match> CPPDubovSystem::Tournament::makeRound1() {
    std::vector<Match> games;
    int middle = floor(this->player_count / 2);
    int opp = floor(this->player_count / 2);
    Color curr = this->round1_top_board_white ? Color::WHITE : Color::BLACK;
//    Utils::sortPlayersRating(&this->players, 0, this->player_count - 1);
    std::sort(this->players.begin(), this->players.end(), [](const Player &a, const Player &b){
        return a.getID() < b.getID();
    });
    // round 1 pairings simply follow the rule of top half vs bottom half
    // and colors alternate for the upper half as we go down
    // this framework however is designed to give white to the top player by default
    for(int i = 0; i < middle; i++) {
        if(curr == Color::WHITE) {
            games.push_back(Match(this->players[i], this->players[opp], false));
            opp += 1;
            curr = Color::BLACK;
        } else {
            games.push_back(Match(this->players[opp], this->players[i], false));
            opp += 1;
            curr = Color::WHITE;
        }
    }
    if(this->player_count % 2 > 0) {
        games.push_back(Match(this->players[this->player_count - 1], Player("", 0, -1, 0.0), true));
    }
    return games;
}

CPPDubovSystem::LinkedList CPPDubovSystem::Tournament::makeGroups() const {
    LinkedList groups;
    // it is assumed that players are already sorted
    std::vector<Player> group;
    group.push_back(this->players[0]);
    double curr_points = this->players[0].getPoints();
    
    // go through all the players and insert them into their own linkedlistnode container
    for(int i = 1; i < this->player_count; i++) {
        if(this->players[i].getPoints() != curr_points) {
            groups.insertNode(group);
            group.clear();
            group.push_back(this->players[i]);
            curr_points = this->players[i].getPoints();
            continue;
        }
        group.push_back(this->players[i]);
    }
    groups.insertNode(group);
    
    return groups;
}

void CPPDubovSystem::Tournament::initPlayers() {
    // this is a small helper function
    // it only has one goal, to make sure player due colors are completely initialized
    // run through all the players and initialize all the due colors
    // we will also validate that each player has a unique id and is not unrated
    std::set<int> unique_id;
    for(int i = 0; i < this->players.size(); i++) {
        // the due color and strength are automatically initialized internally
        this->players[i].getDueColor();
        this->players[i].getPreferenceStrength();
        // also set up aro
        this->players[i].getARO();
        // also make sure the id is unique
        // the id is unique of unique_id doesn't contain the id already
        if(unique_id.contains(this->players[i].getID())) {
            std::cerr << "The player ID " << std::to_string(this->players[i].getID()) << " is a duplicate ID (another player already has this ID in the tournament). This could give invalid pairings." << std::endl;
        } else {
            unique_id.insert(this->players[i].getID());
        }
        
        // also validate that the player is not unrated
        if(this->players[i].getRating() == UNRATED) {
            // also raise a warning for this
            std::cerr << "Player with ID " << std::to_string(this->players[i].getID()) << " appears to be unrated. All players in this pairing system must have a rating (rule 1.1.1)." << std::endl;
        }
    }
}

std::vector<CPPDubovSystem::Match> CPPDubovSystem::Tournament::makePairingForGroup(LinkedListNode *g, int pairing_round) {
    if(g == nullptr) {
        // base case
        return std::vector<Match>();
    }
    std::vector<Match> games;
    Player upfloater; // for storing upfloater in backtracking
    bool contains_upfloaters = false;
    
    std::vector<Player> white_seekers;
    std::vector<Player> black_seekers;
    std::vector<Player> upfloaters;
    std::vector<Player> upfloaters_multi;
    bool floater_required = false;
    
    if(g->data.size() > 0)
        this->logExplanation("Doing pairings for score group with points: " + std::to_string(g->data[0].getPoints()));
    
    // make groups
    this->splitGroups(&white_seekers, &black_seekers, g->data);
    
    // sort groups
    this->sortGroupARO(&white_seekers, 0, ((int) white_seekers.size()) - 1);
//    Utils::sortPlayersRating(&black_seekers, 0, ((int) black_seekers.size()) - 1);
    std::sort(black_seekers.begin(), black_seekers.end(), [](const Player &a, const Player &b) {
        return a.getID() < b.getID();
    });
    
    this->logExplanation("Split players into groups. Those who have the due color White go into the \"White Seekers\" Group while those who are due Black go into the \"Black Seekers\" Group.");
    this->logPlayers(white_seekers, "White Seekers (sorted by ARO)");
    this->logPlayers(black_seekers, "Black Seekers (sorted by pairing number)");
    
    this->logScoreGroup(white_seekers, black_seekers, "### Score Group View");
    
    int num_white = (int) white_seekers.size();
    int num_black = (int) black_seekers.size();
    
    // now check if upfloater is needed for this group in order to complete pairings
    if(g->data.size() % 2 > 0) {
        this->logExplanation("Group has an odd number of players! Attempting to pick an upfloater...");
        // make sure there is a next group
        if(g->next == nullptr) {
            this->logExplanation("Lower score group does not exist! Backtracking to previous group to try to fix this error...");
            // can't do anything more from here!
            this->pairing_error = true;
            if(contains_upfloaters) {
                g->data.push_back(upfloater);
            }
            return std::vector<Match>();
        }
        // find upfloater that satisfies pairings
        upfloaters = this->findUpfloaters(*g->next, num_white - num_black);
        this->logPlayers(upfloaters, "Possible upfloaters (properly sorted by the following: score differences amongst players minimized, players closer to the top of the list are not max upfloaters or upfloated previously, and have the proper due color that would go to the group of seekers with less players).");
        
        // make sure there is at least one valid upfloater present
        if(upfloaters.size() == 0) {
            this->logExplanation("No upfloaters found! Backtracking to previous group to try to fix this error...");
            this->pairing_error = true;
            // put back upfloater if needed
            if(contains_upfloaters) {
                g->data.push_back(upfloater);
            }
            return games;
        }
        floater_required = true;
    }
    
    // attempt to pair the group
    bool pair_complete = false;
//    bool pairing_failure_upfloaters = false;
    std::vector<Player> upfloaters_move_container; // for storing tried upfloaters
//    unsigned int max_upfloater_move = 2;
    while(!pair_complete) {
        // make copies as needed
        std::vector<Player> w_copy = std::vector<Player>(white_seekers);
        std::vector<Player> b_copy = std::vector<Player>(black_seekers);
        LinkedListNode *next_group_use = g->next == nullptr ? nullptr : (new LinkedListNode(*(g->next)));
        // check if we have any floaters
        /*if(floater_required) {
            if(upfloaters.size() == 0) {
                // no floaters left
                this->logExplanation("Group requires an upfloater, but no more possible upfloaters exist! Backtracking to previous group to try to fix this error...");
                this->pairing_error = true;
                if(contains_upfloaters) {
                    g->data.push_back(upfloater);
                }
                if(next_group_use != nullptr) delete next_group_use;
                return games;
            }
            
            // dequeue the floater and add to list
            Player floater = upfloaters[0];
            upfloaters.erase(upfloaters.begin());
            // FIX: issue8
            // if we have exhausted our upfloaters, set that there are no more floaters reuqired and let multi-upfloater handle any other pairing errors
            if(upfloaters.empty()) floater_required = false;
            this->logExplanation("Picking the next upfloater on the list of upfloaters to try on the list: player with ID = " + std::to_string(floater.getID()) + ", Name = " + floater.getName());
            // recreate lower groups
            this->logExplanation("Removing upfloater from lower score group...");
            LinkedListNode ng = this->makeNewGroups(*next_group_use, {floater.getID()}, &w_copy, &b_copy);
            delete next_group_use; // get rid of the old memory
            next_group_use = new LinkedListNode(ng);
            
            // sort new group as necessary
            if(floater.getDueColor() == Color::WHITE) {
                this->logExplanation("Upfloater has the due color White. Inserting this player into White Seekers and resorting this group again...");
//                w_copy.push_back(floater);
                // re-sort white seekers as needed
                this->sortGroupARO(&w_copy, 0, ((int) w_copy.size()) - 1);
                this->logPlayers(w_copy, "Group after inserting upfloater and sorting by ARO...");
            } else {
//                b_copy.push_back(floater);
                // re-sort black seekers as needed
                this->logExplanation("Upflaoter has the due color Black. Inserting this player into Black Seekers and resorting this group again...");
                Utils::sortPlayersRating(&b_copy, 0, ((int) b_copy.size()) - 1);
                this->logPlayers(b_copy, "Group after inserting upfloater and sorting by pairing number...");
            }
            
            this->logScoreGroup(w_copy, b_copy, "### Score Group View After Inserting Upfloater");
        }*/
        if(floater_required) {
            // find best set of upfloaters to move
            this->logExplanation("Searching for optimal set of upfloaters...");
            bool failure = false;
            std::set<int> up = this->findOptimalUpfloaters(w_copy, b_copy, *next_group_use, failure);
            
            // were we successful?
            if(failure) {
                // nothing more can be done from here
                this->logExplanation("No good set of upfloaters possible to satisfy pairings! Backtracking to previous group to try to fix this error...");
                this->pairing_error = true;
                if(next_group_use != nullptr) delete next_group_use;
                break;
            }
            
            this->logExplanation("Find a good set of upfloaters to satisfy all pairing criteria! Inserting them into the score group...");
            
            // recreate the groups
            LinkedListNode new_groups = this->makeNewGroups(*next_group_use, up, &w_copy, &b_copy);
            delete next_group_use;
            next_group_use = new LinkedListNode(new_groups);
            
            // sort both groups again
            this->sortGroupARO(&w_copy, 0, ((int) w_copy.size()) - 1);
            std::sort(b_copy.begin(), b_copy.end(), [](const Player &a, const Player &b) {
                return a.getID() < b.getID();
            });
            this->logScoreGroup(w_copy, b_copy, "### Score Group View After Inserting Upfloater(s)");
        }
        
        /*if(pairing_failure_upfloaters) {
            // find the best upfloaters
            bool failure = false;
            this->logExplanation("Multiple upfloaters are needed to make the pairings work for this section (since there is no other way of pairing this section without the use of upfloaters). Picking upfloaters...");
            if(this->enable_logging) {
                std::vector<Player> temp = this->findUpfloaters(*next_group_use, 0);
                
                // now we just log temp
                this->logPlayers(temp, "Found list of upfloaters as given below. Keep moving the minimum number of upfloaters (from this list) to this score group until all pairings in this score group can be satisfied.");
            }
            std::set<int> up = this->findMultiUpfloaters(w_copy, b_copy, *next_group_use, failure);
            
            // check if this doesn't work
            if(failure) {
                this->logExplanation("No combination of upfloaters satisfies pairings in this group and lower! Backtracking to previous group to try to fix this error...");
                // nothing more can be done
                this->pairing_error = true;
                if(next_group_use != nullptr) delete next_group_use;
                break;
            }
            
            // insert into groups
            this->logExplanation("Removing upfloaters from the lower score groups they were in...");
            LinkedListNode new_groups = this->makeNewGroups(*next_group_use, up, &w_copy, &b_copy);
            delete next_group_use;
            
            next_group_use = new LinkedListNode(new_groups);
            
            this->sortGroupARO(&w_copy, 0, ((int) w_copy.size()) - 1);
            Utils::sortPlayersRating(&b_copy, 0, ((int) b_copy.size()) - 1);
            
            this->logPlayers(w_copy, "White Seekers after inserting upfloaters and sorting by ARO");
            this->logPlayers(b_copy, "Black Seekers after inserting upfloaters and sorting by pairing number");
            this->logScoreGroup(w_copy, b_copy, "### Score Group View After Inserting Upfloaters");
        }*/
        
        // attempt to pair the groups
        bool error_on_pair = false;
        std::vector<Match> games_got = this->maximizePairings(w_copy, b_copy, &error_on_pair);
        
        // check for errors
        if(error_on_pair) {
            this->logExplanation("Pairings could not be completed for score group!");
            if(floater_required) {
                // try another floater
                this->logExplanation("Trying a different upfloater to see if pairings will be satisfied with this different upfloater...");
                this->pairing_error = false; // reset as necessary
                if(next_group_use != nullptr) delete next_group_use;
                continue;
            }
            floater_required = true;
            // we must extract the same number of upfloaters as there are in the group
            /*if(upfloaters.size() == 0) {
                this->logExplanation("Trying to get upfloaters to make pairing in this group possible...");
                // make sure that there is a next group
                if(g->next == nullptr) {
                    // nothing more can be done by this point
                    this->logExplanation("Lower score group doesn't exist! Backtracking to previous group to try to fix this error...");
                    this->pairing_error = true;
                    break;
                }
                pairing_failure_upfloaters = true;
                this->pairing_error = false;
                delete next_group_use;
                continue;
            }*/
            if(next_group_use != nullptr) delete next_group_use;
            continue;
        }
        
        // merge games
        this->logExplanation("Pairing for group completed successfully!");
        this->mergeMatches(games_got, &games);
        
        // try pairing next group lower
//        std::vector<Match> lower = this->makePairingForGroup(g->next, pairing_round);
        this->logExplanation("Attempting to pair next group...");
        std::vector<Match> lower = this->makePairingForGroup(next_group_use, pairing_round);
        
        // check for errors as necessary
        if(this->pairing_error) {
            this->logExplanation("Pairing error occured in one of the lower score groups!");
            // clear merged matches
            games.clear();
            // check if this section required a floater previously
            if(floater_required) {
                // we can try another floater, so continue
                this->logExplanation("Trying a different upfloater to see if pairings will be satisfied with this different upfloater...");
                this->pairing_error = false;
                if(next_group_use != nullptr) delete next_group_use;
                continue;
            }
            
//             we need to extract some floaters to complete the pairing
//             C.5 states to minimize the number of upfloaters
//             so gather the next to available upfloaters
//            if(upfloaters.empty()) {
//                this->logExplanation("Trying to get upfloaters to make pairing in this group possible...");
//                if(g->next == nullptr) {
//                    this->logExplanation("Lower score group doesn't exist! Backtracking to previous group to try to fix this error...");
//                    this->pairing_error = true;
//                    break;
//                }
//                pairing_failure_upfloaters = true;
//                this->pairing_error = false;
//                delete next_group_use;
//                continue;
//            }
            floater_required = true;
            if(next_group_use != nullptr) delete next_group_use;
            continue;
            
//             make sure we still have some upfloaters left
//            if(((int) upfloaters.size()) == 1) {
//                // this is a rare case, but if we reach it, we don't have enough upfloaters to satisfy the pairing
//                break;
//            }
            
//            if(pairing_failure_upfloaters) {
//                // we can continue as upfloaters are already up
//                this->logExplanation("Trying another set of upfloaters...");
//                this->pairing_error = false;
//                if(next_group_use != nullptr) delete next_group_use;
//                continue;
//            }
        }
        
        // merge
        this->mergeMatches(lower, &games);
        
        pair_complete = true; // mark pairing complete
        this->pairing_error = false;
        
        delete next_group_use;
        
        this->logExplanation("Pairing successfully completed for this score group and all the lower ones!");
    }
    
    // backtrack
    // put back upfloater as needed
    if(contains_upfloaters) {
        g->data.push_back(upfloater);
    }
    
    return games;
}

std::vector<CPPDubovSystem::Match> CPPDubovSystem::Tournament::makeSubsequent(int pairing_round) {
    std::vector<Match> games;
    // set up logger
    this->logger.clear();
    this->logExplanation("CPPDubovSystem (https://github.com/MichaelVShapiro/CPPDubovSystem) -- Explanation of pairings.");
    // sort players
    this->logExplanation("Sort players into respective score groups (sorted by points then by pairing number)");
//    Utils::sortPlayersRating(&this->players, 0, this->player_count - 1);
//    this->sortPlayersPoints(&this->players, 0, this->player_count - 1);
    auto global_sorter = [](const Player &p1, const Player &p2) {
        if(p1.getPoints() != p2.getPoints()) return p1.getPoints() > p2.getPoints();
        
        return p1.getID() < p2.getID();
    };
    std::sort(this->players.begin(), this->players.end(), global_sorter);
    
    // handle byes as necessary
    if(((int) this->players.size()) % 2 > 0) {
        this->bye_queue = this->getByePlayerStack(this->players, pairing_round);
    }
    
    // initialize due colors for players
    this->logExplanation("Computing due colors, due color strengths, and ARO for all players.");
    this->initPlayers();
    
    // make groups
//    LinkedList groups = this->makeGroups();
    
    // pair each group
//    LinkedListNode* curr = groups.getHead();
    
    if(this->bye_queue.size() > 0) {
        // since just about anybody can get a bye, we need to constantly keep trying to give different players the bye until we get a valid set of pairings
        LinkedList groups;
        LinkedListNode* curr = nullptr; // just a temporary value
        this->logExplanation("Odd number of players in the tournament! Picking the player who should get the BYE by following rule 3.1...");
        while(this->bye_queue.size() > 0) {
            // remove bye player
            Player p_bye;
            int bye_p = this->removeByePlayer(&this->players, this->bye_queue[0], &p_bye);
            this->bye_queue.erase(this->bye_queue.begin());
            this->player_count -= 1;
            this->logExplanation("Trying to give player BYE: " + p_bye.getName() + " (ID = " + std::to_string(p_bye.getID()) + ")");
            groups = this->makeGroups();
            curr = groups.getHead();
            this->logExplanation("Attempting to pair tournament...");
            games = this->makePairingForGroup(curr, pairing_round);
            
            // put bye player back
            this->players.insert(std::next(this->players.begin(), bye_p), p_bye);
            this->player_count += 1;
            
            // check for errors
            if(this->pairing_error) {
                this->logExplanation("Pairings cannot be generated! Trying to give another player a BYE and do pairings again...");
                // put bye player back and try dequeing another bye player
                this->pairing_error = false;
                games.clear();
                continue;
            }
            
            // assert we have the right number of pairs
            ASSERT(((int) games.size()) == (((int) this->players.size()) / 2), "Improper number of games created!");
            
            // we must have had success
            games.push_back(Match(p_bye, Player("", 0, -1, 0.0), true));
            // clear bye queue
            this->bye_queue.clear();
            // assert that we have the right numbers of pairs
            break;
        }
        
        // check if we got at least one pairing
        if(games.size() == 0) {
            // we failed to find a good pairing
            this->pairing_error = true;
        }
    } else {
        LinkedList groups = this->makeGroups();
        LinkedListNode *curr = groups.getHead();
        games = this->makePairingForGroup(curr, pairing_round);
    }
    
    // if we don't have any errors, then sort as according to 3.6
    if(!this->pairing_error) {
        // sorting for pairings can be found here: https://handbook.fide.com/chapter/GeneralHandlingRulesForSwissTournaments202602
        auto sort_pairings = [](const Match &a, const Match &b) {
            // 3.6.1 sort by those who have more points
            double pt1 = std::max(a.white.getPoints(), a.black.getPoints());
            double pt2 = std::max(b.white.getPoints(), b.black.getPoints());
            
            if(pt1 != pt2) return pt1 > pt2;
            
            // 3.6.2 sort by those who have a higher sum
            double s1 = a.white.getPoints() + a.black.getPoints();
            double s2 = b.white.getPoints() + b.black.getPoints();
            
            if(s1 != s2) return s1 > s2;
            
            // 3.6.3 sort by those who have a higher pairing num
            int h1 = std::min(a.white.getID(), a.black.getID());
            int h2 = std::min(b.white.getID(), b.black.getID());
            
            return h1 < h2;
        };
        
        std::sort(games.begin(), games.end(), sort_pairings);
    } else {
        this->logExplanation("No valid pairing exists!");
    }
    
    return games;
}

std::vector<CPPDubovSystem::Match> CPPDubovSystem::Tournament::fixBakuR1(const std::vector<Match> &games) const {
    // go through it all and correct all colors
    std::vector<Match> g;
    
    Color curr = Color::BLACK;
    
    for(int i = 0; i < games.size(); i++) {
        if(i == 0) {
            if(games[i].white.getRating() > games[i].black.getRating()) {
                curr = Color::BLACK;
                continue;
            }
            g.push_back(Match(games[i].black, games[i].white, false));
            continue;
        }
        if(curr == Color::BLACK) {
            g.push_back(Match(games[i].black, games[i].white, false));
            curr = Color::WHITE;
        } else {
            g.push_back(games[i]);
            curr = Color::BLACK;
        }
    }
    
    return g;
}

std::vector<CPPDubovSystem::Match> CPPDubovSystem::Tournament::generatePairings(int r) {
    if(r == 1) {
        return this->makeRound1();
    }
    this->current_round = r;
    return this->makeSubsequent(r);
}

std::vector<CPPDubovSystem::Match> CPPDubovSystem::Tournament::generatePairings(int r, bool baku_acceleration) {
    if(!baku_acceleration) {
        // do normal pairings
        return this->generatePairings(r);
    }
    // first sort all players properly
//    Utils::sortPlayersRating(&this->players, 0, ((int) this->players.size()) - 1);
//    this->sortPlayersPoints(&this->players, 0, ((int) this->players.size()) - 1);
    std::sort(this->players.begin(), this->players.end(), [](const Player &a, const Player &b){
        // compare points as primary sorting
        if(a.getPoints() != b.getPoints()) return a.getID() < b.getID();
        
        return a.getPoints() > b.getPoints();
    });
    
    // insert data into baku acceleration and apply virtual points
    BakuAcceleration ba(&this->players, r, this->total_rounds);
    ba.accelerate();
    
    // reset players
    this->players = ba.getPlayers();
    
    // apply pairings
    std::vector<Match> m;
    if(r == 1) {
        m = this->generatePairings(0);
        m = this->fixBakuR1(m);
    } else {
        m = this->generatePairings(r);
    }
    
    return m;
}

CPPDubovSystem::Tournament CPPDubovSystem::Tournament::makeTournament(const TRFUtil::TRFData &from_data, int *next_round, int stop_read) {
    Tournament t_main(from_data.getRoundsTnr());
    
    // set round 1 if needed
    t_main.setRound1Color(from_data.shouldRound1TopBeWhite());
    
    // add players
    std::vector<std::map<std::string, std::string>> players = from_data.getPlayerSection();
    
    // get byes for future reference
    std::set<int> byes = from_data.getBYEs();
    
    //std::string valid_res[13] = {"+", "-", "w", "d", "l", "1", "=", "0", "h", "f", "u", "z", " "}; // valid match results
    std::set<std::string> valid_res = {"+", "-", "w", "d", "l", "1", "=", "0", "h", "f", "u", "z", " "}; // valid match res
    std::map<std::string, double> res_pt;
    res_pt["+"] = 1.0;
    res_pt["-"] = 0.0;
    res_pt["="] = 0.5;
    res_pt["w"] = 1.0;
    res_pt["l"] = 0.0;
    res_pt["1"] = 1.0;
    res_pt["0"] = 0.0;
    res_pt["d"] = 0.5;
    res_pt["h"] = 0.5;
    res_pt["f"] = 1.0;
    res_pt["u"] = 1.0;
    res_pt["z"] = 0.0;
    res_pt[" "] = 0.0;
    res_pt["H"] = 0.5;
    res_pt["U"] = 1.0;
    res_pt["F"] = 1.0;
    res_pt[""] = 0.0;
    
    *next_round = from_data.getRounds();
    
    std::map<int, int> opp_ratings;
    std::map<int, double> float_info;
    
    std::vector<Player> players_list;
    int player_count = 0;
    
    std::map<std::string, int> match_eval_loc;
    
    std::vector<Utils::TRFMatch> match_eval; // for distributing floats
    std::vector<Utils::TRFMatch> round_capture; // for capturing rounds
    std::map<int, bool> r_skip; // for capturing rounds
    std::map<std::string, bool> r_skip_hash;
    std::map<int, int> player_pos; // for easy player locating
    std::set<std::string> forfeit_hash;
    
    Player *p_add;
    
    // go through all the raw data and interpret it all
    for(int i = 0; i < players.size(); i++) {
        int id = std::stoi(players[i]["rank"]);
        std::string name = players[i]["name"];
        int rating = std::stoi(players[i]["rating"]);
        double points = std::stod(players[i]["points"]);
        
        opp_ratings[id] = rating;
        
//        Player p_add(name, rating, id, points);
        p_add = new Player(name, rating, id, points);
        
        // render results
        for(int z = 1; z <= from_data.getRounds(); z++) {
            // make sure we don't go further than stop read if set
            if(stop_read > -1 && z > stop_read) {
                break;
            }
            std::string as_str = std::to_string(z);
            // check bye
            std::string opp = players[i]["r" + as_str + "_opp"];
            // 0000 or "   " means not paired
            // since spaces are automatically trimmed, simply check if opp is 000 or just ""
            if(opp == "000" || opp == "") {
                // bye
                // only insert bye status if and only if the round is not on stop read
                if(z < stop_read && stop_read > -1) {
                    p_add->setByeStatus(true);
//                    p_add->addColor(Color::NO_COLOR);
                } else if(stop_read == -1) {
                    p_add->setByeStatus(true);
//                    p_add->addColor(Color::NO_COLOR);
                }
                std::string resb = players[i]["r" + as_str + "_res"];
                Utils::TRFMatch tt = Utils::TRFMatch(p_add, nullptr, res_pt[resb], 0.0, z);
                tt.is_bye = true;
                match_eval.push_back(tt);
                std::string rh = std::to_string(p_add->getID()) + "_bye";
                // if the data read is on stop read make sure the bye is noted
                // this way even if it is not noted anywhere in the trf data, the player still gets the bye
                if(z == stop_read) {
                    // also only insert if this type of bye is not 'U'
                    if(resb == "h" || resb == "z" || resb == "f" || resb == " " || resb == "H" || resb == "Z" || resb == "F") {
                        byes.insert(p_add->getID());
                    }
                    r_skip_hash[rh] = true;
                }
                continue;
            }
            int opp_id = std::stoi(opp);
            
            // expect color
            std::string color = players[i]["r" + as_str + "_color"];
            if(color != "w" && color != "b" && color != "-" && color != " ") {
                throw std::invalid_argument("unexpected character given for color for non bye player. Expected 'w' or 'b' but got " + color);
            }
            
            // expect result
            std::string res = players[i]["r" + as_str + "_res"];
            
            char res_as_char = res.at(0);
            res_as_char = std::tolower(res_as_char);
            res = res_as_char;
            
            // make sure result is valid
//            if(std::find(std::begin(valid_res), std::end(valid_res), res) == std::end(valid_res)) {
            if(!valid_res.contains(res)) {
                throw std::invalid_argument("no such result '" + res + "' appears to exist for result of match");
            }
            
            // now that everything is valid we can add it to the list
            // just make sure res is not forfeit
            if(res == "-" || res == "+" || res == "h" || res == "f" || res == "u" || res == "z" || res == " ") {
//                if(z < stop_read && stop_read > -1) {
//                    p_add->addColor(Color::NO_COLOR);
//                } else if(stop_read == -1) {
//                    p_add->addColor(Color::NO_COLOR);
//                }
                if(res == "-" || res == "+") {
                    if(color == "w") {
                        std::string forfeit_raw_hash = players[i]["rank"] + "_" + as_str + "_" + opp;
                        forfeit_hash.insert(forfeit_raw_hash);
                        if(match_eval_loc.find(forfeit_raw_hash) != match_eval_loc.end()) {
                            match_eval[match_eval_loc[forfeit_raw_hash]].setWhite(p_add);
                            match_eval[match_eval_loc[forfeit_raw_hash]].setWhitePts(res_pt[res]);
                            if(z == stop_read) {
                                round_capture.push_back(match_eval[match_eval_loc[forfeit_raw_hash]]);
                                r_skip_hash[forfeit_raw_hash] = true;
                            }
                            continue;
                        } else {
//                            match_eval[match_eval_loc[forfeit_raw_hash]].setBlack(p_add);
//                            match_eval[match_eval_loc[forfeit_raw_hash]].setBlackPts(res_pt[res]);
//                            if(z == stop_read) {
//                                round_capture.push_back(match_eval[match_eval_loc[forfeit_raw_hash]]);
//                                r_skip_hash[forfeit_raw_hash] = true;
//                            }
                            match_eval_loc[forfeit_raw_hash] = (int) match_eval.size();
                            r_skip_hash[forfeit_raw_hash] = false;
                            r_skip[match_eval_loc[forfeit_raw_hash]] = false;
                            match_eval.push_back(Utils::TRFMatch(p_add, nullptr, res_pt[res], 0.0, z));
                        }
//                        match_eval_loc[forfeit_raw_hash] = (int) match_eval.size();
//                        r_skip_hash[forfeit_raw_hash] = false;
//                        match_eval.push_back(Utils::TRFMatch(p_add, nullptr, res_pt[res], 0, z));
                    } else {
                        std::string forfeit_raw_hash = opp + "_" + as_str + "_" + players[i]["rank"];
                        forfeit_hash.insert(forfeit_raw_hash);
                        if(match_eval_loc.find(forfeit_raw_hash) != match_eval_loc.end()) {
                            match_eval[match_eval_loc[forfeit_raw_hash]].setBlack(p_add);
                            match_eval[match_eval_loc[forfeit_raw_hash]].setBlackPts(res_pt[res]);
                            if(z == stop_read) {
                                round_capture.push_back(match_eval[match_eval_loc[forfeit_raw_hash]]);
                                r_skip_hash[forfeit_raw_hash] = true;
                            }
                            continue;
                        } else {
//                            match_eval_loc[forfeit_raw_hash] = (int) match_eval.size();
//                            r_skip_hash[forfeit_raw_hash] = false;
//                            match_eval.push_back(Utils::TRFMatch(nullptr, p_add, 0.0, res_pt[res], z));
                        }
                        match_eval_loc[forfeit_raw_hash] = (int) match_eval.size();
                        r_skip[match_eval_loc[forfeit_raw_hash]] = false;
                        r_skip_hash[forfeit_raw_hash] = false;
                        match_eval.push_back(Utils::TRFMatch(nullptr, p_add, 0, res_pt[res], z));
                    }
                }
                continue;
            }
            
            std::string raw_match_hash = "";
            
//            p_add->addOpp(opp_id);
            if(stop_read == -1) {
                p_add->addOpp(opp_id);
            } else {
                if(z < stop_read) {
                    p_add->addOpp(opp_id);
                }
            }
            if(color == "w") {
                if(stop_read == -1) {
                    p_add->addColor(Color::WHITE);
                } else {
                    if(z < stop_read) {
                        p_add->addColor(Color::WHITE);
                    }
                }
                raw_match_hash = players[i]["rank"] + "_" + as_str + "_" + opp;
                if(match_eval_loc.find(raw_match_hash) != match_eval_loc.end()) {
                    match_eval[match_eval_loc[raw_match_hash]].setWhite(p_add);
                    match_eval[match_eval_loc[raw_match_hash]].setWhitePts(res_pt[res]);
                    if(z == stop_read) {
                        round_capture.push_back(match_eval[match_eval_loc[raw_match_hash]]);
//                        r_skip[match_eval_loc[raw_match_hash]] = true;
                        r_skip_hash[raw_match_hash] = true;
                    }
                    continue;
                }
                match_eval_loc[raw_match_hash] = (int) match_eval.size();
                r_skip[match_eval_loc[raw_match_hash]] = false;
                r_skip_hash[raw_match_hash] = false;
                match_eval.push_back(Utils::TRFMatch(p_add, nullptr, res_pt[res], 0, z));
            } else {
                if(stop_read == -1) {
                    p_add->addColor(Color::BLACK);
                } else {
                    if(z < stop_read) {
                        p_add->addColor(Color::BLACK);
                    }
                }
                raw_match_hash = opp + "_" + as_str + "_" + players[i]["rank"];
                if(match_eval_loc.find(raw_match_hash) != match_eval_loc.end()) {
                    match_eval[match_eval_loc[raw_match_hash]].setBlack(p_add);
                    match_eval[match_eval_loc[raw_match_hash]].setBlackPts(res_pt[res]);
                    if(z == stop_read) {
                        round_capture.push_back(match_eval[match_eval_loc[raw_match_hash]]);
//                        r_skip[match_eval_loc[raw_match_hash]] = true;
                        r_skip_hash[raw_match_hash] = true;
                    }
                    continue;
                }
                match_eval_loc[raw_match_hash] = (int) match_eval.size();
                r_skip[match_eval_loc[raw_match_hash]] = false;
                r_skip_hash[raw_match_hash] = false;
                match_eval.push_back(Utils::TRFMatch(nullptr, p_add, 0, res_pt[res], z));
            }
        }
        
        // add player
//        t_main.addPlayer(p_add);
        player_pos[p_add->getID()] = (int) players_list.size();
        players_list.push_back(*p_add);
        player_count += 1;
        // we don't need to delete p_add since we are going to be using the object created anyways
    }
    
    Utils::sortRawMatches(&match_eval, 0, ((int) match_eval.size()) - 1);
    
    // set up floats
    for(int i = 0; i < match_eval.size(); i++) {
        // check if we are skipping
        if(match_eval[i].getTargetRound() == stop_read) continue;
        std::string mh = std::to_string(match_eval[i].getWID()) + "_" + std::to_string(match_eval[i].getTargetRound()) + "_" + std::to_string(match_eval[i].getBID());
//        if(r_skip_hash[mh]) {
//            continue;
//        }
        if(match_eval[i].is_bye) {
            std::string rrh = std::to_string(match_eval[i].getWID()) + "_bye";
//            if(r_skip_hash[rrh]) {
//                continue;
//            }
            int wib = match_eval[i].getWID();
            int wbw = player_pos[wib];
            players_list[wbw].trf_pts += match_eval[i].white_pts;
            continue;
        }
        int wi = match_eval[i].getWID();
        int bi = match_eval[i].getBID();
        int w = player_pos[wi];
        int b = player_pos[bi];
        
        // reset floats
        players_list[w].setUpfloatPrevStatus(false);
        players_list[b].setUpfloatPrevStatus(false);
        
        // adjust floats
        // anybody with a forfeit doesn't count for a float because the game wasn't played!
        //TODO: WHAT ABOUT PLAYERS WITH A FORFEIT LOSS? IF THEY PLAYED SOMEONE WITH A HIGHER SCORE AND GOT A FORFEIT LOSS, DO THEY STILL GET MARKED AS AN UPFLOATER EVEN THOUGH THE GAME WAS NEVER PLAYED?
        if(players_list[w].trf_pts > players_list[b].trf_pts && !forfeit_hash.contains(mh)) {
            players_list[b].incrementUpfloat();
            players_list[b].setUpfloatPrevStatus(true);
        } else if(players_list[w].trf_pts < players_list[b].trf_pts && !forfeit_hash.contains(mh)) {
            players_list[w].incrementUpfloat();
            players_list[w].setUpfloatPrevStatus(true);
        }
        players_list[w].trf_pts += match_eval[i].white_pts;
        players_list[b].trf_pts += match_eval[i].black_pts;
    }
    
    // prepare all pairing restrictions to add
    std::map<int, std::set<int>> restrictions_to_add;
    std::vector<std::pair<int, int>> pr = from_data.getPairingRestrictions();
    std::set<int> rexists;
    for(int i = 0; i < pr.size(); i++) {
        if(!rexists.contains(pr[i].first)) {
            restrictions_to_add[pr[i].first] = std::set<int>();
            rexists.insert(pr[i].first);
        }
        if(!rexists.contains(pr[i].second)) {
            restrictions_to_add[pr[i].second] = std::set<int>();
            rexists.insert(pr[i].second);
        }
        restrictions_to_add[pr[i].first].insert(pr[i].second);
        restrictions_to_add[pr[i].second].insert(pr[i].first);
    }
    
    // add opponent ratings to all players
    for(int i = 0; i < player_count; i++) {
        std::vector<int> opp_p = players_list[i].getOppPlayed();
        for(int z = 0; z < players_list[i].getOppCount(); z++) {
            players_list[i].addOppRating(opp_ratings[opp_p[z]]);
        }
        
        // apply pairing restrictions
        if(rexists.contains(players_list[i].getID())) {
            for(auto& rr : restrictions_to_add[players_list[i].getID()]) {
                players_list[i].addPairingRestriction(rr);
            }
        }
        
        // set players points to trf points if stop_read > -1
        if(stop_read > -1) {
            double pt = players_list[i].getPoints();
            double trfpt = players_list[i].trf_pts;
            double add = trfpt - pt;
            players_list[i].addPoints(add);
        }
        
        // if for whatever reason the player requested a bye, don't add to list
        if(byes.contains(players_list[i].getID())) {
            continue;
        }
        
        // add to list
        t_main.addPlayer(players_list[i]);
    }
    
    // set raw capture as necessary
    t_main.setRawMatch(round_capture);
    
    return t_main;
}

std::string CPPDubovSystem::Tournament::simulateTournament(int p_count, int max_round) {
    
    std::map<int, int> player_raw_loc; // this basically points to the index the player object is located in player_raw and normal_player given the id of the player
    std::vector<RandomTournamentGenerator::RawPlayer> player_raw; // this is where the trf data of the player will be stored
    std::vector<Player> normal_player; // this is the list of real players!
    
    std::vector<std::string> match_res;
    // just some random results that will be recorded into the file
    match_res.push_back("1");
    match_res.push_back("0");
    match_res.push_back("=");
    match_res.push_back("-");
    match_res.push_back("+");
    
    // highest and lowest rating allowed to be set
    int highest_rating = 3000;
    int lowest_rating = 100;
    
    std::vector<Player> pairing_num_sort; // for sorting players and making sure their starting rank aligns
    
    srand((unsigned int) time(nullptr));
    
    // add nth players to list and simulate tournament until max_round
    for(int i = 1; i <= p_count; i++) {
//        player_raw_loc[i] = (int) player_raw.size();
        
        int rating = (rand() % (highest_rating - lowest_rating + 1) + lowest_rating); // random rating
        
        // add player to both raw and normal
//        normal_player.push_back(Player("PLAYER " + std::to_string(i), rating, i, 0.0));
//        player_raw.push_back(RandomTournamentGenerator::RawPlayer("PLAYER " + std::to_string(i), rating, i));
        pairing_num_sort.push_back(Player("p", rating, i, 0.0));
    }
    
    Utils::sortPlayersRatingRTG(&pairing_num_sort, 0, ((int) pairing_num_sort.size()) - 1);
    // transfer the sorted player partition to the normal and raw
    for(int i = 0; i < pairing_num_sort.size(); i++) {
        player_raw_loc[i + 1] = (int) player_raw.size();
        normal_player.push_back(Player("PLAYER " + std::to_string(i + 1), pairing_num_sort[i].getRating(), i + 1, 0.0));
        player_raw.push_back(RandomTournamentGenerator::RawPlayer("PLAYER " + std::to_string(i + 1), pairing_num_sort[i].getRating(), i + 1));
    }
    
    int player_ress[12] = {0, 1, 0, 1, 0, 1, 2, 2, 0, 1, 3, 4};
    
    int expected_num_pairings = std::floor(((double) p_count) / 2.0);
    if(p_count % 2 > 0)
        expected_num_pairings += 1;
    
    // begin simulation by generating round by round
    for(int i = 1; i <= max_round; i++) {
        // make a new tournament and add out player list into that tournament
        Tournament simulated_tournament(max_round);
        
        for(int z = 0; z < normal_player.size(); z++) {
            simulated_tournament.addPlayer(normal_player[z]);
        }
        
        // generate pairings
        std::vector<Match> simulated_output = simulated_tournament.generatePairings(i);
        
        // make sure of course there are no errors
        if(simulated_tournament.pairingErrorOccured()) {
            // hopefully this is unlikely to happen!
            break;
        }
        
        // assert that we have the right number of pairings
        ASSERT(((int) simulated_output.size()) == expected_num_pairings, "Improper number of pairings generated!");
        
        int bye_count = 0; // for assertion purposes
        
        // apply random results
        // then apply those results to the players
        for(int z = 0; z < simulated_output.size(); z++) {
            Player wg = simulated_output[z].white;
            int w_loc = player_raw_loc[wg.getID()];
            
            
            // check bye inf
            if(simulated_output[z].is_bye) {
//                Player p_repw(wg.getName(), wg.getRating(), wg.getID(), wg.getPoints() + 1.0);
//                p_repw.setByeStatus(true);
//                normal_player[w_loc] = p_repw;
                normal_player[w_loc].setByeStatus(true);
                normal_player[w_loc].addPoints(1.0);
                
                // also reflect in raw
                player_raw[w_loc].incrementRawPoints(1.0);
                player_raw[w_loc].appendRawGame("0000 - +");
                bye_count += 1;
                continue;
            }
            
            Player bg = simulated_output[z].black;
            int b_loc = player_raw_loc[bg.getID()];
            
            // recreate player in the same way as bye
            int rr = (rand() % 12);
//            int white_rand_res = (rand() % 5);
            int white_rand_res = player_ress[rr];
            
            // reset float prev status
            normal_player[w_loc].setUpfloatPrevStatus(false);
            normal_player[b_loc].setUpfloatPrevStatus(false);
            
            // adjust upfloats
            if(wg.getPoints() < bg.getPoints() && white_rand_res <= 2) {
                normal_player[w_loc].setUpfloatPrevStatus(true);
                normal_player[w_loc].incrementUpfloat();
            } else if(wg.getPoints() > bg.getPoints() && white_rand_res <= 2) {
                normal_player[b_loc].setUpfloatPrevStatus(true);
                normal_player[b_loc].incrementUpfloat();
            }
            
            ASSERT(white_rand_res <= 4, "Improper computed random result for rtg!"); // just to make sure that the value is proper
            
            switch (white_rand_res) {
                case 0:
                    normal_player[w_loc].addPoints(1.0);
                    normal_player[w_loc].addOpp(bg.getID());
                    normal_player[w_loc].addColor(Color::WHITE);
                    player_raw[w_loc].appendRawGame(std::to_string(bg.getID()) + " w " + match_res[white_rand_res]);
                    normal_player[w_loc].addOppRating(bg.getRating());
                    
                    normal_player[b_loc].addOpp(wg.getID());
                    normal_player[b_loc].addColor(Color::BLACK);
                    normal_player[b_loc].addOppRating(wg.getRating());
                    
                    // also reflect in raw
                    player_raw[w_loc].incrementRawPoints(1.0);
                    player_raw[b_loc].appendRawGame(std::to_string(wg.getID()) + " b 0");
                    break;
                
                case 1:
                    normal_player[b_loc].addPoints(1.0);
                    normal_player[w_loc].addOpp(bg.getID());
                    normal_player[w_loc].addColor(Color::WHITE);
                    player_raw[w_loc].appendRawGame(std::to_string(bg.getID()) + " w " + match_res[white_rand_res]);
                    normal_player[w_loc].addOppRating(bg.getRating());
                    
                    normal_player[b_loc].addOpp(wg.getID());
                    normal_player[b_loc].addColor(Color::BLACK);
                    normal_player[b_loc].addOppRating(wg.getRating());
                    
                    // also reflect in raw
                    player_raw[b_loc].incrementRawPoints(1.0);
                    player_raw[b_loc].appendRawGame(std::to_string(wg.getID()) + " b 1");
                    break;
                    
                case 2:
                    normal_player[w_loc].addPoints(0.5);
                    normal_player[b_loc].addPoints(0.5);
                    normal_player[w_loc].addOpp(bg.getID());
                    normal_player[w_loc].addColor(Color::WHITE);
                    player_raw[w_loc].appendRawGame(std::to_string(bg.getID()) + " w " + match_res[white_rand_res]);
                    normal_player[w_loc].addOppRating(bg.getRating());
                    
                    normal_player[b_loc].addOpp(wg.getID());
                    normal_player[b_loc].addColor(Color::BLACK);
                    normal_player[b_loc].addOppRating(wg.getRating());
                    
                    // also reflect in raw
                    player_raw[w_loc].incrementRawPoints(0.5);
                    player_raw[b_loc].incrementRawPoints(0.5);
                    player_raw[b_loc].appendRawGame(std::to_string(wg.getID()) + " b =");
                    break;
                    
                case 3:
                    normal_player[w_loc].addPoints(1.0);
                    normal_player[w_loc].setByeStatus(true);
                    player_raw[w_loc].appendRawGame(std::to_string(bg.getID()) + " w +");
                    
                    // also reflect in raw
                    player_raw[w_loc].incrementRawPoints(1.0);
                    player_raw[b_loc].appendRawGame(std::to_string(wg.getID()) + " b -");
                    break;
                    
                case 4:
                    normal_player[b_loc].addPoints(1.0);
                    normal_player[b_loc].setByeStatus(true); // anybody with forfeit win is marked as one who got the bye
                    player_raw[w_loc].appendRawGame(std::to_string(bg.getID()) + " w -");
                    
                    // also reflect in raw
                    player_raw[b_loc].incrementRawPoints(1.0);
                    player_raw[b_loc].appendRawGame(std::to_string(wg.getID()) + " b +");
                    break;
                default:
                    break;
            }
        }
        
        // lets just make sure we don't have more than 1 bye assigned!
        ASSERT(bye_count <= 1, "More than 1 byes were given in rtg!");
    }
    
    // now that the simulation of rounds is done, sort the players and insert their final rankings
//    Utils::sortPlayersRating(&normal_player, 0, ((int) normal_player.size()) - 1);
    std::sort(normal_player.begin(), normal_player.end(), [](const Player &a, const Player &b) {
        return a.getID() < b.getID();
    });
    sortPlayersPoints(&normal_player, 0, ((int) normal_player.size()) - 1);
    
    for(int i = 0; i < normal_player.size(); i++) {
        int p = player_raw_loc[normal_player[i].getID()];
        
        player_raw[p].setFinalRank(i + 1);
    }
    
    // now write up the trf data with simulated tournament
    return RandomTournamentGenerator::trfString(p_count, max_round, player_raw);
}
