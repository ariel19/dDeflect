[bits 64]; @rax = ntdll!NtQueryInformationProcess			push	rax						or		rcx, -1			mov		rdx, 0x1E			mov		r8, rsp			mov		r9, 8			push	0			sub		rsp, 0x20			call	rax			mov		rax, [rsp + 0x28]			add		rsp, 0x30			