#ifndef SEARCH_H
#define SEARCH_H

#include "tourist.h"

typedef struct BSTNode {
    Place *place; // pointer to place in the main list
    struct BSTNode *left, *right;
} BSTNode;

BSTNode* bst_insert(BSTNode *root, Place *p);
Place* bst_search(BSTNode *root, const char *name);
void bst_free(BSTNode *root);
BSTNode* build_bst_from_list(Place *head);

#endif 
