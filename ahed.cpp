/*
 * Autor: Jordan Jarolim (xjarol03)
 * Datum: 16.2.2017
 * Soubor: ahed.c
 * Komentar: Implementace knihovny pro adaptivni huffmanovo kodovani
 */

#include <iostream>
#include <stdio.h>
#include <bitset>

#include "ahed.hpp"
#include "tree.hpp"
#include "code.hpp"


using namespace std;


/* Nazev:
 *   AHEDEncoding
 * Cinnost:
 *   Funkce koduje vstupni soubor do vystupniho souboru a porizuje zaznam o kodovani.
 * Parametry:
 *   ahed - zaznam o kodovani
 *   inputFile - vstupni soubor (nekodovany)
 *   outputFile - vystupní soubor (kodovany)
 * Navratova hodnota:
 *    0 - kodovani probehlo v poradku
 *    -1 - pri kodovani nastala chyba
 */
int AHEDEncoding(tAHED *ahed, FILE *inputFile, FILE *outputFile)
{
    int64_t symbol;
    Code *code = new Code;
    Tree::root = Tree::nyt = new Tree(ROOT, INIT_WEIGHT, INIT_ORDER, NULL, NULL, NULL);
    vector<int64_t> output;
    tuple<vector<int64_t>, Tree*> foundSymbol;
    int64_t outputSize = 0, inputSize = 0;
    
    while ((symbol = fgetc(inputFile)) != EOF)
    {
        inputSize++;
        code->coding(symbol, &output);
    }
    
    /* save final codes */
    foundSymbol = code->getCode(NYT);
    output.insert(output.end(), get<0>(foundSymbol).begin(), get<0>(foundSymbol).end());
    output.push_back(Tree::peof);

    
    /* get vector of booleans */
    vector<bool> finalOutput;
    for (auto i: output){
        if (i == MOVE_LEFT){
            finalOutput.push_back(0);
        }
        else if (i == MOVE_RIGHT){
            finalOutput.push_back(1);

        }
        else if (i == Tree::peof){
            bitset<16> transformedChar(i);
            for (size_t j = 0; j < transformedChar.size(); ++j) {
                finalOutput.push_back(transformedChar[j]);
            }
        }
        else{
            bitset<8> transformedChar(i);
            for (size_t j = 0; j < transformedChar.size(); ++j) {
                finalOutput.push_back(transformedChar[j]);
            }
        }
    }
    
    
    vector<bool> oneChar;
    char c = 0;

    /* push all complete bytes */
    for (int j = 0; j < finalOutput.size() - finalOutput.size()%8; j += 8){
        for (int k = 0; k < 8; k++){
            oneChar.push_back(finalOutput[j+k]);
        }
        
        /* write each complete byte into a file */
        c = 0;
        for(int i = 0; i < 8; i++)
            c += (oneChar[i] << i);
        
        if(outputFile)
            fprintf(outputFile, "%c", c);
        else
            cout<<c;
        outputSize++;
        oneChar.clear();
    }

    /* push incomplete byte */
    for (unsigned long m = (finalOutput.size() - finalOutput.size()%8); m < finalOutput.size(); m++){
        oneChar.push_back(finalOutput[m]);
    }
     /* fill incomplete byte with 0s */
    for (unsigned long m = 0; m < 8 - finalOutput.size()%8; m++){
        oneChar.push_back(0);
    }
    
    /* write into a file */
    c = 0;
    for(int i = 0; i < 8; i++)
        c += (oneChar[i] << i);
    
    if(outputFile)
        fprintf(outputFile, "%c", c);
    else
        cout<<c;
    outputSize++;
    oneChar.clear();
    
    ahed->uncodedSize = inputSize;
    ahed->codedSize = outputSize;
    
    delete(code);
    delete(Tree::root);
    
    return AHEDOK;
}

/* Nazev:
 *   AHEDDecoding
 * Cinnost:
 *   Funkce dekoduje vstupni soubor do vystupniho souboru a porizuje zaznam o dekodovani.
 * Parametry:
 *   ahed - zaznam o dekodovani
 *   inputFile - vstupni soubor (kodovany)
 *   outputFile - vystupní soubor (nekodovany)
 * Navratova hodnota:
 *    0 - dekodovani probehlo v poradku
 *    -1 - pri dekodovani nastala chyba
 */
int AHEDDecoding(tAHED *ahed, FILE *inputFile, FILE *outputFile)
{
    Code *code = new Code;
    Tree::root = Tree::nyt = new Tree(ROOT, INIT_WEIGHT, INIT_ORDER, NULL, NULL, NULL);
   
    code->decoding(ahed, inputFile, outputFile);
    delete(code);
    delete(Tree::root);
    
    return AHEDOK;
}



