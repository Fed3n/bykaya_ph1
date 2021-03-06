/*****************************************************************************
 * Copyright 2004, 2005 Michael Goldweber, Davide Brini.                     *
 * Modified 2012 by Marco Di Felice                                          *
 * Modified 2018 by Renzo Davoli
 *                                                                           *
 * This file is part of BIKAYA.                                              *
 *                                                                           *
 * bikaya is free software;you can redistribute it and/or modify it under the*
 * terms of the GNU General Public License as published by the Free Software *
 * Foundation; either version 2 of the License, or (at your option) any      *
 * later version.                                                            *
 * This program is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General *
 * Public License for more details.                                          *
 * You should have received a copy of the GNU General Public License along   *
 * with this program; if not, write to the Free Software Foundation, Inc.,   *
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.                  *
 *****************************************************************************/

/*********************************P1TEST.C*******************************
 *
 *	Test program for the modules ASL and PCB (phase 1).
 *
 *	Produces progress messages on terminal 0 in addition
 *		to the array ``okbuf[]''
 *		Error messages will also appear on terminal 0 in
 *		addition to the array ``errbuf[]''.
 *
 *		Aborts as soon as an error is detected.
 *
 *
 */

#include "stdio.h"
#include "const.h"
#include "listx.h"
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
pcb_t * procp[MAXPROC], *p, *q, *maxproc, *minproc, *proc;
semd_t *semd[MAXSEM];
int     sem[MAXSEM + 1];

struct list_head qa;
char *           mp = okbuf;


/******************************************************************************
 * Main Test File
 ******************************************************************************/


