source /usr/share/vim/vimrc
source $HOME/.vimrc

set autowrite

set makeprg=$HOME/src/AdventOfCode_2021/build.sh

"    map QP   <F1>
"    map QQ   <F2>
"    map QR   <F3>
"    map QS   <F4>
"    map [16~ <F5>
"    map [17~ <F6>
"    map [18~ <F7>
"    map [19~ <F8>
"    map [20~ <F9>
"    map [21~ <F10>
"    map [23~ <F11>
"    map [24~ <F12>

" ctrl-F5
map [15;5~    :!$HOME/src/AdventOfCode_2021/run.sh
" ctrl-F7
map [18;5~    :make
" F8
map [19~      :cnext
" ctrl-F8
map [19;5~    :cprevious
" F9
map [20~      :copen

function! ManPage()
    if &ft =~ "c"
        let s:template = "%"
    endif
    let s:wordUnderCursor = expand("<cword>")
    let s:cmd = "!man " . s:wordUnderCursor
    execute s:cmd
endfunction
map <A-m> :call ManPage()

