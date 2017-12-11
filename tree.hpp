/*
 * Autor: Jordan Jarolim (xjarol03)
 * Datum: 16.2.2017
 * Soubor: tree.hpp
 * Komentar:
 */

#ifndef tree_hpp
#define tree_hpp

#include <stdio.h>
#include <vector>
#include <stdint.h>

/* Value in ROOT node */
#define ROOT -1
/* Value in NYT node */
#define NYT -2
/* Value on intersection */
#define CROSS 0

/* Init values */
#define INIT_WEIGHT 0
#define INIT_SYMBOL_WEIGHT 1
#define INIT_ORDER 512

/* directions */
#define MOVE_LEFT 1000
#define MOVE_RIGHT 1001

class Tree{
public:

    int64_t val;      //character in tree node
    int64_t weight;   //number of times val has occured in file
    int64_t order;    //ordering system to track weights
    
    Tree* left;
    Tree* right;
    Tree* parent;

    static std::vector<int64_t> _listOfUsedChars;
    static bool alreadySeen(int64_t symbol);
    static void printTree(Tree* p, int indent=0);
    static Tree* nyt;
    static Tree* root;
    static unsigned short peof;

    /* Constructors */
    Tree(int64_t value, int64_t wei, int64_t num, Tree* l, Tree* r, Tree* p);
    Tree();
    
    /* Other methods */
    void newSymbol(int64_t symbol);
    void updateSymbol(int64_t symbol);
    Tree* findSymbol(int64_t symbol, Tree* parent);
    Tree* findFarSibling(int64_t order, Tree* parent);
    void updateTree();
    void getWeightClass(std::vector<Tree*> *weightClass, Tree* parent);


};

#endif /* tree_hpp */
