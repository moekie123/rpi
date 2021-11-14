PROJECT_DIR='/root/project'

# Navigation shorcuts
alias cdp='cd $PROJECT_DIR/'
alias cdb='cd $PROJECT_DIR/build'
alias cdbi='cd $PROJECT_DIR/bin'

alias cds='cd $PROJECT_DIR/src'
alias cdsct='cd $PROJECT_DIR/src/controller'
alias cdscl='cd $PROJECT_DIR/src/client'

alias cdi='cd $PROJECT_DIR/include'
alias cdim='cd $PROJECT_DIR/include/mocks'

alias cdc='cd $PROJECT_DIR/config'

# Docker shortcuts
alias dcu='docker-compose -f $PROJECT_DIR/docker-compose.yml up -d'
alias dcd='docker-compose -f $PROJECT_DIR/docker-compose.yml stop'
alias dcb='docker-compose build'
alias dca='docker attach environment'

# Build shortcuts
alias build='make -C $PROJECT_DIR/build/ --no-print-directory'
alias verify='make -C $PROJECT_DIR/build/ --no-print-directory test'
alias format='make -C $PROJECT_DIR/build/ --no-print-directory clangformat'

# Database
alias db='psql -h database -d postgres -U postgres'

# Include binaries
export PATH=$PROJECT_DIR/bin:$PATH
