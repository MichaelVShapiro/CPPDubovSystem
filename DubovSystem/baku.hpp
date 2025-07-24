//
//  baku.hpp
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

#ifndef baku_hpp
#define baku_hpp

#include <stdio.h>
#include <cmath>
#include "Player.hpp"

/// This file is responsible for initializaing the "virtual points" for the baku acceleration
/// It is no way generates baku acceleration pairings, it only sets it all up

// https://handbook.fide.com/chapter/C0405202507

namespace CPPDubovSystem {
/**
 * For making accelerated pairings
 */
class BakuAcceleration {
private:
    /**
     * The players we are working with
     */
    std::vector<Player> *players;
    /**
     * Target round
     */
    int target_round;
    /**
     * Total rounds in tournament
     */
    int total_rounds;
    /**
     * Gives virtual points for acceleration in round 1
     */
    void r1Acceleration();
public:
    /**
     * For initializing the acceleration. This already assumes that players are properly sorted
     */
    explicit BakuAcceleration(std::vector<Player> *players, int target_round, int total_rounds);
    /**
     * Copy not allowed
     */
    BakuAcceleration(const BakuAcceleration &ba) = delete;
    /**
     * Move not allowed
     */
    BakuAcceleration(BakuAcceleration &ba) = delete;
    /**
     * Assignment operation not allowed
     */
    BakuAcceleration &operator=(const BakuAcceleration &ba) = delete;
    /**
     * Move assignment not allowed
     */
    BakuAcceleration &operator=(BakuAcceleration &&ba) = delete;
    /**
     * Gets the rounds which should have accelerated pairings
     */
    int getAcceleratedRounds() const;
    /**
     * Gets the number of virtual points to give
     */
    double getVirtualPoints() const;
    /**
     * Applies the "virtaul points" to the players
     */
    void accelerate();
    /**
     * Gets the players
     */
    std::vector<Player> getPlayers();
};
}

#endif /* baku_hpp */
