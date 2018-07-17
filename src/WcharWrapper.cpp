/*
взято из примера its и вынесено в отдельный блок,
т.к. не требует редактирования при написании нового 
компонента и является базовым функционалом.

Эта часть вынесена в отдельный модуль, что бы не загружать кодом
основной рабочий файл, в данном случае templNative.cpp
*/
#include "WcharWrapper.h"

WcharWrapper::WcharWrapper(const wchar_t* str) :
	m_str_wchar(NULL)
{
	if (str)
	{
		int len = wcslen(str);
		m_str_wchar = new wchar_t[len + 1];
		memset(m_str_wchar, 0, sizeof(wchar_t) * (len + 1));
		memcpy(m_str_wchar, str, sizeof(wchar_t) * len);
	}

}
//---------------------------------------------------------------------------//
WcharWrapper::~WcharWrapper()
{
	if (m_str_wchar)
	{
		delete[] m_str_wchar;
		m_str_wchar = NULL;
	}
}
//---------------------------------------------------------------------------//