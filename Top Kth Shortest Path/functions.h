#pragma once
#include <stdio.h> // Standard I/O library
#include <stdlib.h> // Standard library

// Structure to represent a node in the graph
struct Node {
    int dest; // Destination node
    int cost; // Cost of the edge
    struct Node* next; // Pointer to the next node
};

// Function to create a new node
inline struct Node* createNode(int dest, int cost) {
    // Allocating memory for the new node
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));

    newNode->dest = dest;
    newNode->cost = cost;
    newNode->next = NULL;
    return newNode;
}

typedef struct {
    int priority; // stores the cost
    int item;     // stores the node index
} MinHeapNode;

typedef struct {
    MinHeapNode* elements; // Array of heap nodes
    int size;              // Current size of the heap
    int capacity;          // Maximum capacity of the heap
} MinHeap;

// Function to create a new heap
inline MinHeap* createMinHeap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->elements = (MinHeapNode*)malloc(sizeof(MinHeapNode) * capacity);
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

// Function to insert a node into the heap
inline void minHeapInsert(MinHeap* heap, int priority, int item) {
    if (heap->size == heap->capacity) {
        fprintf(stderr, "Heap overflow\n");
        return;
    }
    // Insert the new node at the end of the heap
    int i = heap->size++;
    heap->elements[i].priority = priority;
    heap->elements[i].item = item;

    // Fix the min heap property if it is violated
    while (i != 0 && heap->elements[(i - 1) / 2].priority > heap->elements[i].priority) {
        MinHeapNode temp = heap->elements[i];
        heap->elements[i] = heap->elements[(i - 1) / 2];
        heap->elements[(i - 1) / 2] = temp;
        i = (i - 1) / 2;
    }
}

// Min heapify function
inline void minHeapify(MinHeap* heap, int i) {
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    int smallest = i;
    if (left < heap->size && heap->elements[left].priority < heap->elements[smallest].priority)
        smallest = left;
    if (right < heap->size && heap->elements[right].priority < heap->elements[smallest].priority)
        smallest = right;
    if (smallest != i) {
        MinHeapNode temp = heap->elements[i];
        heap->elements[i] = heap->elements[smallest];
        heap->elements[smallest] = temp;
        minHeapify(heap, smallest);
    }
}

// Function to extract the node with the smallest priority
inline MinHeapNode minHeapExtractMin(MinHeap* heap) {
    if (heap->size <= 0) {
        return (MinHeapNode) { .priority = INT_MAX, .item = -1 };
    }
    MinHeapNode root = heap->elements[0];
    heap->elements[0] = heap->elements[--heap->size];
    minHeapify(heap, 0);
    return root;
}

// Function to find K shortest path lengths
inline void findKShortest(struct Node** g, int n, int m, int k, int srcNode, int destNode) {
    // Initialize distance array
    int** dis = (int**)malloc((n + 1) * sizeof(int*));
    for (int i = 0; i <= n; i++) {
        dis[i] = (int*)malloc(k * sizeof(int));
        for (int j = 0; j < k; j++) {
            dis[i][j] = INT_MAX;
        }
    }

    // Initialize priority queue
    MinHeap* pq = createMinHeap(m); // Initialize the heap with maximum capacity as the number of edges
    minHeapInsert(pq, 0, srcNode); // Insert the source node
    dis[srcNode][0] = 0;

    // while pq has elements
    while (pq->size > 0) {
        MinHeapNode minNode = minHeapExtractMin(pq);
        int u = minNode.item;
        int d = minNode.priority;

        if (d > dis[u][k - 1]) continue; // Only continue if the current distance is within the k shortest found so far

        struct Node* current = g[u];
        while (current) {
            int v = current->dest;
            int newDist = d + current->cost;

            // If newDist can be a candidate for k-shortest paths, process it
            if (newDist < dis[v][k - 1]) {
                // Find the correct position for the new distance
                int pos;
                for (pos = k - 1; pos >= 0 && dis[v][pos] > newDist; pos--) {
                    if (pos < k - 1) dis[v][pos + 1] = dis[v][pos]; // Shift distances right
                }
                dis[v][pos + 1] = newDist; // Insert the new distance

                // Insert the new distance into the heap if it is within the k shortest distances
                minHeapInsert(pq, newDist, v);
            }
            current = current->next;
        }
    }

    // Printing K shortest paths
    for (int i = 0; i < k; i++) {
        if (dis[destNode][i] == INT_MAX) {
            printf("INF ");
        }
        else {
            printf("%d ", dis[destNode][i]);
        }
    }
    printf("\n");

    // Free allocated memory
    for (int i = 0; i <= n; i++) {
        free(dis[i]);
    }
    free(dis);
    free(pq->elements);
    free(pq);
}