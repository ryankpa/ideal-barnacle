/*  CS 3310 LAB 5: Critical Path Procedure
    Author: Ryan Atienza
    Due: 11/26/19
    The goal of this program is to find the Critical Path for a set of tasks, also known as the most efficient ordering of tasks that minimizes time spent completing them.
    The tasks are represented in an acyclic graph using an adjacency matrix. Nodes are ordered using a topological sort before they are processed by the Critical Path procedure
    to ensure that any node's immediate predecessors have been processed.
*/

#include <cstdlib>
#include <iostream>
#include <string.h>
#include <queue>
#include <list>

using namespace std;

// node struct to represent tasks
struct Node
{
    string taskName = "";
    int timeNeeded = 0;
    int ES = 0;    // earliest start
    int EF = 0;    // earliest finish
    int LS = 0;    // latest start
    int LF = 0;    // latest finish
    int inDeg = 0; // in-degree count, for topological sort
    int slack = 0;
    bool processed = false;
};

void initializeData(int **G, Node *T);
void topologicalSort(list<Node> &order, int **G, Node *T);
string criticalPath(int **G, Node *T, list<Node> &order);
void forwardPass(int **G, Node *T, list<Node> &order);  // calculating ES, EF
void backwardPass(int **G, Node *T, list<Node> &order); // calculating LS, LF
const int N = 9;                                        // dimensions of the matrix

int main()
{
    // using Western Hills Shopping Center Project graph as test data
    // 0 = A, 1 = B, 2 = C, etc.
    string critPath;
    Node *T = new Node[N];
    int **G = new int *[N];
    for (int i = 0; i < N; i++)
        G[i] = new int[N];
    list<Node> order;

    initializeData(G, T);
    // printing G
    cout << "Adjacency Matrix:\n\t  ";
    for (int i = 0; i < N; i++)
        cout << T[i].taskName << ' ';
    cout << endl;
    for (int i = 0; i < N; i++)
    {
        cout << '\t' << T[i].taskName << ' ';
        for (int j = 0; j < N; j++)
            cout << G[i][j] << ' ';
        cout << endl;
    }

    topologicalSort(order, G, T);
    // print out the sorting
    list<Node>::iterator p = order.begin();
    cout << "Toplogical sorting: ";
    while (p != order.end())
    {
        cout << p->taskName << ' ';
        ++p;
    }
    cout << endl;

    critPath = criticalPath(G, T, order);
    // print every node's ES and EF
    cout << "Earliest start and finish times for each task:\n";
    for (int i = 0; i < N; i++)
    {
        cout << '\t' << T[i].taskName << ": " << T[i].ES << ' ' << T[i].EF << '\t';
        if (i % 3 == 2)
            cout << endl;
    }

    // print every node's LS and LF
    cout << "Latest start and finish times for each task:\n";
    for (int i = 0; i < N; i++)
    {
        cout << '\t' << T[i].taskName << ": " << T[i].LS << ' ' << T[i].LF << '\t';
        if (i % 3 == 2)
            cout << endl;
    }
    // print every node's slack
    cout << "Delay allowed for each task without compromising the minimum completion time:\n";
    for (int i = 0; i < N; i++)
    {
        cout << '\t' << T[i].taskName << ": " << T[i].slack << '\t';
        if (i % 3 == 2)
            cout << endl;
    }
    cout << "Critical path: " << critPath;

    // deleting allocated data
    for(int i = 0; i < N; i++)
        delete[] G[i];
    delete[] G;
    delete[] T;
    
    // debugging purposes

    // // printing in-degrees of nodes
    // for(int i = 0; i < 9; i++)
    //     cout << T[i].taskName << ": " << T[i].inDeg << ' ';
    // cout << endl;

    return 0;
}

void initializeData(int **G, Node *T)
{
    // initializing the graph
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            G[i][j] = 0;

    // initializing nodes
    T[0].taskName = "A"; T[0].timeNeeded = 5;
    T[1].taskName = "B"; T[1].timeNeeded = 6;
    T[2].taskName = "C"; T[2].timeNeeded = 4;
    T[3].taskName = "D"; T[3].timeNeeded = 3;
    T[4].taskName = "E"; T[4].timeNeeded = 1;
    T[5].taskName = "F"; T[5].timeNeeded = 4;
    T[6].taskName = "G"; T[6].timeNeeded = 14;
    T[7].taskName = "H"; T[7].timeNeeded = 12;
    T[8].taskName = "I"; T[8].timeNeeded = 2;

    // adding in the edges to the graph
    // A -> EDC
    G[0][4] = 1; G[0][3] = 1; G[0][2] = 1;
    G[1][7] = 1; // B -> H
    G[2][7] = 1; // C -> H
    G[3][6] = 1; // D -> G
    G[4][5] = 1; // E -> F
    G[5][6] = 1; // F -> G
    G[6][8] = 1; // G -> I
    G[7][8] = 1; // H -> I

    // get in-degrees by scanning the matrix vertically
    for (int j = 0; j < N; j++)
        for (int i = 0; i < N; i++)
            if (G[i][j] == 1)
                T[j].inDeg++;
}

