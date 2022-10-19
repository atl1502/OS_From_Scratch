#include "terminal.h"
#include "../types.h"
#include "../lib.h"


/* 
 * terminal_open
 * DESCRIPTION: Terminal open() initializes terminal stuff (or nothing), return 0
 * INPUTS:
 * SIDE EFFECTS:
 * RETURN VALUE: 0
 */
int terminal_open() {
    return 0;
}

/* 
 * terminal_close
 * DESCRIPTION: Terminal close() clears any terminal specific variables (or do nothing), 
 * INPUTS:
 * SIDE EFFECTS:
 * RETURN VALUE: return 0
 */
int terminal_close() {
    return 0;
}

/* 
 * terminal_read
 * DESCRIPTION: Terminal read() reads FROM the keyboard buffer into buf
 * INPUTS:
 * SIDE EFFECTS:
 * RETURN VALUE: return number of bytes read
 */
int terminal_read() {
    int bytes_read;
    return bytes_read;
}

/* 
 * terminal_write
 * DESCRIPTION: Terminal write() writes TO the screen from buf
 * INPUTS:
 * SIDE EFFECTS:
 * RETURN VALUE: number of bytes written or -1
 */
int terminal_write() {
    int bytes_written;
    return bytes_written;
}