int main() {
    int i;

    initPcbs();
    printf("Initialized Process Control Blocks   \n");

    /* Check allocPcb */
    for (i = 0; i < MAXPROC; i++) {
        if ((procp[i] = allocPcb()) == NULL)
            printf("allocPcb(): unexpected NULL   ");
    }

    if (allocPcb() != NULL) {
        printf(" ERROR: allocPcb(): allocated more than MAXPROC entries   ");
    }
    printf(" allocPcb test OK   \n");


    /* Return the last 10 entries back to free list */
    for (i = 10; i < MAXPROC; i++)
        freePcb(procp[i]);

    printf(" Added 10 entries to the free PCB list   \n");

    /* Create a 10-element process queue */
    INIT_LIST_HEAD(&qa);

    if (!emptyProcQ(&qa))
        printf("ERROR: emptyProcQ(qa): unexpected FALSE   ");

    printf("Testing insertProcQ ...   \n");

    for (i = 0; i < 10; i++) { //Va ordinato direttamente nell'inserimento
			       //La sentinella é quella di algoritmi, QA é la sentinella
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

    printf(" Testing process trees...\n");

    if (!emptyChild(procp[2]))
        printf("ERROR: emptyChild: unexpected FALSE   ");

    /* make procp[1],procp[2],procp[3], procp[7] children of procp[0] */
    printf("Inserting...   \n");
    insertChild(procp[0], procp[1]);
    insertChild(procp[0], procp[2]);
    insertChild(procp[0], procp[3]);
    insertChild(procp[0], procp[7]);
    printf("Inserted 2 children of pcb0  \n");

    /* make procp[8],procp[9] children of procp[7] */
    insertChild(procp[7], procp[8]);
    insertChild(procp[7], procp[9]);
    printf("Inserted 2 children of pcb7  \n");

    if (emptyChild(procp[0]))
        printf("ERROR: emptyChild(procp[0]): unexpected TRUE   ");

    if (emptyChild(procp[7]))
        printf("ERROR: emptyChild(procp[0]): unexpected TRUE   ");

    /* Check outChild */
    q = outChild(procp[1]);
    if (q == NULL || q != procp[1])
        printf("ERROR: outChild(procp[1]) failed ");

    q = outChild(procp[8]);
    if (q == NULL || q != procp[8])
        printf("ERROR: outChild(procp[8]) failed ");

    /* Check removeChild */
    q = removeChild(procp[0]);
    if (q == NULL || q != procp[2])
        printf("ERROR: removeChild(procp[0]) failed ");

    q = removeChild(procp[7]);
    if (q == NULL || q != procp[9])
        printf("ERROR: removeChild(procp[7]) failed ");

    q = removeChild(procp[0]);
    if (q == NULL || q != procp[3])
        printf("ERROR: removeChild(procp[0]) failed ");

    q = removeChild(procp[0]);
    if (q == NULL || q != procp[7])
        printf("ERROR: removeChild(procp[0]) failed ");


    if (removeChild(procp[0]) != NULL)
        printf("ERROR: removeChild(): removes too many children   ");

    if (!emptyChild(procp[0]))
        printf("ERROR: emptyChild(procp[0]): unexpected FALSE   ");

    printf("Test: insertChild(), removeChild() and emptyChild() OK   \n");
    printf("Testing process tree module OK      \n");


    freePcb(procp[0]);
    freePcb(procp[1]);
    freePcb(procp[2]);
    freePcb(procp[3]);
    freePcb(procp[4]);
    freePcb(procp[5]);
    freePcb(procp[6]);
    freePcb(procp[7]);
    freePcb(procp[8]);
    freePcb(procp[9]);


    /* check ASL */
    initASL();
    printf("Initializing active semaphore list   \n");

    /* check removeBlocked and insertBlocked */
    printf(" Test insertBlocked(): test #1 started  \n");
    for (i = 10; i < MAXPROC; i++) {
        procp[i] = allocPcb();
        if (insertBlocked(&sem[i], procp[i]))
            printf("ERROR: insertBlocked() test#1: unexpected TRUE   ");
    }

    printf("Test insertBlocked(): test #2 started  \n");
    for (i = 0; i < 10; i++) {
        procp[i] = allocPcb();
        if (insertBlocked(&sem[i], procp[i]))
            printf("ERROR:insertBlocked() test #2: unexpected TRUE   ");
    }

    /* check if semaphore descriptors are returned to the free list */
    p = removeBlocked(&sem[11]);
    if (insertBlocked(&sem[11], p))
        printf("ERROR: removeBlocked(): fails to return to free list   ");

    if (insertBlocked(&sem[MAXSEM], procp[9]) == FALSE)
        printf("ERROR: insertBlocked(): inserted more than MAXPROC   ");

    printf("Test removeBlocked(): test started   \n");
    for (i = 10; i < MAXPROC; i++) {
        q = removeBlocked(&sem[i]);
        if (q == NULL)
            printf("ERROR: removeBlocked(): wouldn't remove   ");
        if (q != procp[i])
            printf("ERROR: removeBlocked(): removed wrong element   ");
    }

    if (removeBlocked(&sem[11]) != NULL)
        printf("ERROR: removeBlocked(): removed nonexistent blocked proc   ");

    printf("Test insertBlocked() and removeBlocked() ok   \n");

    if (headBlocked(&sem[11]) != NULL)
        printf("ERROR: headBlocked(): nonNULL for a nonexistent queue   ");

    if ((q = headBlocked(&sem[9])) == NULL)
        printf("ERROR: headBlocked(1): NULL for an existent queue   ");
    if (q != procp[9])
        printf("ERROR: headBlocked(1): wrong process returned   ");
    p = outBlocked(q);
    if (p != q)
        printf("ERROR: outBlocked(1): couldn't remove from valid queue   ");

    /* Creating a 2-layer tree */
    insertChild(procp[0], procp[1]);
    insertChild(procp[0], procp[2]);
    insertChild(procp[0], procp[3]);
    insertChild(procp[3], procp[4]);

    /* Testing outChildBlocked */
    outChildBlocked(procp[0]);

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

    printf("Test headBlocked() and outBlocked(): OK   \n");

    printf("ASL module OK   \n");
    printf("So Long and Thanks for All the Fish\n");

    return 0;
}
