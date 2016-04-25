
Pour tester avec le serveur mysql:
Télécharger avec : sudo apt-get install mysql-server libmysqld-dev
Pour avoir les -dev client et serveur. Le mysql-server permet de lancer un serveur mysql.

Ensuite configurer avec : mysql-secure-installation dans /usr/bin
Retenez les ID pour gérer la base de données.
Lancer mysql avec : mysql -u'user' -p'password'

Créer une base de donnée avec: CREATE DATABASE pswd;
Puis créer les tables: CREATE TABLE user(ID INT, NAME TEXT, IP TEXT);

Remplacer ensuite les infos dans conf.txt, après les champs user: et password:
Lancer le ./install
