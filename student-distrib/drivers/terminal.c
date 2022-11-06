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
int32_t terminal_open(const uint8_t* filename) {
    return 0;
}

/*
 * terminal_close
 * DESCRIPTION: Terminal close() clears any terminal specific variables (or do nothing),
 * INPUTS: fd: Ignored
 * SIDE EFFECTS: Loads screen with previous data from memory
 * RETURN VALUE: 0
 */
int32_t terminal_close(uint32_t fd) {
    return -1;
}

/*
 * terminal_read
 * DESCRIPTION: Terminal read() reads FROM the keyboard buffer into buf
 * INPUTS: fd : file    buf: buffer to be read    nbytes: number of bytes of buf
 * SIDE EFFECTS:
 * RETURN VALUE: return number of bytes read
 */
int32_t terminal_read(uint32_t fd, void* buf, int32_t nbytes) {
    if (buf == NULL) // null check
        return -1;

    // hold until there is a new line char
    while(1){
        nbytes = (int32_t)get_keyboard_buffer_length();
        get_keyboard_buffer((char *)buf);
        if(nbytes > 0 && ((char *)buf)[nbytes-1] == '\n')
            break;
    }
    reset_keyboard_buffer(); // ENTER pressed reset buf
    return nbytes;
}

/*
 * terminal_write
 * DESCRIPTION: Terminal write() writes TO the screen from buf
 * INPUTS: fd : file    buf: buffer to be writen    nbytes: number of bytes of buf
 * SIDE EFFECTS: prints the buf to screen
 * RETURN VALUE: number of bytes written or -1
 */
int32_t terminal_write(uint32_t fd, const void* buf, int32_t nbytes) {
    int i;
    int bytes = 0;
    if (buf == NULL) // null check
        return -1;

    // iterate through keyboard buffer and print character to screen
    for (i = 0; i < nbytes; i++) {
        if ( ((char *) buf)[i] != NULL ) {
            putc( ((char *) buf)[i] );
            bytes++; // byte written
        }
    }
    return bytes;
}

/*
 * terminal_bad_read
 * DESCRIPTION: For fd 1 should not be called
 * INPUTS: fd : file    buf: buffer to be writen    nbytes: number of bytes of buf
 * SIDE EFFECTS: bad boy
 * RETURN VALUE:  -1
 */
int32_t terminal_bad_read(uint32_t fd, void* buf, int32_t nbytes){
    printf("bad boy");
    return -1;
}

/*
 * terminal_bad_write
 * DESCRIPTION: For fd 0 should not be called
 * INPUTS: fd : file    buf: buffer to be writen    nbytes: number of bytes of buf
 * SIDE EFFECTS: bad boy
 * RETURN VALUE:  -1
 */
int32_t terminal_bad_write(uint32_t fd, const void* buf, int32_t nbytes){
    printf("bad boy");
    return -1;
}
