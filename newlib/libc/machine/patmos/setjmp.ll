
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
    swc  [$$r3 + 0] = $$r20
    swc  [$$r3 + 1] = $$r21
    swc  [$$r3 + 2] = $$r22
    swc  [$$r3 + 3] = $$r23
    swc  [$$r3 + 4] = $$r24
    swc  [$$r3 + 5] = $$r25
    swc  [$$r3 + 6] = $$r26
    swc  [$$r3 + 7] = $$r27
    swc  [$$r3 + 8] = $$r28
    swc  [$$r3 + 9] = $$r29
    swc  [$$r3 + 10] = $$r30
    swc  [$$r3 + 11] = $$r31
    mfs  $$r9  = $$s0 
    mfs  $$r10 = $$s2
    mfs  $$r11 = $$s3
    mfs  $$r12 = $$s5
    mfs  $$r13 = $$s6
    mfs  $$r14 = $$s7
    mfs  $$r15 = $$s8
    sub  $$r12 = $$r12, $$r13
    swc  [$$r3 + 12] = $$r9
    swc  [$$r3 + 13] = $$r10
    swc  [$$r3 + 14] = $$r11
    swc  [$$r3 + 15] = $$r12
    swc  [$$r3 + 16] = $$r13
    swc  [$$r3 + 17] = $$r14
    swc  [$$r3 + 18] = $$r15
    clr  $0
    ", "=r"()
  ret i32 %0
}

define void @longjmp(i32* %env, i32 %value) #0 {
entry:
  tail call void asm sideeffect "
    lwc $$r20 = [$$r3 + 0]
    lwc $$r21 = [$$r3 + 1]
    lwc $$r22 = [$$r3 + 2]
    lwc $$r23 = [$$r3 + 3]
    lwc $$r24 = [$$r3 + 4]
    lwc $$r25 = [$$r3 + 5]
    lwc $$r26 = [$$r3 + 6]
    lwc $$r27 = [$$r3 + 7]
    lwc $$r28 = [$$r3 + 8]
    lwc $$r29 = [$$r3 + 9]
    lwc $$r30 = [$$r3 + 10]
    lwc $$r31 = [$$r3 + 11]
    mfs  $$r12 = $$s5\09   
    mfs  $$r13 = $$s6\09   
    sub  $$r12 = $$r12, $$r13
    sspill $$r12\09   
    lwc $$r9  = [$$r3 + 12]
    lwc $$r10 = [$$r3 + 13]
    lwc $$r11 = [$$r3 + 14]
    lwc $$r12 = [$$r3 + 15]
    lwc $$r13 = [$$r3 + 16]
    lwc $$r14 = [$$r3 + 17]
    lwc $$r15 = [$$r3 + 18]
    mts $$s2 = $$r10     
    mts $$s5 = $$r13     
    mts $$s6 = $$r13     
    mts $$s7 = $$r14     
    mts $$s8 = $$r15     
    sens $$r12\09   
    ret                
    mts $$s3 = $$r11     
    mts $$s0 = $$r9      
    mov $$r1 = $$r4
    ", ""()
  unreachable
}

attributes #0 = { naked noinline }