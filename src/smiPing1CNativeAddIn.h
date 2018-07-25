
//блокировка повторной загрузки заголовка
//возможно любое уникальное имя, обычно используют имя файла заголовка
#ifndef __SMIPING1CNATIVEADDIN_H__
#define __SMIPING1CNATIVEADDIN_H__

#include "stdafx.h"

#define ICMP_ECHOREPLY	0
#define ICMP_ECHOREQ	8
#define REQ_DATASIZE  32

#include "ComponentBase.h"
#include "AddInDefBase.h"
#include "IMemoryManager.h"

//меняем имя класса из примера CAddInNative на свое CTemplNative
///////////////////////////////////////////////////////////////////////////////
// class CTemplNative
class CTemplNative : public IComponentBase
{
public:
    enum Props
    {
		//поля для инициализации
		eAddress = 0, //ip адрес или имя компьютера для пинга. По-умолчанию localhost
		ePingCount, //количество пингов по-умолчанию 1
		ePackageSizeB, //размер тестового пакета в Байтах по-умолчанию 8B
		//индикатор завершения пингования
		ePingIsComplete, //по-умолчанию false
		//поля полученного результатта
		eGoodPingPercent, //процент успешных пингов
		eMinTTL, //минимальное время прохождения пинга
		eMaxTTL, //минимальное время прохождения пинга
		eIsError, //признак возникновения ошибки при пинге
		eErrMessage, //сообщение об ошибках в процессе пингования
        eLastProp      // Always last
    };

    enum Methods
    {
		ePing = 0, //посылаем пинг по заданным параметрам 
		/*
		при запуске устанавливает ePingIsComplete в false
		при завершении в true
		*/
        eLastMethod      // Always last
    };

	//меняем имя конструктора из примера CAddInNative на свое CTemplNative
	CTemplNative(void);
    virtual ~CTemplNative();
    // IInitDoneBase
    virtual bool ADDIN_API Init(void*);
    virtual bool ADDIN_API setMemManager(void* mem);
    virtual long ADDIN_API GetInfo();
    virtual void ADDIN_API Done();
    // ILanguageExtenderBase
    virtual bool ADDIN_API RegisterExtensionAs(WCHAR_T** wsLanguageExt);
    virtual long ADDIN_API GetNProps();
    virtual long ADDIN_API FindProp(const WCHAR_T* wsPropName);
    virtual const WCHAR_T* ADDIN_API GetPropName(long lPropNum, long lPropAlias);
    virtual bool ADDIN_API GetPropVal(const long lPropNum, tVariant* pvarPropVal);
    virtual bool ADDIN_API SetPropVal(const long lPropNum, tVariant* varPropVal);
    virtual bool ADDIN_API IsPropReadable(const long lPropNum);
    virtual bool ADDIN_API IsPropWritable(const long lPropNum);
    virtual long ADDIN_API GetNMethods();
    virtual long ADDIN_API FindMethod(const WCHAR_T* wsMethodName);
    virtual const WCHAR_T* ADDIN_API GetMethodName(const long lMethodNum, 
                            const long lMethodAlias);
    virtual long ADDIN_API GetNParams(const long lMethodNum);
    virtual bool ADDIN_API GetParamDefValue(const long lMethodNum, const long lParamNum,
                            tVariant *pvarParamDefValue);
    virtual bool ADDIN_API HasRetVal(const long lMethodNum);
    virtual bool ADDIN_API CallAsProc(const long lMethodNum,
                    tVariant* paParams, const long lSizeArray);
    virtual bool ADDIN_API CallAsFunc(const long lMethodNum,
                tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
    operator IComponentBase*() { return (IComponentBase*)this; }
    // LocaleBase
    virtual void ADDIN_API SetLocale(const WCHAR_T* loc);
private:
	long findName(const wchar_t* names[], const wchar_t* name, const uint32_t size) const;
	void DropResultData();
	wchar_t* SetWCharPropertyVal(const WCHAR_T* src);
	wchar_t* to_wstring(std::string const str); //преобразование C-string в wchar_t, которые использует WinAPI
	 
    // Attributes
	//взято из примера its
	IAddInDefBase * m_iConnect;
	IMemoryManager *m_iMemory;

	//поля для инициализации
	wchar_t* m_strAddress; //ip адрес или имя компьютера для пинга. По-умолчанию localhost
	int m_intPingCount; //количество пингов по-умолчанию 1
	int	m_intPackageSizeB; //размер тестового пакета в Байтах по-умолчанию 8B
	bool m_boolPingIsComplete; //индикатор завершения пингования по-умолчанию false
	//поля полученного результатта
	int m_intGoodPingPercent; //процент успешных пингов
	int m_intMinElapsedTime; //минимальное время прохождения пинга
	int m_intMaxElapsedTime; //минимальное время прохождения пинга
	
	bool m_boolIsError; //признак возникновения ошибки при пинге
	wchar_t* m_strErrMessage; //сообщение об ошибках в процессе пингования

	//---------------------------------------------------------------------------//
	//прикладные методы компоненты
	bool Ping(); // посылаем пинг по заданным параметрам 
					 // при запуске устанавливает ePingIsComplete в false
					 // при завершении в true
					 
	int SendEchoRequest(SOCKET s, LPSOCKADDR_IN lpstToAddr);
	DWORD RecvEchoReply(SOCKET s, LPSOCKADDR_IN lpsaFrom, u_char *pTTL);
	int WaitForEchoReply(SOCKET s);
	u_short in_cksum(u_short *addr, int len);

};

// IP Header -- RFC 791
typedef struct tagIPHDR
{
	u_char  VIHL;			// Version and IHL
	u_char	TOS;			// Type Of Service
	short	TotLen;			// Total Length
	short	ID;				// Identification
	short	FlagOff;		// Flags and Fragment Offset
	u_char	TTL;			// Time To Live
	u_char	Protocol;		// Protocol
	u_short	Checksum;		// Checksum
	struct	in_addr iaSrc;	// Internet Address - Source
	struct	in_addr iaDst;	// Internet Address - Destination
}IPHDR, *PIPHDR;


// ICMP Header - RFC 792
typedef struct tagICMPHDR
{
	u_char	Type;			// Type
	u_char	Code;			// Code
	u_short	Checksum;		// Checksum
	u_short	ID;				// Identification
	u_short	Seq;			// Sequence
	char	Data;			// Data
}ICMPHDR, *PICMPHDR;

// ICMP Echo Request
typedef struct tagECHOREQUEST
{
	ICMPHDR icmpHdr;
	DWORD	dwTime;
	char	cData[REQ_DATASIZE];
}ECHOREQUEST, *PECHOREQUEST;


// ICMP Echo Reply
typedef struct tagECHOREPLY
{
	IPHDR	ipHdr;
	ECHOREQUEST	echoRequest;
	char    cFiller[256];
}ECHOREPLY, *PECHOREPLY;

#endif //__SMIPING1CNATIVEADDIN_H__
