//#include <stdio.h>
//#include "asl.h"

//#include "listx.h"

#include "const.h"
#include "listx.h"

#ifdef TARGET_UMPS
#include "umps/libumps.h"
#include "umps/arch.h"
#endif
#ifdef TARGET_UARM
#include "uarm/libuarm.h"
#include "uarm/arch.h"
#endif

#include "pcb.h"
#include "asl.h"

#define MAXSEM MAXPROC
#define MAX_PCB_PRIORITY     10
#define MIN_PCB_PRIORITY     0
#define DEFAULT_PCB_PRIORITY 5

char okbuf[2048]; /* sequence of progress messages */
char errbuf[128]; /* contains reason for failing */
char msgbuf[128]; /* nonrecoverable error message before shut down */



int     onesem;
pcb_t  *procp[MAXPROC], *p, *q, *maxproc, *minproc, *proc;
semd_t *semd[MAXSEM];
#define sem keys //int     sem[MAXSEM + 1];

struct list_head qa;
char *           mp = okbuf;

#define ST_READY       1
#define ST_BUSY        3
#define ST_TRANSMITTED 5

#define CMD_ACK      1
#define CMD_TRANSMIT 2

#define CHAR_OFFSET      8
#define TERM_STATUS_MASK 0xFF

int main(){

	/*riempo il vettore di pcb*/
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
	int i;
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

	/* Creating a 2-layer tree */
/*	insertChild(procp[0], procp[1]);
	insertChild(procp[0], procp[2]);
	insertChild(procp[0], procp[3]);
	insertChild(procp[3], procp[4]);
*/
	/* Testing outChildBlocked */
/*	outChildBlocked(procp[0]);

	if (headBlocked(&sem[0]) != NULL)
	printf("ERROR: outChildBlocked(): nonNULL for a nonexistent queue (0)  ");
	if (headBlocked(&sem[1]) != NULL)
	printf("ERROR: outChildBlocked(): nonNULL for a nonexistent queue (1)  ");
	if (headBlocked(&sem[2]) != NULL)
	printf("ERROR: outChildBlocked(): nonNULL for a nonexistent queue  (2) ");
	if (headBlocked(&sem[3]) != NULL)
	printf("ERROR: outChildBlocked(): nonNULL for a nonexistent queue (3)  ");
	if (headBlocked(&sem[4]) != NULL)
	printf("ERROR: outChildBlocked(): nonNULL for a nonexistent queue (4)  ");
	if (headBlocked(&sem[5]) == NULL)
	printf("ERROR: outChildBlocked(): NULL for an existent queue  (5) ");
*/
	printf("Test headBlocked() and outBlocked(): OK   \n");

	printf("ASL module OK   \n");
	printf("So Long and Thanks for All the Fish\n");

	/*
	initASL();
	semd_tTablePrint();
	printf("insertBlocked res: %d \n",insertBlocked(&(semd_table[2].s_key),initPcb()));
	printf("insertBlocked res: %d \n",insertBlocked(&(semd_table[3].s_key),initPcb()));
	printf("insertBlocked res: %d \n",insertBlocked(&(semd_table[4].s_key),initPcb()));
	printf("insertBlocked res: %d \n",insertBlocked(&(semd_table[15].s_key),initPcb()));
	printf("insertBlocked res: %d \n",insertBlocked(&(semd_table[3].s_key),initPcb()));
	pcb_t *p0 = initPcb();
	printf("insertBlocked res: %d \n",insertBlocked(&(semd_table[6].s_key),p0));
	printf("insertBlocked res: %d \n",insertBlocked(&(semd_table[3].s_key),initPcb()));
	pcb_t *p1 = initPcb();
	printf("insertBlocked res: %d \n",insertBlocked(&(semd_table[3].s_key),p1));
	pcb_t *p2 = initPcb();
	printf("insertBlocked res: %d \n",insertBlocked(&(semd_table[15].s_key),p2));
	printf("insertBlocked res: %d \n",insertBlocked(&(semd_table[4].s_key),initPcb()));
	printf("insertBlocked res: %d \n",insertBlocked(&(semd_table[15].s_key),initPcb()));
	printf("insertBlocked res: %d \n",insertBlocked(&(semd_table[19].s_key),initPcb()));
	printf("insertBlocked res: %d \n",insertBlocked(&(semd_table[3].s_key),initPcb()));
	pcb_t *p3 = initPcb();
	printf("insertBlocked res: %d \n",insertBlocked(&(semd_table[23].s_key),p3));
	semd_tTablePrint();
	printf("removed proc address: %d \n",removeBlocked(&(semd_table[3].s_key)));
	printf("removed proc address: %d \n",removeBlocked(&(semd_table[15].s_key)));
	printf("removed proc address: %d \n",removeBlocked(&(semd_table[3].s_key)));
	printf("removed proc address: %d \n",removeBlocked(&(semd_table[19].s_key)));
	printf("removed proc address: %d \n",removeBlocked(&(semd_table[19].s_key)));
	printf("rem proc: %d \n",outBlocked(p0));
	printf("rem proc: %d \n",outBlocked(p1));
	printf("rem proc: %d \n",outBlocked(p2));
	printf("rem proc: %d \n",outBlocked(p3));
	printf("rem proc: %d \n",outBlocked(p3));
	semd_tTablePrint();
	*/
}
