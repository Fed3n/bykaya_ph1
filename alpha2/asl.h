#ifndef ASL_H
#define ASL_H

#include "types_bikaya.h"
#include "const.h"
#include "listx.h"

/*
//temporanee qui per i test
semd_t semd_table[MAXPROC+1];
int keys[MAXPROC+1];
*/

/* ASL handling functions */
semd_t* getSemd(int *key);
void initASL();

int insertBlocked(int *key, pcb_t* p);
pcb_t* removeBlocked(int *key);
pcb_t* outBlocked(pcb_t *p);
pcb_t* headBlocked(int *key);
void outChildBlocked(pcb_t *p);


/*
//mia funzione per la stampa
void semd_tTablePrint();
pcb_t* initPcb();

*/
#endif
