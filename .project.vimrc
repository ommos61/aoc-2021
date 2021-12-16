
set number
set relativenumber

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
map [18;5~    :make<CR>:copen<CR><CR>k
" F8
map [19~      :cnext<CR>
" ctrl-F8
map [19;5~    :cprevious<CR>
" F9
map [20~      :copen<CR>

