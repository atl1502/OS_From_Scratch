#include "terminal.h"
#include "../types.h"
#include "../lib.h"
#include "keyboard.h"

/*
 * terminal_open
 * DESCRIPTION: Terminal open() initializes terminal stuff (or nothing), return 0
 * INPUTS: filename: Ignored
 * SIDE EFFECTS: Wipes screen and changes how keyboard acts
 * RETURN VALUE: 0
 */
int terminal_open(const uint8_t* filename) {
    program_clear();
    set_terminal_mode(1);
    return 0;
}

/*
 * terminal_close
 * DESCRIPTION: Terminal close() clears any terminal specific variables (or do nothing),
 * INPUTS: fd: Ignored
 * SIDE EFFECTS: Loads screen with previous data from memory
 * RETURN VALUE: 0
 */
int terminal_close(int32_t fd) {
    set_terminal_mode(0);
    program_reload();
    return 0;
}

/*
 * terminal_read
 * DESCRIPTION: Terminal read() reads FROM the keyboard buffer into buf
 * INPUTS: fd : file    buf: buffer to be read    nbytes: number of bytes if buf
 * SIDE EFFECTS:
 * RETURN VALUE: return number of bytes read
 */
int terminal_read(int32_t fd, void* buf, int32_t nbytes) {
    // hold until there is a new line char
    int len;
    while(1){
        len = get_keyboard_buffer_length();
        get_keyboard_buffer((char *)buf);
        if(len > 0 && ((char *)buf)[len-1] == '\n')
            break;
    }

    return nbytes;
}

/*
 * terminal_write
 * DESCRIPTION: Terminal write() writes TO the screen from buf
 * INPUTS: fd : file    buf: buffer to be writen    nbytes: number of bytes of buf
 * SIDE EFFECTS: prints the buf to screen
 * RETURN VALUE: number of bytes written or -1
 */
int terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
    int i;
    int bytes = 0;
    if (buf == NULL){
        return -1;
    }
    for (i = 0; i < nbytes; i++) {
        if ( ((char *) buf)[i] != NULL ) {
            putc( ((char *) buf)[i] );
            bytes++;
        }
    }
    return bytes;
}
