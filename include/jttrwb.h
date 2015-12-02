/* jteague6 - definition of the reader-writer lock struct. Inspiration came
 * from the Silberschatz book. My method of preventing starvation makes me 
 * concerned about how well this will work. */

#ifndef MAXRWB
#define MAXRWB 10
#endif

struct jttrwb {
    qid16 readq;                    /* The read queue */
    qid16 writeq;                   /* Th write queue */    
    uint32 writing;                 /* Are we writing? */
    uint32 reading;                 /* Are we reading? */
    struct procent *writeproc;      /* WHO is writing? */
};

extern struct jttrwb rwbtab[];

/* Create a blocker */
syscall rwb_create();
/* Enter a blocker's write queue */
syscall rwb_write( uint32, uint32 );
/* Enter a blocker's read queue */
syscall rwb_read( uint32, uint32 );
/* Flip a blocker's mode */
syscall rwb_flip( uint32 );
