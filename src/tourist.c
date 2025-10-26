#include "tourist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUF 512

// CSV format assumed: id,name,type,rating,distance
Place* create_place(int id, const char *name, const char *type, double rating, double distance) {
    Place *p = malloc(sizeof(Place));
    p->id = id;
    strncpy(p->name, name, sizeof(p->name)-1); p->name[sizeof(p->name)-1]=0;
    strncpy(p->type, type, sizeof(p->type)-1); p->type[sizeof(p->type)-1]=0;
    p->rating = rating;
    p->distance = distance;
    p->next = NULL;
    return p;
}

Place* load_places_from_csv(const char *csvfile) {
    FILE *f = fopen(csvfile, "r");
    if (!f) return NULL;
    Place *head = NULL, *tail = NULL;
    char line[LINE_BUF];
    // skip header if present by checking first line contains non-digit start
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '\n' || line[0]=='\r') continue;
        // Trim newline
        size_t ln = strlen(line);
        if (ln && (line[ln-1]=='\n' || line[ln-1]=='\r')) line[ln-1]=0;
        int id;
        char name[128], type[64];
        double rating, dist;
        // handle quoted name containing commas
        char *p = line;
        // parse id
        char *tok = strtok(p, ",");
        if (!tok) continue;
        id = atoi(tok);
        // name
        tok = strtok(NULL, ",");
        if (!tok) continue;
        strncpy(name, tok, sizeof(name)-1); name[sizeof(name)-1]=0;
        // type
        tok = strtok(NULL, ",");
        if (!tok) continue;
        strncpy(type, tok, sizeof(type)-1); type[sizeof(type)-1]=0;
        // rating
        tok = strtok(NULL, ",");
        if (!tok) continue;
        rating = atof(tok);
        // distance
        tok = strtok(NULL, ",");
        if (!tok) continue;
        dist = atof(tok);
        Place *pl = create_place(id, name, type, rating, dist);
        if (!head) head = tail = pl;
        else { tail->next = pl; tail = pl; }
    }
    fclose(f);
    return head;
}

void free_places(Place *head) {
    while (head) {
        Place *t = head; head = head->next; free(t);
    }
}

void print_places(Place *head) {
    printf("%-4s %-35s %-12s %-6s %-8s\n", "ID", "Name", "Type", "Rating", "Dist(km)");
    printf("-------------------------------------------------------------------------------\n");
    while (head) {
        printf("%-4d %-35s %-12s %-6.1f %-8.2f\n",
            head->id, head->name, head->type, head->rating, head->distance);
        head = head->next;
    }
}

// helper: convert linked list to array
static Place** list_to_array(Place *head, int *out_n) {
    int n=0; Place *cur=head;
    while (cur) { n++; cur=cur->next; }
    if (out_n) *out_n = n;
    if (n==0) return NULL;
    Place **arr = malloc(sizeof(Place*)*n);
    cur = head; for (int i=0;i<n;i++) { arr[i]=cur; cur=cur->next; }
    return arr;
}

static Place* rebuild_list_from_array(Place **arr, int n) {
    if (n==0) return NULL;
    for (int i=0;i<n-1;i++) arr[i]->next = arr[i+1];
    arr[n-1]->next = NULL;
    Place *head = arr[0];
    free(arr);
    return head;
}

Place* sort_places_by_distance(Place *head) {
    int n; Place **arr = list_to_array(head, &n);
    if (!arr) return NULL;
    // simple insertion sort (n small)
    for (int i=1;i<n;i++) {
        Place *key = arr[i];
        int j = i-1;
        while (j>=0 && arr[j]->distance > key->distance) { arr[j+1]=arr[j]; j--; }
        arr[j+1]=key;
    }
    return rebuild_list_from_array(arr, n);
}

Place* sort_places_by_rating(Place *head) {
    int n; Place **arr = list_to_array(head, &n);
    if (!arr) return NULL;
    for (int i=1;i<n;i++) {
        Place *key = arr[i];
        int j = i-1;
        while (j>=0 && arr[j]->rating < key->rating) { arr[j+1]=arr[j]; j--; } // desc
        arr[j+1]=key;
    }
    return rebuild_list_from_array(arr, n);
}

Place* filter_places_by_type(Place *head, const char *type) {
    Place *res_head = NULL, *res_tail = NULL;
    Place *cur = head;
    while (cur) {
        if (strstr(cur->type, type) != NULL) {
            Place *copy = create_place(cur->id, cur->name, cur->type, cur->rating, cur->distance);
            if (!res_head) res_head = res_tail = copy;
            else { res_tail->next = copy; res_tail = copy; }
        }
        cur = cur->next;
    }
    return res_head;
}
