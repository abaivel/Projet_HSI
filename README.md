# Projet_HSI
Réalisé par Alexandra Baivel et Dawid Malicki

Projet dans le cadre du cours de Hardware Software Interface dont l'objectif est de coder le logiciel applicatif du calculateur principal d’une voiture.

## Compilation et lancement de l'application

Pour installer les bibliothèques nécessaires au projet, il faut exécuter cette commande :
```
./install.sh
```
Pour compiler et lancer le projet, il faut exécuter cette commande dans le dossier racine du projet :
```
./start.sh
```
Le script shell compilera et lancera le projet automatiquement. Lors de l'arrêt du script, les fichiers .o et l'exécutable générés seront supprimés.
Pour accéder au tableau de bord, allez à l'url localhost:3000 sur votre navigateur.

## Explication des questions

### Question 1
Afin de stocker tous les types et toutes les variables utiles à notre projet nous avons choisi un document Excel, *data_project.xlsx*.
Le document contient une première feuille avec la définition de tous les types et la deuxième feuille contient les variables concernant le BCGV.
Nous n'avons pas commencé le projet en créant le fichier parce que nous voulions comprendre l'utilité des types et des variables à créer. Nous l'avons donc créé lorsque nous avons fait la question 4.

### Question 2
Pour générer notre librairie statique grâce à notre fichier Excel créé à la question 1, nous avons fait un programme en python se trouvant dans le fichier *generate_static_library.py*. ce programme génère les fichiers *bcgv_lib.c* et *bcgv_lib.h* en se basant sur le fichier Excel.

### Question 3
La question 3 est la première question que nous avons faite dans le sujet, comme nous avions sauté les questions 1 et 2. Nous avons créé un fichier *app.c* dans lequel se trouvait une fonction *main* avec une boucle while qui lisait les trames reçues en UDP en boucle. Nous avons aussi ajouté la lecture des messages reçus en série pour pouvoir tester la fonction *drv_read_ser*.

### Question 4
Nous avons créé trois fichiers se basant sur le fichier *fsm.c* pour gérer les différentes states machines : *fsm_lights.c* pour les feux, *fsm_blinkers.c* pour les clignotants et *fsm_wipers.c* pour les essuie-glaces. Nous avons configuré les différents états et transitions de chacune des states machines ainsi que la table de transitions. Nous avons modifié les fonctions *main* et nous avons rempli les fonctions *get_next_event*. Ces fonctions testent l'état actuel de la state machine puis testent les données correspondantes du bcgv (*cmd*, *acq* et *timer*) pour pouvoir déterminer le prochain évènement et mettre à jour les données nécessaires (comme *timer* ou *acq*). 

Notre programme principal utilise ces states machines en stockant dans des variables statiques le statut de chaque élément (les feux de position, de route, de croisement, le clignotant droit, le clignotant gauche, etc...) puis appelle les fonctions des states machines pour mettre à jour les variables statiques.

### Question 5
Nous avons créé trois fichiers Makefile : *Makefile_app*, *Makefile_lib* et *Makefile* qui appelle les deux autres fichiers Makefile. Le fichier *Makefile_app* compile notre fichier app.c et le fichier *Makefile_lib* compile notre libraire statique. 
Nous avons ensuite un fichier *start.sh* qui permet de lancer le docker, le driver, les makefiles puis l'exécutable *app*.
Nous avons aussi un fichier *install.sh* pour installer les bibliothèques nécessaires au fonctionnement du projet.

### Question 6
Nous n'avons pas fait la question 6 en entier directement, nous avons implémenté fonctionnalité par fonctionnalité la question 4, 6 et 7 et nous avons commencé par les feux, puis les clignotants puis les essuie-glaces.
La lecture des trames est faite par deux fonctions : *translate_udp_frame* et *translate_serial* qui traduisent respectivement la trame udp et la trame serial. Pour la trame serial, la fonction décompose le nombre reçu en puissance de deux pour mettre à jour les données. Ces fonctions sont appelées dans la fonction *receive_frames* qui reçoit les trames udp et serial. Cette fonction est appelée par notre fonction main. 

### Question 7
L'envoi des trames est géré entièrement par la fonction *send_frames* qui gère la traduction et l'envoi des trames udp et serial. Pour éviter d'envoyer trop de trames serial (ce qui enverrait trop de trames alors qu'on peut les recevoir toutes les 500ms seulement), on en envoit seulement si l'état de l'élément a changé. Pour cela, nous stockons dans des variables globales l'état précédent des states machines.

C'est dans cette fonction *receive_frames* qu'est géré l'acquittement. Selon le serial number de la trame serial reçue, la fonction *receive_frames* appelle la fonction *translate_serial* ou met à jour l'acquittement de la donnée adaptée.

Pour la donnée de la trame udp appelée "Voyant défaut moteur", nous sommes partis du principe qu'il fallait que le voyant s'allume s'il y avait un problème moteur donc soit un défaut de pression, soit un problème de température du liquide de refroidissemnt ou un problème de surchauffe huile.

### Question 8
Pour respecter le schéma, nous avons juste ajouté quelques lignes à la fonction *receive_frames* pour récupérer le numéro de trame et le comparer au précédent.