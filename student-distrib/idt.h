// internal irq table
typedef void (*isr_t) ();
extern void register_interrupt_handler (int n);

// initialises IDT
extern void idt_init();

// exception handlers
extern void handler_divide();
extern void handler_debug();
extern void handler_nmi();
extern void handler_breakpoint();
extern void handler_overflow();
extern void handler_bounds();
extern void handler_inv_opcode();
extern void handler_dev_na();
extern void handler_doub_fault();
extern void handler_cso();
extern void handler_inv_tss();
extern void handler_seg_np();
extern void handler_stk_fault();
extern void handler_gen_prot();
extern void handler_page_fault();
extern void handler_fpu_error();
extern void handler_align_chk();
extern void handler_machine_chk();
extern void handler_simd_fp();
