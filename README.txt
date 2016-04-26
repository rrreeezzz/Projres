/////INSTALLATION\\\\\\

Librairies à installer :

sudo apt-get install mysql-server
sudo apt-get install libmysqld-dev
sudo apt-get install g++
sudo apt-get install libgtk-3-dev

Pour le transfert de message vocaux :

sudo apt-get install libopenal-dev
sudo apt-get install libsndfile1-dev


/////SERVEUR MYSQL\\\\\

Pour tester avec le serveur d'annuaire, qui repose sur un serveur mysql, suivez les démarches ci-dessous.

- Configurer avec la commande :
1  $ mysql_secure_installation    (dans /usr/bin si nécessaire)
- Retenir les ID root rentrés pour gérer la base de données
- Lancer mysql avec le user (root) et le password défini durant l'étape précédente
2  $ mysql -u'user' -p'password'
- Créer une base de donnée avec:
3  $ CREATE DATABASE pswd;
- Ensuite :
4  $ USE pswd;
- Puis créer les tables:
5  $ CREATE TABLE user(ID INT, NAME TEXT, IP TEXT);
(Pour plus d'informations, se référer à mysql)

- Remplacer ensuite les infos dans conf.txt : champs user: et password: définis précédemment
De plus, veuillez rentrer l'IP de la machine sur laquelle vous avez installé le serveur annuaire
champ server_address: -> " 0.0.0.0 " ou " 127.0.0.1 " sur la machine locale

//////Compilation\\\\\
Lancer  ./install.sh {ecole|normal}  à la racine du dossier de l'application.
-> paramètre ecole dans le cas où il n'y a pas eu possibilité d'installer les librairies de la 1ère étape
-> paramètre normal si toutes les librairies sont installées
