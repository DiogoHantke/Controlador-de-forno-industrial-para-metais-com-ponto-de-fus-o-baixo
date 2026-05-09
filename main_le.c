/*
 * Projeto: Forno de Fundicao Programavel
 * Microcontrolador: ATmega328P
 */

#include "PRINCIPALS.h"
#include "LCDPRINCIPALS.h"
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdio.h>

#define BAUD 9600UL
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1UL)


/* ================================
   ESTADOS DO SISTEMA
   ================================ */

typedef enum {
    STATE_START,
    STATE_MENU,
    STATE_CONFIGURATION,
    STATE_SAFETY_CHECK,
    STATE_START_CONFIRMATION,
    STATE_PREHEATING,
    STATE_HEATING,
    STATE_SOAKING,
    STATE_COOLING,
    STATE_FINISHED,
    STATE_MANUAL_MODE,
    STATE_ERROR
} SystemState;


/* ================================
   PERFIS DE MATERIAL
   ================================ */

typedef enum {
    PROFILE_ALUMINUM,
    PROFILE_STEEL,
    PROFILE_BRASS,
    PROFILE_CUSTOM
} MaterialProfile;


/* ================================
   MODOS DE OPERACAO
   ================================ */

typedef enum {
    MODE_AUTOMATIC,
    MODE_MANUAL
} OperationMode;


/* ================================
   CONFIGURACAO DO PROCESSO
   ================================ */

/*
 * Estrutura que guarda todos os parametros configuraveis do processo.
 */
typedef struct {
    MaterialProfile profile;
    OperationMode mode;
    uint16_t targetTemperatureC;
    uint16_t heatingTimeS;
    uint16_t soakingTimeS;
    uint8_t coolingEnabled;
} ProcessConfig;

/* ================================
   VARIAVEIS PRINCIPAIS DO SISTEMA
   ================================ */

SystemState currentState = STATE_START;
SystemState previousState = STATE_START;

ProcessConfig currentConfig;
ProcessConfig lastConfig;

uint8_t hasLastConfig = 0;

volatile uint16_t currentTemperatureC = 25;
volatile uint8_t lidClosed = 1;
volatile uint8_t emergencyActive = 0;
volatile uint8_t overtemperature = 0;
volatile uint8_t sensorFailure = 0;
volatile uint32_t systemSeconds = 0;

/* ================================
   MENSAGENS NA MEMORIA FLASH
   ================================ */

const char MSG_WELCOME[] PROGMEM = "Bem vindo :)";
const char MSG_TITLE[] PROGMEM = "Forno Fundicao";
const char MSG_STATE[] PROGMEM = "Estado:";
const char MSG_CONFIG[] PROGMEM = "Config atual";
const char MSG_STATE_START[] PROGMEM = "Inicio";
const char MSG_STATE_MENU[] PROGMEM = "Menu";
const char MSG_STATE_CONFIGURATION[] PROGMEM = "Configuracao";
const char MSG_STATE_SAFETY_CHECK[] PROGMEM = "Seguranca";
const char MSG_STATE_START_CONFIRMATION[] PROGMEM = "Confirmar";
const char MSG_STATE_PREHEATING[] PROGMEM = "Pre-aquec.";
const char MSG_STATE_HEATING[] PROGMEM = "Aquecendo";
const char MSG_STATE_SOAKING[] PROGMEM = "Patamar";
const char MSG_STATE_COOLING[] PROGMEM = "Resfriando";
const char MSG_STATE_FINISHED[] PROGMEM = "Finalizado";
const char MSG_STATE_MANUAL_MODE[] PROGMEM = "Modo Manual";
const char MSG_STATE_ERROR[] PROGMEM = "Erro";
const char MSG_PROFILE_ALUMINUM[] PROGMEM = "Aluminio";
const char MSG_PROFILE_STEEL[] PROGMEM = "Aco";
const char MSG_PROFILE_BRASS[] PROGMEM = "Latao";
const char MSG_PROFILE_CUSTOM[] PROGMEM = "Personalizado";
const char MSG_MODE_AUTOMATIC[] PROGMEM = "Automatico";
const char MSG_MODE_MANUAL[] PROGMEM = "Manual";

