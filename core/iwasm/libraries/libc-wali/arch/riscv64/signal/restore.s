.global __libc_restore_rt
.global __libc_restore
.type __libc_restore_rt,@function
.type __libc_restore,@function
__libc_restore_rt:
__libc_restore:
	li a7, 139 # SYS_rt_sigreturn
	ecall

