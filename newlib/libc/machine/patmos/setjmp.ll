
; We define setjmp and longjmp in LLVM-IR because there is no way in legal C
; to define them. This is because naked functions in C don't allow anything
; other than basic inline assembly in naked functions.
; However, if we do so in C, the LLVM optimizer will notice (incorrectly) that
; the function arguments aren't used (since we would have to refer to their registers
; as defined by the ABI) and would optimize calls to these function by passing 'undef'.
;
; On the other hand, naked function in LLVM are defined as target-specific.
; Therefore, we will have to maintain that this code works ourselves, which
; is a better long-term solution.

define i32 @setjmp(i32* %env) #0 {
entry:
  %0 = tail call i32 asm sideeffect "
    swc  [$1 + 0] = $$r20
    swc  [$1 + 1] = $$r21
    swc  [$1 + 2] = $$r22
    swc  [$1 + 3] = $$r23
    swc  [$1 + 4] = $$r24
    swc  [$1 + 5] = $$r25
    swc  [$1 + 6] = $$r26
    swc  [$1 + 7] = $$r27
    swc  [$1 + 8] = $$r28
    swc  [$1 + 9] = $$r29
    swc  [$1 + 10] = $$r30
    swc  [$1 + 11] = $$r31
    mfs  $$r9  = $$s0 
    mfs  $$r10 = $$s2
    mfs  $$r11 = $$s3
    mfs  $$r12 = $$s5
    mfs  $$r13 = $$s6
    mfs  $$r14 = $$s7
    mfs  $$r15 = $$s8
    sub  $$r12 = $$r12, $$r13
    swc  [$1 + 12] = $$r9
    swc  [$1 + 13] = $$r10
    swc  [$1 + 14] = $$r11
    swc  [$1 + 15] = $$r12
    swc  [$1 + 16] = $$r13
    swc  [$1 + 17] = $$r14
    swc  [$1 + 18] = $$r15
    clr  $0
    ", "=r,r"(i32* %env)
  ret i32 %0
}

define void @longjmp(i32* %env, i32 %value) #0 {
entry:
  tail call void asm sideeffect "
    lwc $$r20 = [$0 + 0]
    lwc $$r21 = [$0 + 1]
    lwc $$r22 = [$0 + 2]
    lwc $$r23 = [$0 + 3]
    lwc $$r24 = [$0 + 4]
    lwc $$r25 = [$0 + 5]
    lwc $$r26 = [$0 + 6]
    lwc $$r27 = [$0 + 7]
    lwc $$r28 = [$0 + 8]
    lwc $$r29 = [$0 + 9]
    lwc $$r30 = [$0 + 10]
    lwc $$r31 = [$0 + 11]
    mfs  $$r12 = $$s5\09   
    mfs  $$r13 = $$s6\09   
    sub  $$r12 = $$r12, $$r13
    sspill $$r12\09   
    lwc $$r9  = [$0 + 12]
    lwc $$r10 = [$0 + 13]
    lwc $$r11 = [$0 + 14]
    lwc $$r12 = [$0 + 15]
    lwc $$r13 = [$0 + 16]
    lwc $$r14 = [$0 + 17]
    lwc $$r15 = [$0 + 18]
    mts $$s2 = $$r10     
    mts $$s5 = $$r13     
    mts $$s6 = $$r13     
    mts $$s7 = $$r14     
    mts $$s8 = $$r15     
    sens $$r12\09   
    ret                
    mts $$s3 = $$r11     
    mts $$s0 = $$r9      
    mov $$r1 = $1
    ", "r,r"(i32* %env, i32 %value)
  unreachable
}

attributes #0 = { naked noinline }