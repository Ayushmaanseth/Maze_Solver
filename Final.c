#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "abdrive.h"
#include "simpletext.h"
#include "simpletools.h"
#include "ping.h"
#include <stdbool.h>  
#include <time.h>

int V = 16;

int irRight = 0;
int irRightT = 0;
int irLeftT = 0;

int irLeft = 0;
int dist = 0;
int irLeftTicks,irRightTicks,irLeftTicksBefore = 0,irRightTicksBefore = 0;

int matrixForVals[17][4];
int matrixForCells[5][4];

bool visited[16];
int parent[16],shortestPath[16];

int x = 0;

struct StackNode
{
    int data;
    struct StackNode* next;
};
 
struct StackNode* newNode(int data)
{
    struct StackNode* stackNode =
              (struct StackNode*) malloc(sizeof(struct StackNode));
    stackNode->data = data;
    stackNode->next = NULL;
    return stackNode;
}
 
int isE(struct StackNode *root)
{
    return !root;
}
 
void push(struct StackNode** root, int data)
{
    struct StackNode* stackNode = newNode(data);
    stackNode->next = *root;
    *root = stackNode;
    //printf("%d pushed to stack\n", data);
}
 
int pop(struct StackNode** root)
{
    if (isE(*root))
        return INT_MIN;
    struct StackNode* temp = *root;
    *root = (*root)->next;
    int popped = temp->data;
    free(temp);
 
    return popped;
}
 


 struct StackNode* st = NULL;
//----------------------------------------------------------------------------------
struct AdjListNode
{
    int dest;
    int weight;
    struct AdjListNode* next;
};
 
// A structure to represent an adjacency liat
struct AdjList
{
    struct AdjListNode *head;  // pointer to head node of list
};
 
// A structure to represent a graph. A graph is an array of adjacency lists.
// Size of array will be V (number of vertices in graph)
struct Graph
{
    int V;
    struct AdjList* array;
};
 
// A utility function to create a new adjacency list node
struct AdjListNode* newAdjListNode(int dest, int weight)
{
    struct AdjListNode* newNode =
            (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}
 
// A utility function that creates a graph of V vertices
struct Graph* createGraph(int V)
{
    struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));
    graph->V = V;
 
    // Create an array of adjacency lists.  Size of array will be V
    graph->array = (struct AdjList*) malloc(V * sizeof(struct AdjList));
 
     // Initialize each adjacency list as empty by making head as NULL
    for (int i = 0; i < V; ++i)
        graph->array[i].head = NULL;
 
    return graph;
}
 
// Adds an edge to an undirected graph
void addEdge(struct Graph* graph, int src, int dest, int weight)
{
    // Add an edge from src to dest.  A new node is added to the adjacency
    // list of src.  The node is added at the begining
    struct AdjListNode* newNode = newAdjListNode(dest, weight);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
 
    // Since graph is undirected, add an edge from dest to src also
    newNode = newAdjListNode(src, weight);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
}

bool checkEdge(struct Graph* graph,int src,int dest)
{

        struct AdjListNode* pCrawl = graph->array[src].head;
        //printf("\n Adjacency list of vertex %d\n head ", v);
        while (pCrawl)
        {
            //printf("-> %d", pCrawl->dest);
            if((pCrawl->dest) == dest)
              return true;
            pCrawl = pCrawl->next;  
        }

        return false;
        //printf("\n");
    }


 
// Structure to represent a min heap node
struct MinHeapNode
{
    int  v;
    int dist;
};
 
// Structure to represent a min heap
struct MinHeap
{
    int size;      // Number of heap nodes present currently
    int capacity;  // Capacity of min heap
    int *pos;     // This is needed for decreaseKey()
    struct MinHeapNode **array;
};
 
// A utility function to create a new Min Heap Node
struct MinHeapNode* newMinHeapNode(int v, int dist)
{
    struct MinHeapNode* minHeapNode =
           (struct MinHeapNode*) malloc(sizeof(struct MinHeapNode));
    minHeapNode->v = v;
    minHeapNode->dist = dist;
    return minHeapNode;
}
 