void topologicalSort(list<Node> &order, int **G, Node *T)
{
    queue<Node> Q; // for processing the nodes
    Node dummy;
    for (int i = 0; i < N; i++) // insert the nodes w/ 0 in-deg into Q
        if (T[i].inDeg == 0)
            Q.push(T[i]);
    while (!(Q.empty()))
    {
        dummy = Q.front();
        Q.pop();
        order.push_back(dummy);
        // locating the corresponding task in the Node array
        int k = 0;
        while (k < N && T[k].taskName != dummy.taskName)
            k++;
        // marking the Node as processed
        T[k].processed = true;
        // adjusting the in-degrees of the affected nodes
        for (int i = 0; i < N; i++)
        {
            if (G[k][i] == 1) // if dummy is adjacent to T[i], T[i].inDeg--
            {
                T[i].inDeg--;
                if (T[i].inDeg == 0 && !T[i].processed) // if T[i].inDeg is 0 and not processed, push to queue to be processed
                    Q.push(T[i]);
            }
        }
    }
    // restore in-degrees of the nodes
    for (int j = 0; j < N; j++)
        for (int i = 0; i < N; i++)
            if (G[i][j] == 1)
                T[j].inDeg++;
}

string criticalPath(int **G, Node *T, list<Node> &order)
{
    int k;
    string path = "";
    list<Node>::iterator p = order.begin();
    // calculating ES and EF
    forwardPass(G, T, order);
    // calculating LS and LF
    backwardPass(G, T, order);
    // calculating slack (LS - ES) for each node,
    // and determining the critical path
    while (p != order.end())
    {
        // find the node in T
        k = 0;
        while (k < N && T[k].taskName != p->taskName)
            k++;
        T[k].slack = T[k].LS - T[k].ES;
        if (T[k].slack == 0)
            path += p->taskName + "->";
        ++p;
    }
    // chopping off the extra arrow at the end
    path = path.substr(0, path.length() - 2);
    return path;
}

void forwardPass(int **G, Node *T, list<Node> &order)
{
    // use a list iterator to iterate through order
    list<Node>::iterator p = order.begin();
    int max;    // for computing the max EF of immediate predecessors
    int k;      // for storing the index of a node in T
    bool found; // controlling while loops
    while (p != order.end())
    {
        // find the node in the list
        k = 0;
        found = false;
        max = 0;
        while (k < N && !found)
            if (T[k].taskName == p->taskName)
                found = true;
            else
                k++;

        // calculate ES
        if (T[k].inDeg == 0) // node is at the start of the graph
            T[k].ES = 0;
        else // node has 1 or more predecessors
        {
            // out of T[k]'s predecessors, find the max EF
            for (int i = 0; i < N; i++)
                if (G[i][k] == 1 && max < T[i].EF)
                    max = T[i].EF;
            // set it equal to T[k]'s ES
            T[k].ES = max;
        }

        // calculate T[k]'s EF
        T[k].EF = p->timeNeeded + T[k].ES;
        ++p;
    }
}

void backwardPass(int **G, Node *T, list<Node> &order)
{
    list<Node>::reverse_iterator p = order.rbegin(); // going backwards
    int outDeg;
    int min;    // for computing min LS of immediate successors
    int k;      // stores the index of a node in T
    bool found; // controlling while loops

    while (p != order.rend())
    {
        // find node in list
        k = 0;
        found = false;
        min = INT_MAX;
        outDeg = 0;
        while (k >= 0 && !found)
            if (T[k].taskName == p->taskName)
                found = true;
            else
                k++;
        // compute outDeg for the node
        for (int i = 0; i < N; i++)
            if (G[k][i] == 1)
                outDeg++;

        if (outDeg == 0) // node is a final task
        {
            T[k].LS = T[k].ES;
            T[k].LF = T[k].EF;
        }
        else
        {
            // out of T[k]'s immediate successors, find the min LS
            for (int i = 0; i < N; i++)
                if (G[k][i] == 1 && T[i].LS < min)
                    min = T[i].LS;

            T[k].LF = min; // set T[k].LF = min
            T[k].LS = T[k].LF - T[k].timeNeeded;
        }
        ++p;
    }
}