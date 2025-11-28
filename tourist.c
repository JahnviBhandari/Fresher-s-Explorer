#include "tourist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LINE_BUF 512

Place* create_place(int id, const char *name, const char *type, double rating, double distance) {
    Place *p = malloc(sizeof(Place));
    p->id = id;
    strncpy(p->name, name, sizeof(p->name)-1); 
    p->name[sizeof(p->name)-1] = 0;
    strncpy(p->type, type, sizeof(p->type)-1); 
    p->type[sizeof(p->type)-1] = 0;
    p->rating = rating;
    p->distance = distance;
    p->next = NULL;
    return p;
}

Place* load_places_from_csv(const char *csvfile) {
    FILE *f = fopen(csvfile, "r");
    if (!f) {
        printf("Error: Could not open %s\n", csvfile);
        return NULL;
    }
    Place *head = NULL, *tail = NULL;
    char line[LINE_BUF];
    int id_counter = 0;
    if (fgets(line, sizeof(line), f)) {
        printf("Skipping header: %s", line);
    }
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '\n' || line[0] == '\r') continue;
        line[strcspn(line, "\r\n")] = '\0';
        char name[128], type[64];
        double rating, dist;
        char *tok = strtok(line, ",");
        if (!tok) continue;
        strncpy(name, tok, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
        tok = strtok(NULL, ",");
        if (!tok) continue;
        strncpy(type, tok, sizeof(type) - 1);
        type[sizeof(type) - 1] = '\0';
        tok = strtok(NULL, ",");
        if (!tok) continue;
        rating = atof(tok);
        tok = strtok(NULL, ",");
        if (!tok) continue;
        dist = atof(tok);
        Place *pl = create_place(id_counter++, name, type, rating, dist);
        if (!head) {
            head = tail = pl;
        } else {
            tail->next = pl;
            tail = pl;
        }
    }
    fclose(f);
    printf("Loaded %d places from CSV\n", id_counter);
    return head;
}

void free_places(Place *head) {
    while (head) {
        Place *temp = head;
        head = head->next;
        free(temp);
    }
}

void print_places(Place *head) {
    if (!head) {
        printf("No places to display.\n");
        return;
    }
    printf("\n%-4s %-35s %-12s %-6s %-8s\n", "ID", "Name", "Type", "Rating", "Dist(km)");
    printf("\n");
    while (head) {
        printf("%-4d %-35s %-12s %-6.1f %-8.2f\n", 
               head->id, head->name, head->type, head->rating, head->distance);
        head = head->next;
    }
}

Place* sort_places_by_distance(Place *head) {
    if (!head || !head->next) return head;
    
    int swapped;
    Place *ptr1;
    Place *lptr = NULL;
    
    do {
        swapped = 0;
        ptr1 = head;
        
        while (ptr1->next != lptr) {
            if (ptr1->distance > ptr1->next->distance) {
                // Swap data
                int temp_id = ptr1->id;
                ptr1->id = ptr1->next->id;
                ptr1->next->id = temp_id;
                
                char temp_name[128], temp_type[64];
                strcpy(temp_name, ptr1->name);
                strcpy(temp_type, ptr1->type);
                
                strcpy(ptr1->name, ptr1->next->name);
                strcpy(ptr1->type, ptr1->next->type);
                strcpy(ptr1->next->name, temp_name);
                strcpy(ptr1->next->type, temp_type);
                
                double temp_rating = ptr1->rating;
                double temp_dist = ptr1->distance;
                
                ptr1->rating = ptr1->next->rating;
                ptr1->distance = ptr1->next->distance;
                ptr1->next->rating = temp_rating;
                ptr1->next->distance = temp_dist;
                
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
    
    return head;
}

Place* sort_places_by_rating(Place *head) {
    if (!head || !head->next) return head;
    
    int swapped;
    Place *ptr1;
    Place *lptr = NULL;
    
    do {
        swapped = 0;
        ptr1 = head;
        
        while (ptr1->next != lptr) {
            if (ptr1->rating < ptr1->next->rating) {
                // Swap data
                int temp_id = ptr1->id;
                ptr1->id = ptr1->next->id;
                ptr1->next->id = temp_id;
                
                char temp_name[128], temp_type[64];
                strcpy(temp_name, ptr1->name);
                strcpy(temp_type, ptr1->type);
                
                strcpy(ptr1->name, ptr1->next->name);
                strcpy(ptr1->type, ptr1->next->type);
                strcpy(ptr1->next->name, temp_name);
                strcpy(ptr1->next->type, temp_type);
                
                double temp_rating = ptr1->rating;
                double temp_dist = ptr1->distance;
                
                ptr1->rating = ptr1->next->rating;
                ptr1->distance = ptr1->next->distance;
                ptr1->next->rating = temp_rating;
                ptr1->next->distance = temp_dist;
                
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
    
    return head;
}

Place* filter_places_by_type(Place *head, const char *type_filter) {
    Place *result = NULL, *tail = NULL;
    int new_id = 0;
    
    while (head) {
        if (strstr(head->type, type_filter) != NULL) {
            Place *new_place = create_place(new_id++, head->name, head->type, 
                                           head->rating, head->distance);
            if (!result) {
                result = tail = new_place;
            } else {
                tail->next = new_place;
                tail = new_place;
            }
        }
        head = head->next;
    }  
    return result;
}