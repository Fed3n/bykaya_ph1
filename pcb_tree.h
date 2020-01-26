#ifndef PCB_H
#define PCB_H
#include <types_bikaya.h>

/* PCB handling functions */

/* Tree view functions */
int emptyChild(pcb_t *this);
void insertChild(pcb_t *prnt, pcb_t *p);
pcb_t *removeChild(pcb_t *p);
pcb_t *outChild(pcb_t *p);

#endif
