/*******************************************************************
$Header: C:/Symbol/Csp32Proj/Csp32/src/vcs/Csp32.cpv  1.0  05 Mar 1999  10:00:00 kFallon $
 *
 * © Copyright 1999 Peninsula Solutions, Inc., all rights reserved.
 * © Copyright 1999 Symbol Technologies, Inc., all rights reserved.
 *
 *          ***************************************
 *          * This file was generated by:         *
 *          *       Peninsula Solutions, Inc.     *
 *          *       1265 S. Semoran Blvd.         *
 *          *       Suite 1247                    *
 *          *       Winter Park, FL 32792         *
 *          *       Phone: (407) 673-6544         *
 *          *       Fax:   (407) 673-6545         *
 *          * E-mail: info@PeninsulaSolutions.com *
 *          *                                     *
 *          * under a grant from:                 *
 *          *       Symbol Technologies, Inc.     *
 *          *       Holtsville, NY                *
 *          ***************************************
 *
 * The user is granted a license to use and modify this file for their own
 * purposes so long as the Copyright information remains intact.
 *
 *    MODULE NAME:   Csp32.cpp
 *
 *    DESCRIPTION:
 *                   This file provides the user API interface
 *                   functions for Symbol's Consumer Scanning
 *                   Products as Dynamic Link Library. When
 *                   compiled into a DLL, the user can access
 *                   all of the functions available for the
 *                   Symbol CyberPen.
 *
 *    AUTHOR:        Kim Fallon
 *
 *    DATE CREATED:  03/05/99
 *
 *    HISTORY:
 *
 *       03/05/99    Initial Development
 *
$Log: C:/Symbol/Csp32Proj/Csp32/src/vcs/Csp32.cpv $
 *******************************************************************/
/*******************************************************************
 * MODIFICACIONES HECHAS POR PERSONAL DE COMPUSCIENS (www.compusciens.org.ve)
 *
 * TODO EL TRABAJO FUE DESARROLLADO ORIGINALMENTE POR EL O LOS PARTICIPANTES
 * ESPECIFICADOS EN LA NOTA SUPERIOR A ESTA.
 *
 * PROYECTO TRANSPORTE UCV - 19/06/2014
 *
 ******************************************************************/

// Valores de Retorno de los distintos estados
#define STATUS_OK                   ((int) 0)
#define COMMUNICATIONS_ERROR        ((int)-1)
#define BAD_PARAM                   ((int)-2)
#define SETUP_ERROR                 ((int)-3)
#define INVALID_COMMAND_NUMBER      ((int)-4)
#define COMMAND_LRC_ERROR           ((int)-7)
#define RECEIVED_CHARACTER_ERROR    ((int)-8)
#define GENERAL_ERROR               ((int)-9)
#define FILE_NOT_FOUND              ((int) 2)
#define ACCESS_DENIED               ((int) 5)

// Parametros numericos
#define TL_BITS                     ((char)0x01)
#define VOLUME                      ((char)0x02)
#define BARCODE_REDUNDANCY          ((char)0x04)
#define USER_ID                     ((char)0x06)
#define CONTINUOUS_SCANNING         ((char)0x12)

// Valores de los parametros
#define VOLUME_QUIET                ((int) 0)
#define VOLUME_LOW                  ((int) 1)
#define VOLUME_MEDIUM               ((int) 2)
#define VOLUME_HIGH                 ((int) 3)

#define PARAM_OFF                   ((int) 0)
#define PARAM_ON                    ((int) 1)

#define DETERMINE_SIZE              ((int) 0)

// Valores de los estados de configuracion del puerto serial...
#define SERIAL_RS232                ((int) 0)
#define SERIAL_MODEM                ((int)-1)
#define SERIAL_UNKNOWN              ((int)-2)

/*******************************************************************
 *              Communications setup section...                    *
 *******************************************************************/
#ifndef COM1
    #define COM1                    ((int) 0)
    #define COM2                    ((int) 1)
    #define COM3                    ((int) 2)
    #define COM4                    ((int) 3)
    #define COM5                    ((int) 4)
    #define COM6                    ((int) 5)
    #define COM7                    ((int) 6)
    #define COM8                    ((int) 7)
    #define COM9                    ((int) 8)
    #define COM10                   ((int) 9)
    #define COM11                   ((int)10)
    #define COM12                   ((int)11)
    #define COM13                   ((int)12)
    #define COM14                   ((int)13)
    #define COM15                   ((int)14)
    #define COM16                   ((int)15)
