syntax on

set tabstop=4

set listchars+=space:-
set list

set tags=/root/project/tags

set previewheight=150
nnoremap <C-]> :execute "vertical ptag " . expand("<cword>")<CR>

" Auto generate tags file on file write of *.c and *.h files
autocmd BufWritePost *.cpp,*.h !ctags --exclude=/root/project/build -f /root/project/tags -R /root/project/  
