A ajouter a la fin du fichier .zshrc de l'user raspberry.

if [[ -z "$SSH_TTY" ]]; then
        sudo /home/raspberry/HDMI-Switch/Main "Addresse du raspberry de controle (adresse sur VNC)"
fi
