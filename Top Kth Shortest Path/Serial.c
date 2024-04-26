/*

#define _CRT_SECURE_NO_DEPRECATE // To suppress the warning for fopen
#include <stdio.h> // Standard Input Output
#include <stdlib.h> // Standard Library

// Structure to represent a node in the graph
struct Node {
    int dest; // Destination node
    int cost; // Cost of the edge
    struct Node* next; // Pointer to the next node
};

// Function to create a new node
struct Node* createNode(int dest, int cost) {
    // Allocating memory for the new node
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));

    newNode->dest = dest;
    newNode->cost = cost;
    newNode->next = NULL;
    return newNode;
}

// Function to find K shortest path lengths
void findKShortest(int edges[][3], int n, int m, int k, int srcNode, int destNode) {
    // Initialize graph
    struct Node** g = (struct Node**)malloc((n + 1) * sizeof(struct Node*));
    // Initialize adjacency list
    for (int i = 0; i <= n; i++) {
        g[i] = NULL; // Initialize all nodes to NULL
    }

    // Storing edges
    for (int i = 0; i < m; i++) {
        int src = edges[i][0];
        int dest = edges[i][1];
        int cost = edges[i][2];
        struct Node* newNode = createNode(dest, cost);
        newNode->next = g[src]; // Adding the new node to the adjacency list
        g[src] = newNode; // Updating the adjacency list
    }

    // Initialize distance array
    int** dis = (int**)malloc((n + 1) * sizeof(int*));
    for (int i = 0; i <= n; i++) {
        dis[i] = (int*)malloc(k * sizeof(int));
        for (int j = 0; j < k; j++) {
            dis[i][j] = 1e9;
        }
    }

    // Initialization of priority queue
    typedef struct {
        int first;
        int second;
    } pair;

    // Priority queue
    pair* pq = (pair*)malloc(n * n * sizeof(pair));
    int pq_size = 0; // Size of the priority queue
    pq[pq_size++] = (pair){ 0, srcNode };
    dis[srcNode][0] = 0; // Distance of the source node is 0

    // while pq has elements
    while (pq_size > 0) {
        // Storing the node value
        int u = pq[0].second; // Node value
        int d = pq[0].first; // Distance value

        for (int i = 0; i < pq_size - 1; i++) {
            pq[i] = pq[i + 1];
        }
        pq_size--;

        // Checking for the distance
        if (dis[u][k - 1] < d)
            continue;
        struct Node* current = g[u];

        // Traversing the adjacency list
        while (current != NULL) {
            int dest = current->dest;
            int cost = current->cost;

            // Checking for the cost
            if (d + cost < dis[dest][k - 1]) {
                dis[dest][k - 1] = d + cost;

                // Sorting the distances
                for (int i = 0; i < k; i++) {
                    for (int j = i + 1; j < k; j++) {
                        if (dis[dest][i] > dis[dest][j]) {
                            int temp = dis[dest][i];
                            dis[dest][i] = dis[dest][j];
                            dis[dest][j] = temp;
                        }
                    }
                }

                // Pushing elements to priority queue
                pq[pq_size++] = (pair){ (d + cost), dest };
            }
            current = current->next;
        }
    }

    // Printing K shortest paths
    for (int i = 0; i < k; i++) {
        if (dis[destNode][i] == 1e9) {
            printf("INF ");
        }
        else {
            printf("%d ", dis[destNode][i]);
        }
    }

    // Free allocated memory
    for (int i = 0; i <= n; i++) {
        free(dis[i]);
    }

    free(dis);
    free(pq);

    for (int i = 0; i <= n; i++) {
        struct Node* current = g[i];
        while (current != NULL) {
            struct Node* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(g);
}

int main() {
    // Number of nodes and edges
    int N = 0, M = 0;
    // Number of shortest paths
    const int K = 4;

    // Source and destination nodes
    int srcNode = 0;
    int destNode = 20000;

    // Reading the graph from a file
    FILE* fp = fopen("datasets/Email-Enron.txt", "r");
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

    printf("\nFile Reading Done\n");
    printf("K shortest paths from %d to %d are: ", srcNode, destNode);
    // Find K shortest paths => Function Call
    findKShortest(edges, N, M, K, srcNode, destNode);
    printf("\nDone");

    free(edges);

    return 0;
}

// Time Complexity : O((N + M)* KlogK)
// Auxiliary Space : O(NK)



*/