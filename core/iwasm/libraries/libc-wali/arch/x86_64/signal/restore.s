.global __libc_restore_rt
.global __libc_restore
.type __libc_restore_rt,@function
.type __libc_restore,@function
__libc_restore_rt:
__libc_restore:
	movl $15, %eax
	syscall
