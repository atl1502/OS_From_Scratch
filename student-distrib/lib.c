/*
 *lib.c - Some basic library functions (printf, strlen, etc.)
 */

#include "lib.h"
#include "x86_desc.h"
#include "paging.h"
#include "pcb.h"
#include "scheduling.h"

#define VIDEO       0xB8000
#define TERM0       0xB9000
#define TERM1       0xBA000
#define TERM2       0xBB000
#define NUM_COLS    80
#define NUM_ROWS    25
#define ATTRIB      0x7
#define YELLOW		0xE
#define BLUE_CURS	0x9
#define FOURKB      0x1000

// 2 bytes per ascii char (char and color)
#define SCREEN_SIZE NUM_COLS*NUM_ROWS*2
#define ROW_SIZE    2*NUM_COLS

#define MASK1F 0x0F
#define MASK2F 0xFF
#define MASK1E 0x0E
#define PORT4 0x3D4
#define PORT5 0x3D5
#define RSHIFT1 8

static int screen_x;
static int screen_y;
static char* video_mem = (char *)VIDEO;
static char* term0_mem = (char *)TERM0;
static char* term1_mem = (char *)TERM1;
static char* term2_mem = (char *)TERM2;
static int bscreen_x[3] = {0};
static int bscreen_y[3] = {0};
// buffer used for saving screen after a clear screen
static char prev_screen_buff[SCREEN_SIZE] = {0x20};
static int prev_screen_x;
static int prev_screen_y;
static int kb_flag;

/* void clear(void);
 * Inputs: void
 * Return Value: none
 * Function: Clears video memory */
void clear(void) {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(video_mem + (i << 1)) = ' ';
        *(uint8_t *)(video_mem + (i << 1) + 1) = BLUE_CURS;
    }
    screen_x = 0;
    screen_y = 0;
    if (term_num != running_proc && !kb_flag){
        return;
    }
    update_cursor(screen_x, screen_y);
}

/* void program_clear(void);
 * Inputs: void
 * Return Value: none
 * Function: Same as clear but it saves the previous screen */
void program_clear(void) {
    int32_t i;
    memcpy(prev_screen_buff, video_mem, SCREEN_SIZE);
    prev_screen_x = screen_x;
    prev_screen_y = screen_y;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(video_mem + (i << 1)) = ' ';
        *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB;
    }
    screen_x = 0;
    screen_y = 0;
    update_cursor(screen_x, screen_y);
}

/* void program_reload(void);
 * Inputs: void
 * Return Value: none
 * Function: Loads screen saved in prev_screen_buff after program exits */
void program_reload(void) {
    memcpy(video_mem, prev_screen_buff, SCREEN_SIZE);
    screen_x = prev_screen_x;
    screen_y = prev_screen_y;
    update_cursor(screen_x, screen_y);
}

/* void update_cursor(int x, int y);
 * Inputs: int x, int y
 * Return Value: none
 * Function: Updates cursor to specified location */
void update_cursor(int x, int y) {
    uint16_t pos = y * NUM_COLS + x;

    outb(MASK1F, PORT4);
    outb((uint8_t) (pos & MASK2F), PORT5);
    outb(MASK1E, PORT4);
    outb((uint8_t) ((pos >> RSHIFT1) & MASK2F), PORT5);

}

/* Standard printf().
 * Only supports the following format strings:
 * %%  - print a literal '%' character
 * %x  - print a number in hexadecimal
 * %u  - print a number as an unsigned integer
 * %d  - print a number as a signed integer
 * %c  - print a character
 * %s  - print a string
 * %#x - print a number in 32-bit aligned hexadecimal, i.e.
 *       print 8 hexadecimal digits, zero-padded on the left.
 *       For example, the hex number "E" would be printed as
 *       "0000000E".
 *       Note: This is slightly different than the libc specification
 *       for the "#" modifier (this implementation doesn't add a "0x" at
 *       the beginning), but I think it's more flexible this way.
 *       Also note: %x is the only conversion specifier that can use
 *       the "#" modifier to alter output. */
