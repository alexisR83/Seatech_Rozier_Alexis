#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"
#include "Robot.h"
#include "Toolbox.h"
#include "main.h"
#include "UART.h"

/****************************************************************************************************/
// Fonction OperatingSystemLoop de la machine d'?tats
/****************************************************************************************************/

unsigned char stateRobot;

void OperatingSystemLoop(void)
{
switch (stateRobot)
{
case STATE_ATTENTE:
timestamp = 0;
PWMSetSpeedConsigne(0, MOTEUR_DROIT);
PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
stateRobot = STATE_ATTENTE_EN_COURS;

case STATE_ATTENTE_EN_COURS:
if (timestamp > 1000)
stateRobot = STATE_AVANCE;
break;

case STATE_AVANCE:
PWMSetSpeedConsigne(30, MOTEUR_DROIT);
PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
stateRobot = STATE_AVANCE_EN_COURS;
break;

case STATE_AVANCE_EN_COURS:
SetNextRobotStateInAutomaticMode();
break;

case STATE_TOURNE_GAUCHE:
PWMSetSpeedConsigne(30, MOTEUR_DROIT);
PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
break;

case STATE_TOURNE_GAUCHE_EN_COURS:
SetNextRobotStateInAutomaticMode();
break;

case STATE_TOURNE_DROITE:
PWMSetSpeedConsigne(0, MOTEUR_DROIT);
PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
stateRobot = STATE_TOURNE_DROITE_EN_COURS;
break;

case STATE_TOURNE_DROITE_EN_COURS:
SetNextRobotStateInAutomaticMode();
break;

case STATE_TOURNE_SUR_PLACE_GAUCHE:
PWMSetSpeedConsigne(15, MOTEUR_DROIT);
PWMSetSpeedConsigne(-15, MOTEUR_GAUCHE);
stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
break;

case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
SetNextRobotStateInAutomaticMode();
break;

case STATE_TOURNE_SUR_PLACE_DROITE:
PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
break;

case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
SetNextRobotStateInAutomaticMode();
break;

default :
stateRobot = STATE_ATTENTE;
break;
}
}

unsigned char nextStateRobot=0;

void SetNextRobotStateInAutomaticMode()
{
unsigned char positionObstacle = PAS_D_OBSTACLE;

//D?termination de la position des obstacles en fonction des t?l?m?tres
if ( robotState.distanceTelemetreDroit < 30 &&
robotState.distanceTelemetreCentre > 20 &&
robotState.distanceTelemetreGauche > 30) //Obstacle ? droite
positionObstacle = OBSTACLE_A_DROITE;
else if(robotState.distanceTelemetreDroit > 30 &&
robotState.distanceTelemetreCentre > 20 &&
robotState.distanceTelemetreGauche < 30) //Obstacle ? gauche
positionObstacle = OBSTACLE_A_GAUCHE;
else if(robotState.distanceTelemetreCentre < 20) //Obstacle en face
positionObstacle = OBSTACLE_EN_FACE;
else if(robotState.distanceTelemetreDroit > 30 &&
robotState.distanceTelemetreCentre > 20 &&
robotState.distanceTelemetreGauche > 30) //pas d?obstacle
positionObstacle = PAS_D_OBSTACLE;

//D?termination de l??tat ? venir du robot
if (positionObstacle == PAS_D_OBSTACLE)
nextStateRobot = STATE_AVANCE;
else if (positionObstacle == OBSTACLE_A_DROITE)
nextStateRobot = STATE_TOURNE_GAUCHE;
else if (positionObstacle == OBSTACLE_A_GAUCHE)
nextStateRobot = STATE_TOURNE_DROITE;
else if (positionObstacle == OBSTACLE_EN_FACE)
nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;

//Si l?on n?est pas dans la transition de l??tape en cours
if (nextStateRobot != stateRobot-1)
stateRobot = nextStateRobot;
}

int main (void){
/***************************************************************************************************/
//Initialisation de l'oscillateur
/****************************************************************************************************/

InitOscillator();

/***************************************************************************************************/
//Initialisation des PWM
/****************************************************************************************************/

InitIO();

/***************************************************************************************************/
//Initialisation des PWM
/****************************************************************************************************/

InitADC1();

/***************************************************************************************************/
//Initialisation des PWM
/****************************************************************************************************/

InitPWM();

/****************************************************************************************************/
// Initialisation des timers
/****************************************************************************************************/

InitTimer23();
InitTimer1();
InitTimer4();

/****************************************************************************************************/
// Initialisation de l'UART
/****************************************************************************************************/

InitUART();

/****************************************************************************************************/
// Configuration des entr?es sorties
/****************************************************************************************************/

LED_BLANCHE = 1;
LED_BLEUE = 1;
LED_ORANGE = 1;

/****************************************************************************************************/
// Boucle Principale
/****************************************************************************************************/
while(1){
    if (ADCIsConversionFinished() == 1)
    {
        ADCClearConversionFinishedFlag();
        unsigned int * result = ADCGetResult();
        float volts = ((float) result[2])*3.3 / 4095 * 3.2;
        robotState.distanceTelemetreGauche = 34 / volts - 5;
        volts = ((float) result[1])*3.3 / 4095 * 3.2;
        robotState.distanceTelemetreCentre = 34 / volts - 5;
        volts = ((float) result[0])*3.3 / 4095 * 3.2;
        robotState.distanceTelemetreDroit = 34 / volts - 5;
        if (robotState.distanceTelemetreDroit < 30)
        {
            LED_ORANGE = !LED_ORANGE;
        }
        if (robotState.distanceTelemetreCentre < 30)
        {
            LED_BLEUE = !LED_BLEUE;
        }
        if (robotState.distanceTelemetreGauche < 30)
        {
            LED_BLANCHE = !LED_BLANCHE;
        }
        
    }
    
    //SendMessageDirect((unsigned char*) "Bonjour",7);
    SendMessage((unsigned char*) "Bonjour",7);
        __delay32(40000000);
        
}
}
