
#ifndef General_HH

#define General_HH

#ifdef __STDC__

#ifdef General_IMP
#define General_CONST
#else
#define General_CONST const
#endif

#else
#define General_CONST
#endif

#ifndef PUBLIC
#define PUBLIC
#define PRIVATE static
#endif

/* <MESA:01:@(#):Mmarcadma6U0:interop2:1.1:980520093144:mallorca:1 43 44732:MESA> */
/* <MESA:01:@(#):Mmarcadma6U0:interop1:1.1:981117125821:mallorca:1 43 10859:MESA> */
/***********************************************************************
 *	%W%
 *----------------------------------------------------------------------
 *
 *	  Division:   Plataforma de Explotacion
 *	  Subsistema:
 *	  Componente:
 *	  Modulo:     Definiciones genericas.
 *	  Autor:        
 *	  Fecha:      %G%	%U%
 *
 *
 * -----------------------  DESCRIPCION  -------------------------------
 *        
 * ----------------------  RESTRICCIONES  ------------------------------
 *
 *
 * ---------------------------------------------------------------------
 **********************************************************************/

/***********************************************************************
 * DEFINICIONES GENERALES:    
 **********************************************************************/

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define PROCEDURE void
#define FUNCTION  


/**********************  CONSTANTES EXPORTADAS  ***********************/

/*
 *
 * #ifndef min
 * #define	min(a,b)		((a > b) ? b : a)
 * #endif
 *
 * #ifndef max
 * #define	max(a,b)		((a > b) ? a : b)
 * #endif
 *
 */

#define TAMANYO_BYTE			8

#define	TAMANYO_SHORT_APLANADO		2

#define	TAMANYO_ENTERO_APLANADO		2

#define	TAMANYO_LONG_APLANADO		4
#define	TAMANYO_CHAR_APLANADO		1

#define	MAX_SHORT_APLANABLE	(~(~(0) << (TAMANYO_SHORT_APLANADO * TAMANYO_BYTE) - 1))
#define	MAX_ENTERO_APLANABLE	(~(~(0) << (TAMANYO_ENTERO_APLANADO * TAMANYO_BYTE) - 1))
#define	MAX_LONG_APLANABLE	(~(~(0) << (TAMANYO_LONG_APLANADO * TAMANYO_BYTE) - 1))

/*
#define	MAX_SHORT_APLANABLE	((1 << (TAMANYO_SHORT_APLANADO * TAMANYO_BYTE) - 1) - 1)
#define	MAX_ENTERO_APLANABLE	((1 << (TAMANYO_ENTERO_APLANADO * TAMANYO_BYTE) - 1) - 1)
#define	MAX_LONG_APLANABLE	((1 << (TAMANYO_LONG_APLANADO * TAMANYO_BYTE) - 1) - 1)
*/

#define TAMANYO_STRING_ENTERO		11
#define TAMANYO_STRING_LONG		21

#define MAX_LONG_NOM_FICH		35

/************** JOSE, el max del SO es 255, mas el /0 
   #define TAMANO_NOM_FICH			100	 
**************/
#define TAMANO_NOM_FICH			256


/************************  TIPOS EXPORTADOS  **************************/

#ifndef ASM_SYM
#ifndef ASM_RISC

typedef enum  
	{FALSO, CIERTO}			TipoBoolean;
#endif
#endif


#ifndef DATAVIEW

#ifndef TIPO_BOOL
#define TIPO_BOOL

#ifndef ASM_SYM
#ifndef ASM_RISC

typedef short int			BOOL;
#endif
#endif


#endif 

#endif

#ifndef ASM_SYM
#ifndef ASM_RISC

typedef unsigned char			TipoByte;
#endif
#endif


#ifndef EXISTE_TIPORESULTADO
#define EXISTE_TIPORESULTADO

#ifndef ASM_SYM
#ifndef ASM_RISC
typedef enum {FRACASO, EXITO} TipoResultado;
#endif
#endif

#endif

#ifndef ASM_SYM
#ifndef ASM_RISC

typedef char *				TipoPathName;
#endif
#endif


#ifndef ASM_SYM
#ifndef ASM_RISC

typedef int				TipoIdProceso;
#endif
#endif
 

/* 
 * deberia ser:
 *
 *   typedef void *   TipoObjeto;
 *
 * pero da warnings al compilar.
 */

#ifndef ASM_SYM
#ifndef ASM_RISC

typedef char *				TipoObjeto;
#endif
#endif


#ifndef ASM_SYM
#ifndef ASM_RISC

typedef char *                          TipoNombreSimbolico;
#endif
#endif


#ifndef ASM_SYM
#ifndef ASM_RISC

typedef TipoResultado 			(* TipoFuncion) ();
#endif
#endif


