#indique l'image de base 
FROM tomcat:latest

#copie l'archive téléchargée (le.war) dans l'image afin d'avoir l'ancienne version dans le conteneur
#[fichier sur mon pc en local]    [emplacement dans le conteneur tomcat]
COPY jenkins.war /usr/local/tomcat/webapps/jenkins.war

# utilise le port 8080
EXPOSE 8080
