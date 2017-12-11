/*
 * Autor: Jordan Jarolim (xjarol03)
 * Datum: 16.2.2017
 * Soubor: code.cpp
 * Komentar: Soubor obsahuje hlavni fce coding a decoding ke kodovani a dekodovani vstupu + pomocne fce pro prevod mezi vector<bool> a char
 */

#include "code.hpp"
#include "ahed.hpp"
#include "tree.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <bitset>


using namespace std;

/**
 * Handle coding process
 * @param symbol to be encoded
 * @param output stream
 */
void Code::coding(int64_t symbol, std::vector<int64_t> *output){
    tuple<vector<int64_t>, Tree*> foundSymbol;
    
    /* New symbol */
    if (!(Tree::alreadySeen(symbol))){
        foundSymbol = this->getCode(NYT);
        output->insert(output->end(), get<0>(foundSymbol).begin(), get<0>(foundSymbol).end());
        output->push_back(symbol);
        
        
        
        Tree::_listOfUsedChars.push_back(symbol);
        Tree::root->newSymbol(symbol);
    }
    
    /* Already seen */
    else{
        foundSymbol = this->getCode(symbol);
        output->insert(output->end(), get<0>(foundSymbol).begin(), get<0>(foundSymbol).end());
        
        if (get<1>(foundSymbol) != NULL)
            get<1>(foundSymbol)->updateTree();
        else{
            cerr<<"Internal error\n";
            exit (-1);
        }
    }
}

/**
 * Find symbol and return its path + its node
 * @param symbol to be found
 * @return tuple<string, Tree*>
 */
tuple<vector<int64_t>, Tree*> Code::getCode(int64_t symbol){
    vector<int64_t> pathPart;
    
    Tree *found = Tree::root->findSymbol(symbol, Tree::root);
    Tree *move = found;
    
    /* Is that root? */
    if (move != NULL){
        while (move->parent != NULL){
            if (move->parent->left == move)
                pathPart.push_back(MOVE_LEFT);
            else
                pathPart.push_back(MOVE_RIGHT);
            move = move->parent;
        }
    }
    else if (Tree::root->left != NULL)
        pathPart.push_back(1001);
    
    reverse(pathPart.begin(), pathPart.end());
    return make_tuple(pathPart,found);
}

/**
 * Translate char to booles 
 * @param buffer Buffer of booleans
 * @param symbol - symbol to be encoded
 */
void charToBool(vector<bool> *buffer, char symbol){
    std::bitset<8> transformedChar(symbol);
    for (size_t j = 0; j < transformedChar.size(); ++j) {
        buffer->push_back(transformedChar[j]);
    }
}

/**
 * Read value from vector
 * @param buffer - buffer of bools
 * @param position - position of element
 */
bool popFront2(vector<bool> *buffer, int64_t *position){
    return (*buffer)[*position];
}

/**
 * Vector of bools to char
 * @param buffer - buffer of bools
 * @param position of first bool in symbol
 * @param shift - length of symbol (bits)
 */
char boolToChar2(vector<bool> *buffer,int64_t *position, int shift){
    int64_t symbol = 0;
    int counter = 0;
    for(int64_t i = (*position); i < (*position)+shift; i++){
        symbol += ((*buffer)[i] << counter++);
    }
    return symbol;
}

/**
 * Handle decoding process
 * @param inputFile to decode
 * @param outputFile file to be written
 */
void Code::decoding(tAHED *ahed, FILE *inputFile, FILE *outputFile){
    
    
    tuple<vector<int64_t>, Tree*> foundSymbol;
    int64_t symbol;
    Tree* actualNode = Tree::root;
    int64_t outputSize = 0, inputSize = 0;

    
    /* actual position in vector fo bools */
    int64_t vectorPostition = 0;
    
    /* buffer of vector of bools */
    vector<bool> buffer;
    
    /* actual values - symbol on input or direction to be moved to */
    bool actualBool;
    int64_t actualSymbol;
    
    /* Read file as booles */
    while((symbol = fgetc(inputFile)) != EOF){
        inputSize++;
        charToBool(&buffer, symbol);
    }

    bool repeat = true;
    while (repeat){
        /* if is not leaf */
        if (actualNode->left != NULL){
            actualBool = popFront2(&buffer, &vectorPostition);
            vectorPostition++;
            
            /* move */
            if (actualBool == 0)
                actualNode = actualNode->left;
            else if (actualBool == 1)
                actualNode = actualNode->right;

        }else{
            /* read in the next 8/16-bit character */
            if (actualNode->val == NYT || actualNode->val == ROOT){
                
                /* Is this PEOF? */
                actualSymbol = boolToChar2(&buffer, &vectorPostition, 16);
                if ((unsigned short)actualSymbol != Tree::peof){
                    /* Read following symbol */
                    actualSymbol = boolToChar2(&buffer, &vectorPostition, 8);
                    vectorPostition += 8;
                    
                    /* print it to the file */
                    if(outputFile)
                        fprintf(outputFile, "%c", (char)actualSymbol);
                    else
                        cout<<(char)actualSymbol;
                    outputSize++;
                    
                    /* Update Tree */
                    if (!(Tree::alreadySeen(actualSymbol))){
                        Tree::_listOfUsedChars.push_back(actualSymbol);
                        Tree::root->newSymbol(actualSymbol);
                        
                    }else{
                        foundSymbol = this->getCode(actualSymbol);
                        if (get<1>(foundSymbol) != NULL)
                            get<1>(foundSymbol)->updateTree();
                        else{
                            cerr<<"Internal error\n";
                            exit(-1);
                        }
                    }
                    actualNode = Tree::root;
                }
                else{
                    repeat = false;
                }
                
            /* Known symbol from Tree */
            }else{
                if(outputFile)
                    fprintf(outputFile, "%c", (char)actualNode->val);
                else
                    cout<<(char)actualNode->val;
                
                outputSize++;
                actualNode->updateTree();
                actualNode = Tree::root;
            }
        }
    }
    ahed->codedSize = inputSize;
    ahed->uncodedSize = outputSize;
}

