#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#define INF -999
using namespace std;

int message_num;
int node_num;

FILE *topology_file, *messages_file, *changes_file, *output_file;

/*
table[a][b][0]: dist from a to b
table[a][b][1]: next
*/
int table[100][100][2]; 
int dist[100][100];

void init_table();
void change_table();
void exchange_table();
void write_output(FILE* messages_file, FILE* output_file);


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
				table[i][j][0] = INF; //dist
				table[i][j][1] = INF; //next 
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
			if (i == j) continue;
			
			if (dist[i][j] != 0)
			{
				table[i][j][0] = dist[i][j]; //dist
				table[i][j][1] = j; //next 
			}
		}
	}
}


void exchange_table()
{
	while (1)
	{
		bool is_changed = false; 

		for (int i = 0; i < node_num; i++)
		{
			for (int j = 0; j < node_num; j++)
			{
				/* If i and j are direct neighbor, i sends its table to j */
				if (dist[i][j] > 0 && table[i][j][1] == j)
				{
					/* Find path (j-i-k) */
					for (int k = 0; k < node_num; k++)
					{
						if (table[i][k][0] > 0) /* If path (i-k) exists */
						{
							/* Update 1. no path (j-k) exists, 2. path (j-i-k) is shorter than (j-k) */
							if (table[j][k][0] < 0 || table[j][k][0] > table[i][j][0] + table[i][k][0])
							{
								table[j][k][0] = table[i][j][0] + table[i][k][0];
								table[j][k][1] = i;
								is_changed = true;
							}
						}
						else if (table[j][k][0] == table[i][j][0] + table[i][k][0])
						{
							if (i < table[j][k][1]) /* tie breaking 1 */
							{
								table[j][k][1] = i;
								is_changed = true;
							}
						}
					}
				}
			}
		}

		if (!is_changed) return;
	}
}


void write_output(FILE* messages_file, FILE* output_file)
{
    /* write routing tables */
    for (int i = 0; i < node_num; i++)
    {
        for (int j = 0; j < node_num; j++)
        {
            if (table[i][j][0] == INF) continue;
            fprintf(output_file, "%d %d %d\n", j, table[i][j][1], table[i][j][0]);
        }
        fprintf(output_file, "\n");
    }

    /* write messages */
    fseek(messages_file, 0, SEEK_SET); 
    char line[1500];
    while (fgets(line, sizeof(line), messages_file))
    {
        int source, dest;
        char msg[1000];

        if (sscanf(line, "%d %d %[^\n]", &source, &dest, msg) == 3)
        {
            if (table[source][dest][0] == INF) 
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
		fprintf(stderr, "usage: distvec topologyfile messagesfile changesfile\n");
		return 1;
	}

	topology_file = fopen(argv[1], "r");
	messages_file = fopen(argv[2], "r");
	changes_file = fopen(argv[3], "r");

	if (topology_file == NULL || messages_file == NULL || changes_file == NULL)
	{
		fprintf(stderr, "Error: open input file\n");
		return 1;
	}

	output_file = fopen("output_dv.txt", "w");
	if (output_file == NULL)
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

	exchange_table();
	write_output(messages_file, output_file);

	/* changes */
	while (fscanf(changes_file, "%d %d %d", &a, &b, &cost) != EOF)
	{
		dist[a][b] = cost;
		dist[b][a] = cost;

		init_table();
		change_table();
		exchange_table();
		write_output(messages_file, output_file);
	}

	fclose(topology_file); 
	fclose(messages_file); 
	fclose(changes_file); 
	fclose(output_file);

	return 0;
}