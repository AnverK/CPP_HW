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
				
				; mov				rdx, 1
				
				XCHG 			rsi, rdi
				
				lea				r15, [rsp + 2*128*8]				; ответ
				lea				r14, [rsp + 3*128*8]				; копия 2 числа
				
				call            mul_long_long
				
				XCHG			rdi, r15
				; mov				rdx, 2
				; mov				rbx, 1
				
				; call			add_with_mul_and_shift

				; call			add_long_long_with_shift
				
				; XCHG			rdi, r15
				
                call            write_long
                mov             al, 0x0a
                call            write_char

                jmp             exit
				
mul_long_long:
				push			r15
				push			r14
				push			rdi
				push			rcx
				push			rsi
				push			rdx
				
				XCHG			rsi, rdi
				XCHG			rdi, r14
				call			copy_long
				XCHG			rdi, r14
				XCHG			rsi, rdi
				
				; в r14 и rdi лежит первый множитель 
				
				; храним в r14 первый множитель. В rdi передаём r15. Внутри: первый множитель *= цифра второго. Значит, нужно сохранять
				; первый множитель, потом восстанавливать его.
				
				mov				rdx, 0
.loop:			
				mov				rbx, [rsi + 8 * rdx]
				
				
				; XCHG			rdi, r15
				call			add_with_mul_and_shift	; результат суммируется к r15, а rsi как был, так и остаётся вторым множителем. Но он мб портится
				; XCHG			rdi, r15

				; XCHG			rsi, rdi
				XCHG			rsi, r14
				call			copy_long
				XCHG			rsi, r14
				; XCHG			rsi, rdi
				
				; XCHG			rdi, r15
				; call			write_long	

				
				inc				rdx
				dec				rcx
								
				jnz				.loop
				
				pop				rdx
				pop				rsi
                pop             rcx
                pop             rdi
				pop				r14
				pop				r15
				
                ret 

; D += S*b (сложение со сдвигом, да)
; rcx -- number length
; rdx -- shift
; rbx -- short multiply operand (b)
; r15 -- adress of current sum (D)
; rdi -- address of multiplier (S)
; rcx -- length of long numbers in qwords
; result:
; sum is written to r15
add_with_mul_and_shift:
				push			r15
				push            rax
				push			rdx
                push            rdi
                push            rcx
				push			rsi
				
				
				; XCHG			r15, rdi
				; call			write_long
				; XCHG			rsi, rdi
				call			mul_long_short	
				; XCHG			rs, rdi	
				; в rsi содержится результат вызова, то есть тут rsi реально портится, это плоховато, но что поделать
				
				
				
				; XCHG			rdi, rsi
				; call			write_long
				
                clc
.loop:
                mov             rax, [rdi]
                lea             rdi, [rdi + 8]
                adc             [r15 + rdx * 8], rax
                lea             r15, [r15 + 8]
                dec             rcx
                jnz             .loop
				
				pop				rsi
                pop             rcx
                pop             rdi
				pop 			rdx
				pop				rax
				pop				r15

                ret 
				
; rcx -- number length
; rdx -- shift
; rdi -- address of summand #1 (long number)
; rsi -- address of summand #2 (long number with shift)
; rcx -- length of long numbers in qwords
; result:
; sum is written to rdi
add_long_long_with_shift:
				push            rax
				push			rdx
                push            rdi
                push            rcx
				push			rsi
				
                clc
.loop:
                mov             rax, [rsi]
                lea             rsi, [rsi + 8]
                adc             [rdi + rdx * 8], rax
                lea             rdi, [rdi + 8]
                dec             rcx
                jnz             .loop
				
				pop				rsi
                pop             rcx
                pop             rdi
				pop 			rdx
				pop				rax
				
				ret 
				
; rsi -- source
; rdi	-- destination
; rcx	-- length			
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
