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
#* Tous les droits sont réservés.
#****************************************************** **************************/
import socket

import RPi.GPIO as GPIO #import the RPi.GPIO library

from time import sleep #import the sleep from time library

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

#start the PWM at 0 duty cycle
PWM1.start(0)
PWM2.start(0)
duty = 16

HOST = '192.168.1.103'
PORT = 8888

print('programme start')

etat = 0
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print('init connexion au serveur')
client.connect((HOST, PORT))
print('Connexion vers ' + HOST + ':' + str(PORT) + ' reussie.')
etat = 1
#message envoie 
        #message = 'Hello, world'
        #n = client.send(message)
        #print ('Envoi de :' + message)
        
        #if (n != len(message)):
        #        print('Erreur envoi.')
        #else:
        #       print ('Envoi ok.')

while(etat == 1):

        print ('Reception...')
        donnees = client.recv(1024)
        print ('Recu :', donnees)

        #if(donnees == b'$r0' or donnees == b'$l0' or donnees == b'/q' or donnees == b'$r1' or donnees == b'$l1'):            
        if (donnees == b'$r1'):
            print('moteur droit start')
            #duty = 16 # marche arriere >15
            duty = 12 # marche avant < 15
            PWM2.ChangeDutyCycle(duty)

        elif (donnees == b'$r0'):
            print('moteur droit stop')
            duty = 15 # largeur d'impultion permettant l'arret 
            PWM2.ChangeDutyCycle(duty)
                        
        elif (donnees == b'$l1'):
            print('moteur gauche start')
            #duty = 16 # marche arriere >15
            duty = 12 # marche avant < 15 
            PWM1.ChangeDutyCycle(duty)

        elif (donnees == b'$l0'):
            print('moteur gauche stop ')
            duty = 15 # largeur d'impultion permettant l'arret 
            PWM1.ChangeDutyCycle(duty)
                
        elif (donnees == b'/q'):
            print ('close connexion')
            etat = 0
                
        #else :
        #        print('message reçu :', donnees, 'non conforme')
        #        etat = 0

print ('Deconnexion')
client.close()
