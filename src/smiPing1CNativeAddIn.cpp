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

static const wchar_t *g_PropNames[] = {
	//поля для инициализации
	L"Address", //ip адрес или имя компьютера для пинга. По-умолчанию localhost
	L"PingCount", //количество пингов по-умолчанию 1
	L"PackageSizeB", //размер тестового пакета в Байтах по-умолчанию 8B
	L"PingIsComplit", //индикатор завершения пингования по-умолчанию false
	//поля полученного результатта
	L"GoodPingPercent", //процент успешных пингов
	L"MinTTL" //минимальное время прохождения пинга
	L"MaxTTL",//минимальное время прохождения пинга
};
static const wchar_t *g_MethodNames[] = {
	L"SendPing"
};

static const wchar_t *g_PropNamesRu[] = {
	//поля для инициализации
	L"Адрес", //ip адрес или имя компьютера для пинга. По-умолчанию localhost
	L"КоличествоПингов", //количество пингов по-умолчанию 1
	L"РазмерПакета", //размер тестового пакета в Байтах по-умолчанию 8B
	L"Завершено", //индикатор завершения пингования по-умолчанию false
	//поля полученного результатта
	L"ПроцентУспешныхПингов", //процент успешных пингов
	L"МинВремяПрохождения" //минимальное время прохождения пинга
	L"МаксВремяПрохождения",//минимальное время прохождения пинга
};
static const wchar_t *g_MethodNamesRu[] = {
	L"Пинг"
};

static const wchar_t g_kClassNames[] = L"CTemplNative";

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
	
	//поля для инициализации
	m_strAddress = "localhost"; //ip адрес или имя компьютера для пинга. По-умолчанию localhost
	m_intPingCount = 1; //количество пингов по-умолчанию 1
	m_intPackageSizeB =8; //размер тестового пакета в Байтах по-умолчанию 8B
	DropResultData();
}
//---------------------------------------------------------------------------//
CTemplNative::~CTemplNative()
{
}
//---------------------------------------------------------------------------//
void CTemplNative::DropResultData()
{
	m_boolPingIsComplit = false; //индикатор завершения пингования по-умолчанию false
								 //поля полученного результатта
	m_intGoodPingPercent = 0; //процент успешных пингов
	m_intMinTTL = 0; //минимальное время прохождения пинга
	m_intMaxTTL = 0; //минимальное время прохождения пинга
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
}
//---------------------------------------------------------------------------//
bool CTemplNative::RegisterExtensionAs(WCHAR_T** wsExtensionName)
{ 
	const wchar_t *wsExtension = L"smiPingNativeExtension"; //имя компоненты
	//которую мы будем вызывать из 1С
	//например НашОбъект = Новый("AddIn.MyComponent.smiPingNativeExtension"); 
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
	case eAddress:
		TV_VT(pvarPropVal) = VTYPE_PSTR;
		TV_STR(pvarPropVal) = m_strAddress;
		break;
	case ePingCount: 
		TV_VT(pvarPropVal) = VTYPE_INT;
		TV_INT(pvarPropVal) = m_intPingCount;
		break;
	case ePackageSizeB:
		TV_VT(pvarPropVal) = VTYPE_INT;
		TV_INT(pvarPropVal) = m_intPackageSizeB;
		break;
	case ePingIsComplit:
		TV_VT(pvarPropVal) = VTYPE_BOOL;
		TV_BOOL(pvarPropVal) = m_boolPingIsComplit;
		break;
	case eGoodPingPercent:
		TV_VT(pvarPropVal) = VTYPE_INT;
		TV_INT(pvarPropVal) = m_intGoodPingPercent;
		break;
	case eMinTTL:
		TV_VT(pvarPropVal) = VTYPE_INT;
		TV_INT(pvarPropVal) = m_intMinTTL;
		break;
	case eMaxTTL:
		TV_VT(pvarPropVal) = VTYPE_INT;
		TV_INT(pvarPropVal) = m_intMaxTTL;
		break;
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
	case eAddress: //устанавливаем логическое значение в свойство
		if (TV_VT(varPropVal) != VTYPE_PSTR)
			return false;
		m_strAddress = TV_STR(varPropVal);
		break;
	case ePingCount:
		if (TV_VT(varPropVal) != VTYPE_INT)
			return false;
		m_intPingCount = TV_INT(varPropVal);
		break;
	case ePackageSizeB:
		if (TV_VT(varPropVal) != VTYPE_INT)
			return false;
		m_intPackageSizeB = TV_INT(varPropVal);
		break;
	default:
		return false;
	}

	//значение установлено
	return true;
}
//---------------------------------------------------------------------------//
bool CTemplNative::IsPropReadable(const long lPropNum)
{ 
	//Взято из примера its
	/*
	switch (lPropNum)
	{
	//пример указания не читабельного поля
	case eTempProp:
		return false;
	//сообщаем что свойство соответствующее eTempPropReadable можно читать
	case eTempPropReadable:
		return true;
	default:
	//по-дефолту читать нельзя
		return false;
	}
	*/
	//все свойства читаемые
	return true;
}
//---------------------------------------------------------------------------//
bool CTemplNative::IsPropWritable(const long lPropNum)
{
	/*Взято из примера its*/
	switch (lPropNum)
	{
	/*сообщаем что эти свойства можно записывать*/
	case eAddress:
	case ePingCount:
	case ePackageSizeB:
		return true;
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
	/*
	switch (lMethodNum)
	{
	case eTempMethod:
		return 1; //у демо метода один параметр
	
	case eTempMethodFunc: //у функции нет параметров
		return 0;
	default:
		return 0;
	}
	*/
	//у методов класса нет параметров
    return 0;
}
//---------------------------------------------------------------------------//
bool CTemplNative::GetParamDefValue(const long lMethodNum, const long lParamNum,
                          tVariant *pvarParamDefValue)
{ 
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
	case ePing:
		return false;
	/*У метода есть возвращаемое значение*/
	//case eTempMethodFunc:
	//	return true;
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
	case ePing: //вызов тестового метода как процедуры
		return SendPing();
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
bool CTemplNative::SendPing() //обработка ePing
{
	DropResultData(); //обнуляем предыдущие результаты

	return true;
}
