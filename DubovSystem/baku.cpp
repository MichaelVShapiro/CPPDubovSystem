//
//  baku.cpp
//  DubovSystem
//
//  Created by Michael Shapiro on 7/1/24.
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

#include "baku.hpp"

CPPDubovSystem::BakuAcceleration::BakuAcceleration(std::vector<Player> *players, int target_round, int total_rounds) {
    this->players = players;
    this->target_round = target_round;
    this->total_rounds = total_rounds;
}

std::vector<CPPDubovSystem::Player> CPPDubovSystem::BakuAcceleration::getPlayers() {
    return *this->players;
}

int CPPDubovSystem::BakuAcceleration::getAcceleratedRounds() const {
    // accelerated rounds is total round in tournament / 2 (and rounded up)
    double d1 = ((double) this->total_rounds) / 2.0;
    int acc = (int) ceil(d1);
    
    return acc;
}

double CPPDubovSystem::BakuAcceleration::getVirtualPoints() const {
    double virtual_points = 1.0;
    // the number of virtual points given for a player is reduced to 0.5 if the targer round is > the number of accelerated rounds / 2 (rounded upwards)
    double d1 = ((double) this->getAcceleratedRounds()) / 2.0;
    int ru = (int) ceil(d1);
    if(this->target_round > ru) {
        virtual_points = 0.5;
    }
    return virtual_points;
}

void CPPDubovSystem::BakuAcceleration::r1Acceleration() {
    double d1 = ceil(((double) this->players->size()) / 4.0);
    int d2 = 2 * ((int) d1);
    // anyone in the range of d2 gets a virtual point
    for(int i = 0; i < d2; i++) {
        (*this->players)[i].addPoints(1.0);
    }
}

void CPPDubovSystem::BakuAcceleration::accelerate() {
    // if this is round 1, do a separate acceleration
    if(this->target_round == 1) {
        this->r1Acceleration();
        return;
    }
    // check if target round is accelerated
    int accelerated_rounds = this->getAcceleratedRounds();
    if(this->target_round > accelerated_rounds) {
        // not accelerated round
        return;
    }
    // it is already assumed players are properly sorted
    // find the bounds for each group
    std::vector<std::pair<int, int>> group_bounds;
    
    int bound1 = 0;
    int bound2 = 0;
    
    double curr_pt = (*this->players)[0].getPoints();
    
    int n = 0;
    
    for(int i = 1; i < this->players->size(); i++) {
        Player p = (*this->players)[0];
        // check if scores match
        if(p.getPoints() != curr_pt) {
            // group end point found!
            bound2 = i - 1;
            group_bounds.push_back(std::make_pair(bound1, bound2));
            bound1 = i;
        }
        n += 1;
    }
    
    // include ending point as final bound
    group_bounds.push_back(std::make_pair(bound1, n));
    
    // get number of points to give to player
    double virtual_points = this->getVirtualPoints();
    
    // go through groups and assign virtual points
    for(int i = 0; i < group_bounds.size(); i++) {
        // get number of players in group
        int group_count = abs(group_bounds[i].second - group_bounds[i].first);
        // get upper half players
        int upper_half = (int) std::floor(((double) group_count) / 2.0);
        for(int z = group_bounds[i].first; z < upper_half; z++) {
            (*this->players)[z].addPoints(virtual_points);
        }
    }
}
