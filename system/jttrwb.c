/* jteague6 - definition of the reader-writer lock struct. Inspiration came
 * from the Silberschatz book. My method of preventing starvation makes me 
 * concerned about how well this will work. */

#include <xinu.h>

//struct jttrwb {
//    struct sentry readsem;          /* Read semaphore/queue */
//    struct sentry writesem;         /* Write semaphore/queue */
//    uint32 writing;                 /* Are we writing or reading? */
//    struct procent *writeproc;      /* WHO is writing? */
//};

/* The table is just an array with MAXRWB elements (see header) */
struct jttrwb rwbtab[MAXRWB];

/* Create a blocker */
syscall rwb_create() {
    static uint32 id = 0;   /* ID that will be returned */
    intmask mask;           /* Interrupt mask */

    mask = disable();
    if( id > MAXRWB ) {
        restore( mask );
        return SYSERR;
    }

    /* Set up the queues and blocking variables */
    rwbtab[id].writing = 0;
    rwbtab[id].reading = 0;
    rwbtab[id].writeproc = NULL;
    
    restore(mask);
    return id++;
}
    
/* Enter a blocker's write queue */
syscall rwb_write( uint32 rwb, uint32 proc ) {
    intmask mask;
    /* Process pointer */
    struct procent *prptr;

    mask = disable();
    /* If we're reading or writing, the current process must queue and block */
    if( rwbtab[rwb].reading == 1 || rwbtab[rwb].writing == 1 ) {
        prptr = &proctab[currpid];
        prptr->prstate = PR_WAIT;
        enqueue(currpid, rwbtab[rwb].writeq );
        resched();
    }

    rwbtab[rwb].writing = 1;
    restore(mask);

    return OK;
}

/* Enter a blocker's read queue */
syscall rwb_read( uint32 rwb, uint32 proc) {
    intmask mask;
    struct procent *prptr;

    mask = disable();
    /* In the case of a read request, we only block if there's a write in
     * progress */
    if( rwbtab[rwb].writing == 1 ) {
        prptr = &proctab[currpid];
        prptr->prstate = PR_WAIT;
        enqueue( currpid, rwbtab[rwb].readq );
        resched();
    }

    rwbtab[rwb].reading = 1;
    restore( mask );
    return OK;
}

/* Flip a blocker's mode */
syscall rwb_flip( uint32 rwb ) {
    struct procent *prptr;
    intmask mask;

    mask = disable();
    if( rwbtab[rwb].writing == 1 ) {
        if( rwbtab[rwb].writeproc != NULL ) {
            prptr = rwbtab[rwb].writeproc;
            prptr->prstate = PR_WAIT;
            enqueue( prptr, rwbtab[rwb].writeq );
        }
        rwbtab[rwb].writing = 0;
        rwbtab[rwb].reading = 1;

        /* Wake up read waiters */
    }
    else if( rwbtab[rwb].reading == 1 ) {
        rwbtab[rwb].reading = 0;
        rwbtab[rwb].writing = 1;

        /* If the write queue isn't empty, activate the first process */
    }

    restore( mask );
    return OK;
}

