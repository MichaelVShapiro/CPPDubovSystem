//
//  trf.hpp
//  DubovSystem
//
//  Created by Michael Shapiro on 5/14/24.
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

#ifndef trf_hpp
#define trf_hpp

#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <set>

/**
 * For reading & writing TRF data
 */
namespace TRFUtil {
/**
 * Removes all spaces from a given string
 */
void clearSpaces(std::string *str_clear);
/**
 * A sample piece of TRF data
 */
class TRFData {
private:
    /**
     * Data captured in tournament section
     */
    std::map<std::string, std::string> tournament_section;
    /**
     * Data captured in player section
     */
    std::vector<std::map<std::string, std::string>> player_section;
    /**
     * Players who requested byes
     */
    std::set<int> byes;
    /**
     * List of forbidden pairings
     */
    std::vector<std::pair<int, int>> restricted_pairings;
    /**
     * Number of rounds captured
     */
    int rounds_captured;
    /**
     * Total number of tournament rounds (TNR code)
     */
    int rounds_tnr = -1; // we default it to -1 to detect the tnr code more easily
    /**
     * If acceleration was invoked
     */
    bool acceleration_on = false;
    /**
     * If round 1 top board should have the higher ranked player given white. Defaults to true
     */
    bool round1_white = true;
    /**
     * Parses player data
     */
    void parsePlayer(const std::string &line);
    /**
     * Parses the line for pairing restriction
     */
    void parseRestriction(const std::string &line);
    /**
     * Parses the line for XXZ
     */
    void parseXXZ(const std::string &line);
    /**
     * Parses the line for XXC
     */
    void parseXXC(const std::string &line);
public:
    /**
     * Default constructor
     */
    TRFData() = default;
    /**
     * Gets collected data from tournament section
     */
    std::map<std::string, std::string> getTournamentSection() const {return tournament_section;}
    /**
     * Gets player section info
     */
    std::vector<std::map<std::string, std::string>> getPlayerSection() const {return player_section;}
    /**
     * Gets all players who requested byes for a round
     */
    std::set<int> getBYEs() const {return byes;}
    /**
     * Gets the number of rounds read
     */
    int getRounds() const {return rounds_captured;}
    /**
     * Gets the rounds from the captured TNR code
     */
    int getRoundsTnr() const {return rounds_tnr;}
    /**
     * Determines if the top player in round 1 gets white
     */
    bool shouldRound1TopBeWhite() const {return round1_white;}
    /**
     * Determines if the tnr code exists in the file. When it is not present, false is returned. This should of course only be executed after the file has actually been read
     */
    bool tnrCodeExists() const {return this->rounds_tnr > 0;}
    /**
     * Parses a trf line
     */
    void parseLine(const std::string &line);
    /**
     * Determines if acceleration was invoked
     */
    bool isAccelerationOn() const;
    /**
     * Gets pairing restrictions imposed
     */
    std::vector<std::pair<int, int>> getPairingRestrictions() const {return restricted_pairings;}
};

/**
 * A single TRF file
 */
class TRFFile {
private:
    /**
     * The path of the file
     */
    std::string path;
public:
    /**
     * A simple constructor initalizing the path
     */
    TRFFile(const std::string &path);
    /**
     * A simple getter for path
     */
    std::string getPath() const {return path;}
    /**
     * Writes the TRF data to the path
     */
    void write(const std::string &trf);
    /**
     * Reads the TRF data
     */
    TRFData read();
};
}

#endif /* trf_hpp */
