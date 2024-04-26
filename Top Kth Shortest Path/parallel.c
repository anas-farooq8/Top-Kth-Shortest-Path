

#define _CRT_SECURE_NO_DEPRECATE // To suppress the warning for fopen
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

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

// Main function
int main(int argc, char* argv[]) {

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Seed random number generator
    srand(time(NULL) + rank);

    int N = 0, M = 0; // Number of nodes and edges
    int K = 3;         // Number of shortest paths to find

    // Get the name of the processor
    char processorName[MPI_MAX_PROCESSOR_NAME];
    int nameLen;
    MPI_Get_processor_name(processorName, &nameLen);

    // Only the root process will read the graph and distribute the necessary data
    int(*edges)[3] = NULL;
    if (rank == 0) {
        FILE* fp = fopen("Email-EuAll.txt", "r");
        if (fp == NULL) {
            printf("Error opening file.\n");
            MPI_Abort(MPI_COMM_WORLD, 1); // Exit all processes if file opening fails
        }
        if (fscanf(fp, "Nodes: %d Edges: %d", &N, &M) != 2) {
            printf("Error reading the number of nodes and edges.\n");
            fclose(fp);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Allocate memory for edges
        edges = malloc(M * sizeof(*edges));
        if (edges == NULL) {
            printf("Memory allocation failed.\n");
            fclose(fp);
            MPI_Abort(MPI_COMM_WORLD, 1);
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
    }

    // Broadcast the number of nodes and edges to all processes
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Print N and M on all processes
    // printf("Processor %s, rank %d out of %d, N = %d, M = %d\n", processorName, rank, size, N, M);
  
    // Allocate memory for edges in all processes
    if (rank != 0) {
        edges = malloc(M * sizeof(*edges));
        if (edges == NULL) {
            printf("Memory allocation failed on rank %d.\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    // Broadcast edges to all processes
    MPI_Bcast(edges, M * 3, MPI_INT, 0, MPI_COMM_WORLD);

    // Generate 10 random pairs
    const int total_pairs = 10;
    int pairs[10][2];
    if (rank == 0) {
        for (int i = 0; i < total_pairs; i++) {
            pairs[i][0] = rand() % N;
            do { 
                pairs[i][1] = rand() % N;
            } while (pairs[i][1] == pairs[i][0]);
        }
    }

    // Calculate how many pairs each process will handle
    int* sendCounts = malloc(size * sizeof(int));
    int* displs = malloc(size * sizeof(int));
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sendCounts[i] = (i < total_pairs % size) ? (total_pairs / size + 1) * 2 : (total_pairs / size) * 2;
        displs[i] = sum;
        sum += sendCounts[i];
    }

    // Allocate memory for local pairs on each process
    int local_pairs_count = sendCounts[rank] / 2;
    int(*local_pairs)[2] = malloc(local_pairs_count * sizeof(*local_pairs));

    // Scatter the pairs to all processes
    MPI_Scatterv(pairs, sendCounts, displs, MPI_INT, local_pairs, sendCounts[rank], MPI_INT, 0, MPI_COMM_WORLD);

    printf("Processor %s, rank %d out of %d, handling %d pairs\n",
        processorName, rank, size, local_pairs_count);

    double start_time, end_time;
    start_time = MPI_Wtime();

    for (int i = 0; i < local_pairs_count; i++) {
        printf("Processor %s, rank %d has pair (%d, %d)\n",
            processorName, rank, local_pairs[i][0], local_pairs[i][1]);

        // Find K shortest paths for each pair
        findKShortest(edges, N, M, K, local_pairs[i][0], local_pairs[i][1]);
    }

    end_time = MPI_Wtime();

    free(edges);
    free(sendCounts);
    free(displs);
    free(local_pairs);

    if (rank == 0)
        printf("Time taken for execution: %.8f seconds\n\n", end_time - start_time);

    MPI_Finalize();
    return 0;
}

