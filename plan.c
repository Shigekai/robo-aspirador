
#include "plan.h"
#include "stdio.h"
#include <unistd.h>

typedef struct {
    int row;
    int col;
} node;

bool existsDirt(enviroment E){
    for(int i=0;i<E.h;i++)
        for(int j=0;j<E.w;j++)
            if(E.grid[i][j].dirt)
                return true;

    return false;
}

place* findNearestDirt(cleaner* C, enviroment E){
    
    bool visited[E.h][E.w];

    for(int i=0;i<E.h;i++)
        for(int j=0;j<E.w;j++)
            visited[i][j] = false;

    node queue[E.h*E.w];
    int start = 0;
    int end = 0;

    queue[end++] = (node){C->whereCleaner->row, C->whereCleaner->col};
    visited[C->whereCleaner->row][C->whereCleaner->col] = true;

    while(start < end){

        node n = queue[start++];

        // procura sujeira na posição atual
        if(E.grid[n.row][n.col].dirt)
            return &E.grid[n.row][n.col];

        // 4 vizinhos
        int moves[4][2] = {
            {1,0},{-1,0},{0,1},{0,-1}
        };


        for(int i=0;i<4;i++){

            int r = n.row + moves[i][0];
            int c = n.col + moves[i][1];

            if(r>=0 && r<E.h && c>=0 && c<E.w){

                if(!visited[r][c]){
                    visited[r][c] = true;
                    queue[end++] = (node){r,c};
                }

            }
        }
    }

    return NULL;
}

void cleanEnviroment(cleaner* C, enviroment E){
/*
    Plano de limpeza para um agente que conhece o ambiente, mas não sabe onde 
    está a sujeira a ser limpada.
*/

/*
    //Contador de lugares
    int k = 0;
    //Para checar direção de movimento
    bool goUp = true;
    bool goDown = false;
    //Passa por todos os lugares do ambiente
    while(k<E.h*E.w){
        //Anda pela altura
        for (int i=0; i<E.h; i++){
            printSimulation(*C,E);
            clean(C);
            //Checa se bateria atingiu limite crítico
            if (C->battery<E.h+E.w){
                int p = C->whereCleaner->row;
                int t = C->whereCleaner->col;
                //Retorna para carregar bateria
                charge(C,E);
                //Retorna para posição anterior
                goTarget(C,E,&E.grid[p][t]);
            }
            //Se subindo, move para cima
            if (goUp)
                if (C->whereCleaner->row<E.h-1)
                    move(
                        C,E,&E.grid[C->whereCleaner->row+1][C->whereCleaner->col]
                    );
            //Se descendo, move para baixo
            if (goDown)
                if (C->whereCleaner->row>0)
                    move(
                        C,E,&E.grid[C->whereCleaner->row-1][C->whereCleaner->col]
                    );
            k++;
        }
        if (C->battery<E.h+E.w){
            int p = C->whereCleaner->row;
            int t = C->whereCleaner->col;
            //Retorna para carregar bateria
            charge(C,E);
            //Retorna para posição anterior
            goTarget(C,E,&E.grid[p][t]);
        }
        move(C,E,&E.grid[C->whereCleaner->row][C->whereCleaner->col+1]);
        //Atualiza sentido de movimento
        goUp = !goUp;
        goDown = !goDown;
    }
*/
    printSimulation(*C,E);
    while(existsDirt(E)){

        place* target = findNearestDirt(C,E);
        if(target==NULL)
            break;

        // dist aspirador -> sujeira mais perto
        int dist =
            abs(C->whereCleaner->row - target->row) +
            abs(C->whereCleaner->col - target->col);

        // dist sujeira -> carregador
        int distCharger =
            abs(target->row - C->whereCharger->row) +
            abs(target->col - C->whereCharger->col);

        if(C->battery <= dist + distCharger + 1){
            if(2 * distCharger + 1 > MAX_BATTERY){
                printf("Bateria insuficiente para completar a tarefa!\n");
                break;
            }
            charge(C,E);
            continue;
        }

        goTarget(C,E,target);
        clean(C);
    }

    // goTarget(C,E,C->whereCharger);
}