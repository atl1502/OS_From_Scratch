#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

#define PASS 1
#define FAIL 0

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
int divide_by_zero(){
	TEST_HEADER;
	int result = FAIL;
	int z;
	int temp;
	z = 0;
	temp = 1/z;

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
int page_fault(){
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
int no_page_fault(){
	TEST_HEADER;
	int result = PASS;
	int* test = (int*)0x600000;
	int uhoh = *test;
	uhoh++;
	return result;
}

// add more tests here

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	TEST_OUTPUT("no_page_fault_test", no_page_fault());
	// Exception testing, comment out or in based on needs
	// TEST_OUTPUT("divide_by_zero_test", divide_by_zero());
	// TEST_OUTPUT("page_fault_test", page_fault());
}
