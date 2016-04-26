/////INSTALLATION\\\\\\

Librairires à installer :
sudo apt-get install mysql-server
sudo apt-get install libmysqld-dev
sudo apt-get install g++
sudo apt-get install libgtk-3-dev
sudo apt-get install libopenal-dev
sudo apt-get install libsndfile1-dev


/////SERVEUR MYSQL\\\\\

Pour tester avec le serveur annuaire, qui repose sur un serveur mysql, suivez les démarches ci-dessous.

Configurer avec la commande : "mysql_secure_installation" dans /usr/bin
Retenez les ID root pour gérer la base de données.
- Lancer mysql avec :
  $ mysql -u'user' -p'password'
- Créer une base de donnée avec:
  $ CREATE DATABASE pswd;
- Ensuite :
  $ USE pswd;
- Puis créer les tables:
  $ CREATE TABLE user(ID INT, NAME TEXT, IP TEXT);

Pour plus d'informations, se référer à mysql.

Remplacer ensuite les infos dans conf.txt, après les champs user: et password:
De plus, veuillez rentrer l'IP du de la machine sur laquelle vous avez installé le serveur annuaire (0.0.0.0 ou 127.0.0.1) si c'est la votre, après server_address:


Lancer le ./install
