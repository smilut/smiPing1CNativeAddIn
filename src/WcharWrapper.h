#ifndef __WCHARWRAPPER_H__
#define __WCHARWRAPPER_H__

#include "ComponentBase.h"

/*
перенесено из примера its. В инете пишут, что это для преобразования
текста при передаче его в 1С в юникод.

Удалил секции компилируемые для linux

Эта часть вынесена в отдельный модуль, что бы не загружать кодом
основной рабочий файл, в данном случае templNative.h
*/
class WcharWrapper
{
public:
	WcharWrapper(const wchar_t* str);
	~WcharWrapper();

	operator const wchar_t*() { return m_str_wchar; }
	operator wchar_t*() { return m_str_wchar; }
private:
	WcharWrapper & operator = (const WcharWrapper& other) {};
	WcharWrapper(const WcharWrapper& other) {};
private:
	wchar_t* m_str_wchar;
};

#endif //__WCHARWRAPPER_H__