#ifndef ASM_SYM
#ifndef ASM_RISC
typedef void	      			(* TipoFuncionVoid) ();
#endif
#endif


#if defined(__cplusplus) || defined(c_plusplus)

#ifndef ASM_SYM
#ifndef ASM_RISC
typedef void	      			(* TipoFuncionVoidParaSignal)(int);
#endif
#endif


#ifndef ASM_SYM
#ifndef ASM_RISC
typedef void	      			(* TipoFuncionVoidParams)(...);
#endif
#endif


#ifndef ASM_SYM
#ifndef ASM_RISC
typedef TipoResultado 			(*TipoNombreProcedimiento)(...) ;
#endif
#endif


#ifndef ASM_SYM
#ifndef ASM_RISC
typedef TipoResultado 			(*TipoNombProcConOSinParam)(...) ;
#endif
#endif


#else

#ifndef ASM_SYM
#ifndef ASM_RISC
typedef void	      			(* TipoFuncionVoidParaSignal)();
#endif
#endif


#ifndef ASM_SYM
#ifndef ASM_RISC
typedef void	      			(* TipoFuncionVoidParams)();
#endif
#endif


#ifndef ASM_SYM
#ifndef ASM_RISC
typedef TipoResultado 			(*TipoNombreProcedimiento)() ;
#endif
#endif


#ifndef ASM_SYM
#ifndef ASM_RISC
typedef TipoResultado 			(*TipoNombProcConOSinParam)() ;
#endif
#endif


#endif


#ifndef ASM_SYM
#ifndef ASM_RISC

#if defined (__cplusplus) || defined (c__plusplus)
extern "C" {
#endif

#if defined (__STDC__) || defined (__cplusplus) || defined (c__plusplus)
PUBLIC TipoFuncionVoidParaSignal 
PX_signal(int sig_num,TipoFuncionVoidParaSignal func);

#if defined (__cplusplus) || defined (c__plusplus)
} /* de extern "C" */
#endif

#else
PUBLIC TipoFuncionVoidParaSignal 
PX_signal();
#endif
#else
#ifdef General_IMP

#else
	.globl PX_signal
#endif
#endif
#else
#ifdef General_IMP
	SECTION	prog,,c
	XDEF	_PX_signal
#else
	XREF	_PX_signal
#endif
#endif



#ifndef ASM_SYM
#ifndef ASM_RISC

#if defined (__cplusplus) || defined (c__plusplus)
extern "C" {
#endif

#if defined (__STDC__) || defined (__cplusplus) || defined (c__plusplus)
PUBLIC TipoFuncionVoidParaSignal 
PX_Signal(int sig_num, TipoFuncionVoidParaSignal func);

#if defined (__cplusplus) || defined (c__plusplus)
} /* de extern "C" */
#endif

#else
PUBLIC TipoFuncionVoidParaSignal 
PX_Signal();
#endif
#else
#ifdef General_IMP

#else
	.globl PX_Signal
#endif
#endif
#else
#ifdef General_IMP
	SECTION	prog,,c
	XDEF	_PX_Signal
#else
	XREF	_PX_Signal
#endif
#endif


#ifndef ASM_SYM
#ifndef ASM_RISC

typedef char          			TipoNombreFichero[MAX_LONG_NOM_FICH];
#endif
#endif


#ifndef ASM_SYM
#ifndef ASM_RISC

typedef unsigned int			TipoPrioridad;
#endif
#endif


#ifndef ASM_SYM
#ifndef ASM_RISC

typedef int 				TipoIndice;
#endif
#endif


#ifndef ASM_SYM
#ifndef ASM_RISC

typedef unsigned long                   TipoProcesador;
#endif
#endif


/* ================================================================= */
/*   Utilizado por las tablas hash's : con la clave en el elemento   */
/*   o la clave pasada como parametro.                               */
/* ================================================================= */

#ifndef ASM_SYM
#ifndef ASM_RISC
typedef         char  * PtrElemento;
#endif
#endif


#ifndef ASM_SYM
#ifndef ASM_RISC
typedef         char  * PtrClaveTh;
#endif
#endif

#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)

#ifndef ASM_SYM
#ifndef ASM_RISC
typedef         BOOL    (*TipoFuncionDeIgualdad)(PtrElemento, PtrElemento);
#endif
#endif


#ifndef ASM_SYM
#ifndef ASM_RISC
typedef         int     (*TipoFuncionHash)(PtrElemento, int MaxTamanyo);
#endif
#endif

#else

#ifndef ASM_SYM
#ifndef ASM_RISC
typedef         BOOL    (*TipoFuncionDeIgualdad)();
#endif
#endif


#ifndef ASM_SYM
#ifndef ASM_RISC
typedef         int     (*TipoFuncionHash)();
#endif
#endif

