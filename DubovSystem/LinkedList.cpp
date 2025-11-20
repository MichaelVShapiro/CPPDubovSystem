//
//  LinkedList.cpp
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

#include "LinkedList.hpp"

CPPDubovSystem::LinkedListNode::LinkedListNode(std::vector<Player> data): data(data), next(nullptr) {}

CPPDubovSystem::LinkedListNode::LinkedListNode(const LinkedListNode &other): data(other.data) {
    // if next is null, we are basically done
    if(other.next == nullptr)
        this->next = nullptr;
    else
        this->next = new LinkedListNode(*(other.next)); // recursively call the constructor
}

CPPDubovSystem::LinkedList::LinkedList(): head(nullptr) {
    
}

CPPDubovSystem::LinkedList::LinkedList(const LinkedList &linked_list) {
    *this = linked_list;
}

CPPDubovSystem::LinkedList::~LinkedList() {
    this->clear();
}

void CPPDubovSystem::LinkedList::insertNode(const std::vector<Player> &data) {
    LinkedListNode* d = new LinkedListNode(data);
    // this really is just some standard insert operations
    if(this->head == nullptr) {
        this->head = d;
        return;
    }
    LinkedListNode* c = this->head;
    while(c->next != nullptr) {
        c = c->next;
    }
    c->next = d;
}

void CPPDubovSystem::LinkedList::clear() {
    while(this->head != nullptr) {
        LinkedListNode *temp = this->head;
        this->head = this->head->next;
        delete temp;
    }
}

void CPPDubovSystem::LinkedList::operator=(const LinkedList &linked_list) {
    if(linked_list.head == nullptr) return;
    
    LinkedListNode *temp = linked_list.head;
    while(temp != nullptr) {
        this->insertNode(temp->data);
        temp = temp->next;
    }
}
