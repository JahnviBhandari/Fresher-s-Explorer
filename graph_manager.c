#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfiles/graph.h"  // Updated path

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <print|shortest_path> [source] [destination]\n", argv[0]);
        return 1;
    }
    
    char *command = argv[1];
    Graph g;
    graph_in(&g);
    graph_load_from_file(&g, "graph.txt");
    
    if (strcmp(command, "print") == 0) {
        graph_print(&g);
    } else if (strcmp(command, "shortest_path") == 0 && argc == 4) {
        int src = -1, dest = -1;
        for (int i = 0; i < g.vcount; i++) {
            if (strcmp(g.verts[i].name, argv[2]) == 0) src = i;
            if (strcmp(g.verts[i].name, argv[3]) == 0) dest = i;
        }
        if (src != -1 && dest != -1) {
            graph_dijkstra(&g, src, dest);
        } else {
            printf("Invalid source or destination\n");
        }
    } else {
        printf("Invalid command\n");
    }
    
    return 0;
}