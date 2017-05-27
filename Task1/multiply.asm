                section         .text

                global          _start
_start:
                sub             rsp, 4 * 128 * 8
                lea             rdi, [rsp + 128 * 8]	
                mov             rcx, 128
                call            read_long
                mov             rdi, rsp
                call            read_long
                lea             rsi, [rsp + 128 * 8]	
				
				lea				rax, [rsp + 2*128*8]
				lea				rbx, [rsp + 3*128*8]	
				
				call            mul2_long_long

                call            write_long
                mov             al, 0x0a
                call            write_char

                jmp             exit

mul2_long_long:
				
				push			rax
                push            rdi
                push            rcx
				
				XCHG			rax, rdi
				call			set_zero
				XCHG			rax, rdi ; теперь в rax начало нашего ответа, ещё и нулёвое, в rdi по-прежнему первый множитель
				
				push			rsi
				push			rdi
				
				mov				rsi, rdi	; теперь источник копирования это rdi
				XCHG			rdi, rbx	; теперь копия будет в rbx
				call			copy_long		
				XCHG			rbx, rdi	
				
				pop				rdi
				pop				rsi
				
				; до этого момента всё идеально (upd. уже везде всё топ)
.loop:

; rsi -- начало второго множителя 
; rax -- начало результата
				
				push			rcx
				mov				rcx, 128
				
				push			rbx
				mov				rbx, [rsi]			; rbx готовится быть умноженным на длинное число (на rdi)				
				call			mul_long_short		; попорчен rdi -- там теперь первый множитель, умноженный на rbx лежит, но нас это не смущает, ибо rbx есть
				pop				rbx
				
				mov				rdx, [rsp]
				
				push			rbx					; копию начала числа сохраняем
				cmp				rdx, rcx
				
				jge				.ok
.mul_2:			
				mov				rbx, 9223372036854775808			; 2^63
				call			mul_long_short
				mov				rbx, 2
				call			mul_long_short						; так умножили rdi в 2^64 раза, то есть на разряд наш
				inc				rdx						
				cmp				rdx, rcx
				jl				.mul_2	
.ok:			
				pop				rbx					; копию начала числа возвращаем обратно	
													; в rdi лежит длинное слагаемое после произведения (rdi = rbx * rdi * 2^(64*(128-rdx))) 
													; в rcx длина, ок, в rsi надо на текущий результат (rax)
				push			rsi					; текущий разряд множителя 
				
				mov				rsi, rax			; временно пихнули в rsi указатель на ответ
				call			add_long_long		
				
				XCHG			rax, rdi
				
				mov				rsi, rbx

				call			copy_long			; теперь в rdi снова всё как в начале цикла -- длинный множитель				
				
				pop				rsi
				pop				rcx
				lea             rsi, [rsi + 8]
				dec				rcx		
				jnz				.loop
				
				
				pop				rcx
				mov				rsi, rax
				call			copy_long
				pop rdi
				pop rax
				ret
			
;	rsi -- source
;	rdi	-- destination
;	rcx	-- length			
copy_long:
				push			rax
				push			rdi
				push			rcx
				push			rsi
.loop:			
				mov				rax, [rsi]
				mov				[rdi], rax
				lea				rsi, [rsi+8]
				lea				rdi, [rdi+8]
				dec				rcx
				jnz				.loop
				
				pop				rsi
				pop				rcx
				pop				rdi
				pop				rax
				ret
; adds two long number
;    rdi -- address of summand #1 (long number)
;    rsi -- address of summand #2 (long number)
;    rcx -- length of long numbers in qwords
; result:
;    sum is written to rdi
add_long_long:
                
                push            rax
                push            rdi
                push            rcx
				push			rsi
				
                clc
.loop:
                mov             rax, [rsi]
                lea             rsi, [rsi + 8]
                adc             [rdi], rax
                lea             rdi, [rdi + 8]
                dec             rcx
                jnz             .loop
				
				pop				rsi
                pop             rcx
                pop             rdi
				pop				rax
				
                ret

; adds 64-bit number to long number
;    rdi -- address of summand #1 (long number)
;    rax -- summand #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    sum is written to rdi
add_long_short:
                push            rax
                push            rdi
                push            rcx
				push			rdx

                xor             rdx,rdx
.loop:
                add             [rdi], rax
                adc             rdx, 0
                mov             rax, rdx
                xor             rdx, rdx
                add             rdi, 8
                dec             rcx
                jnz             .loop
				
				pop				rdx	
                pop             rcx
                pop             rdi
                pop             rax
                ret

