NOTES_PATH=/home/edo/notes/
RUN_APP="Run App"
NOTES="Notes"
USEFUL_LINKS="Useful Links"

declare -A LINKS
LINKS["WeBeep"]="https://webeep.polimi.it/"
LINKS["Instagram"]="https://www.instagram.com/"
LINKS["PokemonShowdown"]="https://play.pokemonshowdown.com"
LINKS["Telegram"]="https://web.telegram.org/"

function dmenu_cmd() {
  dmenu -nb '#673147' -sb '#CF9FFF' -sf '#000000'
}

function handle_run() {
  dmenu_run -nb '#673147' -sb '#CF9FFF' -sf '#000000'
}

function print_links() {
  for name in ${!LINKS[@]};
  do
    printf "$name\n"
  done
}

function handle_links() {
  sel_name=$(print_links | dmenu_cmd)  
  for name in ${!LINKS[@]}
  do
    if [ "$sel_name" = "$name" ] ;
    then
      ungoogled-chromium ${LINKS["$name"]}
    fi
  done
}
function handle_notes() {
  NOTE_PATH="$NOTES_PATH""$(ls "$NOTES_PATH" | dmenu_cmd)"
  dmenu_out="initial"
  while [[ -d "$NOTE_PATH" ]] && [ -n "$dmenu_out" ]; 
  do
    dmenu_out=$(ls "$NOTE_PATH" | dmenu_cmd)
    NOTE_PATH="$NOTE_PATH/$dmenu_out"
  done
  mupdf "$NOTE_PATH"
}

function print_start_menu() {
  printf "Run App\n"
  printf "Notes\n"
  printf "Useful Links\n"
}
# Log and save initial option. Then route user to the desired action
start_option=$(print_start_menu | dmenu_cmd)

case "$start_option" in
  "$RUN_APP")
    handle_run
    ;;
  "$NOTES")
    handle_notes
    ;;
  "$USEFUL_LINKS")
    handle_links
    ;;
esac
