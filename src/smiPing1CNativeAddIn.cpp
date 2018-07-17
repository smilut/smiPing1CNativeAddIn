/*
Использовал https://infostart.ru/public/184119/
Для успешной сборки добавил папку 
include этого проекта в строку 'Включаемые каталоги'
lib в строку 'Каталоги библиотек'

Свойства проекта (правая клавиша мыши) - Свойства конфигурации - Каталоги C++

Расcчитываем, что это работает только под Win
*/

#include "stdafx.h"

#include <stdio.h>
#include <wchar.h>
#include <string>

//создан по образцу из its.
//класс для преобразования строки к юникоду
//урезан для функционирования только для win 
#include "WcharWrapper.h"

//собственный заголовочный файл, в котором будем описывать свои классы
#include "smiPing1CNativeAddIn.h"

/*объявляем коллекции свойств и методов для en и ru, взято из примена its
но упрощено до двух полей: Field1,2 соответствуют Поле1,2
и двух методов Method1,2 соответствуют Метод1,2
*/

static const wchar_t *g_PropNames[] = {
	L"ForWrite", /*свойство соответствует eTempProp*/
	L"ForRead"
};
static const wchar_t *g_MethodNames[] = {
	L"WriteField", //метод соответствует eTempMethod
	L"ReadField"   //метод соответствует eTempMethodFunc
};

static const wchar_t *g_PropNamesRu[] = {
	L"ДляЗаписи",  /*свойство соответствует eTempProp*/
	L"ДляЧтения"   /*свойство соответствует eTempPropReadable*/
};
static const wchar_t *g_MethodNamesRu[] = {
	L"ЗаписьПоля", //метод соответствует eTempMethod
	L"ЧтениеПоля"  //метод соответствует eTempMethodFunc
};

//меняем имя класса из примера CAddInNative на свое CTemplNative
//для разработки нового класса надо будед заменить CTemplNative на более подходящее имя
//в файлах templNative.cpp, h, def. Файлы и проект переименовываем соответственно
//
//Не забыть переименовать имя класса во всех объявлениях методов, 
//а так же конструктора и деструктора
//
static const wchar_t g_kClassNames[] = L"CTemplNative"; //|OtherClass1|OtherClass2";

uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len = 0);
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len = 0);
uint32_t getLenShortWcharStr(const WCHAR_T* Source);

//добавлено из примера its
static AppCapabilities g_capabilities = eAppCapabilitiesInvalid;
static WcharWrapper s_names(g_kClassNames);