int32_t printf(int8_t *format, ...) {

    /* Pointer to the format string */
    int8_t* buf = format;

    /* Stack pointer for the other parameters */
    int32_t* esp = (void *)&format;
    esp++;

    while (*buf != '\0') {
        switch (*buf) {
            case '%':
                {
                    int32_t alternate = 0;
                    buf++;

format_char_switch:
                    /* Conversion specifiers */
                    switch (*buf) {
                        /* Print a literal '%' character */
                        case '%':
                            putc('%');
                            break;

                        /* Use alternate formatting */
                        case '#':
                            alternate = 1;
                            buf++;
                            /* Yes, I know gotos are bad.  This is the
                             * most elegant and general way to do this,
                             * IMHO. */
                            goto format_char_switch;

                        /* Print a number in hexadecimal form */
                        case 'x':
                            {
                                int8_t conv_buf[64];
                                if (alternate == 0) {
                                    itoa(*((uint32_t *)esp), conv_buf, 16);
                                    puts(conv_buf);
                                } else {
                                    int32_t starting_index;
                                    int32_t i;
                                    itoa(*((uint32_t *)esp), &conv_buf[8], 16);
                                    i = starting_index = strlen(&conv_buf[8]);
                                    while(i < 8) {
                                        conv_buf[i] = '0';
                                        i++;
                                    }
                                    puts(&conv_buf[starting_index]);
                                }
                                esp++;
                            }
                            break;

                        /* Print a number in unsigned int form */
                        case 'u':
                            {
                                int8_t conv_buf[36];
                                itoa(*((uint32_t *)esp), conv_buf, 10);
                                puts(conv_buf);
                                esp++;
                            }
                            break;

                        /* Print a number in signed int form */
                        case 'd':
                            {
                                int8_t conv_buf[36];
                                int32_t value = *((int32_t *)esp);
                                if(value < 0) {
                                    conv_buf[0] = '-';
                                    itoa(-value, &conv_buf[1], 10);
                                } else {
                                    itoa(value, conv_buf, 10);
                                }
                                puts(conv_buf);
                                esp++;
                            }
                            break;

                        /* Print a single character */
                        case 'c':
                            putc((uint8_t) *((int32_t *)esp));
                            esp++;
                            break;

                        /* Print a NULL-terminated string */
                        case 's':
                            puts(*((int8_t **)esp));
                            esp++;
                            break;

                        default:
                            break;
                    }

                }
                break;

            default:
                putc(*buf);
                break;
        }
        buf++;
    }
    return (buf - format);
}

/* int32_t puts(int8_t* s);
 *   Inputs: int_8* s = pointer to a string of characters
 *   Return Value: Number of bytes written
 *    Function: Output a string to the console */
int32_t puts(int8_t* s) {
    register int32_t index = 0;
    while (s[index] != '\0') {
        putc(s[index]);
        index++;
    }
    return index;
}

// buffer used for scrolling
static char screen_buff[SCREEN_SIZE] = {0x20};
/* void putc(uint8_t c);
 * Inputs: uint_8* c = character to print
 * Return Value: void
 *  Function: Output a character to the console */
void putc(uint8_t c) {
    uint8_t i;
    if(c == '\n' || c == '\r') {
        screen_y++;
        screen_x = 0;
    } else if (c == '\t'){
        // print 4 spaces for the tab
        for (i = 0; i <4 ; i++){
            if(screen_x >= NUM_COLS)
                break;
            *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = ' ';
            *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = ATTRIB;
            screen_x++;
        }
    } else {
        *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = c;
        *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = ATTRIB;
        screen_x++;
    }
    // move down to next line if it is over the x value
    if(screen_x >= NUM_COLS){
        screen_x = 0;
        screen_y++;
    }
    // if at the end of the screen have the screen move down
    if(screen_y >= NUM_ROWS){
        // copy all but the top line into buffer
        memcpy(screen_buff, video_mem+ROW_SIZE, SCREEN_SIZE - ROW_SIZE);
        memcpy(video_mem, screen_buff, SCREEN_SIZE - ROW_SIZE);
        int32_t i;
        for (i = 0; i < NUM_COLS; i++) {
            *(uint8_t *) (video_mem + (NUM_COLS * (NUM_ROWS - 1) * 2) + (i << 1)) = ' ';
            *(uint8_t *) (video_mem + (NUM_COLS * (NUM_ROWS - 1) * 2) + (i << 1) + 1) = ATTRIB;
        }
        screen_y--;
    }
    if (term_num != running_proc && !kb_flag){
        return;
    }
    update_cursor(screen_x, screen_y);
}

