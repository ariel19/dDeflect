[bits 32]
; @eax = ntdll!NtQueryInformationProcess


			push	0
			
			push	0
			push	4			; ProcessInformationLength
			push	esp
			add		DWORD [esp], 8
			push	0x1F		; ProcessDebugFlags
			push	-1
			call	eax
			
			pop		ecx
			xor		eax, eax
			test	ecx, ecx
			setz	al
			
			