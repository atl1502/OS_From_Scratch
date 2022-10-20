#include "terminal.h"
#include "../types.h"
#include "../lib.h"

char terminal_buf [BUF_LEN]; 

/* 
 * terminal_open
 * DESCRIPTION: Terminal open() initializes terminal stuff (or nothing), return 0
 * INPUTS:
 * SIDE EFFECTS:
 * RETURN VALUE: 0
 */
int terminal_open(const uint8_t* filename) {
    return 0;
}

/* 
 * terminal_close
 * DESCRIPTION: Terminal close() clears any terminal specific variables (or do nothing), 
 * INPUTS:
 * SIDE EFFECTS:
 * RETURN VALUE: return 0
 */
int terminal_close(int32_t fd) {
    return 0;
}

/* 
 * terminal_read
 * DESCRIPTION: Terminal read() reads FROM the keyboard buffer into buf
 * INPUTS:
 * SIDE EFFECTS:
 * RETURN VALUE: return number of bytes read
 */
int terminal_read(int32_t fd, void* buf, int32_t nbytes) {
    int bytes_read, i;

    // copy to buf
    if (nbytes > BUF_LEN) {
        // bytes to read is > 128, do i chop it off?
    } else {
        for (i = 0; i < BUF_LEN; i++) {
            if (i < nbytes) 
                ((uint8_t *) buf)[i] = terminal_buf[i];
            else
                ((uint8_t *) buf)[i] = 0;

            // finish reading at \n
            if ('\n' == terminal_buf[i]) {
                return i + 1;
            }
        }
    }
    return -1;
}

/* 
 * terminal_write
 * DESCRIPTION: Terminal write() writes TO the screen from buf
 * INPUTS:
 * SIDE EFFECTS:
 * RETURN VALUE: number of bytes written or -1
 */
int terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        if ( ((uint8_t *) buf)[i] != NULL ) {
            putc( ((uint8_t *) buf)[i] );
        }
    }
    return nbytes;
}
