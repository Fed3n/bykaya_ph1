#include "types_bikaya.h"
#include "const.h"
#include "pcb.h"

/*Dichiaro un vettore di PCB con dimensione massima 20 (MAXPROC)*/
HIDDEN pcb_t pcbFree_table[MAXPROC];

/*Dichiaro ed inizializzo una nuova lista, dichiarandone la variabile*/
HIDDEN LIST_HEAD(pcbFree_h);

/* Funzione che inserisce il PCB
puntato da p nella lista dei PCB liberi*/
void initPcbs(void) {
	/*Inizializzo la lista come vuota e rendo entrambi i campi che puntano a se*/
	INIT_LIST_HEAD(&pcbFree_h);
	for(int i = 0; i < MAXPROC; i++) {
		/*Inserisco l'indirizzo i di pcbFree_table in coda a pcbFree*/
		list_add_tail(&(pcbFree_table[i].p_next), &(pcbFree_h));
	}
}

/*Inserisce il PCB
puntato da p nella lista dei PCB liberi*/
void freePcb(pcb_t *p){
	list_add_tail(&(p->p_next), &(pcbFree_h));
} 

/*Restituisce NULL se la
pcbFree è vuota. Altrimenti rimuove
un elemento dalla pcbFree, inizializza
tutti i campi (NULL/0) e restituisce
l’elemento rimosso.*/
pcb_t *allocPcb(void) {
	/*Controllo se la lista é vuota, in tal caso non devo fare nulla*/
	if (list_empty(&(pcbFree_h))) return NULL;
	else {
		/*Estraggo il primo PCB nella coda dei processi*/
		pcb_t *temp = container_of(pcbFree_h.next, pcb_t, p_next);

		/*Rimuovo l'indirizzo del PCB rimosso dalla lista che lo contiene*/
		list_del(&(temp->p_next));

		/*Faccio tornare i campi del PCB ai valori di default*/
		INIT_LIST_HEAD(&(temp->p_next));
		temp->p_parent = NULL;
		INIT_LIST_HEAD(&(temp->p_child));
		INIT_LIST_HEAD(&(temp->p_sib));
		//temp->p_s = NULL; /*Non trovo le specifiche per settare gli opportuni campi a0*/
		temp->priority = 0;
		temp->p_semkey = NULL;

		/*Ritorno il puntatore temporaneo per terminare la funzione*/
		return temp;
	}
}

/*Inizializza la lista dei PCB,
inizializzando l’elemento sentinella*/
//Nella specifica di pcb.h la funzione é dichiarata come void, mentre 
//nel file PDF di istruzioni é dichiarata come pcb_t*.
//Per attinenza ai file giá forniteci continueró ad utilizzare la dichiarazione void.
void mkEmptyProcQ(struct list_head *head) {
	INIT_LIST_HEAD(head);
}

/*Restituisce TRUE se la
lista puntata da head è vuota, FALSE
altrimenti.*/
//TRUE E FALSE definiti in const.h
int emptyProcQ(struct list_head *head) {
	if(list_empty(head)) return TRUE;
	else return FALSE;
}

/*inserisce l’elemento puntato da p nella
coda dei processi puntata da head. L’inserimento deve
avvenire tenendo conto della priorita’ di ciascun pcb
(campo p->priority). La coda dei processi deve essere
ordinata in base alla priorita’ dei PCB, in ordine
decrescente (i.e. l’elemento di testa è l’elemento con la
priorita’ più alta).*/
void insertProcQ(struct list_head *head, pcb_t *p) {
	/*Grazie al metodo list_for_each_entry, temp punterá alla struttura che contiene p_next*/
	pcb_t *temp;
	int flag = 0;
	list_for_each_entry(temp, head, p_next) {
		/*Aggiungo il PCB ove la priority sia maggiore del PCB seguente*/
		if (p->priority > temp->priority && flag == 0) { 
			list_add_tail(&(p->p_next), &(temp->p_next));
			flag = 1;
		}
	}
	if (flag == 0) list_add_tail(&(p->p_next), head);
}

/*Restituisce l’elemento di testa della
coda dei processi da head, SENZA RIMUOVERLO.
Ritorna NULL se la coda non ha elementi.*/
pcb_t *headProcQ(struct list_head *head) {
	if(emptyProcQ(head)==TRUE) return NULL;
	else return (container_of(head->next, pcb_t, p_next));
}

/*rimuove il primo elemento dalla
coda dei processi puntata da head. Ritorna
NULL se la coda è vuota. Altrimenti ritorna il
puntatore all’elemento rimosso dalla lista.*/
pcb_t *removeProcQ(struct list_head *head){
	if(emptyProcQ(head)) return NULL;
	else { 
		/*Utilizzo la funzione precedente per estrarre la testa della lista*/
		pcb_t *temp = headProcQ(head);
		list_del(head->next);
		return temp;
	}
}
		
/*Rimuove il PCB puntato da p
dalla coda dei processi puntata da head. Se p
non è presente nella coda, restituisce NULL.*/
pcb_t *outProcQ(struct list_head *head, pcb_t *p) {
	if(emptyProcQ(head)) return NULL;
	else {
		pcb_t *temp;
		//temp punta al contenitore di p_next
		list_for_each_entry(temp, head, p_next){
			if( p == temp) {
				/*termino la funzione solo se p é uguale a temp*/
				list_del(&(p->p_next));
				return p;
			}
		}
	}
}



/* INIZIO PCB TREE */

/*
Chiama list_empty (che prende in input un list_head e ritorna 1 se la lista è vuota) su this->p_child,
che è il list_head della lista dei figli di pcb_t
*/
int emptyChild(pcb_t *this){
	return list_empty(&this->p_child);
}

/*
Prima assegna prnt come genitore di p e poi aggiunge in coda il list_head p->sib al list_head prnt->p_child
*/
void insertChild(pcb_t *prnt, pcb_t *p){
	p->p_parent = prnt;
	list_add_tail(&p->p_sib, &prnt->p_child);
}

/*
Se la lista dei figli di p non è vuota, rimuove p->child.next e lo ritorna
*/
pcb_t *removeChild(pcb_t *p){
	if(emptyChild(p)) return NULL;
	else{
		/*Si memorizza il pcb_t da rimuovere*/
		pcb_t *removed_p = container_of(p->p_child.next, pcb_t, p_sib);
		list_del(p->p_child.next);
		return removed_p;
	}
}

/*
Se p ha un padre, allora rimuove p dalla lista di p->parent->p_child e lo ritorna,
altrimenti ritorna NULL
*/
pcb_t *outChild(pcb_t *p){
	if(p->p_parent == NULL) return NULL;
	else{
		/*Si usa un puntatore al list_head p_sib di p per identificare l'elemento da rimuovere*/
		struct list_head *target = &(p->p_sib);
		struct list_head *iter;
		/*Con un iteratore e il puntatore alla lista di figli del padre di p, si itera sulla lista
		fino a trovare l'elemento cercato per poi cancellarlo*/
		list_for_each(iter, &(p->p_parent->p_child)){
			if(iter == target){
				list_del(target);
				break;
			}
		}
		return p;
	}
}
