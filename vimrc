

"====================================================================================
"--------------------------------------------------
" NeoBundle Init

" Use 256 colors in vim
" some plugins not work without it
set t_Co=256

" Turn off filetype plugins before bundles init
filetype off
" Auto installing NeoNeoBundle
let isNpmInstalled = executable("npm")
" default path for node-modules
let s:defaultNodeModules = '~/.vim/node_modules/.bin/'
let iCanHazNeoBundle=1
let neobundle_readme=expand($HOME.'/.vim/bundle/neobundle.vim/README.md')
if !filereadable(neobundle_readme)
    if !isNpmInstalled
        echo "==============================================="
        echo "Your need to install npm to enable all features"
        echo "==============================================="
    endif
    echo "Installing NeoBundle.."
    silent !mkdir -p $HOME/.vim/bundle
    silent !git clone https://github.com/Shougo/neobundle.vim $HOME/.vim/bundle/neobundle.vim
    let iCanHazNeoBundle=0
endif

" Call NeoBundle
if has('vim_starting')
    set runtimepath+=$HOME/.vim/bundle/neobundle.vim/
endif

call neobundle#begin(expand($HOME.'/.vim/bundle/'))

" Determine make or gmake will be used for making additional deps for Bundles
let g:make = 'gmake'
if system('uname -o') =~ '^GNU/'
    let g:make = 'make'
endif


" Add code static check on write
" need to be properly configured.
" I just enable it, with default config,
" many false positive but still usefull
NeoBundle 'scrooloose/syntastic'
" Install jshint and csslint for syntastic
" Path to jshint if it not installed, then use local installation
if isNpmInstalled
    if !executable(expand(s:defaultNodeModules . 'jshint'))
        silent ! echo 'Installing jshint' && npm --prefix ~/.vim/ install jshint
    endif
    if !executable(expand(s:defaultNodeModules . 'csslint'))
        silent ! echo 'Installing csslint' && npm --prefix ~/.vim/ install csslint
    endif
endif

" Nice statusline/ruler for vim
NeoBundle 'vim-airline/vim-airline'
NeoBundle 'vim-airline/vim-airline-themes'
" JShint :)
" But not necessary with syntastics
NeoBundle 'walm/jshint.vim'

call neobundle#end()

" Enable Indent in plugins
filetype plugin indent on
" Enable syntax highlighting
syntax on

" If there are uninstalled bundles found on startup,
" this will conveniently prompt you to install them.

" disable annoying prompt on initial bundle install
set nomore

" fix issue when github refuse connections on initial install
let g:neobundle#install_max_processes=2

" Install all bundles on first launch
if !iCanHazNeoBundle
    NeoBundleInstall
endif

" Check new bundles on startup
NeoBundleCheck


"====================================================================================

"-------------------------
" vim-airline

" Colorscheme for airline
let g:airline_theme='understated'

" Set custom left separator
let g:airline_left_sep = '▶'

" Set custom right separator
let g:airline_right_sep = '◀'

" Enable airline for tab-bar
let g:airline#extensions#tabline#enabled = 1

" Don't display buffers in tab-bar with single tab
let g:airline#extensions#tabline#show_buffers = 0

" Display only filename in tab
let g:airline#extensions#tabline#fnamemod = ':t'

" Don't display encoding
let g:airline_section_y = ''

" Don't display filetype
let g:airline_section_x = ''

" General options

" Enable per-directory .vimrc files and disable unsafe commands in them
"set exrc secure

" Set up leader key <leader>, i use default \
"let mapleader = ','

" Buffer will be hidden instead of closed when no one display it
"set hidden

" Auto reload changed files
set autoread

" Always change current dirrectory to current-editing-file dir
"set autochdir

" Indicates fast terminal connection
set ttyfast

" Set character encoding to use in vim
set encoding=utf-8

" Let vim know what encoding we use in our terminal
set termencoding=utf-8

" Which EOl used. For us it's unix
" Not works with modifiable=no
if &modifiable
    set fileformat=unix
endif


" Enable Tcl interface. Not shure what is exactly mean.
" set infercase

" Interprete all files like binary and disable many features.
" set binary

"--------------------------------------------------
" Wildmenu

" Extended autocmpletion for commands
set wildmenu

" Autocmpletion hotkey
set wildcharm=<TAB>