// A utility function to create a Min Heap
struct MinHeap* createMinHeap(int capacity)
{
    struct MinHeap* minHeap =
         (struct MinHeap*) malloc(sizeof(struct MinHeap));
    minHeap->pos = (int *)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array =
         (struct MinHeapNode**) malloc(capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}
 
// A utility function to swap two nodes of min heap. Needed for min heapify
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}
 
// A standard function to heapify at given idx
// This function also updates position of nodes when they are swapped.
// Position is needed for decreaseKey()
void minHeapify(struct MinHeap* minHeap, int idx)
{
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;
 
    if (left < minHeap->size &&
        minHeap->array[left]->dist < minHeap->array[smallest]->dist )
      smallest = left;
 
    if (right < minHeap->size &&
        minHeap->array[right]->dist < minHeap->array[smallest]->dist )
      smallest = right;
 
    if (smallest != idx)
    {
        // The nodes to be swapped in min heap
        struct MinHeapNode *smallestNode = minHeap->array[smallest];
        struct MinHeapNode *idxNode = minHeap->array[idx];
 
        // Swap positions
        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;
 
        // Swap nodes
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
 
        minHeapify(minHeap, smallest);
    }
}
 
// A utility function to check if the given minHeap is ampty or not
int isEmpty(struct MinHeap* minHeap)
{
    return minHeap->size == 0;
}
 
// Standard function to extract minimum node from heap
struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
    if (isEmpty(minHeap))
        return NULL;
 
    // Store the root node
    struct MinHeapNode* root = minHeap->array[0];
 
    // Replace root node with last node
    struct MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;
 
    // Update position of last node
    minHeap->pos[root->v] = minHeap->size-1;
    minHeap->pos[lastNode->v] = 0;
 
    // Reduce heap size and heapify root
    --minHeap->size;
    minHeapify(minHeap, 0);
 
    return root;
}
 
// Function to decreasy dist value of a given vertex v. This function
// uses pos[] of min heap to get the current index of node in min heap
void decreaseKey(struct MinHeap* minHeap, int v, int dist)
{
    // Get the index of v in  heap array
    int i = minHeap->pos[v];
 
    // Get the node and update its dist value
    minHeap->array[i]->dist = dist;
 
    // Travel up while the complete tree is not hepified.
    // This is a O(Logn) loop
    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist)
    {
        // Swap this node with its parent
        minHeap->pos[minHeap->array[i]->v] = (i-1)/2;
        minHeap->pos[minHeap->array[(i-1)/2]->v] = i;
        swapMinHeapNode(&minHeap->array[i],  &minHeap->array[(i - 1) / 2]);
 
        // move to parent index
        i = (i - 1) / 2;
    }
}
 
// A utility function to check if a given vertex
// 'v' is in min heap or not
bool isInMinHeap(struct MinHeap *minHeap, int v)
{
   if (minHeap->pos[v] < minHeap->size)
     return true;
   return false;
}

void printPath(int parent[], int j)
{
  
    // Base Case : If j is source
    if (parent[j]==-1)
        return;

    push(&st,j);
    printPath(parent, parent[j]);
 
    printf("%d ", j);


}
 
