//
//  explain.cpp
//  DubovSystem
//
//  Created by Michael Shapiro on 11/22/25.
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

#include "explain.hpp"

void CPPDubovSystem::ExplainLogger::setOutputLogFile(const std::string &file) {
    this->output_file = file;
}

void CPPDubovSystem::ExplainLogger::log(const std::string &explain) {
    this->steps.push_back(explain);
}

void CPPDubovSystem::ExplainLogger::clear() {
    this->steps.clear();
    this->output_file.clear();
}

std::string CPPDubovSystem::ExplainLogger::getOutputExplanation() const {
    // format all the steps for output separation
    // format will be in markdown
    std::string output = "# Explanation of Pairings";
    
    for(int i = 0; i < this->steps.size(); i++) {
        // note down step number
        output += "\n\n## Step " + std::to_string(i + 1);
        
        // now output actual explanation text
        output += "\n\n" + this->steps[i];
    }
    
    return output;
}

void CPPDubovSystem::ExplainLogger::outputToFile() const {
    // if no file is set, we don't have to do anything
    if(this->output_file.size() == 0) return;
    
    // if there are no steps, write nothing
    if(this->steps.size() == 0) return;
    
    std::ofstream outputFile(this->output_file);
    
    // get data to write
    std::string output = this->getOutputExplanation();
    
    // output text
    outputFile << output;
    
    // close
    outputFile.close();
}
