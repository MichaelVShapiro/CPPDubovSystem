//
//  trf.cpp
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

#include "trf.hpp"
#include <fstream>
#include <string>

// https://www.fide.com/FIDE/handbook/C04Annex2_TRF16.pdf

//TODO: THIS MIGHT BE THE NEW VERSION FOR TRF: https://tec.fide.com/wp-content/uploads/2025/01/TRF25-FinalDraft.pdf

void TRFUtil::clearSpaces(std::string *str_clear) {
    std::string str_rep = "";
    for(int i = 0; i < str_clear->size(); i++) {
        std::string ss = str_clear->substr(i, 1);
        if(ss == " ") {
            continue;
        }
        str_rep += ss;
    }
    *str_clear = str_rep;
}

void TRFUtil::TRFData::parsePlayer(const std::string &line) {
    // we assume the 001 is not present
    
    // make sure at least 89 characters are present
    if(((int) line.size()) < 89) {
        throw std::length_error("player length is not long enough");
    }
    
    std::map<std::string, std::string> player_data;
    
    // pos 5-8 ranking
    player_data["rank"] = line.substr(4, 4);
    TRFUtil::clearSpaces(&player_data["rank"]);
    
    // pos 10 sex
    player_data["sex"] = line.substr(8, 1);
    
    // pos 11-13 title
    player_data["title"] = line.substr(9, 2);
    
    // pos 15-47 name
    player_data["name"] = line.substr(14, 32);
    // remove any spaces from name
//    std::erase(player_data["name"], " ");
    TRFUtil::clearSpaces(&player_data["name"]);
    
    // pos 49-52 rating
    player_data["rating"] = line.substr(48, 4);
    // also remove spaces
//    std::erase(player_data["rating"], " ");
    TRFUtil::clearSpaces(&player_data["rating"]);
    
    // pos 54-56 fed
    player_data["fed"] = line.substr(53, 2);
    
    // pos 58-68 id
    player_data["id"] = line.substr(57, 10);
    
    // 70-79 player birthdate
    player_data["bday"] = line.substr(69, 9);
    
    // pos 81-84 points
    player_data["points"] = line.substr(80, 4);
//    std::erase(player_data["points"], " ");
    TRFUtil::clearSpaces(&player_data["points"]);
    
    // pos 86-89 rank
    player_data["rankpos"] = line.substr(85, 3);
//    std::erase(player_data["rank"], " ");
    TRFUtil::clearSpaces(&player_data["rankpos"]);
    
    if(((int) line.size()) > 89) {
        // pos 92-n results & pairings
        
        int curr_pos = 92;
//        int curr_pos = 91;
        int r_rec = 1;
        while(curr_pos < ((int) line.size())) {
            std::string r_rec_str = std::to_string(r_rec);
            // n-n+5 opp rank
            player_data["r" + r_rec_str + "_opp"] = line.substr(curr_pos, 3); // ORIGINAL 3
            TRFUtil::clearSpaces(&player_data["r" + r_rec_str + "_opp"]);
            
            curr_pos += 4;
            
            // n+6 color | bye res
            player_data["r" + r_rec_str + "_color"] = line.substr(curr_pos, 1);
            curr_pos += 2;
            
            // n+7 result
            player_data["r" + r_rec_str + "_res"] = line.substr(curr_pos, 1);
            curr_pos += 4;
            
            r_rec += 1;
        }
        
        this->rounds_captured = r_rec - 1;
    }
    
    // add data
    this->player_section.push_back(player_data);
}

bool TRFUtil::TRFData::isAccelerationOn() const {
    // acceleration is on if the ACC code is set to "true" or "1"
    return this->acceleration_on;
}

void TRFUtil::TRFData::parseRestriction(const std::string &line) {
    // it is assumed that 'FOR' is not present in line
    // pairing restrictions should be divided up by a single space and the ID of both players
    std::string p1 = "";
    std::string p2 = "";
    bool add_p1 = true;
    // do some lexing to get both player IDs
    for(int i = 0; i < line.size(); i++) {
        std::string ch = line.substr(i, 1);
        if(ch == " ") {
            // make sure add_p1 is true
            if(add_p1) {
                add_p1 = false;
                continue;
            }
            throw std::invalid_argument("Unexpected space found in 'FOR' keycode");
        }
        if(add_p1) {
            p1 += ch;
        } else {
            p2 += ch;
        }
    }
    // append to pairs
    this->restricted_pairings.push_back(std::make_pair(std::stoi(p1), std::stoi(p2)));
}

