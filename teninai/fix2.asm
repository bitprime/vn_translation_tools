<10018c00> ; fixed
	CALL @FixBuffer1
	NOP
	NOP
	
<10018c4c> ; fixed
	CALL @FixBuffer2
	NOP
	NOP
	
<10018d34> ; fixed
	CALL @FixBuffer3
	NOP
	NOP

<10018da2> ; fixed
	CALL @FixBuffer4
	NOP
	NOP
	
<10018e31> ; fixed
	CALL @FixBuffer5
	NOP
	NOP

<10018ede>  ; fixed
	CALL @FixBuffer6
	NOP
	NOP
	
<10018e1d> ; fixed
	CALL @FixBuffer7
	NOP
	NOP
	
<10024054> ; fixed
	PUSH @NewBuffer2
<10026fc3> ; fixed
	PUSH @NewBuffer2
<10027e19> ; fixed
	PUSH @NewBuffer2
	
<10027d91> ; fixed
	call @PerformWordwrap

<10018632> ; fixed
	jmp @ClearPosition
	
<100167eb> ; fixed
	PUSH @PatchFolderPath

	
<1008f000>
@FixBuffer1:
	PUSH EBX
	MOV EBX, @NewBuffer
	LEA ESI, DWORD PTR DS:[EBX+ECX*8+240]
	POP EBX
    RETN
	
@FixBuffer2:
	PUSH EBX
	MOV EBX, @NewBuffer
	LEA EDI, DWORD PTR DS:[EBX+ECX*8+240]
	POP EBX
    RETN
    
@FixBuffer3:
	PUSH EBX
	MOV EBX, @NewBuffer
	LEA ESI, DWORD PTR DS:[EBX+EDX*8+240]
	POP EBX
    RETN
	
@FixBuffer4:
	PUSH EBX
	MOV EBX, @NewBuffer
	MOV DWORD PTR DS:[EBX+ECX*8+240], EDI
	POP EBX
    RETN
    
@FixBuffer5:
	PUSH EBX
	MOV EBX, @NewBuffer
	MOV EAX, DWORD PTR DS:[EDX+EBP+240]
	POP EBX
    RETN
	
@FixBuffer6:
	PUSH EBX
	MOV EBX, @NewBuffer
	MOV ECX, DWORD PTR DS:[EBX+EAX*8+240]
	POP EBX
	RETN
	
@FixBuffer7:
	PUSH EBX
	MOV EBX, @NewBuffer
	LEA EAX, DWORD PTR DS:[EBX+EDX*8+218]
	POP EBX
	RETN
	
	
	
@ClearPosition:
	MOV DWORD PTR DS:[EAX+3198],0
	mov dword [@widthCount], 0
	pop esi
	retn
	
	
@PerformWordwrap:
	CALL 1001C3A0
	
	; wordwrap code
	pushad
	
	; setup buffers
	mov dword [@lastSpace], 0
	
@WordwrapLoop:
	cmp byte [edx], 0x00 ; end of string
	je @EndWordwrapLoop
	cmp byte [edx], 0x80 ; sjis char
	jge @SingleByte
	movzx ecx, word ptr ds:[edx] ; sjis
	add edx, 2
	jmp @PushedChar
@SingleByte:
	movzx ecx, byte ptr ds:[edx] ; ascii
	inc edx
@PushedChar:
	mov dword [@lastChar], ecx
	cmp ecx, 7f ; newline character
	jne @CheckSpace
	jmp @LineBreak
	
@CheckSpace:
	cmp ecx, 20 ; space
	jne @NotLineBreak
	
	dec edx
	mov dword [@lastSpace], edx
	inc edx
	jmp @NotLineBreak
	
@LineBreak:
	mov dword [@widthCount], 0 ; reset the width if a newline command is encountered
	jmp @WordwrapLoop

@NotLineBreak:
	mov ecx, [1005D6C8] ; font size
	
	mov eax, [@lastChar]
	cmp eax, 100
	jge @SkipDiv
	shr ecx, 1	
@SkipDiv:
	mov eax, [@widthCount]
	
	add eax, ecx	
	cmp eax, dword [10060858] ; check if the current line is over the limit
	jl @SkipWordwrap
	
	cmp dword [@lastSpace], 0 ; check if there was even a space we could put a line break at
	je @SkipWordwrap
	
	mov dword [@widthCount], 0 ; reset the width for the wordwrapped line
	mov edx, dword [@lastSpace] ; go back to the space so we can recalculate everything after that again
	mov byte [edx], 7f ; place a newline where the last space was
	inc edx ; start calculating again after the newline	
	
@SkipWordwrap:
	add [@widthCount], ecx ; add the current char's width to the running count
	jmp @WordwrapLoop
@EndWordwrapLoop:
	popad
	
	TEST EAX, EAX
	retn
	
@FontSize:
	db 0x00
	db 0x00
	db 0x00
	db 0x00
@widthCount:
	db 0x00
	db 0x00
	db 0x00
	db 0x00
@lastSpace:
	db 0x00
	db 0x00
	db 0x00
	db 0x00
@lastChar:
	db 0x00
	db 0x00
	db 0x00
	db 0x00
@PatchFolderPath:
	"./patch/%s.p"
    

<10090000>
@NewBuffer:
<10090240>
@NewBuffer2: