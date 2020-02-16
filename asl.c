#include "asl.h"
#include "pcb.h"

/* Global Variables*/
HIDDEN semd_t semd_table[MAXPROC]; 
HIDDEN LIST_HEAD(semdFree_h);
HIDDEN LIST_HEAD(semd_h);

HIDDEN int init = 0;  //variabile a scopo di inizializzazione degli indirizzi dei semafori

/* funzione di inizializzazione
   si occupa di inizializzare la semd_table e le liste di semafori liberi ed attivi
*/

void initASL()
{
	for (int index = 0; index < MAXPROC+1; index++)
	{
		semd_t semaphore;
		INIT_LIST_HEAD(&semaphore.s_next);
		semaphore.s_key = &init; //per inizializzare ho scelto l'indirizzo di una variabile intera
		INIT_LIST_HEAD(&semaphore.s_procQ);
		semd_table[index] = semaphore;
	}
	INIT_LIST_HEAD(&semdFree_h);
	for (int index = 0; index < MAXPROC; index++)
	{
		list_add_tail(&(semd_table[index].s_next), &semdFree_h);
	}
	INIT_LIST_HEAD(&semd_h);	
}

/* restituisce il puntatore a semd presente nella ASL la cui chiave è key
   NULL se non è presente
*/

semd_t* getSemd(int *key)
{
	semd_t* semd = NULL;
	semd_t* pos = NULL;
	list_for_each_entry(pos,&semd_h,s_next)
		if (pos->s_key == key)
			semd = pos;
	return semd;		
}

/* aggiunge un PCB alla lista dei processi bloccati sul semaforo

	key: chiave del semaforo
	p: puntatore al PBC	

	return: TRUE se il SEMD richiesto non è disponibile in quanto sono terminati i semafori disponibili
*/	

int insertBlocked(int *key, pcb_t *p)
{
	semd_t *semd =  getSemd(key);
	if (semd == NULL) 
		if (list_empty(&semdFree_h))
			return 1;
		else
		{
		   	semd = container_of(semdFree_h.next,semd_t,s_next); 
			list_del(semdFree_h.next);
			semd->s_key = key;			
			INIT_LIST_HEAD(&semd->s_procQ);
			list_add_tail(semd,&semd_h);
		}	
	list_add(&p->p_next,&semd->s_procQ);
	p->p_semkey = key;
	return 0;	
}

/* rimuove il primo processo bloccato sulla coda dei processi del semaforo relativo

	key: chiave del semaforo
	
	return: puntatore al PCB che è stato rimosso; ritorna NULL se il semaforo indicato non è presente nella ASL

	ATTENZIONE: la funzione non aggiorna il campo p_semkey di PCB perchè non so come va inizializzato e non è 
	richiesto dalla specifica
*/

pcb_t* removeBlocked(int *key)
{
	semd_t* semd = getSemd(key);
	pcb_t* pcb = NULL;
	if (semd != NULL)	
	{	
		//semd->s_procQ.next è il puntatore al campo list_head del primo processo in coda
		//per ottenere il puntatore alla struttura uso container_of
		pcb = container_of(semd->s_procQ.next,pcb_t,p_next);
		list_del(semd->s_procQ.next);
		if (list_empty(&semd->s_procQ)) 
		{
			list_del(&semd->s_next);
			list_add(&semd->s_next,&semdFree_h);
		}
	}
	return pcb;
}

/* rimuove il processo puntato da p dalla coda dei processi del semaforo su cui è bloccato

	p: puntatore al processo da rimuovere

	return: il puntatore al processo rimosso; NULL se il processo non è stato trovato nella coda dei processi bloccati
		sul semaforo indicato nel campo p->semkey
*/

pcb_t* outBlocked(pcb_t *p)
{
	pcb_t *pcb = NULL;
	if (p != NULL)
	{
		semd_t *semd = getSemd(p->p_semkey);
		if (semd != NULL)	
		{
			struct list_head *pos;
			list_for_each(pos, &semd->s_procQ)
				if (container_of(pos, pcb_t, p_next) == p) 
				{
					pcb = p;					
					break;
				}
			if (pcb != NULL)
			{
				__list_del(p->p_next.prev,p->p_next.next);
				if (list_empty(&semd->s_procQ)) 
				{
					list_del(&semd->s_next);
					list_add(&semd->s_next,&semdFree_h);
				}
			}
		}
	}
	return pcb;
}

/* restutuisce il puntatore al PCB in testa alla coda dei processi del semaforo identificato da key senza rimuovere il processo

	key: chiave del semaforo
	
	return: processo in testa alla coda; NULL se il semaforo non è nella ASL e quindi non ha processi in coda

*/

pcb_t* headBlocked(int *key)
{
	semd_t *semd = getSemd(key);
	pcb_t* pcb = NULL;
	if (semd != NULL && !(list_empty(&semd->s_procQ)))	
		pcb = container_of(semd->s_procQ.next,pcb_t,p_next);  //stessa operazione effettuata nella funzione removeblocked
	return pcb;
}

/* rimuove il processo puntato da p dalla coda dei processi del semaforo su cui è bloccato e fa lo stesso su tutti 
   i processi dell'albero radicato in p

	p: puntatore al processo da rimuovere
*/


void outChildBlocked(pcb_t *p)
{
	if (p != NULL)
	{
		pcb_t *pcb = outBlocked(p);
		pcb_t *pcb_son = NULL;
		do {
			pcb_son = removeChild(pcb);
			outChildBlocked(pcb_son);
		} while (pcb_son != NULL);
	}
}


/*test*/

/*
//stampa; versione per i terminale
void semd_tTablePrint()
{
	//vettore dei SEMD
	printf("vettore SEMD \n");
	for (int index = 0; index < MAXPROC; index++)
	{
		printf("semd[%d]. %d \n",index,*(semd_table[index].s_key));	
	}
	//lista semdFree
	printf("lista semdFree \n");
	struct list_head* pos;
    	list_for_each(pos,&semdFree_h)
	{
		semd_t* sem = container_of(pos,semd_t,s_next);
		printf("chiave n. %d \n",*(sem->s_key));
	}
	//lista ASL
	printf("lista ASL \n");
	pos = NULL;
    	list_for_each(pos,&semd_h) 
	{
		semd_t* sem = container_of(pos,semd_t,s_next);
		printf("chiave n. %d \n",*(sem->s_key));
		struct list_head* block;
		list_for_each(block,&sem->s_procQ)
			printf("	%d \n",container_of(block,pcb_t,p_next));
	}
}

//stampa; versione per i test sulle macchine
void tablePrint()
{
	//vettore dei SEMD
	addokbuf(" \n");
	addokbuf("vettore SEMD \n");
	for (int index = 0; index < MAXPROC; index++)
	{
		addokbuf("semd[");
		intprint(index); 		
		addokbuf("]. ");
		intprint(semd_table[index].s_key);
		addokbuf(" \n");	
	}
	//lista semdFree
	addokbuf("lista semdFree \n");
	struct list_head* pos;
	int count = 0;
    	list_for_each(pos,&semdFree_h)
	{
		semd_t* sem = container_of(pos,semd_t,s_next);
		intprint(count);
		count++;
		addokbuf("- ");
		intprint(sem->s_key);
		addokbuf("\n");
	}
	//lista ASL
	addokbuf("lista ASL \n");
	pos = NULL;
    	list_for_each(pos,&semd_h) 
	{
		semd_t* sem = container_of(pos,semd_t,s_next);
		addokbuf("chiave n. ");
		intprint(sem->s_key);
		addokbuf("\n");
		struct list_head* block;
		list_for_each(block,&sem->s_procQ){
			addokbuf("	");
			intprint(container_of(block,pcb_t,p_next));
			addokbuf("\n");
		}
	}
	addokbuf(" \n");
}
*/

