
//блокировка повторной загрузки заголовка
//возможно любое уникальное имя, обычно используют имя файла заголовка
#ifndef __SMIPING1CNATIVEADDIN_H__
#define __SMIPING1CNATIVEADDIN_H__

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
		ePingIsComplit, //по-умолчанию false
		//поля полученного результатта
		eGoodPingPercent, //процент успешных пингов
		eMinTTL, //минимальное время прохождения пинга
		eMaxTTL, //минимальное время прохождения пинга
        eLastProp      // Always last
    };

    enum Methods
    {
		ePing = 0, //посылаем пинг по заданным параметрам 
		/*
		при запуске устанавливает ePingIsComplit в false
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

    // Attributes
	//взято из примера its
	IAddInDefBase * m_iConnect;
	IMemoryManager *m_iMemory;

	//поля для инициализации
	char* m_strAddress; //ip адрес или имя компьютера для пинга. По-умолчанию localhost
	int m_intPingCount; //количество пингов по-умолчанию 1
	int	m_intPackageSizeB; //размер тестового пакета в Байтах по-умолчанию 8B
	bool m_boolPingIsComplit; //индикатор завершения пингования по-умолчанию false
	//поля полученного результатта
	int m_intGoodPingPercent; //процент успешных пингов
	int m_intMinTTL; //минимальное время прохождения пинга
	int m_intMaxTTL; //минимальное время прохождения пинга
		 
	//---------------------------------------------------------------------------//
	//прикладные методы компоненты
	bool SendPing(); // посылаем пинг по заданным параметрам 
					 // при запуске устанавливает ePingIsComplit в false
					 // при завершении в true
					 
	
};
#endif //__SMIPING1CNATIVEADDIN_H__
