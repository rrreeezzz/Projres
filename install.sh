#!/bin/bash

if [ "$1x" = "x" ]
then
  echo -e "Pour une utilisation sur les ordinateurs de l'école utilisez l'option : ecole, sinon : normal."
  echo -e "Utilisation : $0 {ecole|normal}"
  exit 0
fi

echo -e "\033[31mInstallation du programme...\033[0m"

if [ "$1" = "normal" ]
then
	cd ./Client && make && cd ../server_lib && make && cd ../ui && gcc `pkg-config --cflags gtk+-3.0` ui.c -o ../ui_normal `pkg-config --libs gtk+-3.0`
fi

if [ "$1" = "ecole" ]
then
	cd ./Client && make client_ecole && cd ../ui/MinimalUI && gcc main.c -o ../../ui_minimale
fi

if test $? == 0
then
  echo -e "\033[31mProgramme installé !\033[0m"
else
  echo -e "\033[31mErreur a l'installation !\033[0m"
fi
