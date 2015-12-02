/* getstk.c - getstk */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getstk  -  Allocate stack memory, returning highest word address
 *------------------------------------------------------------------------
 */
char  	*getstk(
	  uint32	nbytes		/* Size of memory requested	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	memblk	*prev, *curr;	/* Walk through memory list	*/
	struct	memblk	*fits, *fitsprev; /* Record block that fits	*/
    uint32 read = 0;        /* jteague6 - for best-fit finding */

	mask = disable();
	if (nbytes == 0) {
		restore(mask);
		return (char *)SYSERR;
	}

	nbytes = (uint32) roundmb(nbytes);	/* Use mblock multiples	*/

    /* jteague6 - start searching from the back of the list */
	prev = &memtail;
	curr = memtail.mprev;
	fits = NULL;
	fitsprev = NULL;  /* Just to avoid a compiler warning */

	while (curr != NULL) {			/* Scan entire list	*/
		/* jteague6 - for best fit, we want to not only make sure the block
         * fitst, but we want to also make sure it is the first fitting block
         * that has been found, or that it is smaller than the current
         * fitting block. */
        if (curr->mlength >= nbytes &&
                ( read == 0 ||
                  curr->mlength < fits->mlength )) {	/* Record block address	*/
			fits = curr;		/*   when request fits	*/
			fitsprev = prev;
            read = 1;           /* To prevent erroneous positives */
		}
		prev = curr;
		curr = curr->mprev;
	}

	if (fits == NULL) {			/* No block was found	*/
		restore(mask);
		return (char *)SYSERR;
	}
	if (nbytes == fits->mlength) {		/* Block is exact match	*/
        /* jteague6 - additional code to support doubly-linked list and
         * searching from the back of the list. */
        if( fits->mnext == NULL ) {
            prev->mnext = NULL;
            memtail.mprev = prev;
        }
        else {
            fits->mnext->mprev = fits->mprev;
        }
		fitsprev->mnext = fits->mnext;
	} else {				/* Remove top section	*/
		fits->mlength -= nbytes;
		fits = (struct memblk *)((uint32)fits + fits->mlength);
	}
	memlist.mlength -= nbytes;
	restore(mask);
	return (char *)((uint32) fits + nbytes -  sizeof(uint32));
}