void TRFUtil::TRFData::parseXXZ(const std::string &line) {
    // it is assumed data passed into line is everything after 'XXZ '
    // data appears to be stored as a list of numbers with a space separating each id
    
    // loop over the line and add each number to the list of byes
    std::string full_num;
    for(auto i : line) {
        // if i is a space, we are reaching a new item
        if(i == ' ') {
            this->byes.insert(std::stoi(full_num));
            full_num = "";
            continue;
        }
        full_num.push_back(i);
    }
    
    // there is no space at the end of this line
    // so just include this number into the set as well
    // as long as the string is not empty
    if(!full_num.empty()) this->byes.insert(std::stoi(full_num));
}

void TRFUtil::TRFData::parseXXC(const std::string &line) {
    // it is assumed data passed into line is everything after 'XXC '
    // this line can have one of the following:
    // - XXC white1
    // - XXC black1
    // - XXC rank
    // where white1 and black1 represent the start color for round 1 respectively
    // rank is basically where the pairing engine picks the color (in this case, the default is white)
    if(line != "white1" && line != "black1" && line != "rank")
        throw "Expected value 'white1' OR 'black1' OR 'rank' for XXC code";
    
    if(line == "black1") this->round1_white = false;
}

void TRFUtil::TRFData::parseLine(const std::string &line) {
    // make sure we have at least 3 chars
    if(((int) line.size()) == 0) {
        return;
    }
    if(((int) line.size()) < 3) {
        throw std::length_error("invalid input given for line");
    }
    // we start with a identification number (first 3 chars)
    std::string line_id = line.substr(0, 3);
    std::map<std::string, std::string> keys;
    keys["012"] = "name";
    keys["013"] = "team_data";
    keys["022"] = "city";
    keys["032"] = "fed";
    keys["042"] = "start";
    keys["052"] = "end";
    keys["062"] = "player_num";
    keys["072"] = "num_players_rated";
    keys["082"] = "team_num";
    keys["092"] = "type";
    keys["102"] = "arbiter";
    keys["112"] = "deputy_arbiter";
    keys["122"] = "time_control";
    keys["132"] = "dates";
    keys["001"] = "players";
    keys["XXR"] = "rounds";
    keys["TNR"] = "rounds";
    keys["ACC"] = "baku_acceleration";
    keys["BYE"] = "bye";
    keys["FOR"] = "pairing_restriction";
    keys["XXZ"] = "bye_list";
    keys["XXC"] = "round1_color";
    keys["XXP"] = "pairing_restriction";
    
    // make sure line id is valid
    if(keys.find(line_id) == keys.end()) {
        throw std::invalid_argument("Unidentified line " + line_id + " detected");
    }
    
    // we will deal with players separatly
    if(line_id == "001") {
//        this->parsePlayer(line.substr(3));
        this->parsePlayer(line);
        return;
    } else if(line_id == "TNR" || line_id == "XXR") {
        this->rounds_tnr = std::stoi(line.substr(3));
    } else if(line_id == "ACC") {
        // check value
        std::string s = line.substr(4);
        if(s == "true" || s == "1") {
            this->acceleration_on = true;
        } else if(s == "false" || s == "0") {
            this->acceleration_on = false;
        } else {
            throw std::invalid_argument("Unknown acceleration value passed in for TRF code 'ACC'. Expected 'true', '1', 'false', or '0' but got " + s + ". If there are any extra spaces after the value, make sure they are deleted");
        }
    } else if(line_id == "BYE") {
        std::string s = line.substr(4);
        int si = std::stoi(s);
        this->byes.insert(si);
    } else if(line_id == "FOR" || line_id == "XXP") {
        std::string s = line.substr(4);
        this->parseRestriction(s);
    } else if(line_id == "XXZ") {
        this->parseXXZ(line.substr(4));
    } else if(line_id == "XXC") {
        this->parseXXC(line.substr(4));
    }
    
    // record into table
    this->tournament_section[line_id] = line.substr(3);
}

TRFUtil::TRFFile::TRFFile(const std::string &path) {
    this->path = path;
}

TRFUtil::TRFData TRFUtil::TRFFile::read() {
    std::ifstream txt(this->path);
    
    std::string txt_get;
    
    TRFData d;
    
    while (std::getline(txt, txt_get)) {
        d.parseLine(txt_get);
    }
    
    txt.close();
    
    return d;
}

void TRFUtil::TRFFile::write(const std::string &trf) {
    std::ofstream writer(this->path);
    
    writer << trf;
    
    writer.close();
}
