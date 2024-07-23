#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <queue>
#include <vector>
#include <limits>
using namespace std;

#define INF numeric_limits<int>::max()

typedef pair<int, int> PII; /* (dist, node) */

int table[100][100][2];
int dist[100][100];
int node_num;

void init_table();
void change_table();
void dijkstra();
void write_output(FILE* messages_file, FILE* output_file);

void print_table()
{
	for (int i = 0; i < node_num; i++)
	{
		for (int j = 0; j < node_num; j++)
		{
			//if (table[i][j][0] == INF) continue;

			//(목적지, 다음, 거리) 출력
			cout << j << ' ' << table[i][j][1] << ' ' << table[i][j][0] << '\n';
		}
		cout << '\n';
	}
    cout << "---------\n";
}


void init_table() 
{
    for (int i = 0; i < node_num; i++) 
	{
        for (int j = 0; j < node_num; j++) 
		{
            if (i == j) 
			{
                table[i][j][0] = 0;
                table[i][j][1] = i;
            } 
			else 
			{
                table[i][j][0] = INF;
                table[i][j][1] = INF;
            }
        }
    }
}

void change_table() 
{
    for (int i = 0; i < node_num; i++) 
	{
        for (int j = 0; j < node_num; j++) 
		{
            if (i != j && dist[i][j] != 0) 
			{
                table[i][j][0] = dist[i][j];
                table[i][j][1] = j;
            }
        }
    }
}

void dijkstra() {
    for (int src = 0; src < node_num; src++) 
	{
        vector<int> mindist(node_num, INF);
        vector<int> prev(node_num, -1);
        vector<bool> visited(node_num, false);

        mindist[src] = 0;
        priority_queue<PII, vector<PII>, greater<PII>> pq;
        pq.push({0, src});

        while (!pq.empty()) {
            int u = pq.top().second;
            pq.pop();

            if (visited[u]) continue;
            visited[u] = true;

            for (int v = 0; v < node_num; v++) 
			{
                if (dist[u][v] > 0 && mindist[u] != INF) 
				{
                    int new_dist = mindist[u] + dist[u][v];
                    if (new_dist < mindist[v] || (new_dist == mindist[v] && u < prev[v])) 
					{
                        mindist[v] = new_dist;
                        pq.push({new_dist, v});
                        prev[v] = u;
                    }
                }
            }
        }

        for (int dest = 0; dest < node_num; dest++) 
		{
            if (mindist[dest] == INF) continue;

            table[src][dest][0] = mindist[dest];
            if (src == dest) 
			{
                table[src][dest][1] = src;
            } 
			else 
			{
                int next = dest;
                while (prev[next] != src) 
				{
                    next = prev[next];
                }
                table[src][dest][1] = next;
            }
        }
    }
}

void write_output(FILE* messages_file, FILE* output_file) 
{
    for (int i = 0; i < node_num; i++) 
	{
        for (int j = 0; j < node_num; j++) 
		{
            if (table[i][j][0] == INF || table[i][j][0] == -999) continue;

            fprintf(output_file, "%d %d %d\n", j, table[i][j][1], table[i][j][0]);
        }
        fprintf(output_file, "\n");
    }

    char line[1500];
    while (fgets(line, sizeof(line), messages_file)) 
	{
        int source, dest;
        char msg[1000];

        if (sscanf(line, "%d %d %[^\n]", &source, &dest, msg) == 3) 
		{
            if (table[source][dest][0] == INF || table[source][dest][0] == -999) 
			{
                fprintf(output_file, "from %d to %d cost infinite hops unreachable message %s\n", source, dest, msg);
            } 
			
			else 
			{
                fprintf(output_file, "from %d to %d cost %d hops ", source, dest, table[source][dest][0]);

                int node = source;
                int next = table[node][dest][1];
                fprintf(output_file, "%d ", node);

                while (next != dest) 
				{
                    node = next;
                    next = table[node][dest][1];
                    fprintf(output_file, "%d ", node);
                }

                fprintf(output_file, "message %s\n", msg);
            }
        }
    }

    fprintf(output_file, "\n");
}


int main(int argc, char* argv[]) 
{
    if (argc != 4) 
	{
        fprintf(stderr, "usage: linkstate topologyfile messagesfile changesfile\n");
        return 1;
    }

    FILE* topology_file = fopen(argv[1], "r");
    FILE* messages_file = fopen(argv[2], "r");
    FILE* changes_file = fopen(argv[3], "r");

    if (!topology_file || !messages_file || !changes_file) 
	{
        fprintf(stderr, "Error: open input file\n");
        return 1;
    }

    FILE* output_file = fopen("output_ls.txt", "w");
    if (!output_file) 
	{
        fprintf(stderr, "Error: open output file\n");
        return 1;
    }

    fscanf(topology_file, "%d", &node_num);
    init_table();

    int a, b, cost;
    while (fscanf(topology_file, "%d %d %d", &a, &b, &cost) != EOF) 
	{
        dist[a][b] = cost;
        dist[b][a] = cost;
        table[a][b][0] = cost;
        table[b][a][0] = cost;

        table[a][b][1] = b;
        table[b][a][1] = a;
    }

    dijkstra();
    write_output(messages_file, output_file);
    print_table();

    while (fscanf(changes_file, "%d %d %d", &a, &b, &cost) != EOF) 
	{
        dist[a][b] = cost;
        dist[b][a] = cost;

        init_table();
        change_table();
        dijkstra();
        fseek(messages_file, 0, SEEK_SET); 
        write_output(messages_file, output_file);
        print_table();
    }

    fclose(topology_file);
    fclose(messages_file);
    fclose(changes_file);
    fclose(output_file);

    return 0;
}