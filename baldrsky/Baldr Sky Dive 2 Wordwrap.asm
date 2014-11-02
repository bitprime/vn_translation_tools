<0060227F>
	jmp @CheckWordwrap
	nop
	nop
@WordwrapRet:

<00bc1000>
@WordwrapFlag:
	db 00
	db 00
	db 00
	db 00

	db 90
	db 90
	db 90
	db 90
	
@CheckWordwrap:
	pushad
	
; Clear wordwrap flag 
	mov byte [@WordwrapFlag], 0
	mov ebx, dword [esi+1f8]
	
	cmp byte [edi], 40
	je @SkipWordwrap
	
	cmp byte [edi], 20
	jne @SkipWordwrap
	inc edi
	
; Calculate how much 
@WordwrapLoop:
	cmp byte [edi], 40
	je @SkipWordwrap
	
	cmp byte[edi], 00
	je @SkipWordwrap
	
	cmp byte [edi], 7f
	jbe @ASCIIChar
	inc edi
	jmp @TailLoop
	
	
@ASCIIChar:
	cmp byte [edi], 20
	je @SkipWordwrap

@TailLoop:
	add ecx, ebx
	inc edi
	
	cmp eax, ecx
	jge @WordwrapLoop
	
	cmp byte [edi], 0
	je @SkipWordwrap
	
	mov byte [@WordwrapFlag], 1
	

; Must be wordwrapped so set flag
	
@SkipWordwrap:
	popad
	
; Check if we need to wordwrap or not 
	cmp byte [@WordwrapFlag], 1
	je 00602312
	
	test byte [esi+1a8], 27
	jmp @WordwrapRet	