# Switch HDMI pour les salons 3Bis

Gestion automatique du Switch HDMI pour permettre le changement automatique de la chaîne sur la tv en cas de changement d’activité sur le levier principal de la salle.

## Fonctionnement:

Se connecte en TCP a l'adresse spécifiée en argument lors du lancement du programme, puis envoie les commandes pour établir le nom de ce client et recevoir les infos en réponse:

- subscribe
- setname HDMI

Une fois connecte et nomme, le client ignore toutes les infos renvoyées par le serveur, sauf toutes les infos concernant les changements de salons. Le client traite l'info et en fonction du salon demande envoie le signal infrarouge NEC hardcoded dans le code du programme.

En cas de déconnexion, crash, ou autre problème interrompant le programme, le programme est censé repartir au début et continuer d'essayer de se connecter indéfinitivement 

## Utilisation: 

Normalement le programme démarre automatiquement a l'allumage du salon, mais si besoin de le lancer manuellement: \
Lancer Main avec sudo et l'adresse du raspberry contrôlant le salon désiré.

Par exemple pour Cuba: \
`sudo ./Main 192.168.1.104`

Pour faire en sorte que le programme se lance automatiquement au démarrage du raspberry pi, il suffit d'ajouter les lignes suivantes dans le fichier de config du shell par defaut. Par exemple si le shell est zsh, il faut modifier le fichier `/home/<username>/.zshrc`

`if [[ -z "$SSH_TTY" ]]; then` \
`	sudo /home/raspberry/HDMI-Switch/Main "Adresse"` \
`fi`  

ou "adresse" est l'adresse IP du raspberry de contrôle du salon désiré.

## Liste des adresses des salons:

(possible qu'elles changent au fur et a mesure de l’évolution des salles):

`Cuba 192.168.1.104` \
`Sitcom 12 192.168.1.107` \
`Toys 192.168.1.110` \
`80s 192.168.1.113` \
`Sitcom 6 192.168.1.116` \
`Big Bro 192.168.1.119` 
