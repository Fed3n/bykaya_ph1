/*PER COMPILARE: < gcc -I . pcb.c asl.c test.c >*/

#include "const.h"
#include "listx.h"
#include "stdio.h"
#include "pcb.h"
#include "asl.h"

#define MAX_PCB_PRIORITY     10
#define MIN_PCB_PRIORITY     0
#define DEFAULT_PCB_PRIORITY 5
#define MAXSEM MAXPROC
#define sem keys

pcb_t * procp[MAXPROC], *p, *q, *maxproc, *minproc, *proc;
struct list_head qa;
pcb_t pcbvec[10];

int main() {
    	int i;
	
	printf("TEST PCB QUEUE:\n");

	initPcbs();
	printf("Initialized\n");

	/* Check allocPcb */
    	for (i = 0; i < MAXPROC; i++) {
        	if ((procp[i] = allocPcb()) == NULL)
            		printf("allocPcb(): unexpected NULL   ");
    	}

	/* Return the last 10 entries back to free list */
	for (i = 10; i < MAXPROC; i++) freePcb(procp[i]);

	printf("Added 10 entries to the free PCB list   \n");

	/* Create a 10-element process queue */
	   INIT_LIST_HEAD(&qa);

	    if (!emptyProcQ(&qa))
		printf("ERROR: emptyProcQ(qa): unexpected FALSE   ");

	    printf("Testing insertProcQ ...   \n");

	    for (i = 0; i < 10; i++) { //Va ordinato direttamente nell'inserimento
		if ((q = allocPcb()) == NULL)
		    printf("ERROR: allocPcb(): unexpected NULL while insert   ");
		switch (i) {
		    case 3:
		        q->priority = DEFAULT_PCB_PRIORITY;
		        proc        = q;
		        break;
		    case 4:
		        q->priority = MAX_PCB_PRIORITY;
		        maxproc     = q;
		        break;
		    case 5:
		        q->priority = MIN_PCB_PRIORITY;
		        minproc     = q;
		        break;
		    default:
		        q->priority = DEFAULT_PCB_PRIORITY;
		        break;
		}
		insertProcQ(&qa, q);
	    }

	    printf("Test insertProcQ: OK. Inserted 10 elements \n");

	if (emptyProcQ(&qa))
		printf("ERROR: emptyProcQ(qa): unexpected TRUE");

	/* Check outProcQ and headProcQ */
	    if (headProcQ(&qa) != maxproc)
		printf("ERROR: headProcQ(qa) failed   ");

	    /* Removing an element from ProcQ */
	    q = outProcQ(&qa, proc);
	    if ((q == NULL) || (q != proc))
		printf("ERROR: outProcQ(&qa, proc) failed to remove the entry   ");
	    freePcb(q);

	    /* Removing the first element from ProcQ */
	    q = removeProcQ(&qa);
	    if (q == NULL || q != maxproc)
		printf("ERROR: removeProcQ(&qa, midproc) failed to remove the elements in the right order   ");
	    freePcb(q);

	    /* Removing other 7 elements  */
	    printf(" Testing removeProcQ ...   \n");
	    for (i = 0; i < 7; i++) {
		if ((q = removeProcQ(&qa)) == NULL)
		    printf("removeProcQ(&qa): unexpected NULL   ");
		freePcb(q);
	    }

	// Removing the last element
	    q = removeProcQ(&qa);
	    if (q != minproc)
		printf("ERROR: removeProcQ(): failed on last entry   ");
	    freePcb(q);

	    if (removeProcQ(&qa) != NULL)
		printf("ERROR: removeProcQ(&qa): removes too many entries   ");

	    if (!emptyProcQ(&qa))
		printf("ERROR: emptyProcQ(qa): unexpected FALSE   ");

	    printf(" Test insertProcQ(), removeProcQ() and emptyProcQ(): OK   \n");
	    printf(" Test process queues module: OK      \n");

	printf("\n TEST PCB TREE:\n");

	for(i = 0; i < 10; i++){
		pcbvec[i].p_parent = NULL;
		INIT_LIST_HEAD(&(pcbvec[i].p_child));
		INIT_LIST_HEAD(&(pcbvec[i].p_sib));
		pcbvec[i].priority = i;
	}
	printf("Array initialization successful\n");

	insertChild(&pcbvec[0], &pcbvec[1]);
	insertChild(&pcbvec[0], &pcbvec[2]);
	insertChild(&pcbvec[0], &pcbvec[3]);
	insertChild(&pcbvec[1], &pcbvec[4]);
	insertChild(&pcbvec[1], &pcbvec[5]);
	insertChild(&pcbvec[2], &pcbvec[6]);
	insertChild(&pcbvec[2], &pcbvec[7]);
	insertChild(&pcbvec[3], &pcbvec[8]);
	insertChild(&pcbvec[8], &pcbvec[9]);

	pcb_t *p = container_of(pcbvec[0].p_child.next, pcb_t, p_sib);
	printf("0's first child is %d\n", p->priority);

	p = container_of(pcbvec[2].p_child.next, pcb_t, p_sib);
	printf("2's first child is %d\n", p->priority);

	p = container_of(pcbvec[2].p_child.next->next, pcb_t, p_sib);
	printf("2's second child is %d\n", p->priority);

	p = container_of(pcbvec[8].p_child.next, pcb_t, p_sib);
	printf("8's first child is %d\n", p->priority);

	/*Expected 1*/
	printf("Does 8 have a child: %d\n", !emptyChild(&pcbvec[8]));
	removeChild(&pcbvec[8]);
	/*Expected 0*/
	printf("What about now: %d\n", !emptyChild(&pcbvec[8]));

	/*Expected 1*/
	printf("Does 3 have a child: %d\n", !emptyChild(&pcbvec[3]));
	p = outChild(&pcbvec[8]);
	/*Expected 0*/
	printf("What about now: %d\n", !emptyChild(&pcbvec[8]));
	/*Expected 1*/
	printf("Is this 8: %d\n", p == &pcbvec[8]);

	/*ASL TEST*/
	printf("\nTEST ASL: \n");

	/*riempio il vettore di pcb*/
	for (int j = 0; j < MAXPROC ; j++)
	{
		procp[j] = initPcb();
		printf("pcb[%d]: %d \n",j,procp[j]);
	}	

	/* check ASL */
	initASL();
	printf("Initializing active semaphore list   \n");

	semd_tTablePrint();	

	/* check removeBlocked and insertBlocked */
	printf(" Test insertBlocked(): test #1 started  \n");
	for (i = 10; i < MAXPROC; i++) {
	procp[i] = initPcb();
	if (insertBlocked(&sem[i], procp[i]))
	    printf("ERROR: insertBlocked() test#1: unexpected TRUE   ");
	}

	printf("Test insertBlocked(): test #2 started  \n");
	for (i = 0; i < 10; i++) 
	{
		if (insertBlocked(&sem[i], procp[i]))
		    printf("ERROR: insertBlocked() test #2: unexpected TRUE   ");
	}

	/* check if semaphore descriptors are returned to the free list */
	p = removeBlocked(&sem[11]);
	printf(" semd[11]: %d \n",sem[11]);

	//semd_tTablePrint();

	if (insertBlocked(&sem[11], p))
	printf("ERROR: removeBlocked(): fails to return to free list   ");

	if (insertBlocked(&sem[MAXSEM], procp[9]) == FALSE)
	printf("ERROR: insertBlocked(): inserted more than MAXPROC   ");

	printf(" semd[MAXSEM]: %d \n",sem[MAXSEM]);

	//semd_tTablePrint();

	printf("Test removeBlocked(): test started   \n");
	for (i = 10; i < MAXPROC; i++) {
		q = removeBlocked(&sem[i]);
		if (q == NULL)
		    printf("ERROR: removeBlocked(): wouldn't remove   ");
		if (q != procp[i])
		    printf("ERROR: removed wrong element. %d instead of %d\n",q,procp[i]);
	}

	semd_tTablePrint();

	if (removeBlocked(&sem[11]) != NULL)
	printf("ERROR: removeBlocked(): removed nonexistent blocked proc   ");

	printf("Test insertBlocked() and removeBlocked() ok   \n");

	if (headBlocked(&sem[11]) != NULL)
	printf("ERROR: headBlocked(): nonNULL for a nonexistent queue   ");

	if ((q = headBlocked(&sem[9])) == NULL)
	printf("ERROR: headBlocked(1): NULL for an existent queue   ");
	if (q != procp[9])
	printf("ERROR: headBlocked(1): wrong process returned  \n");
	p = outBlocked(q);
	if (p != q)
	printf("ERROR: outBlocked(1): couldn't remove from valid queue  \n");
	printf("\n");

	printf("Test headBlocked() and outBlocked(): OK   \n");

	printf("ASL module OK   \n");
	printf("So Long and Thanks for All the Fish\n");
}