#endif

/*******************************************************************
 *              Th, Th, Th, Th, That's All Folks !                 *
 *******************************************************************/

// Definiciones locales
#define STX                         ((char)0x02)    // <stx> character

// Valores del byte de los comandos
#define INTERROGATE                 ((char)0x01)
#define CLEAR_BAR_CODES             ((char)0x02)
#define DOWNLOAD_PARAMETERS         ((char)0x03)
#define RESERVED_CODE_4             ((char)0x04)
#define POWER_DOWN                  ((char)0x05)
#define RESERVED_CODE_6             ((char)0x06)
#define UPLOAD                      ((char)0x07)
#define UPLOAD_PARAMETERS           ((char)0x08)
#define SEND_SIGNATURE              ((char)0x0C)

// Valor del estado del dispositivo Csp
#define NO_ERROR_ENCOUNTERED        ((int)-6)

#define MAXTIME                     ((int)  18)    // 18 ticks, ~ 1 segundo
#define MAXSIZE                     ((int)4096)    // tamaño maximo de los codigos leidos
#define RX_QUE_SIZE                 ((int)1024)    // communications RX buffer setting
#define TX_QUE_SIZE                 ((int) 512)    // communications Tx buffer setting


/* Acabada la definiciones de la libreria, pasamos a definir nuestra clase */
#ifndef CSP32BRIDGE_H
#define CSP32BRIDGE_H

#include <QObject>

// Nota sobre las siguientes 2 librerias, es probable
// que Qt proporcione soporte interno para esto, pero quedara
// para futuras revisiones su implementacion
#include <windows.h> // Para que Wsc funcione porque define algunos tipos encontrados aca
#include <Wsc.h>

class Csp32Bridge : public QObject
{
    Q_OBJECT
public:
    explicit Csp32Bridge(QObject *parent = 0);

    // Comunicaciones
    int cspInit(int nComPort); // Para abrir comunicacion con el lector
    int cspRestore();

    // Funciones basicas
    int cspReadData();
    int cspClearBarCodes();
    int cspPowerDown();

    // Funciones para obtener los datos
    int cspGetBarcode(char szBarData[], int nBarcodeNumber, int nMaxLength);
    int cspGetDeviceId(char szDeviceId[9], int nMaxLength);
    int cspGetSignature(char aSignature[8], int nMaxLength);
    int cspGetProtocol();
    int cspGetSystemStatus();
    int cspGetSwVersion(char szSwVersion[9], int nMaxLength);

    // Metodos SET para la configuracion
    int cspSetTlBits(char aTlBits[8], int nMaxLength);
    int cspSetVolume(int nVolume);
    int cspSetBarcodeRedundancy(int nOnOff);
    int cspSetUserId(char szUserId[9], int nMaxLength);
    int cspSetContinuousScanning(int nOnOff);

    // Metodos GET para la configuracion
    int cspGetTlBits(char aTlBits[8], int nMaxLength);
    int cspGetVolume(void);
    int cspGetBarcodeRedundancy(void);
    int cspGetUserID(char szUserId[9], int nMaxLength);
    int cspGetContinuousScanning(void);

    // Metodos para configurar nuestra clase
    int cspSetRetryCount(int nRetryCount);
    int cspGetRetryCount();

    // Funciones para debuggear
    int cspGetCommInfo(int nComPort);

    //Funciones avanzadas
    int cspReadRawData(char aBuffer[], int nMaxLength);
    int cspSetParam(int nParam, char szString[], int nMaxLength);
    int cspGetParam(int nParam, char szString[], int nMaxLength);
    int cspInterrogate();

    // Variables de control y demas
    int nCspActivePort;
    int nCspDeviceStatus;
    int nCspProtocolVersion;
    int nCspSystemStatus;
    int nCspStoredBarcodes;
    int nCspRetryCount;

signals:

public slots:

private:

    //Otras variables privadas
    int nCspVolume;
    int nCspBarcodeRedundancy;
    int nCspContinousScanning;

    char aCspTlBits[8];
    char aCspUploadCount[4];
    char aCspSignature[8];
    char szCspUserId[9];
    char szCspSwVersion[9];
    char szCspDeviceId[9];
    char szCspBarData[MAXSIZE];
    char aByteBuffer[MAXSIZE];

    //Metodos privados
    int cspGetc();
    char cspLrcCheck(char aLrcBytes[], int nMaxLength);
    void cspInitParms();
    int cspSendCommand(char *aCommand, int nMaxLength);

};

#endif
