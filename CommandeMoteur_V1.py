#**************************************************** ****************************
#* <NOM DU FICHIER : "CommandeMoteur">
#*
#* EQUIPE T-REX <NOM APPLICATION : "Commande moteur par reception client">
#* Sortie d'origine : 07 juin 2022 
#* Par : JANEZ Maxime (Communication client/Server) Ibrahim ELbahnsi (Commandes moteurs) 
#*	
#* Description du fichier:
#* -Recetion des commandes en TCP/IP
#* -Commande des moteur      
#* 
#*	
#* Historique des modifications :
#* 07 juin 2022 : version originale
#* 
#* Copyright (c) EQUIPE T-REX IUT CACHAN LP MECSE
#* Tous les droits sont reserves.
#****************************************************** **************************/
import socket
import errno
import RPi.GPIO as GPIO #import the RPi.GPIO library

import  time  #import the  time library
import  threading 

#declare the GPIO 18 and 21 pins for the outputs servos
GPIO.setmode(GPIO.BCM)
turnLeftMotor = 19
turnRightMotor = 12
 
#define the behaviour of the turnLeftMotor and turnRightMotor  as outputs
GPIO.setup(turnLeftMotor,GPIO.OUT)
GPIO.setup(turnRightMotor,GPIO.OUT)

GPIO.setwarnings(False)#ignore the warnings


#create PWMs instance with frequency 100 Hz
PWM1 = GPIO.PWM(turnLeftMotor,100)
PWM2 = GPIO.PWM(turnRightMotor,100)

#start the PWMs at 0 duty cycle
PWM1.start(0)
PWM2.start(0)


#info serveur
HOST = '192.168.1.103'  # ip serveur
PORT = 8888             # port serveur

print('programme start')

#init client
etat = 0
time.sleep(15)
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)      # creat socket
print('init connexion au serveur')
client.connect((HOST, PORT))                                    # start connexion to serveur
print('Connexion vers ' + HOST + ':' + str(PORT) + ' reussie.')
etat = 1

#message envoie
##def envoiMsg():
##        
##        message = 'W'
##        n = client.send(message)
##        print ('Envoi de :' + message)
##        
##        if (n != len(message)):
##                print('Erreur envoi.')
##        else:
##               print ('Envoi ok.')

##def rapportCyclyque():
##        a = (int) (donnees[1] + donnees[2])
##        return a

#==================================================================================
# receptionCommande : Recoit les commandes , active/ desactive les moteurs en
# des commandes recus ou ferme le client 
#=================================================================================
        
def receptionCommande() :
        
        print ('Reception...')
        donnees = client.recv(1024)     # receptio de la commande envoyer par le serveur
        print ('Recu :', donnees)

        
        # demmare le moteur droit en fonction de la commande recu par le serveur
        #lorsque la gachette de la manette droite a ete presse
        if (donnees == b'$r1'):
            print('moteur droit start')
            #duty = 16 # marche arriere >15
            duty = 12 # marche avant < 15
            PWM2.ChangeDutyCycle(duty) 

        # arrete le moteur droit en fonction de la commande recu par le serveur
        #lorsque la gachette de la manette droite a ete relache
        elif (donnees == b'$r0'):
            print('moteur droit stop')
            duty = 15 # largeur d'impultion permettant l'arret 
            PWM2.ChangeDutyCycle(duty)

        # demmare le moteur gauche en fonction de la commande recu par le serveur
        #lorsque la gachette de la manette gauche a ete presse
        elif (donnees == b'$l1'):
            print('moteur gauche start')
            #duty = 16 # marche arriere >15
            duty = 12 # marche avant < 15 
            PWM1.ChangeDutyCycle(duty)
            
        # arrete le moteur gauche en fonction de la commande recu par le serveur
        #lorsque la gachette de la manette gauche a ete relache
        elif (donnees == b'$l0'):
            print('moteur gauche stop ')
            duty = 15 # largeur d'impultion permettant l'arret 
            PWM1.ChangeDutyCycle(duty)

        # quitte la boucle while en passant a l'etat 0 pour que le client puisse se fermer  
        elif (donnees == b'/q'):
            print ('close connexion')
            etat = 0

##try :
##        envoiMsg()
##        threading.Timer(5,envoiMsg).start()
##
##except:
##        client.close
            
        

while(etat == 1):

        # lance la fonction reception commande permettant le controle des moteurs par les manettes
        # sauf dans le cas ou la connection a ete couper dans ce cas on coupe tout les moteurs
        try :
                receptionCommande()# commande les moteurs 
        except socket.error as e:        

                if e.errno!= errno.ECONNRESET : # erreur de connexion 
                        raise
                pass
                
                duty = 15 # largeur d'impultion permettant l'arret 
                PWM2.ChangeDutyCycle(duty) # arrete les moteurs 
                etat=0 # sort de la boucle 
                time.sleep(5)
                print ('Deconnexion')
                
        #else :
        #        print('message recu :', donnees, 'non conforme')
        #        etat = 0

print ('Deconnexion')
client.close() # end the connexion
