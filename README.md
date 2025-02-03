# SAE SYSTEME
=============

Un client désire que nous créions et lancions un serveur qui gère une base de données contenant des jeux. Il a une exigence forte,
le serveur doit pouvoir exécuter des opérations en parallèle tout en limitant la consommation de mémoire et enréduisant au maximum les temps d’exécution.
Ainsi, on développera **trois versions différentes et simplifiées** afin de répondre à la demande du client

GIT
---

Il y a un dossier pour chaque version (dont une V0).
Chacun de ces dossiers a des sous-dossiers.
Les sous-dossiers sont nommées en Versions (format Vx.y ; x appartenant à l'ensemble [0, 1, 2, 3] et y appartenant à l'ensemble [a, b]).
Il existe un Makefile pour chaque version qui compilera le projet et sortira un exécutable ; dans la Version 2, on simule plusieurs servers, il y aura donc 6 exécutables en plus dans le sous-sous-dossier Cfichier (Version 2/Vx.y/Cfichier/).

##### Bonne découverte du projet!

(ps: tout est en camel case)
