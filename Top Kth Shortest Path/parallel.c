


#define _CRT_SECURE_NO_DEPRECATE // To suppress the warning for fopen
#include <mpi.h> // MPI library
#include <omp.h> // OpenMP library
#include "functions.h" // Functions for finding K shortest paths

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
        if (!fp) {
            fprintf(stderr, "Error opening file on rank %d.\n", rank);
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        if (fscanf(fp, "Nodes: %d Edges: %d", &N, &M) != 2) {
            fprintf(stderr, "Error reading header on rank %d.\n", rank);
            fclose(fp);
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }

        // Allocate memory for edges
        edges = malloc(M * sizeof(*edges));
        if (edges == NULL) {
            fprintf(stderr, "Memory allocation failed on rank %d.\n", rank);
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
            fprintf(stderr, "Memory allocation failed on rank %d.\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    // Broadcast edges to all processes
    MPI_Bcast(edges, M * 3, MPI_INT, 0, MPI_COMM_WORLD);

    // Generate 10 random pairs, making sure src and dest are different
    const int totalPairs = 10;
    int pairs[10][2];
    if (rank == 0) {
        for (int i = 0; i < totalPairs; i++) {
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
        sendCounts[i] = (i < totalPairs % size) ? (totalPairs / size + 1) * 2 : (totalPairs / size) * 2;
        displs[i] = sum;
        sum += sendCounts[i];
    }

    // Allocate memory for local pairs on each process
    int localPairsCount = sendCounts[rank] / 2;
    int(*localPairs)[2] = malloc(localPairsCount * sizeof(*localPairs));


    // Scatter the pairs to all processes
    MPI_Scatterv(pairs, sendCounts, displs, MPI_INT, localPairs, sendCounts[rank], MPI_INT, 0, MPI_COMM_WORLD);


    // De-Allocating the Memory
    free(sendCounts);
    free(displs);

    printf("Processor %s, rank %d out of %d, handling %d pairs\n",
        processorName, rank, size, localPairsCount);

    // Each process will construct the graph
    // Initialize graph
    struct Node** g = (struct Node**)malloc((N + 1) * sizeof(struct Node*));
    // Initialize adjacency list
    for (int i = 0; i <= N; i++) {
        g[i] = NULL; // Initialize all nodes to NULL
    }

    // Storing edges, T.C: O(M)
    for (int i = 0; i < M; i++) {
        int src = edges[i][0];
        int dest = edges[i][1];
        int cost = edges[i][2];
        struct Node* newNode = createNode(dest, cost);
        newNode->next = g[src]; // Adding the new node to the adjacency list
        g[src] = newNode; // Updating the adjacency list
    }

    // De-Allocating the Memory
    free(edges);

    double start_s, stop_s;
    start_s = MPI_Wtime();

    int i;
    // Check if there are multiple pairs to process and use OpenMP to parallelize
#pragma omp parallel for if(localPairsCount > 1) num_threads(localPairsCount)
    for (i = 0; i < localPairsCount; i++) {
        printf("Processor %s, rank %d, thread %d has pair (%d, %d)\n",
            processorName, rank, omp_get_thread_num(), localPairs[i][0], localPairs[i][1]);

        // Find K shortest paths for each pair
        findKShortest(g, N, M, K, localPairs[i][0], localPairs[i][1]);
    }

    stop_s = MPI_Wtime();

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

    free(localPairs);

    if (rank == 0)
        printf("\nTime taken for execution: %.8f seconds\n\n", stop_s - start_s);

    MPI_Finalize();
    return 0;
}


