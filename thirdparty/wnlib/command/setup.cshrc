if ($?prompt) then
  # this is login shell

  set filec
  set fignore = (.o .out .a)
  set ignoreeof
  set nonomatch
  set history=100

  # clear path, start path
  set path   
  set path=( $path . $wnlib/command ~/bin /usr/bsd /usr/ucb /usr/5bin /usr/local/bin /usr/local/src/rcs/bin /usr/bin /bin )

  set prompt="`whoami`@`hostname`[\!] "

  set mail=$mm/usr/spool/mail/$USER
#  set TERM=aaa-35
endif

alias ls ls -F

alias \> pushd
alias \< popd

