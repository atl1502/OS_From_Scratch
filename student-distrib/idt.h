#ifndef IDT
#define IDT

// Previous Registers Struct
typedef struct prev_reg {
	uint32_t EBX, ECX, EDX, ESI, EDI, EBP, EAX, IRQ;
} prev_reg_t;

// internal irq table
typedef void (*isr_t) ();
extern void register_interrupt_handler (int n);

// exception handlers
void handler_divide();
void handler_debug();
void handler_nmi();
void handler_breakpoint();
void handler_overflow();
void handler_bounds();
void handler_inv_opcode();
void handler_dev_na();
void handler_doub_fault();
void handler_cso();
void handler_inv_tss();
void handler_seg_np();
void handler_stk_fault();
void handler_gen_prot();
void handler_page_fault();
void handler_fpu_error();
void handler_align_chk();
void handler_machine_chk();
void handler_simd_fp();
void handler_keyboard();
void handler_rtc();
void handler_interrupt();

// irq handler
unsigned int do_IRQ(prev_reg_t regs);

// initialises IDT
void idt_init();

#endif
