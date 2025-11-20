//
//  LinkedList.hpp
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

#ifndef LinkedList_hpp
#define LinkedList_hpp

#include <stdio.h>
#include <vector>
#include "Player.hpp"

// this really is just a simple linked list implementation to store players

namespace CPPDubovSystem {
/**
 * A single linked list node
 */
struct LinkedListNode {
    /**
     * The data it holds, which is a vector of players
     */
    std::vector<Player> data;
    /**
     * The next node it points to
     */
    LinkedListNode *next;
    /**
     * Constructor for LinkedListNode
     */
    LinkedListNode(std::vector<Player> data);
    /**
     * Deep copy constructor
     */
    LinkedListNode(const LinkedListNode &other);
};

/**
 * The actual linked list
 */
class LinkedList {
private:
    /**
     * The head (root) of the data
     */
    LinkedListNode* head;
public:
    /**
     * Default constructor
     */
    LinkedList();
    /**
     * Just a deep copy constructor
     */
    LinkedList(const LinkedList &linked_list);
    /**
     * A destructor
     */
    ~LinkedList();
    /**
     * Inserts a node into the list
     */
    void insertNode(const std::vector<Player> &group);
    /**
     * Gets the head of the list
     */
    LinkedListNode* getHead() {return head;}
    /**
     * Completely clears the linkedlist
     */
    void clear();
    /**
     * Just for overloading the '=' operator
     */
    void operator =(const LinkedList &linked_list);
};
}

#endif /* LinkedList_hpp */
