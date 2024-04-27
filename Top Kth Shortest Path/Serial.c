/*


#define _CRT_SECURE_NO_DEPRECATE // To suppress the warning for fopen
#include <time.h> // Time Library
#include "functions.h" // Functions for finding K shortest paths

int main() {
    // Number of nodes and edges
    int N = 0, M = 0;
    // Number of shortest paths
    const int K = 4;

    // Reading the graph from a file
    FILE* fp = fopen("datasets/Email-EuAll.txt", "r");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    // Reading the number of nodes and edges
    if (fscanf(fp, "Nodes: %d Edges: %d", &N, &M) != 2) {
        printf("Error reading the number of nodes and edges.\n");
        fclose(fp);
        return 1;
    }

    // Print the number of nodes and edges
    printf("Nodes: %d Edges: %d\n", N, M);

    // Allocate memory for the edges
    int(*edges)[3] = malloc(M * sizeof(*edges));
    if (edges == NULL) {
        printf("Memory allocation failed.\n");
        fclose(fp);
        return 1;
    }

    // Read the edges
    char buffer[128]; // buffer for reading each line
    int from, to, weight;
    int lineNo = 0;

    fgets(buffer, sizeof(buffer), fp); // skip the first line with node and edge counts
    while (fgets(buffer, sizeof(buffer), fp) != NULL && lineNo < M) {
        int count = sscanf(buffer, "%d %d %d", &from, &to, &weight);
        if (count == 2) { // Only two integers read, no weight provided
            weight = 1; // Assign default weight
        }
        edges[lineNo][0] = from;
        edges[lineNo][1] = to;
        edges[lineNo][2] = weight;
        lineNo++;
    }

    fclose(fp);

    // Print the edges
    //for (int i = 0; i < M; i++) {
    //    printf("%d %d %d\n", edges[i][0], edges[i][1], edges[i][2]);
    //}

    printf("\nFile Reading Done");

    // Initialize graph
    struct Node** g = (struct Node**)malloc((N + 1) * sizeof(struct Node*));
    // Initialize adjacency list
    for (int i = 0; i <= N; i++) {
        g[i] = NULL; // Initialize all nodes to NULL
    }

    // Storing edges
    for (int i = 0; i < M; i++) {
        int src = edges[i][0];
        int dest = edges[i][1];
        int cost = edges[i][2];
        struct Node* newNode = createNode(dest, cost);
        newNode->next = g[src]; // Adding the new node to the adjacency list
        g[src] = newNode; // Updating the adjacency list
    }
    printf("\nGraph Creation Done\n\n");
    free(edges); // Free the allocated memory

    const int totalPairs = 10;
    int pairs[10][2];
    for (int i = 0; i < totalPairs; i++) {
        pairs[i][0] = rand() % N;
        do {
            pairs[i][1] = rand() % N;
        } while (pairs[i][1] == pairs[i][0]);
    }

    clock_t start_s = clock(); // Start the clock
    // Find K shortest paths => Function Call
    for(int i = 0; i < totalPairs; i++) {
		int srcNode = pairs[i][0];
		int destNode = pairs[i][1];
        printf("K shortest paths from %d to %d are: ", srcNode, destNode);
		findKShortest(g, N, M, K, srcNode, destNode);
	}
    clock_t stop_s = clock(); // Stop the clock

    // Calculate the time taken
    printf("\n\nTime Taken: %.8fs\n", (stop_s - start_s) / (double)(CLOCKS_PER_SEC));

    // De-Allocating the Memory
    for (int i = 0; i <= N; i++) {
        struct Node* current = g[i];
        while (current != NULL) {
            struct Node* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(g);

    return 0;
}

// Time Complexity : O((N + M)* KlogK)
// Auxiliary Space : O(NK)



*/