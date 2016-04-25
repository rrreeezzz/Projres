
Pour tester avec le serveur mysql:
Télécharger avec : sudo apt-get install mysql-server libmysqld
Pour avoir les -dev client et serveur. Le mysql-server permet de lancer un serveur mysql.

Ensuite configurer avec : mysql-secure-installation
Lancer mysql avec : mysql -u'user' -p'password'

Créer une base de donnée avec: CREATE DATABASE pswd;
Puis créer les tables: CREATE TABLE user(ID INT, USER TEXT, IP TEXT);

Remplacer ensuite les infos dans conf.txt
Lancer le ./install