//---------------------------------------------------------------------------//
/*Документация its. Получение списка имен объектов компоненты.*/
const WCHAR_T* GetClassNames()
{
	/* так было в шаблоне
	static WCHAR_T* names = 0;
	if (!names)
		::convToShortWchar(&names, g_kClassNames);
	return names;
	*/
	//Взято из применра its
	return s_names;
}
//---------------------------------------------------------------------------//
/*Документация its. Создание экземпляра объекта компоненты. Если объект не может быть создан или не найден объект с указанным именем – возвращается 0.*/
long GetClassObject(const wchar_t* wsName, IComponentBase** pInterface)
{
    if(!*pInterface)
    {
        *pInterface= new CTemplNative();
        return (long)*pInterface;
    }
    return 0;
}
//---------------------------------------------------------------------------//
/*Документация its. Удаление экземпляра ранее созданного объекта.Компонента должна своими средствами 
удалить объект и освободить используемую им память.При успешном завершении возвращается 0, 
иначе – код ошибки(Runtime error).*/
long DestroyObject(IComponentBase** pIntf)
{
   if(!*pIntf)
      return -1;

   delete *pIntf;
   *pIntf = 0;
   return 0;
}
//---------------------------------------------------------------------------//
/*Документация its. Устанавливает версию поддерживаемых платформой возможностей.
Компонента должна вернуть версию, с которой она может работать.
Если функция не реализована, то для компоненты не будут доступны возможности 
вывода сообщений, запроса информации о платформе.

<capabilities> Тип: перечисление AppCapabilities. 
Значения перечисления: eAppCapabilitiesInvalid = -1, 
						eAppCapabilities1 = 1, 
						eAppCapabilitiesLast = eAppCapabilities1
*/
AppCapabilities SetPlatformCapabilities(const AppCapabilities capabilities)
{
	g_capabilities = capabilities;
	return eAppCapabilitiesLast;
}
//---------------------------------------------------------------------------//
//CTemplNative
CTemplNative::CTemplNative()
{
	m_iMemory = 0;
	m_iConnect = 0;

	m_boolTempProp = false; //приватное свойство соответствующее eTempProp
						//для демонстрационных целей только для зписи и чтения через метод
	m_boolTempPropReadable = false; //приватное свойство соответствующее eTempProp
							//для демонстрационных целей только для чтения и записи через метод
}
//---------------------------------------------------------------------------//
CTemplNative::~CTemplNative()
{
}
//---------------------------------------------------------------------------//
bool CTemplNative::Init(void* pConnection)
{ 
	m_iConnect = (IAddInDefBase*)pConnection;
	return m_iConnect != NULL;
}
//---------------------------------------------------------------------------//
long CTemplNative::GetInfo()
{ 
    return 2000; 
}
//---------------------------------------------------------------------------//
void CTemplNative::Done()
{
	//для демонстрации деактивации объекта
	if (m_boolTempProp)
	{
		m_boolTempProp = !m_boolTempProp;
	}
}
//---------------------------------------------------------------------------//
bool CTemplNative::RegisterExtensionAs(WCHAR_T** wsExtensionName)
{ 
	const wchar_t *wsExtension = L"TemplNativeExtension"; //имя компоненты
	//которую мы будем вызывать из 1С
	//например НашОбъект = Новый("AddIn.MyComponent.TemplNativeExtension"); 
	//MyComponent - имя компоненты, которое задается при подключении:
	//ПодключитьВнешнююКомпоненту(Файл, "MyComponent", ТипВнешнейКомпоненты.Native);
	//
	int iActualSize = ::wcslen(wsExtension) + 1;
	WCHAR_T* dest = 0;

	if (m_iMemory)
	{
		if (m_iMemory->AllocMemory((void**)wsExtensionName, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(wsExtensionName, wsExtension, iActualSize);
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------//
long CTemplNative::GetNProps()
{ 
	//возвращает номер последнего свойсва 
	//объявленного в перечислении заголовочного файла
    return eLastProp;
}
//---------------------------------------------------------------------------//
long CTemplNative::FindProp(const WCHAR_T* wsPropName)
{ 
	//Взято из примера its
	//поиск поля по имени
	long plPropNum = -1;
	wchar_t* propName = 0;

	::convFromShortWchar(&propName, wsPropName);
	plPropNum = findName(g_PropNames, propName, eLastProp);

	if (plPropNum == -1)
		plPropNum = findName(g_PropNamesRu, propName, eLastProp);

	delete[] propName;

	return plPropNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CTemplNative::GetPropName(long lPropNum, long lPropAlias)
{ 
	/*Взято из примера its*/
	if (lPropNum >= eLastProp)
		return NULL;

	wchar_t *wsCurrentName = NULL;
	WCHAR_T *wsPropName = NULL;
	int iActualSize = 0;

	switch (lPropAlias)
	{
	case 0: // First language
		wsCurrentName = (wchar_t*)g_PropNames[lPropNum];
		break;
	case 1: // Second language
		wsCurrentName = (wchar_t*)g_PropNamesRu[lPropNum];
		break;
	default:
		return 0;
	}

	iActualSize = wcslen(wsCurrentName) + 1;

	if (m_iMemory && wsCurrentName)
	{
		if (m_iMemory->AllocMemory((void**)&wsPropName, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(&wsPropName, wsCurrentName, iActualSize);
	}

	return wsPropName;
}
//---------------------------------------------------------------------------//
bool CTemplNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{ 
	/*Взято из примера its*/
	//возвращает значение свойства
	switch (lPropNum)
	{
	case eTempPropReadable://отдаем логическое значение свойства
		TV_VT(pvarPropVal) = VTYPE_BOOL;
		TV_BOOL(pvarPropVal) = m_boolTempPropReadable;
		break;
	case eTempProp: //можно не указывать, т.к. по-умолчанию поле не читается
		return false;
	default:
		return false;
	}

	//значение прочитано
	return true;
}
//---------------------------------------------------------------------------//
bool CTemplNative::SetPropVal(const long lPropNum, tVariant* varPropVal)
{ 
	/*Взято из примера its*/
	switch (lPropNum)
	{
	case eTempProp: //устанавливаем логическое значение в свойство
		if (TV_VT(varPropVal) != VTYPE_BOOL)
			return false;
		m_boolTempProp = TV_BOOL(varPropVal);
		break;
	case eTempPropReadable:
		return false; //приведено для примера обработки свойства не доступного для записи
		//поведение системы по-умолчанию
	default:
		return false;
	}

	//значение установлено
	return true;
}
//---------------------------------------------------------------------------//
bool CTemplNative::IsPropReadable(const long lPropNum)
{ 
	/*Взято из примера its*/
	switch (lPropNum)
	{
	/*пример указания не читабельного поля*/
	case eTempProp:
		return false;
	/*сообщаем что свойство соответствующее eTempPropReadable можно читать*/
	case eTempPropReadable:
		return true;
	default:
	/*по-дефолту читать нельзя*/
		return false;
	}

	return false;
}
//---------------------------------------------------------------------------//
bool CTemplNative::IsPropWritable(const long lPropNum)
{
	/*Взято из примера its*/
	switch (lPropNum)
	{
	/*сообщаем что свойство соответствующее eTempProp можно записывать*/
	case eTempProp:
		return true;
	/*пример указания поля не доступного для записи*/
	case eTempPropReadable:
		return false;
	default:
		return false;
	}

	return false;
}
//---------------------------------------------------------------------------//
long CTemplNative::GetNMethods()
{ //возвращает количество методов
    return eLastMethod;
}
//---------------------------------------------------------------------------//
long CTemplNative::findName(const wchar_t* names[], const wchar_t* name,
	const uint32_t size) const
{
	long ret = -1;
	for (uint32_t i = 0; i < size; i++)
	{
		if (!wcscmp(names[i], name))
		{
			ret = i;
			break;
		}
	}
	return ret;
}
//---------------------------------------------------------------------------//
long CTemplNative::FindMethod(const WCHAR_T* wsMethodName)
{ 
	/*Взято из примера its*/
	//ищет метод по имени
	long plMethodNum = -1;
	wchar_t* name = 0;

	::convFromShortWchar(&name, wsMethodName);

	plMethodNum = findName(g_MethodNames, name, eLastMethod);

	if (plMethodNum == -1)
		plMethodNum = findName(g_MethodNamesRu, name, eLastMethod);

	delete[] name;

	return plMethodNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CTemplNative::GetMethodName(const long lMethodNum, 
                            const long lMethodAlias)
{ 
	/*Взято из примера its*/
	//возвращает имя метода по указаному номеру и языку
	if (lMethodNum >= eLastMethod)
		return NULL;

	wchar_t *wsCurrentName = NULL;
	WCHAR_T *wsMethodName = NULL;
	int iActualSize = 0;

	switch (lMethodAlias)
	{
	case 0: // First language
		wsCurrentName = (wchar_t*)g_MethodNames[lMethodNum];
		break;
	case 1: // Second language
		wsCurrentName = (wchar_t*)g_MethodNamesRu[lMethodNum];
		break;
	default:
		return 0;
	}

	iActualSize = wcslen(wsCurrentName) + 1;

	if (m_iMemory && wsCurrentName)
	{
		if (m_iMemory->AllocMemory((void**)&wsMethodName, iActualSize * sizeof(WCHAR_T)))
			::convToShortWchar(&wsMethodName, wsCurrentName, iActualSize);
	}

	return wsMethodName;
}
//---------------------------------------------------------------------------//
long CTemplNative::GetNParams(const long lMethodNum)
{ 
	/*Взято из примера its
	возвращает количество параметров для заданного метода*/
	switch (lMethodNum)
	{
	case eTempMethod:
		return 1; /*у демо метода один параметр*/
	
	case eTempMethodFunc: /*у функции нет параметров*/
		return 0;
	default:
		return 0;
	}

    return 0;
}
//---------------------------------------------------------------------------//
bool CTemplNative::GetParamDefValue(const long lMethodNum, const long lParamNum,
                          tVariant *pvarParamDefValue)
{ 
	/*Взято из примера its
	Возвращает значения по-умолчанию для параметров методов
	*/
	TV_VT(pvarParamDefValue) = VTYPE_EMPTY;

	switch (lMethodNum)
	{
	case eTempMethod:
	case eTempMethodFunc:
	/*case eMethShowInStatusLine:
	case eMethStartTimer:
	case eMethStopTimer:
	case eMethShowMsgBox:
	*/
		// There are no parameter values by default 
		break;
	default:
		return false;
	}

	return false;
} 
//---------------------------------------------------------------------------//
bool CTemplNative::HasRetVal(const long lMethodNum)
{ 
	/*Взять из примера its
	возвращает признак есть ли у метода возвращаемое значение*/
	switch (lMethodNum)
	{
	/*У метода нет возвращаемого значения*/
	case eTempMethod:
		return false;
	/*У метода есть возвращаемое значение*/
	case eTempMethodFunc:
		return true;
	default:
		return false;
	}

	return false;
}
//---------------------------------------------------------------------------//
bool CTemplNative::CallAsProc(const long lMethodNum,
                    tVariant* paParams, const long lSizeArray)
{ 
	/*Взято из примера its*/
	/*вызов метода как процедуры*/
	switch (lMethodNum)
	{
	case eTempMethod: //вызов тестового метода как процедуры
		return fTempMethod(paParams, lSizeArray);
	//методы из примера its
	/*
	case eMethDisable:
		m_boolEnabled = false;
		break;
	case eMethShowInStatusLine:
		if (m_iConnect && lSizeArray)
		{
			tVariant *var = paParams;
			m_iConnect->SetStatusLine(var->pwstrVal);
		}
		break;
	case eMethStartTimer:
		pAsyncEvent = m_iConnect;
		m_hTimerQueue = CreateTimerQueue();
		CreateTimerQueueTimer(&m_hTimer, m_hTimerQueue,
			(WAITORTIMERCALLBACK)MyTimerProc, 0, 1000, 1000, 0);
		break;
	case eMethStopTimer:
		if (m_hTimer != 0)
		{
			DeleteTimerQueue(m_hTimerQueue);
			m_hTimerQueue = 0;
			m_hTimer = 0;
		}

		m_uiTimer = 0;
		pAsyncEvent = NULL;
		break;
	case eMethShowMsgBox:
	{
		if (eAppCapabilities1 <= g_capabilities)
		{
			IAddInDefBaseEx* cnn = (IAddInDefBaseEx*)m_iConnect;
			IMsgBox* imsgbox = (IMsgBox*)cnn->GetInterface(eIMsgBox);
			if (imsgbox)
			{
				IPlatformInfo* info = (IPlatformInfo*)cnn->GetInterface(eIPlatformInfo);
				assert(info);
				const IPlatformInfo::AppInfo* plt = info->GetPlatformInfo();
				if (!plt)
					break;
				tVariant retVal;
				tVarInit(&retVal);
				if (imsgbox->Confirm(plt->AppVersion, &retVal))
				{
					bool succeed = TV_BOOL(&retVal);
					WCHAR_T* result = 0;

					if (succeed)
						::convToShortWchar(&result, L"OK");
					else
						::convToShortWchar(&result, L"Cancel");

					imsgbox->Alert(result);
					delete[] result;

				}
			}
		}
	}
	break;
	*/
	default:
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------//
bool CTemplNative::CallAsFunc(const long lMethodNum,
                tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{ 
	bool ret = false;//признак успешного вызова метода

	switch (lMethodNum)
	{
		case eTempMethodFunc:
		{
			//пример работы с возвращаемым значением
			return fTempMethodFunc(pvarRetValue, paParams, lSizeArray);
		}
		break;

	}
    return ret; 
}
//---------------------------------------------------------------------------//
void CTemplNative::SetLocale(const WCHAR_T* loc)
{
	//Взято из примера its
	_wsetlocale(LC_ALL, loc);
}
//---------------------------------------------------------------------------//
bool CTemplNative::setMemManager(void* mem)
{	//Взято из примера its
	m_iMemory = (IMemoryManager*)mem;
	return m_iMemory != 0;
}
//---------------------------------------------------------------------------//
uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len)
{
    if (!len)
        len = ::wcslen(Source)+1;

    if (!*Dest)
        *Dest = new WCHAR_T[len];

    WCHAR_T* tmpShort = *Dest;
    wchar_t* tmpWChar = (wchar_t*) Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len*sizeof(WCHAR_T));
    do
    {
        *tmpShort++ = (WCHAR_T)*tmpWChar++;
        ++res;
    }
    while (len-- && *tmpWChar);

    return res;
}
//---------------------------------------------------------------------------//
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len)
{
    if (!len)
        len = getLenShortWcharStr(Source)+1;

    if (!*Dest)
        *Dest = new wchar_t[len];

    wchar_t* tmpWChar = *Dest;
    WCHAR_T* tmpShort = (WCHAR_T*)Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len*sizeof(wchar_t));
    do
    {
        *tmpWChar++ = (wchar_t)*tmpShort++;
        ++res;
    }
    while (len-- && *tmpShort);

    return res;
}
//---------------------------------------------------------------------------//
uint32_t getLenShortWcharStr(const WCHAR_T* Source)
{
    uint32_t res = 0;
    WCHAR_T *tmpShort = (WCHAR_T*)Source;

    while (*tmpShort++)
        ++res;

    return res;
}
//---------------------------------------------------------------------------//


//---------------------------------------------------------------------------//
//прикладные методы компоненты
bool CTemplNative::fTempMethod(tVariant* paParams, const long lSizeArray) //обработка eTempMethod1
{
	//устанавливаем не доступное для записи свойство eTempProp1.
	//вызывается в 1С как процедура, для 1С не имеет возвращаемого параметра
	//в компоненте возвращает признак успешного вызова метода
	//пример работы с параметрами
	if (lSizeArray != 1 || !paParams) //если кол-во параметров не 1 или параметр не установлен
		return false;

	if (TV_VT(paParams) != VTYPE_BOOL) //если тип параметра не булево
		return false;

	m_boolTempPropReadable = TV_BOOL(paParams);  //устанавливаем свойство соотв. eTempPropReadable по значению параметра
	return true;
}

bool CTemplNative::fTempMethodFunc(tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) //обработка eTempMethod2
{
	//читаем не доступное для чтения свойство eTempProp2.
	/*вызывается из 1С, как функция, в компоненту возвращает признак успешного выполнения модуля*/

	//у данной функции нет параметров просто возвращаем значение. Пример работы с параметрами в fTempMethod
	//устанавливаем логическое значение и тип булево
	pvarRetValue->bVal = m_boolTempProp;
	TV_VT(pvarRetValue) = VTYPE_BOOL;
	return true;
}