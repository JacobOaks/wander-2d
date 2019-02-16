//
//  SaveManager.cpp
//  wander
//
//  Created by Jacob Oaks on 2/13/19.
//  Copyright © 2019 Jacob Oaks. All rights reserved.
//

#include "SaveManager.h"

//constructor for name, value, and multiple children
Node::Node (std::string name, std::string value, std::vector<Node> children) {
    this->name = name;
    this->value = value;
    this->children = children;
}

//constructor for name, value, and single child
Node::Node (std::string name, std::string value, Node child) {
    std::vector<Node> children;
    this->children.push_back(child);
    this->name = name;
    this->value = value;
    this->children = children;
}

//constructor for just name and value
Node::Node (std::string name, std::string value) {
    this->name = name;
    this->value = value;
    this->children = std::vector<Node>();
}

//returns a specific child by name
Node Node::getCwN(std::string name) {
    for (Node node : this->children)
        if (node.getN() == name)
            return node;
}

//save a master data node to a file
bool sm::saveMasterNode(std::string path, Node* node) {
    std::ofstream write;
    write.open(path); //try to open file
    if (write.fail()) gf::error(S_FILE_NAME, "error opening file '" + path + "' to save to", 0, true, gc::FAILURE_BY_FILEIO);
    
    saveNode(&write, node, ""); //start recursive saving
}

//loads a master data node from a file
Node sm::loadMasterNode(std::string path) {
    
    //open file
    std::ifstream read;
    read.open(path); //try to open file
    if (read.fail()) gf::error(S_FILE_NAME, "error opening file '" + path + "' to lad from", 1, true, gc::FAILURE_BY_FILEIO);
    
    //read entire file into vector of strings
    std::vector<std::string> file;
    std::string nextLine;
    while (!read.eof()) {
        getline(read, nextLine);
        file.push_back(nextLine);
    }
    
    //recursively loop through vector of strings to build the node
    int i = 0;
    return loadNode(&file, &i, 0);
}

//recursively saves a node and all of its children using itself.
void sm::saveNode(std::ofstream* write, Node* node, std::string indent) {
    *write << indent << node->getN() << ": "; //indent and write node name
    if (node->getV() != "") //write node value if there is one
        *write << node->getV();
    *write << std::endl;
    if (node->getCSize() > 0) { //if node has children
        *write << indent << "{" << std::endl;
        for (Node c : node->getC()) //save all children
            saveNode(write, &c, indent + " ");
        *write << indent <<  "}" << std::endl;
    }
}

//recursively loads a node and all of its children using itself. returns node when finished
Node sm::loadNode(std::vector<std::string>* file, int* i, int indent) {
    
    //format next line and find dividing point
    std::string nextLine = file->at(*i); //nextLine
    nextLine = nextLine.substr(indent, nextLine.length() - indent); //remove indent
    int dividerLoc = -1; //location of divider in line
    for (int j = 0; j < nextLine.length() && dividerLoc == -1; j++) //find location of divider in line
        if (nextLine[j] == ':')
            dividerLoc = j;
    
    //throw error if there is no colon in line
    if (dividerLoc == -1) gf::error(S_FILE_NAME, "error interpreting line: '" + nextLine + "' - missing a colon divider ':'", 2, gc::FAILURE_BY_FILEIO, true);
    
    //create node represented by next line
    Node node = Node(nextLine.substr(0, dividerLoc)); //create node with name
    std::string posVal = nextLine.substr(dividerLoc + 1, nextLine.length() - dividerLoc - 1); //load value
    if (posVal != " " && posVal != "") //if there is a value
        node.setV(posVal.substr(1, posVal.length() - 1)); //assign it to the new node (remove space)
    
    //check for more file
    if (*i + 1 <= file->size()) { //if not eof
        
        //check for child nodes
        if (file->at(*i + 1).find("{") != std::string::npos) { //if the node has children
            *i += 2; //iterate twice
            indent++; //iterate indent
            while (file->at(*i).find("}") == std::string::npos) { //if the children are over
                node.addC(loadNode(file, i, indent));
                
                //throw error if file suddenly stop
                if ((*i + 1) > file->size()) gf::error(S_FILE_NAME, "error reading file: sudden file stop after line '" + nextLine + "'", 3, gc::FAILURE_BY_FILEIO, true);
                
                *i += 1; //iterate
            }
        }
    }
    return node; //return newly-loaded node
}
