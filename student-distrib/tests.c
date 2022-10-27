#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "./drivers/filesystem.h"
#include "drivers/terminal.h"
#include "drivers/rtc.h"

#define PASS 1
#define FAIL 0

#define CHECKNUM 5
#define CHECKNUM2 2

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* Exception Test (divide by 0)
 *
 * Tries to divide by 0 to generate the exception
 * Inputs: None
 * Outputs: Should not have an out since it will hold in while loop
 * Side Effects: Stalls processor in while loop, there will be exception printed
 * Coverage: Load IDT, IDT definition
 * Files: idt.h/c
 */
int divide_by_zero_test(){
	TEST_HEADER;
	int result = FAIL;
	int z;
	z = 0;
	int temp = 1/z;
	if (temp) {
	}

	return result;
}

/* Page Fault Test
 *
 * Should casue a page fault
 * Inputs: None
 * Outputs: Should not have an out since it will hold in while loop
 * Side Effects: Stalls processor in while loop, there will be exception printed
 * Coverage: Paging
 * Files: kernal.c
 */
int page_fault_test(){
	TEST_HEADER;
	int result = FAIL;
	int* test = (int*)0x1000000;
	int uhoh = *test;
	uhoh++;
	return result;
}

/* Page Fault Test 2
 *
 * Should not cause a page fault dereferencing within defined addr range
 * Inputs: None
 * Outputs: PASS or held in page fault
 * Side Effects: None
 * Coverage: Paging
 * Files: kernal.c
 */
int no_page_fault_test(){
	TEST_HEADER;
	int result = PASS;
	int* test = (int*)0x600000;
	int uhoh = *test;
	uhoh++;
	return result;
}

/* Checkpoint 2 tests */
/*
 * Filesystem Filename test init
 *
 * Initializes filesystem start in this file for testing
 * Inputs: file_start (start of filesystem in mem)
 * Outputs: none
 * Side Effects: sets file_start global variable
 * return value: 0 or -1 if errors
 */

static uint32_t file_start;

int filesystem_print_files_init(uint32_t file_start_in) {
	file_start = file_start_in;
	return PASS;
}

/*
 * Filesystem Filename test
 *
 * Should print all file names in the root directory
 * Inputs: file_start (start of filesystem in mem)
 * Outputs: none
 * Side Effects: Prints file names to screen and all errors
 * return value: 0 or -1 if errors
 */
int filesystem_print_files() {
	fd_t fd = { 0 };
	boot_block_t * filesys = (boot_block_t *) file_start;
	char filename[33] = { 0 };
	int index = 0;
	uint32_t inode_size;

	clear();
	printf("Filesystem Directory Names:\n");
	for (index = 0; index < 63; index++) {
		dir_read(&fd, filename, 32);
		inode_size = read_inode_size(filesys->direntries[index].inode_num);
		filename[32] = 0;
		if (filename[0]) {
			printf("Size: %u Filename: %s\n", inode_size, filename);
		}
	}
	return PASS;
}

/*
 * Filesystem File Contents test
 *
 * Should print all filecontents of ls and verylargetext...
 * Inputs: none
 * Outputs: none
 * Side Effects: Prints file contents along with any errors
 * return value: 0 or -1 if errors
 */

static unsigned char char_buffer[5349] = { 0 };

int print_file_contents_long() {
	fd_t fd;

	clear();
	printf("Long File Test");
	if (file_open ((const uint8_t *)"verylargetextwithverylongname.tx", &fd)) {
		printf("FAILED FILE FILE!\n");
		return FAIL;
	}

	if (file_read (&fd, char_buffer, 5277)) {
		printf("FAILED READ DATA!\n");
		return FAIL;
	}
	printf("File Contents: %s\n", char_buffer);
	return PASS;
}