#endif

#ifndef ASM_SYM
#ifndef ASM_RISC

typedef struct UnaTablaHash {
        int                     MaxTamanyo;
        int                     IndRecorrido;
        TipoFuncionDeIgualdad   FuncionDeIgualdad;
        TipoFuncionHash         FuncionHash;
        PtrElemento*            PtrElementos;
        int                     Entradas;
} *TipoPxTablaHash;
#endif
#endif


#define MAXIMO  ((char *)2147483647)

/* ================================================================= */


/**********************  FUNCIONES EXPORTADAS  ***********************/

  /*
   *   La siguiente funcion realiza lo mismo que el comando Unix
   * "basename": obtiene el nombre del fichero "fichero" en la
   * variable "base", quitando el path, es decir si "fichero" vale
   * "/users/infra/bin/uproc", la varible "base" contendra "uproc".
   * (Actualizacion PLATEX --> PUEN v1 (3/5/95))
   */



#ifndef ASM_SYM
#ifndef ASM_RISC

#if defined (__cplusplus) || defined (c__plusplus)
extern "C" {
#endif

#if defined (__STDC__) || defined (__cplusplus) || defined (c__plusplus)
PUBLIC void
GENERAL_basename(char * fichero, char * base);

#if defined (__cplusplus) || defined (c__plusplus)
} /* de extern "C" */
#endif

#else
PUBLIC void
GENERAL_basename();
#endif
#else
#ifdef General_IMP

#else
	.globl GENERAL_basename
#endif
#endif
#else
#ifdef General_IMP
	SECTION	prog,,c
	XDEF	_GENERAL_basename
#else
	XREF	_GENERAL_basename
#endif
#endif


  /*
   *   La siguiente funcion quita los blancos del final de un string acabado
   *  en '\0'. Tipicamente se aplicara a strings devueltos por la BDC.
   *  La forma de usarlo tipica es:
   *
   *  BDC_TipoNombreSimbolico miNombre;
   *  BDC_MiNombreSimbolico (&miNombre);
   *  GENERAL_QuitarBlancos(miNombre, sizeof(BDC_TipoNombreSimbolico));
   *
   *  Notar que el tipo BDC_TipoNombreSimbolico es: 
   *
   *  typedef char BDC_TipoNombreSimbolico[BDC_LON_NOM_APLICACION+1];
   */


#ifndef ASM_SYM
#ifndef ASM_RISC

#if defined (__cplusplus) || defined (c__plusplus)
extern "C" {
#endif

#if defined (__STDC__) || defined (__cplusplus) || defined (c__plusplus)
PUBLIC void GENERAL_QuitarBlancos(char * s, int l);

#if defined (__cplusplus) || defined (c__plusplus)
} /* de extern "C" */
#endif

#else
PUBLIC void GENERAL_QuitarBlancos();
#endif
#else
#ifdef General_IMP

#else
	.globl GENERAL_QuitarBlancos
#endif
#endif
#else
#ifdef General_IMP
	SECTION	prog,,c
	XDEF	_GENERAL_QuitarBlancos
#else
	XREF	_GENERAL_QuitarBlancos
#endif
#endif



  /*
   *   La siguiente funcion 'aplana' el valor entero corto pasado como 
   * primer argumento, almacenando el resultado a partir de la 
   * posicion de memoria apuntada por el puntero pasado como segundo 
   * argumento.
   */


#ifndef ASM_SYM
#ifndef ASM_RISC

#if defined (__cplusplus) || defined (c__plusplus)
extern "C" {
#endif

#if defined (__STDC__) || defined (__cplusplus) || defined (c__plusplus)
PUBLIC void
GENERAL_AplanarShort (short int valorShort, char * buffer);

#if defined (__cplusplus) || defined (c__plusplus)
} /* de extern "C" */
#endif

#else
PUBLIC void
GENERAL_AplanarShort ();
#endif
#else
#ifdef General_IMP

#else
	.globl GENERAL_AplanarShort 
#endif
#endif
#else
#ifdef General_IMP
	SECTION	prog,,c
	XDEF	_GENERAL_AplanarShort 
#else
	XREF	_GENERAL_AplanarShort 
#endif
#endif


  /*
   *   La siguiente funcion devuelve un entero corto fruto de 'desaplanar'
   * el valor entero que estaba almacenado en formato aplanado a
   * partir de la posicion de memoria apuntada por el puntero pasado
   * como argumento.
   */


#ifndef ASM_SYM
#ifndef ASM_RISC

#if defined (__cplusplus) || defined (c__plusplus)
extern "C" {
#endif

#if defined (__STDC__) || defined (__cplusplus) || defined (c__plusplus)
PUBLIC short int
GENERAL_DesaplanarShort (char * buffer);

#if defined (__cplusplus) || defined (c__plusplus)
} /* de extern "C" */
#endif

