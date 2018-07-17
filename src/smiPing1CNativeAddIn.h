
//блокировка повторной загрузки заголовка
//возможно любое уникальное им€, обычно используют им€ файла заголовка
#ifndef __SMIPING1CNATIVEADDIN_H__
#define __SMIPING1CNATIVEADDIN_H__

#include "ComponentBase.h"
#include "AddInDefBase.h"
#include "IMemoryManager.h"

//мен€ем им€ класса из примера CAddInNative на свое CTemplNative
///////////////////////////////////////////////////////////////////////////////
// class CTemplNative
class CTemplNative : public IComponentBase
{
public:
    enum Props
    {
		eTempProp = 0, //добавленое собственное свойство, измен€етс€ при написании реального компонента
		eTempPropReadable, //добавленое собственное свойство, измен€етс€ при написании реального компонента
        eLastProp      // Always last
    };

    enum Methods
    {
		eTempMethod = 0, //добавленый собственный метод, измен€етс€ при написании реального компонента дл€ вызова как процедура
		eTempMethodFunc, //добавленый собственный метод, измен€етс€ при написании реального компонента дл€ вызова как функци€
        eLastMethod      // Always last
    };

	//мен€ем им€ конструктора из примера CAddInNative на свое CTemplNative
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

    // Attributes
	//вз€то из примера its
	IAddInDefBase * m_iConnect;
	IMemoryManager     *m_iMemory;

	bool m_boolTempProp; //приватное свойство соответствующее eTempProp
	bool m_boolTempPropReadable; //приватное свойство соответствующее eTempPropReadable
		 
	//---------------------------------------------------------------------------//
	//прикладные методы компоненты
	bool fTempMethod(tVariant* paParams, const long lSizeArray);
	bool fTempMethodFunc(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);

};
#endif //__SMIPING1CNATIVEADDIN_H__
