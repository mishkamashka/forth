section .rodata
msg:
  .no_such_word: db "! No such word: ", 0

section .bss
word_buf:  resq 512
user_dict:  resq 65536
section .data
here:  dq user_dict
fd: dq 0

%define STATE_COMPILE 1
%define STATE_INTERPRET 0
state: dq STATE_INTERPRET

const "forth-here", here
const "forth-last-word", last_word
const "forth-is-compiling", state
const "forth-input-fd", fd
const "forth-stack-start", stack_start

;module to forth interpretator and words to work with dictinory

;add name to word on dictinory
colon "forth-dict-word-name", forth_dict_word_name
          dq xt_lit, 9, xt_plus, xt_exit

;word in dictinory ? if yes ret 1
colon "forth-find-word", forth_find_word
          dq xt_to_r, xt_last_word, xt_fetch
          dq xt_state, xt_fetch
          branch0 .loop
          dq xt_fetch
.loop:
          dq xt_dup
          branch0 .not_found

          dq xt_dup, xt_forth_dict_word_name, xt_r_fetch, xt_string_equals, xt_not
          branch0 .found

          dq xt_fetch
          branch .loop
.found:
          dq xt_lit, 1
.not_found:
      dq xt_from_r, xt_drop, xt_exit

;read word and pale it xt adress on stack
colon "'", forth_tick, 1
          dq xt_fd, xt_fetch, xt_lit, word_buf, xt_file_read_word
          branch0 .eof
          dq xt_lit, word_buf, xt_forth_find_word
          branch0 .no_word
          dq xt_forth_cfa, xt_state
          branch0 .interp
          dq xt_lit, xt_lit, xt_comma, xt_comma
.interp:
          dq xt_exit
.no_word:
      dq xt_lit, msg.no_such_word, xt_print_string, xt_print_string, xt_print_cr, xt_exit
.eof:
      dq xt_drop, xt_exit

;add inft to word place in dictinory
colon "forth-cfa", forth_cfa
          dq xt_lit, 9, xt_plus
          dq xt_dup, xt_string_length, xt_plus
          dq xt_lit, 2, xt_plus, xt_exit
; ( flags name - ) prepare macjine to add new word on dictinory
colon "forth-create", forth_create
          dq xt_here, xt_fetch                ; word start
          dq xt_last_word, xt_fetch, xt_comma ; fill `link`
          dq xt_last_word, xt_save           ; update `last_word`
          dq xt_lit, 0, xt_char_comma
          dq xt_dup, xt_here, xt_fetch, xt_swap, xt_string_copy
          dq xt_string_length, xt_here, xt_fetch, xt_plus, xt_inc, xt_here, xt_save
          dq xt_char_comma
          dq xt_exit

;check that we have word after branch or branch0
colon "forth-follows-branch", forth_follows_branch
          dq xt_here, xt_fetch, xt_lit, 8, xt_sub, xt_fetch
          dq xt_dup, xt_lit, xt_branch0, xt_equal, xt_swap
          dq xt_lit, xt_branch, xt_equal, xt_lor, xt_exit

 ; (  fd -- ) file readet to forth
colon "forth-interpret-fd", forth_interpret_fd
    dq xt_lit, xt_fd, xt_fetch, xt_to_r
    dq xt_fd, xt_save

    .loop:
    dq xt_fd, xt_fetch, xt_lit, word_buf, xt_file_read_word
    branch0 .eof

    dq xt_lit, word_buf, xt_forth_interpret

    branch .loop

    .eof:
    dq xt_from_r, xt_fd, xt_save, xt_exit

;chech to go swith interpretator mod
colon "forth-xt-is-immediate", forth_xt_is_immediate
    dq xt_lit, 1, xt_sub, xt_fetch_char, xt_exit

; ( str - ) the interpreter itself accepts tokens from the prasser, selects the mode of operation and saves and executes the words
colon "forth-interpret", forth_interpret
.start:
    ; ( inbuf )
    dq xt_dup, xt_forth_find_word         ; find in dict
    branch0 .number                       ; ( inbuf dictaddr? 0/1 )
    dq xt_swap, xt_drop, xt_forth_cfa     ; ( xt )

    ; if not compiling
    dq xt_state, xt_fetch
    branch0 .interpreting

    dq xt_dup, xt_forth_xt_is_immediate, xt_not
    branch0 .interpreting

.compiling:
    dq xt_comma, xt_exit
.interpreting:
    dq xt_execute, xt_exit

.number: ; ( inbuf )
    dq xt_dup, xt_string_parse_number      ; ( inbuf num? 0/len )
    branch0 .no_word

    ; ( inbuf num )
    dq xt_swap, xt_drop
    dq xt_state, xt_fetch                  ; if interpreting, that's it
    branch0 .exit

    dq xt_forth_follows_branch, xt_not
    branch0 .is_branch
.not_branch:
    dq xt_lit, xt_lit, xt_comma
.is_branch: ; ( num prev_xt )
    dq xt_comma, xt_exit

.no_word: ; ( inbuf )
  dq xt_lit, msg.no_such_word, xt_print_string, xt_print_string, xt_print_cr

.exit:
    dq xt_exit

colon "trap", trap
    dq xt_lit, wh_trap + 9, xt_print_string, xt_print_cr, xt_exit

;set start state to machine
forth_init:
        dq xt_lit, 0, xt_forth_interpret_fd, xt_bye