// A utility function used to print the solution
/*
int printSolution(int dist[], int n, int parent[])
{
    int src = 0;
    printf("Vertex\t  Distance\tPath");
    for (int i = ; i < V; i++)
    {
        printf("\n%d -> %d \t\t %d\t\t%d ", src, i, dist[i], src);
        printPath(parent, i);
    }
}
*/
// The main function that calulates distances of shortest paths from src to all
// vertices. It is a O(ELogV) function
void dijkstra(struct Graph* graph, int src)
{
    
    int V = graph->V;// Get the number of vertices in graph
    int dist[V];      // dist values used to pick minimum weight edge in cut
 
    // minHeap represents set E
    struct MinHeap* minHeap = createMinHeap(V);
 
    // Initialize min heap with all vertices. dist value of all vertices 
    for (int v = 0; v < V; ++v)
    {
        dist[v] = INT_MAX;
        minHeap->array[v] = newMinHeapNode(v, dist[v]);
        minHeap->pos[v] = v;
    }
 
    // Make dist value of src vertex as 0 so that it is extracted first
    minHeap->array[src] = newMinHeapNode(src, dist[src]);
    minHeap->pos[src]   = src;
    dist[src] = 0;
    decreaseKey(minHeap, src, dist[src]);
 
    // Initially size of min heap is equal to V
    minHeap->size = V;
 
    // In the followin loop, min heap contains all nodes
    // whose shortest distance is not yet finalized.
    while (!isEmpty(minHeap))
    {
        // Extract the vertex with minimum distance value
        struct MinHeapNode* minHeapNode = extractMin(minHeap);
        int u = minHeapNode->v; // Store the extracted vertex number
 
        // Traverse through all adjacent vertices of u (the extracted
        // vertex) and update their distance values
        struct AdjListNode* pCrawl = graph->array[u].head;
        while (pCrawl != NULL)
        {
            int v = pCrawl->dest;
 
            // If shortest distance to v is not finalized yet, and distance to v
            // through u is less than its previously calculated distance
            if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX && 
                                          pCrawl->weight + dist[u] < dist[v])
            {
                parent[v] = u; 
                dist[v] = dist[u] + pCrawl->weight;
 
                // update distance value in min heap also
                decreaseKey(minHeap, v, dist[v]);
            }
            pCrawl = pCrawl->next;
        }
    }
 
    // print the calculated shortest distances
    //printSolution(dist, V, parent);
    printPath(parent,15);
}

void makeZero()
{
  for(int i = 0;i<5;i++)
  {
    for(int j = 0;j<4;j++)
    {
      matrixForCells[i][j] = 0;
    }
  }

  for(int i = 0;i<17;i++)
  {
    for(int j = 0;j<4;j++)
    {
      matrixForVals[i][j] = 0;
    }
  }

  for(int k = 0 ; k < 16 ; k++)
  {
  visited[k] = false;
  } 

  for(int i = 0;i<V;i++)
  {
    parent[i] = -1;
    shortestPath[i] = -1;
  }
}

void getLeftDist()
{
  irLeft = 0;

  for(int dacVal = 0; dacVal <= 160; dacVal += 8)  // <- add
    {                                               // <- add
      dac_ctr(26, 0, dacVal);                       // <- add
      freqout(11, 1, 38000);                        // <- add
      irLeft += input(10);
    }

        
}

void getRightDist()
{
  irRight = 0;

  for(int dacVal = 0; dacVal <= 160; dacVal += 8)  // <- add
    {                                               // <- add
      dac_ctr(26, 0, dacVal);                       // <- add
      freqout(1, 1, 38000);                        // <- add
      irRight += input(2);
    }

        
}

int getWallDist()
{
  return ping_cm(8);
}

int randomnum()
{
  time_t t;
  srand((unsigned) time(&t));
  int num = rand() % (26 + 1 - 25) + 25;
  return num;
}

void driveDefault()
{
  drive_goto(125,125);
}

void turnLeft()
{
  drive_goto(-26,25);
}

