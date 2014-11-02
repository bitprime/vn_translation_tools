<10018ED8>
	CALL @FixBuffer1
	NOP
	NOP
	
<10018F6F>
	CALL @FixBuffer2
	NOP
	NOP
	
<10019015>
	CALL @FixBuffer3
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP

<100190D1>
	CALL @FixBuffer4
	NOP
	NOP
	
<10018DBD>
	CALL @FixBuffer5
	NOP
	NOP

<10018E9A>
	CALL @FixBuffer6
	NOP
	NOP
	
<10019007>
	CALL @FixBuffer7
	NOP
	NOP
	
<10026D60>
	PUSH @NewBuffer2
<10029F4A>
	PUSH @NewBuffer2
<1002AD4F>
	PUSH @NewBuffer2
	
<1002ACCC>
	call @PerformWordwrap

<10018934>
	jmp @ClearPosition
	
<1001A21A>
	PUSH @PatchFolderPath

	
<1008B000>
@FixBuffer1:
	MOV EDI, @NewBuffer
	LEA EDI, DWORD PTR DS:[EDI+EAX*8+240]
	RETN
	
@FixBuffer2:
	PUSH EBX
	MOV EBX, @NewBuffer
	MOV DWORD PTR DS:[EBX+EDX*8+240], EDI
	POP EBX
    RETN
    
@FixBuffer3:
	PUSH EBX
	MOV EBX, @NewBuffer
	CMP DWORD PTR DS:[EBX+EAX*8+240],0
	LEA EAX, DWORD PTR DS:[EBX+EAX*8]
	POP EBX
	RETN
	
@FixBuffer4:
	MOV EAX, @NewBuffer
	MOV EAX, DWORD PTR DS:[EAX+EDX*8+240]
	RETN
	
@FixBuffer5:
	MOV ESI, @NewBuffer
	LEA ESI, DWORD PTR DS:[ESI+EDX*8+240]
	RETN
	
@FixBuffer6:
	MOV ESI, @NewBuffer
	LEA ESI, DWORD PTR DS:[ESI+EAX*8+240]
	RETN
	
@FixBuffer7:
	PUSH EBX
	MOV EBX, @NewBuffer
	LEA EAX, DWORD PTR DS:[EBX+EAX*8+218]
	POP EBX
	RETN
	
	
	
@ClearPosition:
	mov dword [@widthCount], 0
	retn
	
	
@PerformWordwrap:
	CALL 1001CED0
	
	; wordwrap code
	pushad
	
	; setup buffers
	;mov eax, [100576dc]
	;mov dword [@widthCount], eax
	mov dword [@lastSpace], 0
	
@WordwrapLoop:
	cmp byte [ecx], 0x00 ; end of string
	je @EndWordwrapLoop
	cmp byte [ecx], 0x80 ; sjis char
	jge @SingleByte
	movzx edx, word ptr ds:[ecx] ; sjis
	add ecx, 2
	jmp @PushedChar
@SingleByte:
	movzx edx, byte ptr ds:[ecx] ; ascii
	inc ecx
@PushedChar:
	mov dword [@lastChar], edx
	cmp edx, 7f ; newline character
	jne @CheckSpace
	jmp @LineBreak
	
@CheckSpace:
	cmp edx, 20 ; space
	jne @NotLineBreak
	
	dec ecx
	mov dword [@lastSpace], ecx
	inc ecx
	jmp @NotLineBreak
	
@LineBreak:
	mov dword [@widthCount], 0 ; reset the width if a newline command is encountered
	jmp @WordwrapLoop

@NotLineBreak:
	mov edx, [1005A8C4] ; font size
	
	mov eax, [@lastChar]
	cmp eax, 100
	jge @SkipDiv
	shr edx, 1	
@SkipDiv:
	mov eax, [@widthCount]
	
	add eax, edx	
	cmp eax, dword [1005A878] ; check if the current line is over the limit
	jl @SkipWordwrap
	
	cmp dword [@lastSpace], 0 ; check if there was even a space we could put a line break at
	je @SkipWordwrap
	
	mov dword [@widthCount], 0 ; reset the width for the wordwrapped line
	mov ecx, dword [@lastSpace] ; go back to the space so we can recalculate everything after that again
	mov byte [ecx], 7f ; place a newline where the last space was
	inc ecx ; start calculating again after the newline	
	
@SkipWordwrap:
	add [@widthCount], edx ; add the current char's width to the running count
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
    

<1008c000>
@NewBuffer:
<1008c240>
@NewBuffer2: