#ifndef __GSTRING_SMART_STRING_CLASS__
#define __GSTRING_SMART_STRING_CLASS__ 1

// This is just a streamlined version of std::string.
// It cleans itself up automatically, it can copy itself,
//   test for equality, etc. 
// and you can use .c_str() to get the string const, so it's
//   sytactically the same (usage-wise) as std::string.
// Written by Ryan Geiss.

#include <windows.h>

class GString 
{
public:
    GString() 
    { 
        m_data = new wchar_t[1]; 
        m_data[0] = 0;
        m_size = 1; 
    }

	GString(const wchar_t* src)
	{
        m_data = NULL; 
        m_size = 0; 
        operator=(src);
    }

	GString(const GString &src_string)
	{
        m_data = NULL; 
        m_size = 0; 
        operator=(src_string);
    }

    ~GString() 
    {
        delete m_data;  // note: delete is safe on NULL ptrs
        m_data = NULL;
        m_size = 0;
    }

    inline GString& operator=(const wchar_t* src)
	{
        if (src != m_data)  // don't do anything on "x = x.c_str();"
        {
            delete m_data;  // note: delete is safe on NULL ptrs
            if (src) 
            {
                m_size = wcslen(src)+1;
                m_data = new wchar_t[m_size];
                memcpy(m_data, src, m_size*2);
            }
            else
            {
                m_size = 1;
                m_data = new wchar_t[1];
                m_data[0] = 0;
            }
        }
        return *this;
    }

    inline GString& operator=(const GString &src_string)
	{
        if (&src_string != this)  // don't do anything on "x = x;"
        {
            if (src_string.GetSize() != m_size)  //optimization
            {
                delete m_data;
                m_size = src_string.GetSize();
                m_data = new wchar_t[m_size];
            }
            memcpy(m_data, src_string.c_str(), m_size*2);
        }
        return *this;
	}

    inline wchar_t operator[](int index) const
    {
        return m_data[index];
    }

    inline bool operator==(const wchar_t* comp)
	{
        return (wcscmp(m_data,comp) == 0);
	}

    inline bool operator==(const GString &comp)
	{
        if (m_size != comp.m_size)  // shortcut
            return false;
        return (wcscmp(m_data,comp.c_str()) == 0);     //return operator==(comp.m_data);
	}

	inline const wchar_t* c_str() const
	{
        return m_data;
	}

    // This is actually unsafe, but we need it for convenience, unless we really
    // feel like copying all data twice.  BE WARNED!  When this class reallocates
    // memory, all old references to _data are invalid!
    //operator const char*() const { return _data; }
    inline int GetSize() const  //in bytes - including terminating NULL char.
    {
        return m_size;
    }

    inline int GetLength() const  
    {
        return (m_size >= 1) ? m_size-1 : 0;
    }

private:
    wchar_t* m_data;
    int   m_size;
};

class GStringA
{
public:
    GStringA() 
    { 
        m_data = new char[1]; 
        m_data[0] = 0;
        m_size = 1;
    }

	GStringA(const char* src)
	{
        m_data = NULL;
        m_size = 0;
        operator=(src);
    }

	GStringA(const GStringA &src_string)
	{
        m_data = NULL;
        m_size = 0;
        operator=(src_string);
    }

    ~GStringA()
    {
        delete m_data;  // note: delete is safe on NULL ptrs
        m_data = NULL;
        m_size = 0;
    }

    inline GStringA& operator=(const char* src)
	{
        if (src != m_data)  // don't do anything on "x = x.c_str();"
        {
            delete m_data;  // note: delete is safe on NULL ptrs
            if (src)
            {
                m_size = strlen(src)+1;
                m_data = new char[m_size];
                memcpy(m_data, src, m_size);
            }
            else
            {
                m_size = 1;
                m_data = new char[1];
                m_data[0] = 0;
            }
        }
        return *this;
    }

    inline GStringA& operator=(const GStringA &src_string)
	{
        if (&src_string != this)  // don't do anything on "x = x;"
        {
            if (src_string.GetSize() != m_size)  //optimization
            {
                delete m_data;
                m_size = src_string.GetSize();
                m_data = new char[m_size];
            }
            memcpy(m_data, src_string.c_str(), m_size);
        }
        return *this;
	}

    inline char operator[](int index) const
    {
        return m_data[index];
    }

    inline bool operator==(const char* comp)
	{
        return (strcmp(m_data,comp) == 0);
	}

    inline bool operator==(const GStringA &comp)
	{
        if (m_size != comp.m_size)  // shortcut
            return false;
        return (strcmp(m_data,comp.c_str()) == 0);     //return operator==(comp.m_data);
	}

	inline const char* c_str() const
	{
        return m_data;
	}

    // This is actually unsafe, but we need it for convenience, unless we really
    // feel like copying all data twice.  BE WARNED!  When this class reallocates
    // memory, all old references to _data are invalid!
    //operator const char*() const { return _data; }
    inline int GetSize() const  //in bytes - including terminating NULL char.
    {
        return m_size;
    }

    inline int GetLength() const  
    {
        return (m_size >= 1) ? m_size-1 : 0;
    }

private:
    char* m_data;
    int   m_size;
};

#endif