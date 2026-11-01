#!
TOKEN_PATH=~/.token
EMAIL='efallletti@gmail.com'
NAME='oneeyedoll'
export GPG_TTY=$(tty)

print_options() {
    printf "bckp-tlk [OPTION]... [ARGS]..."
    printf "\n\n"
    printf "Options: \n"
    printf " -s [PASSWORD]                     Synchronize with GitHub\n"
    printf " -h                                Show this menu\n"
    printf " -i [PATH] [PASSWORD] [TOKEN]      Initialize\n"
}
dotfiles_func() {
  /usr/bin/git --git-dir="$HOME/.dotfiles/" --work-tree="$HOME"
}
if test "$#" -ge 1 ; then
  # Assign the parameters, then proceed to generate PGP Key
  # It will use name and email as arbitrary values
  if [ "$1" = "-i" ] ; then
    if test "$#" -ne 4 ; then
      echo "Missing arguments. You need to provide a path to a file containing the dotfiles location, the GPG password, and the GitHub token"
    fi
    dotfiles_path=$2
    password=$3
    token=$4
    
    echo "$dotfiles_path" > "$DOTFILES_PATH"
    gpg -v --quick-gen-key --batch --passphrase "$password" --yes "$NAME (ILOVEYOU) <$EMAIL>" default default never
    echo "$token" > "$TOKEN_PATH"
    gpg --encrypt --recipient "$EMAIL" "$TOKEN_PATH"
    rm "$TOKEN_PATH"
  elif [[ "$1" = "-s" ]]; then
    # Enumerate the files and then add them to the dotfiles staged changes. Finally, decrypt the GH token and push in the dotfiles repo
    password=$2
    dotfiles_tree=$(cat "$DOTFILES_PATH")
    token=$(echo "$password" | gpg  -v --batch --pinentry-mode=loopback --passphrase-fd 0 --decrypt "$TOKEN_PATH".gpg) 
    while read p; do 
      /usr/bin/git --git-dir="$HOME/.dotfiles/" --work-tree="$HOME" add "$p"
      echo "$p"
    done < "$dotfiles_tree"
    git --git-dir="$HOME/.dotfiles/" --work-tree="$HOME" commit -m "Updated"
    git --git-dir="$HOME/.dotfiles/" --work-tree="$HOME" push https://"$NAME":"$token"@github.com/$NAME/dotfiles.git nix 
  else
    print_options
  fi
else
  print_options
fi
