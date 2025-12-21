//
//  Player.cpp
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

#include <algorithm>
#include <math.h>
#include "Player.hpp"

CPPDubovSystem::Player::Player(std::string name, int rating, int id, double points) {
    this->name = name;
    this->rating = rating;
    this->id = id;
    this->points = points;
}

CPPDubovSystem::Player::Player() {
    this->name = "UNNAMED PLAYER";
    this->rating = 0;
    this->id = -1;
    this->points = 0.0;
}

CPPDubovSystem::Color CPPDubovSystem::Player::getDueColor() {
    // check if already saved
    if(this->has_saved_color) {
        return this->due_color_saved;
    }
    int num_white = 0;
    int num_black = 0;
    std::vector<Color> storage; // where we store all colors that are not NO_COLOR
    for(int i = 0; i < this->getNumColors(); i++) {
        if(this->color_hist[i] == Color::WHITE) {
            num_white += 1;
            storage.push_back(Color::WHITE);
        } else if(this->color_hist[i] == Color::BLACK) {
            num_black += 1;
            storage.push_back(Color::BLACK);
        }
    }
    this->has_saved_color = true;
    if(num_white == 0 && num_black == 0) {
        this->due_color_saved = Color::NO_COLOR;
        return Color::NO_COLOR;
    }
    if(num_white == num_black) {
//        Color last_color = this->color_hist[this->getNumColors() - 1];
        Color last_color = storage[((int) storage.size()) - 1];
        if(last_color == Color::WHITE) {
            this->due_color_saved = Color::BLACK;
            return Color::BLACK;
        }
        this->due_color_saved = Color::WHITE;
        return Color::WHITE;
    } else if(num_white > num_black) {
        this->due_color_saved = Color::BLACK;
        return Color::BLACK;
    }
    this->due_color_saved = Color::WHITE;
    return Color::WHITE;
}

CPPDubovSystem::ColorPreference CPPDubovSystem::Player::getPreferenceStrength() {
    // check if this is already set
    if(this->has_saved_strength) {
        return this->strength;
    }
    this->has_saved_strength = true;
    // if there is only one color, preference is mild
    std::vector<Color> storage;
    if(this->color_count == 1 && this->color_hist[0] != Color::NO_COLOR) {
        this->strength = ColorPreference::MILD;
        return ColorPreference::MILD;
    }
    // same operation as getDueColor
    int num_white = 0;
    int num_black = 0;
    for(int i = 0; i < this->getNumColors(); i++) {
        if(this->color_hist[i] == Color::WHITE) {
            num_white += 1;
            storage.push_back(Color::WHITE);
        } else if(this->color_hist[i] == Color::BLACK) {
            num_black += 1;
            storage.push_back(Color::BLACK);
        }
    }
    if(num_white == 0 && num_black == 0) {
        this->strength = ColorPreference::NO_PREFERENCE;
        return ColorPreference::NO_PREFERENCE;
    }
    int storage_count = (int) storage.size();
    // if the player played the same colors twice in a row, the preference is absolute
    if(storage_count >= 2) {
//        if(this->color_hist[this->color_count - 1] == this->color_hist[this->color_count - 2]) {
//            this->strength = ColorPreference::ABSOLUTE;
//            return ColorPreference::ABSOLUTE;
//        }
        if(storage[storage_count - 1] == storage[storage_count - 2]) {
            this->strength = ColorPreference::ABSOLUTE;
            return this->strength;
        }
    }
    if(abs(num_white - num_black) == 1) {
        this->strength = ColorPreference::MILD;
        return ColorPreference::MILD;
    }
    if((num_white - num_black) == 0) {
        this->strength = ColorPreference::ALTERNATION;
        return ColorPreference::ALTERNATION;
    }
    // should be at least +2
    this->strength = ColorPreference::ABSOLUTE;
    return ColorPreference::ABSOLUTE;
}

void CPPDubovSystem::Player::addOpp(int id) {
    this->opp_played.insert(id);
    this->opp_played_return.push_back(id);
    this->opp_count += 1;
}

void CPPDubovSystem::Player::addColor(Color c) {
    this->color_count += 1;
    this->color_hist.push_back(c);
    // mark saved preference and strength as unsaved
    // the point of marking saved and unsaved is so that the value is only evaluated once
    // this way, when getPreferenceStrength and getDueColor is executed, at the first time the time complexity is O(n) (where n is the number of colors passed in)
    // after that call is done executing, it will save the values created from those function calls
    // that way, the time complexity becomes O(1)
    // and especially if this is needed to be executed multiple times (which will be the case), direct access to the saved variables will be given
    this->has_saved_color = false;
    this->has_saved_strength = false;
}

bool CPPDubovSystem::Player::canPlayOpp(Player &opp) {
    // check if both players have played each other before
//    if(std::find(this->opp_played.begin(), this->opp_played.end(), opp.getID()) != this->opp_played.end()) {
    if(this->opp_played.contains(opp.getID())) {
        return false;
    }
    // make sure there isn't a pairing restriction set onto this opponent
    if(this->pairing_restrictions.contains(opp.getID())) {
        return false;
    }
    // check if the absolute color preference is present in both players and both players need that color
    Color c1 = this->getDueColor();
    Color c2 = opp.getDueColor();
    if(c1 == c2) {
        ColorPreference s1 = this->getPreferenceStrength();
        ColorPreference s2 = opp.getPreferenceStrength();
        
        if(s1 == ColorPreference::ABSOLUTE && s2 == ColorPreference::ABSOLUTE) {
            return false;
        }
    }
    return true;
}

