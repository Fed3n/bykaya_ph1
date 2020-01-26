#include <pcb_tree.h>

/*
Calls list_empty (which takes a list_head as input and returns 1 if list is empty) on this->p_child,
which is a list_head of pcb_t's children list
*/
int emptyChild(pcb_t *this){
	return list_empty(&this->p_child);
}

/*
Assigns prnt as p's parent and then adds p in tail of prnt->p_child list_head
*/
void insertChild(pcb_t *prnt, pcb_t *p){
	p->p_parent = prnt;
	list_add_tail(&p->p_sib, &prnt->p_child);
}

/*
If p->p_child list is not empty, removes its first element and returns it
*/
pcb_t *removeChild(pcb_t *p){
	if(emptyChild(p)) return NULL;
	else{
		pcb_t *removed_p;
		removed_p = container_of(p->p_child.next, pcb_t, p_child);
		list_del(p->p_child.next);
		return removed_p;
	}
}

pcb_t *outChild(pcb_t *p){
	if(p->p_parent == NULL) return NULL;
	else{
		struct list_head target = p->p_sib;
		struct list_head *iter;
		list_for_each(iter, &(p->p_parent->p_child)){
			if(iter == &target){
				list_del(&target);
				break;
			}
		}
		return p;
	}
}