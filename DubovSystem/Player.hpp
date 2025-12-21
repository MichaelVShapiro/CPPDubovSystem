//
//  Player.hpp
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

#ifndef Player_hpp
#define Player_hpp

#ifndef UNRATED
#define UNRATED 0u
#endif

#include <stdio.h>
#include <string>
#include <vector>
#include <set>

namespace CPPDubovSystem {

/**
 * A simple color for a match
 */
enum Color {
    WHITE, BLACK, NO_COLOR
};
/**
 * Color preference for a match
 */
enum ColorPreference {
    NO_PREFERENCE = 1u, MILD, ALTERNATION, ABSOLUTE
};

/**
 * Core player
 */
class Player {
private:
    /**
     * Player name
     */
    std::string name;
    /**
     * Rating of player
     */
    int rating;
    /**
     * Id
     */
    int id;
    /**
     * Number of points the player has
     */
    double points;
    /**
     * Opponents played to return
     */
    std::vector<int> opp_played_return;
    /**
     * Storage for opponents played
     */
    std::set<int> opp_played;
    /**
     * List of pairing restrictions
     */
    std::set<int> pairing_restrictions;
    /**
     * History of colors played with
     */
    std::vector<Color> color_hist;
    /**
     * Ratings of each opponent
     */
    std::vector<int> opp_rating;
    /**
     * Number of opponents played
     */
    int opp_count = 0;
    /**
     * Number of colors played
     */
    int color_count = 0;
    /**
     * Number of times upfloated
     */
    int numUpfloated = 0;
    /**
     * For easy memory storage of aro when we need it
     */
    double aro = -1.0;
    /**
     * For players who upfloated in the last round
     */
    bool upfloated_prev = false;
    /**
     * If the player had a bye/forfeit win
     */
    bool received_bye = false;
    /**
     * The saved due color
     */
    Color due_color_saved = Color::NO_COLOR;
    /**
     * Saved preference strength
     */
    ColorPreference strength = ColorPreference::NO_PREFERENCE;
    /**
     * If color preference was saved
     */
    bool has_saved_color = false;
    /**
     * If strength has been saved
     */
    bool has_saved_strength = false;
    
public:
    
    /**
     * For quick easy trf parsing
     */
    double trf_pts = 0.0;
    
    /**
     * Initializes the player with a given name, rating, id, and points
     */
    explicit Player(std::string name, int rating, int id, double points);
    /**
     * Default constructor for player
     */
    Player();
    
    /**
     * A getter for player name
     */
    std::string getName() const {return name;}
    /**
     * A getter for rating
     */
    int getRating() const {return rating;}
    /**
     * A getter for ID
     */
    int getID() const {return id;}
    /**
     * A getter for the number of colors played
     */
    int getNumColors() const {return color_count;}
    /**
     * Gets the number of opponents played
     */
    int getOppCount() const {return opp_count;}
    /**
     * A simple getter for numUpfloated
     */
    int getNumUpfloat() const {return numUpfloated;}
    /**
     * A simple getter for points
     */
    double getPoints() const {return points;}
    /**
     * Determines if the two players can play each other
     */
    bool canPlayOpp(Player &opp);
    /**
     * Gets the average rating of opponents
     */
    double getARO();
    /**
     * Adds an opponent to the list of played opponents
     */
    void addOpp(int id);
    /**
     * Adds a color to the list of colors
     */
    void addColor(Color c);
    /**
     * Gets the due color of the player
     */
    Color getDueColor();
    /**
     * Gets the strength of the due color
     */
    ColorPreference getPreferenceStrength();
    /**
     * Increments upfloat count
     */
    void incrementUpfloat();
    /**
     * Determines if the player can upfloat
     */
    bool canUpfloat(int cr) const;
    /**
     * Gets the number of times the player has upfloated while being a max upfloater
     */
    int getNumUpfloatedIfMaxUpfloater(int total_rounds) const;
    /**
     * A getter for opponents played
     */
    std::vector<int> getOppPlayed() const {return opp_played_return;}
    /**
     * Adds the opponents rating to the list of ratings
     */
    void addOppRating(int r);
    /**
     * Checks up a player upfloated previously
     */
    bool upfloatedPreviously() const {return upfloated_prev;}
    /**
     * Sets the status of the player upfloat
     */
    void setUpfloatPrevStatus(bool s);
    /**
     * Adds points to the player
     */
    void addPoints(const double pt);
    /**
     * Checks if the player has received a bye
     */
    bool hasReceievedBye() const {return received_bye;}
    /**
     * Sets the bye status of player
     */
    void setByeStatus(bool s) {received_bye = s;}
    /**
     * Adds a pairing restriction to a given opponent id
     */
    void addPairingRestriction(int opp_id);
    /**
     * Applies rule 5.2.4 and determines if the player should receive the due color
     */
    bool shouldAlternate(const Player &opp);
    /**
     * Determines if the color histories of both players are equal
     */
    bool isColorHistEqual(const Player &opp) const;
    /**
     * Gets the first color played
     */
    Color getFirstColorPlayed() const;
    /**
     * Just for overloading == operator
     */
    bool operator ==(const Player &other);
    /**
     * Just for overloading != operator
     */
    bool operator !=(const Player &other);
    /**
     * Just for overloading the \< operator
     */
    bool operator <(const Player &other);
    /**
     * Just for overloading the > operator
     */
    bool operator >(const Player &other);
    
    /**
     * Gets the maximum number of times a player is allowed to upfloat
     */
    static int getMaxUpfloatTimes(int total_rounds);
};
}

#endif /* Player_hpp */
