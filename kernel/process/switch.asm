[BITS 64]
section .text

global task_switch
extern current_task    ; Reference the global current_task

; void task_switch(task_t* next);
task_switch:
    ; Save current task state
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    ; Get current task structure
    mov rax, [rel current_task]  ; Use RIP-relative addressing
    mov [rax], rsp              ; Save stack pointer

    ; Switch to new task
    mov [rel current_task], rcx  ; Update current_task (rcx contains next task)
    mov rsp, [rcx]              ; Load new stack pointer

    ; Restore new task state
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp

    ret