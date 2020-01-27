#include "const.h"
#include "listx.h"
#include "stdio.h"
#include "pcb.h"
#include "asl.h"

#define MAX_PCB_PRIORITY     10
#define MIN_PCB_PRIORITY     0
#define DEFAULT_PCB_PRIORITY 5

/*PER COMPILARE: < gcc -I . pcb.c pcb_test.c >*/

pcb_t * procp[MAXPROC], *p, *q, *maxproc, *minproc, *proc;
struct list_head qa;

int main() {
    	int i;

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
}