/* void putc_colourised(uint8_t c, uint8_t forecolour);
 * Inputs: uint_8* c = character to print, uint8_t forecolour = text colour in text mode 0
 * Return Value: void
 *  Function: Output a character to the console in the given forecolour */
void putc_colourised(uint8_t c, uint8_t forecolour) {
	uint8_t i;
	if(c == '\n' || c == '\r') {
		screen_y++;
		screen_x = 0;
	} else if (c == '\t'){
		// print 4 spaces for the tab
		for (i = 0; i <4 ; i++){
			if(screen_x >= NUM_COLS)
				break;
			*(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = ' ';
			*(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = forecolour;
			screen_x++;
		}
	} else {
		*(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = c;
		*(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = forecolour;
		screen_x++;
	}
	// move down to next line if it is over the x value
	if(screen_x >= NUM_COLS){
		screen_x = 0;
		screen_y++;
	}
	// if at the end of the screen have the screen move down
	if(screen_y >= NUM_ROWS){
		// copy all but the top line into buffer
		memcpy(screen_buff, video_mem+ROW_SIZE, SCREEN_SIZE - ROW_SIZE);
		memcpy(video_mem, screen_buff, SCREEN_SIZE - ROW_SIZE);
		int32_t i;
		for (i = 0; i < NUM_COLS; i++) {
			*(uint8_t *) (video_mem + (NUM_COLS * (NUM_ROWS - 1) * 2) + (i << 1)) = ' ';
			*(uint8_t *) (video_mem + (NUM_COLS * (NUM_ROWS - 1) * 2) + (i << 1) + 1) = forecolour;
		}
		screen_y--;
	}
	if (term_num != running_proc && !kb_flag){
		return;
	}
	update_cursor(screen_x, screen_y);
}

/* void removec();
 * Inputs: uint_8* c = character to print
 * Return Value: void
 *  Function: Deletes current char from console */
void removec() {
    // ensure screen x/y are not in first position
    if (screen_x == 0 && screen_y == 0){
        return;
    }
    // move to previous position
    if (screen_x == 0){
        screen_x = NUM_COLS-1;
        screen_y--;
    } else {
        screen_x--;
    }
    // print out space in the current location
    *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = 0x20;
    *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = ATTRIB;
    if (term_num != running_proc && !kb_flag){
        return;
    }
    update_cursor(screen_x, screen_y);
}


/* int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix);
 * Inputs: uint32_t value = number to convert
 *            int8_t* buf = allocated buffer to place string in
 *          int32_t radix = base system. hex, oct, dec, etc.
 * Return Value: number of bytes written
 * Function: Convert a number to its ASCII representation, with base "radix" */
int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix) {
    static int8_t lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int8_t *newbuf = buf;
    int32_t i;
    uint32_t newval = value;

    /* Special case for zero */
    if (value == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }

    /* Go through the number one place value at a time, and add the
     * correct digit to "newbuf".  We actually add characters to the
     * ASCII string from lowest place value to highest, which is the
     * opposite of how the number should be printed.  We'll reverse the
     * characters later. */
    while (newval > 0) {
        i = newval % radix;
        *newbuf = lookup[i];
        newbuf++;
        newval /= radix;
    }

    /* Add a terminating NULL */
    *newbuf = '\0';

    /* Reverse the string and return */
    return strrev(buf);
}

/* int8_t* strrev(int8_t* s);
 * Inputs: int8_t* s = string to reverse
 * Return Value: reversed string
 * Function: reverses a string s */
int8_t* strrev(int8_t* s) {
    register int8_t tmp;
    register int32_t beg = 0;
    register int32_t end = strlen(s) - 1;

    while (beg < end) {
        tmp = s[end];
        s[end] = s[beg];
        s[beg] = tmp;
        beg++;
        end--;
    }
    return s;
}

/* uint32_t strlen(const int8_t* s);
 * Inputs: const int8_t* s = string to take length of
 * Return Value: length of string s
 * Function: return length of string s */
uint32_t strlen(const int8_t* s) {
    register uint32_t len = 0;
    while (s[len] != '\0')
        len++;
    return len;
}

/* void* memset(void* s, int32_t c, uint32_t n);
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set n consecutive bytes of pointer s to value c */
void* memset(void* s, int32_t c, uint32_t n) {
    c &= 0xFF;
    asm volatile ("                 \n\
            .memset_top:            \n\
            testl   %%ecx, %%ecx    \n\
            jz      .memset_done    \n\
            testl   $0x3, %%edi     \n\
            jz      .memset_aligned \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            subl    $1, %%ecx       \n\
            jmp     .memset_top     \n\
            .memset_aligned:        \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            movl    %%ecx, %%edx    \n\
            shrl    $2, %%ecx       \n\
            andl    $0x3, %%edx     \n\
            cld                     \n\
            rep     stosl           \n\
            .memset_bottom:         \n\
            testl   %%edx, %%edx    \n\
            jz      .memset_done    \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            subl    $1, %%edx       \n\
            jmp     .memset_bottom  \n\
            .memset_done:           \n\
            "
            :
            : "a"(c << 24 | c << 16 | c << 8 | c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memset_word(void* s, int32_t c, uint32_t n);
 * Description: Optimized memset_word
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set lower 16 bits of n consecutive memory locations of pointer s to value c */
void* memset_word(void* s, int32_t c, uint32_t n) {
    asm volatile ("                 \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            cld                     \n\
            rep     stosw           \n\
            "
            :
            : "a"(c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memset_dword(void* s, int32_t c, uint32_t n);
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set n consecutive memory locations of pointer s to value c */
void* memset_dword(void* s, int32_t c, uint32_t n) {
    asm volatile ("                 \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            cld                     \n\
            rep     stosl           \n\
            "
            :
            : "a"(c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memcpy(void* dest, const void* src, uint32_t n);
 * Inputs:      void* dest = destination of copy
 *         const void* src = source of copy
 *              uint32_t n = number of byets to copy
 * Return Value: pointer to dest
 * Function: copy n bytes of src to dest */
void* memcpy(void* dest, const void* src, uint32_t n) {
    asm volatile ("                 \n\
            .memcpy_top:            \n\
            testl   %%ecx, %%ecx    \n\
            jz      .memcpy_done    \n\
            testl   $0x3, %%edi     \n\
            jz      .memcpy_aligned \n\
            movb    (%%esi), %%al   \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            addl    $1, %%esi       \n\
            subl    $1, %%ecx       \n\
            jmp     .memcpy_top     \n\
            .memcpy_aligned:        \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            movl    %%ecx, %%edx    \n\
            shrl    $2, %%ecx       \n\
            andl    $0x3, %%edx     \n\
            cld                     \n\
            rep     movsl           \n\
            .memcpy_bottom:         \n\
            testl   %%edx, %%edx    \n\
            jz      .memcpy_done    \n\
            movb    (%%esi), %%al   \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            addl    $1, %%esi       \n\
            subl    $1, %%edx       \n\
            jmp     .memcpy_bottom  \n\
            .memcpy_done:           \n\
            "
            :
            : "S"(src), "D"(dest), "c"(n)
            : "eax", "edx", "memory", "cc"
    );
    return dest;
}

/* void* memmove(void* dest, const void* src, uint32_t n);
 * Description: Optimized memmove (used for overlapping memory areas)
 * Inputs:      void* dest = destination of move
 *         const void* src = source of move
 *              uint32_t n = number of byets to move
 * Return Value: pointer to dest
 * Function: move n bytes of src to dest */
void* memmove(void* dest, const void* src, uint32_t n) {
    asm volatile ("                             \n\
            movw    %%ds, %%dx                  \n\
            movw    %%dx, %%es                  \n\
            cld                                 \n\
            cmp     %%edi, %%esi                \n\
            jae     .memmove_go                 \n\
            leal    -1(%%esi, %%ecx), %%esi     \n\
            leal    -1(%%edi, %%ecx), %%edi     \n\
            std                                 \n\
            .memmove_go:                        \n\
            rep     movsb                       \n\
            "
            :
            : "D"(dest), "S"(src), "c"(n)
            : "edx", "memory", "cc"
    );
    return dest;
}

/* int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
 * Inputs: const int8_t* s1 = first string to compare
 *         const int8_t* s2 = second string to compare
 *               uint32_t n = number of bytes to compare
 * Return Value: A zero value indicates that the characters compared
 *               in both strings form the same string.
 *               A value greater than zero indicates that the first
 *               character that does not match has a greater value
 *               in str1 than in str2; And a value less than zero
 *               indicates the opposite.
 * Function: compares string 1 and string 2 for equality */
int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n) {
    int32_t i;
    for (i = 0; i < n; i++) {
        if ((s1[i] != s2[i]) || (s1[i] == '\0') /* || s2[i] == '\0' */) {

            /* The s2[i] == '\0' is unnecessary because of the short-circuit
             * semantics of 'if' expressions in C.  If the first expression
             * (s1[i] != s2[i]) evaluates to false, that is, if s1[i] ==
             * s2[i], then we only need to test either s1[i] or s2[i] for
             * '\0', since we know they are equal. */
            return s1[i] - s2[i];
        }
    }
    return 0;
}

/* int8_t* strcpy(int8_t* dest, const int8_t* src)
 * Inputs:      int8_t* dest = destination string of copy
 *              const int8_t* src = source string of copy
 * Return Value: pointer to dest
 * Function: copy the source string into the destination string */
int8_t* strcpy(int8_t* dest, const int8_t* src) {
    int32_t i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

/* int8_t* strcpy(int8_t* dest, const int8_t* src, uint32_t n)
 * Inputs:      int8_t* dest = destination string of copy
 *              const int8_t* src = source string of copy
 *              uint32_t n = number of bytes to copy
 * Return Value: pointer to dest
 * Function: copy n bytes of the source string into the destination string */
int8_t* strncpy(int8_t* dest, const int8_t* src, uint32_t n) {
    int32_t i = 0;
    while (src[i] != '\0' && i < n) {
        dest[i] = src[i];
        i++;
    }
    while (i < n) {
        dest[i] = '\0';
        i++;
    }
    return dest;
}

/* void test_interrupts(void)
 * Inputs: void
 * Return Value: void
 * Function: increments video memory. To be used to test rtc */
void test_interrupts(void) {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        video_mem[i << 1]++;
    }
}

/* void test_interrupts(void)
 * Inputs: void
 * Return Value: void
 * Function: flips the kb flag every time its called
 */
void flip_kb_flag(){
    kb_flag ^= 1;
}

/*
 * void restore screen
 * Function: restores screen print position for given terminal
 * Inputs: term (Terminal number to set screen to)
 * Outputs: None
 * Returne Value: None
 * Side Effects: modifies global screen x and y values to that terminals cursor location
 */
void restore_screen(int term) {
    screen_x = bscreen_x[term];
    screen_y = bscreen_y[term];
}

/*
 * void save screen
 * Function: saves screen x and y into array to be restored later
 * Inputs: term (Terminal number to save screen of)
 * Outputs: None
 * Returne Value: None
 * Side Effects: Saves global variables into static array which holds saved screen values
 */
void save_screen(int term) {
    bscreen_x[term] = screen_x;
    bscreen_y[term] = screen_y;
}


/* void switch_term(int dest, int src)
 * Replaces new terminal with old terminal on screen
 * No scheduling can occur therefore no interrupts :3
 * Inputs: new and old terminals
 * Return Value: void
 * Side Effects: Modifies global term_num variable
 * Function: switches video memory
 */
void switch_term(int new, int old) {

    cli();

    /* Array of three screen buffers */
    char * bterm[NUM_TERM] = {term0_mem, term1_mem, term2_mem};

    /* Don't do anything if no changes */
    if (new == old) {
        return;
    }

    /* Set currently viewing terminal to global term_num */
    term_num = new;

    /* Point video_mem page to physical video_mem */
    unmap();

    /* Copy current video_mem into src terminal */
    memcpy((bterm[old]), video_mem, FOURKB);

    /* Copy destination terminal into video_mem */
    memcpy(video_mem, (bterm[new]), FOURKB);

    /* Restore screen to scheduled process */
    restore_screen(running_proc);

    /* Change cursor to new terminal view */
    update_cursor(bscreen_x[new], bscreen_y[new]);

    /* If not the process' terminal, make sure video_mem page remaps to buffer */
    if (running_proc != term_num) {
        remap(running_proc);
    }

    sti();

    return;
}
