/*
 * Autor: Jordan Jarolim (xjarol03)
 * Datum: 16.2.2017
 * Soubor: code.hpp
 * Komentar:
 */

#ifndef code_hpp
#define code_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <stdint.h>
#include <tuple>


#include "ahed.hpp"
#include "tree.hpp"

class Code{
    public:
        void coding(int64_t symbol, std::vector<int64_t> *output);
        void decoding(tAHED *ahed, FILE *inputFile, FILE *outputFile);
        std::tuple<std::vector<int64_t>, Tree*> getCode(int64_t symbol);

    
    private:
        void getPath(std::string pathPart, int64_t symbol, Tree* parent);
    
};

#endif /* code_hpp */