void turnRight()
{
  drive_goto(26,-25);
}

  bool checkWall()
{
  int dist = ping_cm(8);
  if (dist < 34)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool checkLeftObstruction()
{
  getLeftDist();

  if(irLeft < 20)
    return true;
  else
    return false;

}

bool checkRightObstruction()
{
  getRightDist();

  if(irRight < 20)
    return true;
  else
    return false;

}

void getInitial()
{
  getLeftDist();
  getRightDist();

  dist = getWallDist();
 
}

bool checkInit(int leftDist,int rightDist, int wallDist)
{
  getLeftDist();
  irLeftT = irLeft;
  getRightDist();
  irRightT = irRight;

  int temp = getWallDist();

  if (leftDist == irLeftT && rightDist == irRightT && temp == dist)
  {
    return true;
  }

  else
    return false;

}

void followLeft()
{
  int y=0,x=0;
  int direction = 0;
  getInitial();
  makeZero();

  int V = 17;
  struct Graph* graph = createGraph(V);

  int cellCounter = 0;


  while(true)
  {
    
    //turnLeft();
    getLeftDist();
    getRightDist();

    
    if (checkLeftObstruction() == false)
    {
      turnLeft();
      direction -= 1;
      driveDefault();
    }
    else
    {
      //turnRight();
      if (checkWall() == false)
      {
        driveDefault();
      }
      else
      {
        turnRight();
        direction +=1;
        if (checkWall() == false)
        {
          driveDefault();
        }
        else
        {
          turnRight();
          direction += 1;
          driveDefault();
        }
      }
    }

    if(checkInit(irLeft,irRight,dist) == true)
      break;
    /*
    if (checkRightObstruction() == true && checkWall() == true)
    {
      break;
    }
    */
    //drive_getTicks(&irLeftTicks,&irRightTicks);
    //printf("%d %d\n",irRightTicks,irRightTicks);

    if(direction < 0)
      direction += 4;

    else if(direction > 3)
    direction -= 4;

  //printf("%d\n",direction );

  if(direction == 0)
  {
    if(visited[cellCounter + 4] == false || (visited[cellCounter + 4] == true && !checkEdge(graph,cellCounter,cellCounter + 4)))
    {
    addEdge(graph,cellCounter,cellCounter + 4,1);
    }
    cellCounter += 4;
    visited[cellCounter] = true;
  }

  else if(direction == 1 )
  {
    if(visited[cellCounter + 1] == false  || (visited[cellCounter + 1] == true && !checkEdge(graph,cellCounter,cellCounter + 1)))
    {
    addEdge(graph,cellCounter,cellCounter + 1,1);
    } 
    cellCounter += 1;
    visited[cellCounter] = true;
  }

  else if(direction == 2)
  {
    if(visited[cellCounter - 4] == false || (visited[cellCounter -4] == true && !checkEdge(graph,cellCounter,cellCounter - 4)) )
    {
    addEdge(graph,cellCounter,cellCounter - 4,1);
    }
    cellCounter -= 4;
    visited[cellCounter] = true;
  }

  else if(direction == 3)
  {
    if(visited[cellCounter - 1] == false  || (visited[cellCounter - 1] == true && !checkEdge(graph,cellCounter,cellCounter - 1)))
    {
      addEdge(graph,cellCounter,cellCounter -1,1);
    }
    cellCounter -= 1;
    visited[cellCounter] = true;
  }

  printf("%d\n",cellCounter);

  if(visited[0] == true)
    break;
  }

/*
  bool b = checkEdge(graph,9,5);
  if(b == true)
    printf("true\n");
  else
    printf("false\n");
  */
  turnLeft();
  drive_goto(10,10);
  turnRight();
  turnRight();

  dijkstra(graph,0);

  drive_goto(15,15);

  int cc = 0;

while(!isE(st))
{
  int dir = pop(&st);

  if((dir - cc) == 1)
  {
    turnRight();
    driveDefault();
    turnLeft();
    cc += 1;
  }

  else if((dir - cc) == -1)
  {
    turnLeft();
    driveDefault();
    turnRight();
    cc -= 1;
  }

  else if((dir - cc) == 4)
  {
    driveDefault();
    cc += 4;
  }

  else if((dir - cc) == -4)
  {
    turnLeft();
    turnLeft();
    driveDefault();
    turnRight();
    turnRight();
  }

}
  
}


void main()
{
  driveDefault();
  followLeft();

}