int print_file_contents_frame0() {
	fd_t fd;

	clear();
	printf("Frame 0 Test\n");

	if (file_open ((const uint8_t *)"frame0.txt", &fd)) {
		printf("FAILED FILE FILE!\n");
		return FAIL;
	}

	if (file_read (&fd, char_buffer, 187)) {
		printf("FAILED READ DATA!\n");
		return FAIL;
	}
	char_buffer[187] = 0;
	printf("File Contents: 187 Bytes\n%s\n", char_buffer);
	return PASS;
}

int print_file_contents_ls() {
	fd_t fd;

	clear();
	printf("ls Test\n");
	if (file_open ((const uint8_t *)"ls", &fd)) {
		printf("FAILED FILE FILE!\n");
		return FAIL;
	}

	if (file_read (&fd, char_buffer, 5349)) {
		printf("FAILED READ DATA!\n");
		return FAIL;
	}
	printf("File Contents: %d Bytes:\n", 5349);
	int i = 0;
	for (i = 0; i < 5350; i++) {
		if (char_buffer[i])
			putc(char_buffer[i]);
	}
	printf("\n");
	return PASS;
}

/* RTC test
 *
 * Show that rtc_open(), rtc_close(), rtc_read(), rtc_write() all work
 * Inputs: None
 * Outputs: PASS
 * Side Effects: None
 * Coverage: RTC
 * Files: rtc.c, rtc.h
 */
int rtc_test(){
	TEST_HEADER;
	int result = PASS;
	int i, j, p;
	int cur_freq = 1;

	//frequency sweep test
	program_clear();
	printf("First let's do a frequency sweep! \n");
	count_init(FREQ_ST);
	while (counter()) {
		continue;
	}

	for (i = 1; i < 11; i++) {
		clear();
		cur_freq = cur_freq*2;
		rtc_write(&cur_freq);
		printf("Current frequency: %d \n", frequency);
		count_init(cur_freq);
		while (counter()) {
			continue;
		}
		count_init(cur_freq);
		print_on();
		while (counter()) {
			continue;
		}
		print_off();
	}
	clear();

	//rtc_open test
	printf("Now let's check rtc_open()! \n");
	j = rtc_open();
	printf("Return result of rtc_open(): %d \n", j);
	printf("Current frequency after rtc_open(): %d \n", frequency);
	count_init(FREQ_OP*CHECKNUM);
	print_on();
	while (counter()) {
		continue;
	}
	print_off();
	clear();

	//rtc_close test
	printf("Checking rtc_close() ... \n");
	j = rtc_close();
	printf("Return result of rtc_close(): %d \n", j);
	count_init(FREQ_OP*CHECKNUM2);
	while (counter()) {
		continue;
	}
	clear();

	//rtc_read test
	printf("Okay let's check rtc_read() now. \n");
	j = rtc_close();
	printf("Return result of rtc_close(): %d \n", j);
	clear();
	printf("I will turn printing on again. \n");
	printf("Then I will turn on rtc_read(). \n");
	count_init(FREQ_OP*CHECKNUM);
	p = 1;
	print_on();
	while (counter()) {
		if (counter() == CHECKNUM) {
			if (p == 1) {
				printf(" rtc_read() called! ");
				p = 0;
			}
			j = rtc_read();
		}
		continue;
	}
	print_off();
	clear();

	printf("Return result of rtc_read(): %d \n", j);
	count_init(FREQ_OP);
	while (counter()) {
		continue;
	}
	clear();

	//rtc_write test illegal input 9
	printf("Finally time to do some final checks on rtc_write() \n");
	printf("Let's do an illegal input 9 since 9 is not a power of 2 \n");
	cur_freq = 9;
	p = 1;
	count_init(FREQ_OP*CHECKNUM);
	print_on();
	while (counter()) {
		if (counter() == CHECKNUM2*CHECKNUM) {
			if (p == 1) {
				printf(" Illegal rtc_write() called! ");
				p = 0;
			}
			j = rtc_write(&cur_freq);
		}
		continue;
	}
	print_off();
	clear();

	printf("As you can see the frequency didn't change \n");
	printf("Return result of rtc_write(): %d \n", j);
	count_init(FREQ_OP);
	while (counter()) {
		continue;
	}
	clear();

	//rtc_write test illegal input NULL
	printf("Let's repeat the previous test but input a NULL pointer to rtc_write() \n");
	count_init(FREQ_OP*CHECKNUM);
	p = 1;
	print_on();
	while (counter()) {
		if (counter() == CHECKNUM2*CHECKNUM) {
			if (p == 1) {
				printf(" Illegal rtc_write() called! ");
				p = 0;
			}
			j = rtc_write(NULL);
		}
		continue;
	}
	print_off();
	clear();

	printf("As you can see the frequency didn't change \n");
	printf("Return result of rtc_write(): %d \n", j);
	count_init(FREQ_OP);
	while (counter()) {
		continue;
	}
	clear();

	//rtc_write test legal input change freq
	printf("Finally let's change rtc_write() back to 1024 Hz and see the return value \n");
	count_init(FREQ_ST);
	p = 1;
	cur_freq = FREQ_ST;
	print_on();
	while (counter()) {
		if (counter() == 3*FREQ_ST - CHECKNUM) {
			if (p == 1) {
				printf(" Legal rtc_write() called! ");
				p = 0;
			}
			j = rtc_write(&cur_freq);
		}
		continue;
	}
	print_off();
	clear();

	printf("As you can see the frequency did change \n");
	printf("Return result of rtc_write(): %d \n", j);
	count_init(FREQ_ST);
	while (counter()) {
		continue;
	}
	program_reload();

	return result;
}