; multiplies long number by a short
;    rdi -- address of multiplier #1 (long number)
;    rbx -- multiplier #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    product is written to rdi
mul_long_short:
                push            rax
                push            rdi
                push            rcx
				push			rdx
				push			rsi
				push			rbx
				
                xor             rsi, rsi
.loop:
                mov             rax, [rdi]
                mul             rbx
                add             rax, rsi
                adc             rdx, 0
                mov             [rdi], rax
                add             rdi, 8
                mov             rsi, rdx
                dec             rcx
                jnz             .loop
				
				pop				rbx
				pop				rsi
				pop				rdx
                pop             rcx
                pop             rdi
                pop             rax
                ret

; divides long number by a short
;    rdi -- address of dividend (long number)
;    rbx -- divisor (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    quotient is written to rdi
;    rdx -- remainder
div_long_short:
                push            rdi
                push            rax
                push            rcx

                lea             rdi, [rdi + 8 * rcx - 8]
                xor             rdx, rdx

.loop:
                mov             rax, [rdi]
                div             rbx
                mov             [rdi], rax
                sub             rdi, 8
                dec             rcx
                jnz             .loop

                pop             rcx
                pop             rax
                pop             rdi
                ret

; assigns a zero to long number
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
set_zero:
                push            rax
                push            rdi
                push            rcx

                xor             rax, rax
                rep stosq

                pop             rcx
                pop             rdi
                pop             rax
                ret

; checks if a long number is a zero
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
; result:
;    ZF=1 if zero
is_zero:
                push            rax
                push            rdi
                push            rcx

                xor             rax, rax
                rep scasq

                pop             rcx
                pop             rdi
                pop             rax
                ret

; read long number from stdin
;    rdi -- location for output (long number)
;    rcx -- length of long number in qwords

read_long:
                push            rcx
                push            rdi

                call            set_zero
.loop:
                call            read_char
                or              rax, rax
                js              exit
                cmp             rax, 0x0a
                je              .done
                cmp             rax, '0'
                jb              .invalid_char
                cmp             rax, '9'
                ja              .invalid_char

                sub             rax, '0'
                mov             rbx, 10
                call            mul_long_short
                call            add_long_short
                jmp             .loop

.done:
                pop             rdi
                pop             rcx
                ret

.invalid_char:
                mov             rsi, invalid_char_msg
                mov             rdx, invalid_char_msg_size
                call            print_string
                call            write_char
                mov             al, 0x0a
                call            write_char

.skip_loop:
                call            read_char
                or              rax, rax
                js              exit
                cmp             rax, 0x0a
                je              exit
                jmp             .skip_loop

; write long number to stdout
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
write_long:
                push            rax
                push            rcx

                mov             rax, 20
                mul             rcx
                mov             rbp, rsp
                sub             rsp, rax

                mov             rsi, rbp

.loop:
                mov             rbx, 10
                call            div_long_short
                add             rdx, '0'
                dec             rsi
                mov             [rsi], dl
                call            is_zero
                jnz             .loop

                mov             rdx, rbp
                sub             rdx, rsi
                call            print_string

                mov             rsp, rbp
                pop             rcx
                pop             rax
                ret

; read one char from stdin
; result:
;    rax == -1 if error occurs
;    rax \in [0; 255] if OK
read_char:
                push            rcx
                push            rdi

                sub             rsp, 1
                xor             rax, rax
                xor             rdi, rdi
                mov             rsi, rsp
                mov             rdx, 1
                syscall

                cmp             rax, 1
                jne             .error
                xor             rax, rax
                mov             al, [rsp]
                add             rsp, 1

                pop             rdi
                pop             rcx
                ret
.error:
                mov             rax, -1
                add             rsp, 1
                pop             rdi
                pop             rcx
                ret

; write one char to stdout, errors are ignored
;    al -- char
write_char:
                sub             rsp, 1
                mov             [rsp], al

                mov             rax, 1
                mov             rdi, 1
                mov             rsi, rsp
                mov             rdx, 1
                syscall
                add             rsp, 1
                ret

exit:
                mov             rax, 60
                xor             rdi, rdi
                syscall

; print string to stdout
;    rsi -- string
;    rdx -- size
print_string:
                push            rax

                mov             rax, 1
                mov             rdi, 1
                syscall

                pop             rax
                ret


                section         .rodata
invalid_char_msg:
                db              "Invalid character: "
invalid_char_msg_size: equ             $ - invalid_char_msg