#else
PUBLIC short int
GENERAL_DesaplanarShort ();
#endif
#else
#ifdef General_IMP

#else
	.globl GENERAL_DesaplanarShort 
#endif
#endif
#else
#ifdef General_IMP
	SECTION	prog,,c
	XDEF	_GENERAL_DesaplanarShort 
#else
	XREF	_GENERAL_DesaplanarShort 
#endif
#endif


  /*
   *   La siguiente funcion 'aplana' el valor entero pasado como 
   * primer argumento, almacenando el resultado a partir de la 
   * posicion de memoria apuntada por el puntero pasado como segundo 
   * argumento.
   */


#ifndef ASM_SYM
#ifndef ASM_RISC

#if defined (__cplusplus) || defined (c__plusplus)
extern "C" {
#endif

#if defined (__STDC__) || defined (__cplusplus) || defined (c__plusplus)
PUBLIC void
GENERAL_AplanarEntero (int valorEntero, char * buffer);

#if defined (__cplusplus) || defined (c__plusplus)
} /* de extern "C" */
#endif

#else
PUBLIC void
GENERAL_AplanarEntero ();
#endif
#else
#ifdef General_IMP

#else
	.globl GENERAL_AplanarEntero 
#endif
#endif
#else
#ifdef General_IMP
	SECTION	prog,,c
	XDEF	_GENERAL_AplanarEntero 
#else
	XREF	_GENERAL_AplanarEntero 
#endif
#endif


  /*
   *   La siguiente funcion devuelve un entero fruto de 'desaplanar'
   * el valor entero que estaba almacenado en formato aplanado a
   * partir de la posicion de memoria apuntada por el puntero pasado
   * como argumento.
   */


#ifndef ASM_SYM
#ifndef ASM_RISC

#if defined (__cplusplus) || defined (c__plusplus)
extern "C" {
#endif

#if defined (__STDC__) || defined (__cplusplus) || defined (c__plusplus)
PUBLIC int
GENERAL_DesaplanarEntero (char * buffer);

#if defined (__cplusplus) || defined (c__plusplus)
} /* de extern "C" */
#endif

#else
PUBLIC int
GENERAL_DesaplanarEntero ();
#endif
#else
#ifdef General_IMP

#else
	.globl GENERAL_DesaplanarEntero 
#endif
#endif
#else
#ifdef General_IMP
	SECTION	prog,,c
	XDEF	_GENERAL_DesaplanarEntero 
#else
	XREF	_GENERAL_DesaplanarEntero 
#endif
#endif


  /*
   *   La siguiente funcion 'aplana' el valor entero largo pasado 
   * como primer argumento, almacenando el resultado a partir de la 
   * posicion de memoria apuntada por el puntero pasado como segundo 
   * argumento.
   */


#ifndef ASM_SYM
#ifndef ASM_RISC

#if defined (__cplusplus) || defined (c__plusplus)
extern "C" {
#endif

#if defined (__STDC__) || defined (__cplusplus) || defined (c__plusplus)
PUBLIC void
GENERAL_AplanarLong (long valorLong, char * buffer);

#if defined (__cplusplus) || defined (c__plusplus)
} /* de extern "C" */
#endif

#else
PUBLIC void
GENERAL_AplanarLong ();
#endif
#else
#ifdef General_IMP

#else
	.globl GENERAL_AplanarLong 
#endif
#endif
#else
#ifdef General_IMP
	SECTION	prog,,c
	XDEF	_GENERAL_AplanarLong 
#else
	XREF	_GENERAL_AplanarLong 
#endif
#endif


  /*
   *   La siguiente funcion devuelve un entero largo fruto de 
   * 'desaplanar' el valor entero que estaba almacenado en formato 
   * aplanado a partir de la posicion de memoria apuntada por el 
   * puntero pasado como argumento.
   */


#ifndef ASM_SYM
#ifndef ASM_RISC

#if defined (__cplusplus) || defined (c__plusplus)
extern "C" {
#endif

#if defined (__STDC__) || defined (__cplusplus) || defined (c__plusplus)
PUBLIC long
GENERAL_DesaplanarLong (char * buffer);

#if defined (__cplusplus) || defined (c__plusplus)
} /* de extern "C" */
#endif

#else
PUBLIC long
GENERAL_DesaplanarLong ();
#endif
#else
#ifdef General_IMP

#else
	.globl GENERAL_DesaplanarLong 
#endif
#endif
#else
#ifdef General_IMP
	SECTION	prog,,c
	XDEF	_GENERAL_DesaplanarLong 
#else
	XREF	_GENERAL_DesaplanarLong 
#endif
#endif



#endif
