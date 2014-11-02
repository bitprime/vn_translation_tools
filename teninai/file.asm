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
	
<10018F20>
	call @GetWidth2
	nop
	nop
	nop
	
<10016017>
@_CreateFont:

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
	


@CreateFont:
	pushad
	; delete old object if it exists
	mov eax, dword [@FontObject]
	cmp eax, 0
	je @SkipObjectDelete
	push eax
	call dword ptr ds:[1008441C] ; DeleteObject
@SkipObjectDelete:
	push ebp
	push @CreateFontRet ; since i'm too lazy to copy all of the createfont code again, this is a little hack that'll put the correct return address into the correct spot when the createfont function returns
	push ebp
	push ebp
	push ebp
	mov dword [@FontSize], ebp
	call @_CreateFont
@CreateFontRet:
	mov dword [@FontObject], eax	
	; create object for GetGlyohOutline
	cmp dword [@FontHDC], 0
	jne @GotHDC
	push 0
	call dword ptr ds:[10084400] ; CreateCompatibleDC
	mov [@FontHDC], eax
@GotHDC:
	push [@FontObject]
	push [@FontHDC]
	call dword ptr ds:[10084414] ; SelectObject
	popad
	retn  


	
@GetWidth:
	mov eax, [@FontHDC]
	cmp eax, 0
	je @NoHDC
	push @lpmat2
	push 0
	push 0
	push @lpgm
	push 0 ; GGO_METRICS
	push edx
	push eax
	call dword ptr ds:[1008442C] ; GetGlyphOutlineA
@NoHDC:
	retn
	
	
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
	mov eax, [1005A8C4] ; font size
	cmp [@FontSize], eax
	je @WordwrapLoop
	mov [@FontSize], eax
	mov ebp, eax
	mov edx, 10057690 ; font name
	call @CreateFont
	
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
	push ecx
	call @GetWidth
	pop ecx
	
	mov edx, [@lpgm+10] ; gmCellIncX
	add dx, word [@lpgm+8] ; gmptGlyphOrigin.x
	
; uncomment the following lines for variable width
;	cmp edx, [1005A8C4] ; font size
;	jl @SkipFontSize
;	mov edx, [1005A8C4] ; font size
;@SkipFontSize:
;	mov eax, [@lastChar]
;	cmp eax, 100
;	jge @SkipDiv
;	shr edx, 1	
;@SkipDiv:
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
	
@GetWidth2:
	pushad
	mov eax, [1005A8C4] ; font size
	cmp [@FontSize], eax
	je @SkipFontCreation
	mov [@FontSize], eax
	mov ebp, eax
	mov edx, 10057690 ; font name
	call @CreateFont
@SkipFontCreation:
	mov edx, ecx
	call @GetWidth
	mov esi, [@lpgm+10] ; gmCellIncX
	add si, word [@lpgm+8] ; gmptGlyphOrigin.x
	sub esi, dword [@lpgm] ; gmptGlyphOrigin.x
	mov [esp+8], esi ; fixme
	popad
	
	xor eax, eax
	cmp ecx, 100
	retn
	
	
@FontHDC:
	db 0x00
	db 0x00
	db 0x00
	db 0x00
@FontSize:
	db 0x00
	db 0x00
	db 0x00
	db 0x00
@FontObject:
	db 0x00
	db 0x00
	db 0x00
	db 0x00
@lpmat2: ; 0x10 bytes
	db 0x00
	db 0x00
	db 0x01
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x01
	db 0x00
@lpgm: ; 0x14 bytes
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
	db 0x00
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
    

<1008c000>
@NewBuffer:
<1008c240>
@NewBuffer2: