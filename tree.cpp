/*
 * Autor: Jordan Jarolim (xjarol03)
 * Datum: 16.2.2017
 * Soubor: tree.cpp
 * Komentar: Trida zajistujici veskere operace se stromem a to jak pri kodovani tak i pri dekodovani. Predevsim vsak vkladani do stromu, rekonfiguraci stromu, update vah uzlu a hledani prvku ve strome. Dale take vypis stromu pro pomocne ucely.
 */

#include "tree.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>


using namespace std;

std::vector<int64_t> Tree::_listOfUsedChars;
Tree* Tree::nyt = NULL;
Tree* Tree::root = NULL;

/* Terminal symbol of 16 1's */
unsigned short Tree::peof = 65535;


/* Constructors */
Tree::Tree(int64_t value, int64_t wei, int64_t num, Tree* l, Tree* r, Tree* p)
    :val(value), weight(wei), order(num), left(l), right(r), parent(p)
    {}

Tree::Tree()
    :val(0),weight(0),order(0), left(0), right(0), parent(0)
    {}


/* Methods */

/**
 * Check if incoming symbol was already seen
 * @param symbol to check
 * @return bool
 */
bool Tree::alreadySeen(int64_t symbol){
    if(std::find(Tree::_listOfUsedChars.begin(), Tree::_listOfUsedChars.end(), symbol) != Tree::_listOfUsedChars.end())
        return true;
    else
        return false;
        
}

/**
 * Create new NYT node and new symbol node
 * @param symbol to be added
 */
void Tree::newSymbol(int64_t symbol){
    Tree* originalNyt = Tree::nyt;
    
    Tree::nyt = new Tree(NYT, INIT_WEIGHT, originalNyt->order - 2, NULL, NULL, originalNyt);
    originalNyt->val = CROSS;
    originalNyt->left = Tree::nyt;
    originalNyt->right = new Tree(symbol, INIT_WEIGHT, originalNyt->order -1, NULL, NULL, originalNyt);
    originalNyt->right->updateTree();

}

/**
 * Update symbol weight
 * @param symbol to be updated
 */
void Tree::updateSymbol(int64_t symbol){
    Tree* found = NULL;
    if ((found = this->findSymbol(symbol, this)) != NULL){
        found->updateTree();
    }
    else{
        cerr<<"Internal error\n";
        exit(-1);
    }
}

/**
 * Find symbol to update its weight
 * @param symbol to be updated
 * @param parent recursive parent
 * @return Tree* or NULL
 */
Tree* Tree::findSymbol(int64_t symbol, Tree* parent){
    Tree* found = NULL;
    
    if (parent != NULL){
        if (parent->val != symbol){
            if ((found = this->findSymbol(symbol, parent->left)) != NULL)
                return found;
            else if((found = this->findSymbol(symbol, parent->right)) != NULL)
                return found;
            else return NULL;
        }
        else return parent;
    }
    else return NULL;
}

/**
 * Find nex sibling to swap
 * @param order to be found
 * @param parent recursive parent
 * @return Tree* or NULL
 */
Tree* Tree::findFarSibling(int64_t order, Tree* parent){
    Tree* found = NULL;
    
    if (parent != NULL){
        if (parent->order != order){
            if ((found = this->findFarSibling(order, parent->left)) != NULL)
                return found;
            else if((found = this->findFarSibling(order, parent->right)) != NULL)
                return found;
            else return NULL;
        }
        else return parent;
    }
    else return NULL;
}

/**
 * Reorder tree nodes to meet requirements for Huffman tree
 */
void Tree::updateTree(){
    Tree* current = this;
    Tree* toSwap = NULL;
    Tree* currentParent = NULL;
    Tree* swapParent = NULL;
    Tree* tempNode = NULL;
    int64_t tempOrder = 0;
    //current->weight = current->left->weight + current->right->weight;
    
    /* while not the root */
    while (current->parent != NULL){
      
        bool repeat = true;
        while (repeat){
            repeat = false;
            
            /*cout<<"find node to swap:"<<current->order<<" : "<<current->order+1<<"\n\n";
            Tree::printTree(Tree::root);
            cout<<"\n\n----------------------------------\n";*/
            
            toSwap = findFarSibling(current->order+1, Tree::root);
            if (toSwap == current->parent){
                toSwap = findFarSibling(current->order+2, Tree::root);
            }
            
            /* Swap nodes */
            if (toSwap != NULL && current->weight == toSwap->weight){
                repeat = true;
                /*
                cout<<"should swap:"<<current->order<<" : "<<toSwap->order<<" and parent is:"<<current->parent->order<<"\n\n";
                Tree::printTree(Tree::root);
                cout<<"\n\n----------------------------------\n";*/

                /* Swap in one subtree */
                if (toSwap == current->parent->right || toSwap == current->parent->left){
                    //cout<<"Swapping\n";
                    tempNode = current;
                    current->parent->left = current->parent->right;
                    current->parent->right = tempNode;
                    
                    tempOrder = current->order;
                    current->order = current->parent->left->order;
                    current->parent->left->order = tempOrder;
                    
                    /*cout<<"swapped:"<<current->order<<" : "<<toSwap->order<<" and parent is:"<<current->parent->order<<"\n\n";
                    Tree::printTree(Tree::root);
                    cout<<"\n\n----------------------------------\n";*/
                }
                /* Swap with element from other subtree */
                else{
                    if (toSwap->parent != NULL){
                        
                        swapParent = toSwap->parent;
                        currentParent = current->parent;
                        
                        
                        if (current == currentParent->left){
                            currentParent->left = toSwap;
                        }
                        else{
                            currentParent->right = toSwap;
                        }
                        
                        if (toSwap == swapParent->left){
                            swapParent->left = current;
                        }
                        else{
                            swapParent->right = current;
                        }
                        
                        toSwap->parent = currentParent;
                        current->parent = swapParent;
                        
                        tempOrder = current->order;
                        current->order = toSwap->order;
                        toSwap->order = tempOrder;
                        
                        /*cout<<"swapped:"<<current->order<<" : "<<toSwap->order<<" and parent is:"<<current->parent->order<<"\n\n";
                        Tree::printTree(Tree::root);
                        cout<<"\n\n----------------------------------\n";*/
                    }
                }
            }else{
                current->weight++;
                //cout << "raised weight of "<<current->order<<"\n";
                current=current->parent;
            }
        }
    }
    /* update root */
    current->weight = current->left->weight + current->right->weight;
    /*cout<<"Weight of root updated\n";
    Tree::printTree(Tree::root);
    cout<<"\n\n------------------------------------------------------------\n\n\n";*/
}

void Tree::getWeightClass(std::vector<Tree*> *weightClass, Tree* parent){
    if (parent != NULL){
        if ((parent->weight == this->weight) && (parent->order != this->order) && (parent != Tree::root) && (parent != this->parent)){
            weightClass->push_back(parent);
        }
        this->getWeightClass(weightClass, parent->left);
        this->getWeightClass(weightClass, parent->right);
    }
}

/**
 * Print Tree recursively
 * @param p - tree pointer
 * @param indent - indentation
 */
void Tree::printTree(Tree* p, int indent)
{
    int64_t parent;
    if(p != NULL) {
        if(p->right != NULL) {
            printTree(p->right, indent+4);
        }
        if (indent) {
            std::cout << std::setw(indent) << ' ';
        }
        if (p->right != NULL) std::cout<<" /\n" << std::setw(indent) << ' ';{
            if (p->parent != NULL)
                parent = p->parent->order;
            else
                parent = 0;
            cout<< "order: " << p->order << " | parent: "<<parent<< " | weight: "<<p->weight<<" symbol: "<<(char)p->val<<"\n";
        }
        if(p->left != NULL) {
            std::cout << std::setw(indent) << ' ' <<" \\\n";
            printTree(p->left, indent+4);
        }
    }
}











