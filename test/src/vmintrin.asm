.code

__read_rip proc
    mov rax, [rsp]
    ret
__read_rip endp

__read_rsp proc
    mov rax, rsp
    add rax, 8h
    ret
__read_rsp endp

__read_cs proc
        mov     ax, cs
        ret
__read_cs endp

__read_ss proc
        mov     ax, ss
        ret
__read_ss endp

__read_ds proc
        mov     ax, ds
        ret
__read_ds endp

__read_es proc
        mov     ax, es
        ret
__read_es endp

__read_fs proc
        mov     ax, fs
        ret
__read_fs endp

__read_gs proc
        mov     ax, gs
        ret
__read_gs endp

__read_ldtr proc
        sldt    ax
        ret
__read_ldtr endp

__read_tr proc
        str     ax
        ret
__read_tr endp

__load_ar proc
        lar     rax, rcx
        jz      no_error
        xor     rax, rax
no_error:
        ret
__load_ar endp

end