double CPPDubovSystem::Player::getARO() {
    // check if aro was already created
    if(this->aro > -1) {
        return this->aro;
    }
    // calculate average rating of opponents
    // if no opponents were played, aro is 0
    if(this->opp_count == 0) {
        this->aro = 0.0;
        return 0.0;
    }
    int tot = 0;
    for(int i = 0; i < this->opp_count; i++) {
        tot += this->opp_rating[i];
    }
    double tot_double = (double) tot;
    double opp_double = (double) this->opp_count;
    this->aro = (tot_double / opp_double);
    // 1.6.1 -> round to the nearest integer
    this->aro = std::round(this->aro);
    return this->aro;
}

void CPPDubovSystem::Player::addOppRating(int r) {
    this->opp_rating.push_back(r);
    // and mark aro as not saved
    this->aro = -1;
}

void CPPDubovSystem::Player::incrementUpfloat() {
    this->numUpfloated += 1;
}

bool CPPDubovSystem::Player::canUpfloat(int cr) const {
    // 1.7 -> max upfloat equals to 2 + [cr / 5]
    double crd = (double) cr;
    double f = floor(crd / 5.0);
    int fi = (int) f;
    int max_allowed = 2 + fi;
    if(this->numUpfloated >= max_allowed) {
        return false;
    }
    return true;
}

int CPPDubovSystem::Player::getNumUpfloatedIfMaxUpfloater(int total_rounds) const {
    // is this player a max upfloater?
    if(this->canUpfloat(total_rounds)) return 0; // can still upfloat
    
    // determine the number of times the player can upfloat
    int max_upfloat = Player::getMaxUpfloatTimes(total_rounds);
    
    // this is easy substraction
    return this->numUpfloated - max_upfloat;
}

void CPPDubovSystem::Player::setUpfloatPrevStatus(bool s) {
    this->upfloated_prev = s;
}

void CPPDubovSystem::Player::addPoints(const double pt) {
    this->points += pt;
}

void CPPDubovSystem::Player::addPairingRestriction(int opp_id) {
    this->pairing_restrictions.insert(opp_id);
}

CPPDubovSystem::Color CPPDubovSystem::Player::getFirstColorPlayed() const {
    if(this->color_count == 0) {
        return Color::NO_COLOR;
    }
    return this->color_hist[0];
}

bool CPPDubovSystem::Player::shouldAlternate(const Player &opp) {
    // first get the due color
    // it is assumed that both players have the same due color
    Color c = this->getDueColor();
    // locate the most recent round where both players have different colors
    // check if both color lengths are even
    if(this->color_count == opp.color_count) {
        // rounds follow up
        for(int i = this->color_count - 1; i >= 0; i--) {
            if(this->color_hist[i] != opp.color_hist[i] && this->color_hist[i] != Color::NO_COLOR && opp.color_hist[i] != Color::NO_COLOR) {
                // different color!
                // if this player had the same color which is due, the player alternates
                if(this->color_hist[i] == c) {
                    return false;
                }
                return true;
            }
        }
        // color history must be identical
        // since this function already assumes that the strength of preference for the color is equal, 5.2.5 applies
        // give the due color to the higher ranked player
        return *this > opp;
    }
    
    // this is harder since round count is not the same
    // however the logic is the same
    int maxi = std::max(this->color_count, opp.color_count);
    
    for(int i = maxi; i >= 0; i--) {
        if(i >= this->color_count || i >= opp.color_count) {
            // no reason to check
            continue;
        }
        if(this->color_hist[i] != opp.color_hist[i]) {
            // different color!
            if(this->color_hist[i] == c) {
                return false;
            }
            return true;
        }
    }
    
    // apply similar operation
//    return this->getID() < opp.getID();
    return *this > opp;
}

bool CPPDubovSystem::Player::isColorHistEqual(const Player &opp) const {
    // lets check the lengths of both due colors
    if(this->color_hist.size() != opp.color_hist.size()) return false;
    
    // this is easy, just check each index position of the color hist
    for(int i = 0; i < this->color_hist.size(); i++) {
        if(this->color_hist[i] != opp.color_hist[i]) return false;
    }
    
    return true;
}

bool CPPDubovSystem::Player::operator==(const Player &other) {
    return this->getID() == other.getID();
}

bool CPPDubovSystem::Player::operator!=(const Player &other) {
    return this->getID() != other.getID();
}

bool CPPDubovSystem::Player::operator<(const Player &other) {
    // we first compare the points
    if(this->getPoints() > other.getPoints()) return false;
    else if(this->getPoints() < other.getPoints()) return true;
    
    // points are equal, so we look at the pairing number
    // it is known that pairing numbers are unique, so those simply cannot be equal
    // a smaller pairing number denotes a higher position
    if(this->getID() < other.getID()) return false;
    
    return true; // other has a smaller pairing number
}

bool CPPDubovSystem::Player::operator>(const Player &other) {
    // we basically do the oppoisite of < operator
    if(this->getPoints() > other.getPoints()) return true;
    else if(this->getPoints() < other.getPoints()) return false;
    
    if(this->getID() < other.getID()) return true;
    
    return false;
}

int CPPDubovSystem::Player::getMaxUpfloatTimes(int total_rounds) {
    // 1.7 -> max upfloat equals to 2 + [cr / 5]
    double crd = (double) total_rounds;
    double f = floor(crd / 5.0);
    int fi = (int) f;
    int max_allowed = 2 + fi;
    return max_allowed;
}
