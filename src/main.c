#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "authentication.h"
#include "tourist.h"
#include "search.h"
#include "graph.h"

#define PLACES_CSV "dehradun_data.csv"
#define GRAPH_FILE "graph.txt"

void tourist_menu(Place **places_ptr, BSTNode **bst_ptr) {
    Place *places = *places_ptr;
    BSTNode *bst = *bst_ptr;
    int choice=0;
    while (1) {
        printf("\n--- Tourist Module ---\n");
        printf("1. List all places\n");
        printf("2. Sort by distance\n");
        printf("3. Sort by rating\n");
        printf("4. Filter by type\n");
        printf("5. Search by name\n");
        printf("0. Back\n");
        printf("Choice: "); if (scanf("%d%*c", &choice)!=1) { choice=0; }
        if (choice==0) break;
        if (choice==1) print_places(places);
        else if (choice==2) {
            places = sort_places_by_distance(places);
            *places_ptr = places;
            printf("Sorted by distance.\n");
            print_places(places);
            bst_free(bst); bst = build_bst_from_list(places); *bst_ptr = bst;
        } else if (choice==3) {
            places = sort_places_by_rating(places);
            *places_ptr = places;
            printf("Sorted by rating.\n");
            print_places(places);
            bst_free(bst); bst = build_bst_from_list(places); *bst_ptr = bst;
        } else if (choice==4) {
            char typeq[64];
            printf("Enter type substring (e.g., cafe, tourist, hotel): ");
            fgets(typeq, sizeof(typeq), stdin); if (typeq[strlen(typeq)-1]=='\n') typeq[strlen(typeq)-1]=0;
            Place *filtered = filter_places_by_type(places, typeq);
            if (!filtered) printf("No places found of that type.\n");
            else print_places(filtered);
            free_places(filtered);
        } else if (choice==5) {
            char nameq[128];
            printf("Enter exact name to search: ");
            fgets(nameq, sizeof(nameq), stdin); if (nameq[strlen(nameq)-1]=='\n') nameq[strlen(nameq)-1]=0;
            Place *res = bst_search(bst, nameq);
            if (res) {
                printf("Found:\n");
                printf("%-4s %-35s %-12s %-6s %-8s\n", "ID", "Name", "Type", "Rating", "Dist(km)");
                printf("%-4d %-35s %-12s %-6.1f %-8.2f\n", res->id, res->name, res->type, res->rating, res->distance);
            } else printf("Not found.\n");
        } else printf("Invalid choice.\n");
    }
}

void graph_menu(Graph *g) {
    int choice=0;
    while (1) {
        printf("\n--- Graph Module ---\n");
        printf("1. Print graph\n");
        printf("2. Shortest path (by vertex index)\n");
        printf("0. Back\n");
        printf("Choice: "); if (scanf("%d%*c", &choice)!=1) { choice=0; }
        if (choice==0) break;
        if (choice==1) graph_print(g);
        else if (choice==2) {
            char sname[128], dname[128];
            printf("Enter source vertex name: ");
            fgets(sname, sizeof(sname), stdin); if (sname[strlen(sname)-1]=='\n') sname[strlen(sname)-1]=0;
            printf("Enter destination vertex name: ");
            fgets(dname, sizeof(dname), stdin); if (dname[strlen(dname)-1]=='\n') dname[strlen(dname)-1]=0;
            int s = -1, d = -1;
            for (int i=0;i<g->vcount;i++) {
                if (strcmp(g->verts[i].name, sname)==0) s=i;
                if (strcmp(g->verts[i].name, dname)==0) d=i;
            }
            if (s==-1 || d==-1) printf("Could not find vertex names.\n");
            else graph_dijkstra(g, s, d);
        } else printf("Invalid choice.\n");
    }
}

int main() {
    printf("Fresher's Explorer - Console App\n");

    // Load places
    Place *places = load_places_from_csv(dehradun_data.csv);
    if (!places) {
        printf("No places loaded from %s. Create that file or run python api_fetch.py to generate it.\n", PLACES_CSV);
    } else {
        printf("Loaded places from %s\n", PLACES_CSV);
    }
    // Build BST
    BSTNode *bst = build_bst_from_list(places);

    // Load Graph
    Graph g;
    graph_init(&g);
    graph_load_from_file(&g, GRAPH_FILE);

    int choice = 0;
    char username[64], password[64];
    bool logged_in = false;
    char current_user[64] = "";

    while (1) {
        printf("\n--- Main Menu ---\n");
        if (!logged_in) {
            printf("1. Register\n");
            printf("2. Login\n");
            printf("0. Exit\n");
            printf("Choice: "); if (scanf("%d%*c", &choice)!=1) choice=0;
            if (choice==0) break;
            if (choice==1) {
                printf("Username: "); fgets(username, sizeof(username), stdin); if (username[strlen(username)-1]=='\n') username[strlen(username)-1]=0;
                printf("Password: "); fgets(password, sizeof(password), stdin); if (password[strlen(password)-1]=='\n') password[strlen(password)-1]=0;
                if (register_user(username, password)) {
                    printf("Registered successfully. Please login.\n");
                } else {
                    printf("Registration failed: user may already exist.\n");
                }
            } else if (choice==2) {
                printf("Username: "); fgets(username, sizeof(username), stdin); if (username[strlen(username)-1]=='\n') username[strlen(username)-1]=0;
                printf("Password: "); fgets(password, sizeof(password), stdin); if (password[strlen(password)-1]=='\n') password[strlen(password)-1]=0;
                if (login_user(username, password)) {
                    logged_in = true; strncpy(current_user, username, sizeof(current_user)-1); current_user[sizeof(current_user)-1]=0;
                    printf("Welcome, %s!\n", current_user);
                } else printf("Login failed.\n");
            } else printf("Invalid choice.\n");
        } else {
            // logged in menu
            printf("Logged in as: %s\n", current_user);
            printf("1. Tourist Module\n");
            printf("2. Search Module (BST)\n");
            printf("3. Graph Module\n");
            printf("4. Logout\n");
            printf("0. Exit\n");
            printf("Choice: "); if (scanf("%d%*c", &choice)!=1) choice=0;
            if (choice==0) break;
            if (choice==1) {
                tourist_menu(&places, &bst);
            } else if (choice==2) {
                char q[128];
                printf("Enter exact name to find: ");
                fgets(q, sizeof(q), stdin); if (q[strlen(q)-1]=='\n') q[strlen(q)-1]=0;
                Place *res = bst_search(bst, q);
                if (res) {
                    printf("Found: %s (%s) Rating: %.1f Dist: %.2f km\n", res->name, res->type, res->rating, res->distance);
                } else printf("Not found.\n");
            } else if (choice==3) {
                graph_menu(&g);
            } else if (choice==4) {
                logged_in = false; current_user[0]=0;
                printf("Logged out.\n");
            } else printf("Invalid choice.\n");
        }
    }

    // cleanup
    bst_free(bst);
    free_places(places);
    printf("Exiting. Bye!\n");
    return 0;
}
