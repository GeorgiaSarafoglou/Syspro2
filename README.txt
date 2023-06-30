Georgia Sarafoglou sdi1900168

Compile: make
Run: ./dataServer <arguments>
    ./remoteClient <arguments>

Ypothetoume oti tha boun sosta ola ta arguments me opoiadipote seira.


O server kata tin idrisi tou arxikopoiei to Queue kai to thread pool
me ta worker threads.

To Queue einai global gia na ginetai access apo tis sinartiseis kai ta
threads.

O server trexei ena loop pou apodexetai connections apo clients
kai ftiaxnei ena communication_thread gia kathe client kai ena
antistoixo socket.

Sti sunartisi tou communication thread diabazetai to directory pou esteile
o client, metraei ta arxeia pou periexontai se auto me tin countfiles
kai eisagei kathe arxeio stin oura.

O arithmos arxeion stelnetai sto client gia na gnorizei pote teleiose
i antigrafi kai na enimerosei to server na kleisei to socket tou.

Sti sinartisi readdirect diabazetai anadromika o katalogos gia antigrafi
kai se kathe arxeio ginetai prospatheia eisagogis stin oura.
An i oura einai gemati ginetai pthread_cond_wait mexri na steilei
sima kapoio worker thread oti ekane pop arxeio apo auti.

I oura exei ena mutex gia na kanei lock kathe fora pou paei na eisagei
arxeio i na kanei Pop ena.

Kathe worker thread perimenei mexri i oura na min einai adeia gia na parei
ena arxeio.

O worker pairnei to onoma tou arxeiou kai to megethos tou kai stelnei
sto client to megethos tou onomatos tou arxeiou gia na to diabasei meta
to onoma tou arxeiou, meta to megethos tou arxeiou kai telos ta periexomena
tou ana block_size.

Sto telos perimenei na tou steilei o client minima oti teleiose tin antigrafi
arxeiou oste na kanei unlock to thread ta resources poi xrisimopoiei
kai na proxorisei se epomeno arxeio.

An o client steilei minima "end" tote o worker kleinei kai to socket.


O client diabazei to path tou katalogou pou theloume na antigrapsei
kai xorizei to onoma tou katalogou apo to path tou sto server.

I antigrafi tha ginei se ena subdirectory "output" tou client.

O client sindeetai me ton server kai tou dinei meso socket to path
tou directory.

Epeita diabazei to mikos tou filename, to filename, to megethos tou arxeiou
kai telos diabazei epanaliptika ta periexomena tou arxeiou mexri na ta
eksantlisei.

Otan tha labei to filename tha xorisei to path tou server sto opoio
anikei kai tha balei to neo path proorismou.

tha ftiaksei to arxeio kai ta katallila directories sta opoia brisketai
apo ti sinartisi makefile. An to arxeio iparxei idi tha to diagrapsei prota.

telos tha to anoiksei kai tha grapsei ta periexomena pou diabastikan.


Gia tin epikoinonia Server-Client xrisimopoiithike kodikas apo tis
diafaneies tou mathimatos.






