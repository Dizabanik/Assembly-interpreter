mov r, 3   ; instruction mov r, 3
mov m, 6   ; instruction mov m, 6
call func
msg 'Random result: ', l
end
func:
	cmp r, m
	jge exit
	mov l, r
	add l, m
	ret
; Do nothing
exit:
	msg 'Do nothing'