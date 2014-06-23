/*
**  wsc.h
**
**  Use for both Win16 & Win32.
**  For Win32 compilers supporting the "declspec" keyword.
*/
#define STATIC_LIBRARY 1

#ifdef STATIC_LIBRARY
  #define DLL_IMPORT_EXPORT
#else
  #ifdef WIN32
    #ifdef DLL_SOURCE_CODE
       #define DLL_IMPORT_EXPORT __declspec(dllexport) __stdcall
    #else
       #define DLL_IMPORT_EXPORT __declspec(dllimport) __stdcall
    #endif
  #else
    #define DLL_IMPORT_EXPORT FAR PASCAL
  #endif
#endif  

#ifdef __cplusplus
  #define NoMangle extern "C"
#else
  #define NoMangle
#endif

NoMangle int DLL_IMPORT_EXPORT SioBaud(int,unsigned);
NoMangle int DLL_IMPORT_EXPORT SioBrkSig(int,char);
NoMangle int DLL_IMPORT_EXPORT SioCTS(int);
NoMangle int DLL_IMPORT_EXPORT SioDCD(int);
NoMangle int DLL_IMPORT_EXPORT SioDebug(int);
NoMangle int DLL_IMPORT_EXPORT SioDone(int);
NoMangle int DLL_IMPORT_EXPORT SioDSR(int);
NoMangle int DLL_IMPORT_EXPORT SioDTR(int,char);
NoMangle int DLL_IMPORT_EXPORT SioFlow(int,char);
NoMangle int DLL_IMPORT_EXPORT SioInfo(char);
NoMangle int DLL_IMPORT_EXPORT SioGetc(int);
NoMangle int DLL_IMPORT_EXPORT SioGets(int,LPSTR,unsigned);
NoMangle int DLL_IMPORT_EXPORT SioParms(int,int,int,int);
NoMangle int DLL_IMPORT_EXPORT SioPutc(int,char);
NoMangle int DLL_IMPORT_EXPORT SioPuts(int,LPSTR,unsigned);
NoMangle int DLL_IMPORT_EXPORT SioRead(int,int);
NoMangle int DLL_IMPORT_EXPORT SioReset(int,int,int);
NoMangle int DLL_IMPORT_EXPORT SioRI(int);
NoMangle int DLL_IMPORT_EXPORT SioRTS(int,char);
NoMangle int DLL_IMPORT_EXPORT SioRxClear(int);
NoMangle int DLL_IMPORT_EXPORT SioRxQue(int);
NoMangle int DLL_IMPORT_EXPORT SioStatus(int,unsigned);
NoMangle DWORD DLL_IMPORT_EXPORT SioTimer(void);
NoMangle int DLL_IMPORT_EXPORT SioTxClear(int);
NoMangle int DLL_IMPORT_EXPORT SioTxQue(int);
NoMangle int DLL_IMPORT_EXPORT SioUnGetc(int,char);
#ifdef WIN32
NoMangle int DLL_IMPORT_EXPORT SioWinError(LPSTR,int);
#endif

#ifndef COM1
    #define COM1   0
    #define COM2   1
    #define COM3   2
    #define COM4   3
    #define COM5   4
    #define COM6   5
    #define COM7   6
    #define COM8   7
    #define COM9   8
    #define COM10  9
    #define COM11 10
    #define COM12 11
    #define COM13 12
    #define COM14 13
    #define COM15 14
    #define COM16 15
#endif

/* Parity Codes */

#define NoParity 0
#define OddParity  1
#define EvenParity 2
#define MarkParity 3
#define SpaceParity 4

/* Stop Bit Codes */

#define OneStopBit  0
#define TwoStopBits 2

/* Word Length Codes */

#define WordLength5  5
#define WordLength6  6
#define WordLength7  7
#define WordLength8  8

/* return codes */

#define WSC_NO_DATA   (-100)
#define WSC_RANGE     (-101)
#define WSC_ABORTED   (-102)
#define WSC_WIN32ERR  (-103)
#define WSC_EXPIRED   (-104)
#define IE_BADID      (-1)
#define IE_OPEN       (-2)
#define IE_NOPEN      (-3)
#define IE_MEMORY     (-4)
#define IE_DEFAULT    (-5)
#define IE_HARDWARE   (-10)
#define IE_BYTESIZE   (-11)
#define IE_BAUDRATE   (-12)

/* baud codes */

#define Baud110    0
#define Baud300    1
#define Baud1200   2
#define Baud2400   3
#define Baud4800   4
#define Baud9600   5
#define Baud19200  6
#define Baud38400  7
#define Baud57600  8
#define Baud115200 9

/* SioGetError masks */

#define WSC_RXOVER   0x0001
#define WSC_OVERRUN  0x0002
#define WSC_PARITY   0x0004
#define WSC_FRAME    0x0008
#define WSC_BREAK    0x0010
#define WSC_TXFULL   0x0100

/* Line Commands */

#define SET_LINE     'S'
#define CLEAR_LINE   'C'
#define READ_LINE    'R'

/* BREAK Commands */

#define ASSERT_BREAK 'A'
#define CANCEL_BREAK 'C'
#define DETECT_BREAK 'D'