""set guifont=Courier_New:h10:cANSI   " 设置字体  
set showcmd         " 输入的命令显示出来，看的清楚些  
set novisualbell    " 不要闪烁(不明白)  
set statusline=%F%m%r%h%w\ [FORMAT=%{&ff}]\ [TYPE=%Y]\ [POS=%l,%v][%p%%]\ %{strftime(\"%d/%m/%y\ -\ %H:%M\")}   "状态行显示的内容  
set laststatus=1    " 启动显示状态行(1),总是显示状态行(2) 
set nocompatible  "去掉讨厌的有关vi一致性模式，避免以前版本的一些bug和局限  
" 显示中文帮助
if version >= 603
    set helplang=cn
    set encoding=utf-8
endif
set fencs=utf-8,ucs-bom,shift-jis,gb18030,gbk,gb2312,cp936
set termencoding=utf-8
set encoding=utf-8
set fileencodings=ucs-bom,utf-8,cp936
set fileencoding=utf-8

"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

"代码补全 
"set completeopt=preview,menu 
"允许插件  
filetype plugin on
"从不备份  
set nobackup
"make 运行
:set makeprg=g++\ -Wall\ \ %
set magic                   " 设置魔术
set guioptions-=T           " 隐藏工具栏
set guioptions-=m           " 隐藏菜单栏
" 不要使用vi的键盘模式，而是vim自己的
set nocompatible
" 语法高亮
set syntax=on
" 去掉输入错误的提示声音
set noeb
" 在处理未保存或只读文件的时候，弹出确认
set confirm
" 自动缩进
"set autoindent
"set cindent
" Tab键的宽度
set tabstop=4
" 统一缩进为4
set softtabstop=8
set shiftwidth=4
" 不要用空格代替制表符
set noexpandtab
" 在行和段开始处使用制表符
set smarttab
" 历史记录数
set history=1000
"禁止生成临时文件
set nobackup
set noswapfile
"搜索逐字符高亮
set hlsearch
set incsearch
"http://blog.csdn.net/ariessurfer/article/details/8135775  修改查询单词颜色
hi Search term=standout ctermfg=0 ctermbg=3  
"行内替换
set gdefault
"编码设置
set enc=utf-8

set fencs=utf-8,ucs-bom,shift-jis,gb18030,gbk,gb2312,cp936

"语言设置

set langmenu=zh_CN.UTF-8
set helplang=cn
" 总是显示状态行

set laststatus=2

" 命令行（在状态行下）的高度，默认为1，这里是2

set cmdheight=1

" 侦测文件类型

filetype on

" 载入文件类型插件

filetype plugin on

" 为特定文件类型载入相关缩进文件

""filetype indent on
" 带有如下符号的单词不要被换行分割

set iskeyword+=_,$,@,%,#,-
" 高亮显示匹配的括号

""set showmatch

" 匹配括号高亮的时间（单位是十分之一秒）

"set matchtime=1

" 光标移动到buffer的顶部和底部时保持3行距离

set scrolloff=3

" 为C程序提供自动缩进

"set smartindent

" 高亮显示普通txt文件（需要txt.vim脚本）

"au BufRead,BufNewFile *  setfiletype txt

"自动补全

"":inoremap ( ()<ESC>i

"":inoremap ) <c-r>=ClosePair(')')<CR>

"":inoremap { {<CR>}<ESC>O

"":inoremap } <c-r>=ClosePair('}')<CR>

"":inoremap [ []<ESC>i

"":inoremap ] <c-r>=ClosePair(']')<CR>

"":inoremap " ""<ESC>i

"":inoremap ' ''<ESC>i

""function! ClosePair(char)

""    if getline('.')[col('.') - 1] == a:char

""        return "\<Right>"

""    else

""        return a:char

""    endif

""endfunction

""filetype plugin indent on 

"打开文件类型检测, 加了这句才可以用智能补全

""set completeopt=longest,menu

"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

" CTags的设定  

"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

let Tlist_Sort_Type = "name"    " 按照名称排序  

let Tlist_Use_Right_Window = 1  " 在右侧显示窗口  

let Tlist_Compart_Format = 1    " 压缩方式  

let Tlist_Exist_OnlyWindow = 1  " 如果只有一个buffer，kill窗口也kill掉buffer  

let Tlist_File_Fold_Auto_Close = 0  " 不要关闭其他文件的tags  

let Tlist_Enable_Fold_Column = 0    " 不要显示折叠树  

autocmd FileType java set tags+=D:\tools\java\tags  

"autocmd FileType h,cpp,cc,c set tags+=D:\tools\cpp\tags  

"let Tlist_Show_One_File=1            "不同时显示多个文件的tag，只显示当前文件的

"设置tags  

set tags=tags  

"set autochdir 
" When editing a file, always jump to the last cursor position.
autocmd BufReadPost * if line("'\"") && line("'\"") <= line("$")
			\ | exe "normal `\"" | endif

set cscopetag

if has("cscope")
	cscope add cscope.out
	set cscopequickfix=s-,c-,d-,i-,t-,e-
	set csto=0
	set cst
	set csverb
endif

set nocompatible              " be iMproved
filetype off                  " required!

" set the runtime path to include Vundle and initialize
"set rtp+=~/.vim/bundle/Vundle.vim
"call vundle#begin()

" alternatively, pass a path where Vundle should install plugins
"call vundle#begin('~/some/path/here')
" let Vundle manage Vundle, required
"Bundle 'gmarik/vundle'

"Bundle 'Valloric/YouCompleteMe'
"Bundle 'Valloric/ListToggle'
"Bundle 'scrooloose/syntastic'


"filetype plugin indent on     " required!

"colorscheme ron
"colorscheme delek

"colorscheme darkblue

"colorscheme default


" 显示行号
set number

"=======================================dannywang===========================
" set background=dark
" We know xterm-debian is a color terminal
if &term =~ "xterm-debian" || &term =~ "xterm-xfree86"
  set t_Co=16
  set t_Sf=[3%dm
  set t_Sb=[4%dm
endif

"	read/write a .viminfo file, don't store more than
"set viminfo='64,"128,n~/.viminfo
set viminfo=%,'64,\"128,:128,n~/.viminfo

set tags=tags,TAGS,ctags
filetype plugin indent on

if exists("*Lcpp") | finish | endif

" When editing a file, always jump to the last cursor position.
autocmd BufReadPost * if line("'\"") && line("'\"") <= line("$")
			\ | exe "normal `\"" | endif

augroup cprog
  " Set some sensible defaults for editing C-files
  " Remove all cprog autocommands
  au!

  " When starting to edit a file:
  "   For *.c and *.h files set formatting of comments and set C-indenting on.
  "   For other files switch it off.
  "   Don't change the order, it's important that the line with * comes first.
  "autocmd BufRead *.cpp,*.c,*.h 1;/^{
  autocmd BufNewFile *.c,*.C,*.cc,*.cpp,*.cxx,*.h,*.hpp  call Lcpp()
  autocmd BufNewFile *.sh  call Lsh()
  autocmd BufEnter *.cpp,*.c,*.h abbr _dtrace
	\ #define dtrace	do { fprintf(stdout, "\033[36mTRACE"      \<CR>
	\				"\033[1;34m==>\033[33m%16s"       \<CR>
	\				"\033[36m: \033[32m%4d\033[36m: " \<CR>
	\				"\033[35m%-24s \033[34m"          \<CR>
	\				"[\033[0;37m%s\033[1;34m,"        \<CR>
	\				" \033[0;36m%s\033[1;34m]"        \<CR>
	\				"\033[0m\n", __FILE__, __LINE__,  \<CR>
	\				__FUNCTION__ /* __func__ */,      \<CR>
	\				__TIME__, __DATE__);              \<CR>
	\			} while (0)          /* defined by mhfan */
  autocmd BufLeave *.cpp,*.c,*.h unabbr _dtrace

  imap <F4> <C-R>=strftime("/* dannywang@zhytek.com %Y-%m-%d %H:%M */")<CR>

  command Ma : call Main_Add()
  func Main_Add() " add main info
  	let l=line(".")
	call append(l+0, "#include <stdio.h>")
	call append(l+1, "#include <stdlib.h>")
	call append(l+2, "#include <string.h>")
	call append(l+3, "#include <unistd.h>")
	call append(l+4, "int main(int argc, char *argv[])")
	call append(l+5, "{")
	call append(l+6, "")
	call append(l+7, "")
	call append(l+8, "")
	call append(l+9, "    return 0;")
	call append(l+10, "}")
  	exec l+6 .   ""
  endf

  command Dc : call DCOMM()
  func DCOMM()  " delete the block comment macro lines.
  	exec "normal 1l"
  	let l1 = searchpair('^\s*#\s*if\s\+\d\+', '', '^\s*#\s*endif', 'Wb')
  	if l1 < 1
  		return
  	endif
  	exec "normal ]#"
  	if getline(".") =~ '^\s*#\s*else'
  		return
  	endif
  	exec "normal dd" . l1 . "Gdd"
  endf
  
  command Rc :call RCOMM()
  func RCOMM()  " reverse the block comment.
  	exec "normal 1l"
  	if searchpair('^\s*#\s*if\s\+\d\+', '', '^\s*#\s*enif', 'Wb') < 1
  		return
  	endif
  	exec '.s#\d\+#\=submatch(0)==0 ? 1 : 0#'
  endf
  
  command -range Co : call  COMM(<line1>,<line2>)
  func COMM(l1, l2) " add the MACRO comment around the block of C/Cpp code.
"  	  exec a:l2+1 . \"s%^%#endif    /* comment by WangGang */\<CR>%\"
  	  exec a:l2+1 . "s%^%#endif\<CR>%"
  	  exec a:l1 .   "s%^%#if 0     /* by danny ZHYTEK on ".strftime("%Y-%m-%d")." */\<CR>%"
  endf

  command CC :call CCOMM()
  func CCOMM()	" convert #define <<-->> #undef
  	let iLn=line(".")
  	let strLn=getline(".")
	if strLn =~ '^\s*#\s*define'
		let strNew=substitute(strLn, "define", "undef", "")
		exec setline(iLn, strNew)
		exec iLn
	elseif strLn =~ '^\s*#\s*undef'
		let strNew=substitute(strLn, "undef", "define", "")
		exec setline(iLn, strNew)
		exec iLn
	endif
  endf

  command PT :call PTRACE()
  func PTRACE()	" ptrace func for cursor

  endf

  func Lcpp()
  	call Title("c")		" diff commect char
  endfun

  func Lsh()
  	call Title("sh")	" comment char
  endfun

  func Title(type)
    let ctype=a:type
  	if strridx(ctype, "sh") == 0
	  let fch="#! \\/bin\\/bash\\r#"
	  let cch="#"
	else
	  let fch="\\/"
	  let cch="*"
	endif

    let fn = strpart(@%, strridx(@%, "/") + 1)
    if strridx(fn,"\.h") > 0
	  let defn = substitute(toupper(fn), "\\.H", "_DEF_H", "")
	  let defh = "#ifndef  __" . defn . "\r#define  __" . defn . "\r"
    else
	  let defh = "\r\r"
    endif

    execute "lang time en_US.UTF8"
    let rspace = "                                      "
    execute "1g/^/s//" .
	\"".fch."********************************************************************\r" .
	\"".cch." $ID: " . fn . strpart(rspace, 0, 22-strlen(fn)-strlen($USER))   .
	\         strftime(" %a %Y-%m-%d %H:%M:%S %z  ") . $USER "      *\r" .
	\"".cch."                                                                   *\r" .
	\"".cch." Description:                                                      *\r" .
	\"".cch."                                                                   *\r" .
	\"".cch." Maintainer:  (lizhu)  <lizhu@zhytek.com>                 *\r" .
	\"".cch."                                                                   *\r" .
	\"".cch." CopyRight (c)". strftime(" %Y ") . "ZHYTEK   "   . rspace .      "*\r" .
	\"".cch."   All rights reserved.                                            *\r" .
	\"".cch."                                                                   *\r" .
	\"".cch." This file is free software;                                       *\r" .
	\"".cch.'   you are free to modify and\/or redistribute it                   *\r' .
	\"".cch."   under the terms of the GNU General Public Licence (GPL).        *\r" .
	\"".cch."                                                                   *\r" .
	\"".cch." Last modified:                                                    *\r" .
	\"".cch."                                                                   *\r" .
	\"".cch." No warranty, no liability, use this at your own risk!             *\r" .
	\"".cch."*******************************************************************\\/".
	\"\r" . defh

    if strridx(fn,"\.h") > 0
      let defh="\r\r\r#endif\\/\\/ __" . defn . "\r"
    endif

 	if strridx(ctype, "sh") == 0
	  let fch="#"
	else
	  let fch="\\/"
	endif

    let rspace = "****************************"
	execute "$g/$/s//" . defh . fch .
		\ strpart(rspace, 0, 24-strlen(fn)/2) . " End Of File: " . fn . " " .
		\ strpart(rspace, 0, 24-strlen(fn)/2) . '\/'

	execute 19
  endfun
augroup END
