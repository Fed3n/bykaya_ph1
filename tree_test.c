#include<stdio.h>

#include <const.h>
#include <listx.h>
#include <pcb_tree.h>
#include <asl.h>

pcb_t pcbvec[10];

int main(){
	int i;
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
}