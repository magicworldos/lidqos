.global _log2x, _atan, _pow2, _sin, _cos, _tan

.section .text

_pow2:
    push   %ebp
   	mov    %esp,%ebp
 	sub    $0x10,%esp
	mov    0x8(%ebp),%eax
 	mov    %eax,-0x8(%ebp)
 	mov    0xc(%ebp),%eax
 	mov    %eax,-0x4(%ebp)
 	fldl   -0x8(%ebp)
    f2xm1
 	fstpl  -0x10(%ebp)
 	fldl   -0x10(%ebp)
    leave
    ret

_log2x:
	push   %ebp
	mov    %esp,%ebp
 	sub    $0x18,%esp
	mov    0x8(%ebp),%eax
 	mov    %eax,-0x8(%ebp)
 	mov    0xc(%ebp),%eax
 	mov    %eax,-0x4(%ebp)
 	mov    0x10(%ebp),%eax
 	mov    %eax,-0x10(%ebp)
 	mov    0x14(%ebp),%eax
 	mov    %eax,-0xc(%ebp)
 	fldl   -0x8(%ebp)
 	fldl   -0x10(%ebp)
 	fyl2x
	fstpl  -0x18(%ebp)
	fldl   -0x18(%ebp)
    leave
    ret

_sin:
    push   %ebp
   	mov    %esp,%ebp
 	sub    $0x10,%esp
	mov    0x8(%ebp),%eax
 	mov    %eax,-0x8(%ebp)
 	mov    0xc(%ebp),%eax
 	mov    %eax,-0x4(%ebp)
 	fldl   -0x8(%ebp)
    fsin
 	fstpl  -0x10(%ebp)
 	fldl   -0x10(%ebp)
    leave
    ret

_cos:
    push   %ebp
   	mov    %esp,%ebp
 	sub    $0x10,%esp
	mov    0x8(%ebp),%eax
 	mov    %eax,-0x8(%ebp)
 	mov    0xc(%ebp),%eax
 	mov    %eax,-0x4(%ebp)
 	fldl   -0x8(%ebp)
    fcos
 	fstpl  -0x10(%ebp)
 	fldl   -0x10(%ebp)
    leave
    ret

_atan:
	push   %ebp
	mov    %esp,%ebp
 	sub    $0x18,%esp
	mov    0x8(%ebp),%eax
 	mov    %eax,-0x8(%ebp)
 	mov    0xc(%ebp),%eax
 	mov    %eax,-0x4(%ebp)
 	mov    0x10(%ebp),%eax
 	mov    %eax,-0x10(%ebp)
 	mov    0x14(%ebp),%eax
 	mov    %eax,-0xc(%ebp)
 	fldl   -0x8(%ebp)
 	fldl   -0x10(%ebp)
 	fpatan
	fstpl  -0x18(%ebp)
	fldl   -0x18(%ebp)
    leave
    ret
