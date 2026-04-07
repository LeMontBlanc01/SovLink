TEMPLATE  = subdirs
SUBDIRS   = server client

# Le client n'a pas besoin du serveur pour compiler,
# mais si tu veux lancer les deux en séquence : client.depends = server
