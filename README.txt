Pour tester avec le serveur mysql:
Télécharger avec : sudo apt-get install mysql-server libmysqld 
Pour avoir les -dev client et serveur. Le mysql-server permet de lancer un serveur mysql.

Ensuite configurer avec : mysql-secure-installation
Lancer mysql avec : mysql -u'user' -p'password'

Créer une base de donnée avec: CREATE DATABASE pswd;
Puis créer les tables: CREATE TABLE pswd(ID INT, USER TEXT);

Ensuite remplacer dans db.c a la ligne 18 avec les identifiants avec lesquels vous vous connectez au serveur mysql.

