#include "search.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>


BSTNode* bst_insert(BSTNode *root, Place *p) {
    if (!p) return root;
    if (!root) {
        BSTNode *n = malloc(sizeof(BSTNode));
        n->place = p; n->left = n->right = NULL;
        return n;
    }
    int cmp = strcasecmp(p->name, root->place->name);
    if (cmp < 0) root->left = bst_insert(root->left, p);
    else if (cmp > 0) root->right = bst_insert(root->right, p);
    // if equal: do nothing (no duplicates)
    return root;
}

Place* bst_search(BSTNode *root, const char *name) {
    if (!root) return NULL;
    int cmp = strcasecmp(name, root->place->name);
    if (cmp == 0) return root->place;
    if (cmp < 0) return bst_search(root->left, name);
    return bst_search(root->right, name);
}

void bst_free(BSTNode *root) {
    if (!root) return;
    bst_free(root->left);
    bst_free(root->right);
    free(root);
}

BSTNode* build_bst_from_list(Place *head) {
    BSTNode *root = NULL;
    while (head) {
        root = bst_insert(root, head);
        head = head->next;
    }
    return root;
}