/* ================================
   PROTOTIPOS DAS FUNCOES DO LCD
   ================================ */

void showWelcomeScreen(void);
void showStateScreen(void);
void showConfigPreviewScreen(void);

const char* getStateText(SystemState state);
const char* getProfileText(MaterialProfile profile);
const char* getModeText(OperationMode mode);


/*
 * Retorna o texto correspondente ao estado atual.
 * O texto fica salvo na memoria Flash usando PROGMEM.
 */
const char* getStateText(SystemState state)
{
    switch (state)
    {
        case STATE_START:
            return MSG_STATE_START;

        case STATE_MENU:
            return MSG_STATE_MENU;

        case STATE_CONFIGURATION:
            return MSG_STATE_CONFIGURATION;

        case STATE_SAFETY_CHECK:
            return MSG_STATE_SAFETY_CHECK;

        case STATE_START_CONFIRMATION:
            return MSG_STATE_START_CONFIRMATION;

        case STATE_PREHEATING:
            return MSG_STATE_PREHEATING;

        case STATE_HEATING:
            return MSG_STATE_HEATING;

        case STATE_SOAKING:
            return MSG_STATE_SOAKING;

        case STATE_COOLING:
            return MSG_STATE_COOLING;

        case STATE_FINISHED:
            return MSG_STATE_FINISHED;

        case STATE_MANUAL_MODE:
            return MSG_STATE_MANUAL_MODE;

        case STATE_ERROR:
            return MSG_STATE_ERROR;

        default:
            return MSG_STATE_ERROR;
    }
}

const char* getProfileText(MaterialProfile profile)
{
    switch (profile)
    {
        case PROFILE_ALUMINUM:
            return MSG_PROFILE_ALUMINUM;

        case PROFILE_STEEL:
            return MSG_PROFILE_STEEL;

        case PROFILE_BRASS:
            return MSG_PROFILE_BRASS;

        case PROFILE_CUSTOM:
            return MSG_PROFILE_CUSTOM;

        default:
            return MSG_PROFILE_CUSTOM;
    }
}

const char* getModeText(OperationMode mode)
{
    switch (mode)
    {
        case MODE_AUTOMATIC:
            return MSG_MODE_AUTOMATIC;

        case MODE_MANUAL:
            return MSG_MODE_MANUAL;

        default:
            return MSG_MODE_AUTOMATIC;
    }
}

/* ================================
   FUNCOES DE TELA DO LCD
   ================================ */

void showWelcomeScreen(void)
{
    clearDisplay();

    lcdPrintFlash(0, MSG_WELCOME);
    lcdPrintFlash(1, MSG_TITLE);
}

/*
 * Mostra o estado atual do sistema.
 */
void showStateScreen(void)
{
    clearDisplay();

    lcdPrintFlash(0, MSG_STATE);
    lcdPrintFlash(1, getStateText(currentState));
}

/*
 * Mostra uma previa simples da configuracao atual.
 * Essa tela usa snprintf porque mistura texto com numeros.
 */
void showConfigPreviewScreen(void)
{
    char line[17];
    clearDisplay();
    lcdPrintFlash(0, MSG_CONFIG);
    snprintf(line, sizeof(line), "%uC %us", currentConfig.targetTemperatureC, currentConfig.heatingTimeS);
    sendString(line, 1);
}

/* ================================
   FUNCAO PRINCIPAL
   ================================ */

int main(void)
{
    initLCD();
    currentConfig.profile = PROFILE_ALUMINUM;
    currentConfig.mode = MODE_AUTOMATIC;
    currentConfig.targetTemperatureC = 660;
    currentConfig.heatingTimeS = 120;
    currentConfig.soakingTimeS = 60;
    currentConfig.coolingEnabled = 1;
  
    showWelcomeScreen();

    while (1)
    {
       
    }
}