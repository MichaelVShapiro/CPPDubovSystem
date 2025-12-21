//
//  explain.hpp
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

// this is what is used to "explain" how the pairings work

#ifndef EXPLAIN_HPP
#define EXPLAIN_HPP

#include <string>
#include <fstream>
#include <vector>

namespace CPPDubovSystem {
/**
 * The core logger that holds the explain data
 */
class ExplainLogger {
private:
    
    /**
     * The file to output everything to explain
     */
    std::string output_file = "";
    /**
     * Different steps (or logs) to input
     */
    std::vector<std::string> steps;
public:
    /**
     * Default constructor
     */
    ExplainLogger() = default;
    /**
     * Sets the file to output the explanations to
     */
    void setOutputLogFile(const std::string &file);
    /**
     * Logs an explanation
     */
    void log(const std::string &explain);
    /**
     * Clears everything in the logger, including any written steps and output file
     */
    void clear();
    /**
     * Gets how the explanation output appears
     */
    std::string getOutputExplanation() const;
    /**
     * Outputs the explanation to a file
     */
    void outputToFile() const;
};
}

#endif // !EXPLAIN_HPP