/* Terminal Run Test
 *
 * Opens terminal and holds until newline and should exit terminal,
 * printed value should match length of input (including newline)
 * Inputs: None
 * Outputs: PASS or held in an exception
 * Side Effects: None
 * Coverage: Terminal and keyboard
 * Files: kernal.c
 */
int terminal_run_test(){
	TEST_HEADER;
	char buff[128] = {0};
	uint32_t len;
	uint32_t i;
	while(1){
		// will hold untill newline
		len = terminal_read(0, buff, 0);
		terminal_write(0, buff, len);
		for(i = 0; i < len; i++)
			buff[i] = 0;
	}
	return PASS;
}

/* File system misc. test
 *
 * Tests return values for file system functions
 * Inputs: None
 * Outputs: PASS or held in an exception
 * Side Effects: None
 * Coverage: File systems
 * Files: kernal.c
 */
int file_misc_test() {
	TEST_HEADER;
	fd_t * p;
	int i;
	i = file_close(p);
	if (i != 0) {
		assertion_failure();
		return FAIL;
	}
	i = file_write(p);
	if (i != -1) {
		assertion_failure();
		return FAIL;
	}
	return PASS;

}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// TEST_OUTPUT("idt_test", idt_test());
	// TEST_OUTPUT("no_page_fault_test", no_page_fault_test());
	// Exception testing, comment out or in based on needs
	// CP 1 Tests:
	// TEST_OUTPUT("divide_by_zero_test", divide_by_zero_test());
	// TEST_OUTPUT("page_fault_test", page_fault_test());
	// CP 2 Tests:
	TEST_OUTPUT("rtc_test", rtc_test());
	// TEST_OUTPUT("terminal_open_close_test", terminal_open_close_test());
	// TEST_OUTPUT("Filename Test", filesystem_print_files());
	// TEST_OUTPUT("Long File Test", print_file_contents_long());
	// TEST_OUTPUT("frame0.txt Test", print_file_contents_frame0());
	// TEST_OUTPUT("ls Test", print_file_contents_ls());
	// TEST_OUTPUT("file_misc_test", file_misc_test());
	TEST_OUTPUT("terminal_run_test", terminal_run_test());
}
