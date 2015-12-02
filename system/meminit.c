/* meminit.c - meminit */

#include <xinu.h>

void	*minheap;	/* Start address of heap	*/
void	*maxheap;	/* End address of heap		*/

/*------------------------------------------------------------------------
 * meminit - Initialize the free memory list for BeagleBone Black
 *------------------------------------------------------------------------
 */
void	meminit(void)
{
	struct	memblk *memptr;	/* Memory block pointer	*/

	/* Initialize the minheap and maxheap variables */

	minheap = (void *)&end;
	maxheap = (void *)MAXADDR;

	/* Initialize the memory list as one big block */

	memlist.mnext = (struct memblk *)minheap;
	memptr = memlist.mnext;
    /* jteague6 - configure backwards searching */
    memtail.mprev = memptr;


	memptr->mnext = (struct memblk *)NULL;
	memptr->mlength = (uint32)maxheap - (uint32)minheap;
    /* jteague6 - additional backwards search functionality. */
    memptr->mprev = (struct memblk *)NULL;
}
