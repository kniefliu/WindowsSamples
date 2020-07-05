#ifndef QUI_UTILS_STRING_H_
#define QUI_UTILS_STRING_H_

#include "qui/core/qui_prerequisites.h"

#include <string>

class QUI_EXPORT QuiString {
public:
    QuiString();
    QuiString(const wchar_t ch);
    QuiString(const QuiString& rhs);
    QuiString(const wchar_t * wstrz, int len = -1);
    ~QuiString();
    std::wstring ToStdWString();
    std::string ToStdString();

    void Empty();
    int GetLength() const;
    bool IsEmpty() const;
    wchar_t GetAt(int index) const;
    void Append(const wchar_t * wstrz);
    void Assign(const wchar_t * wstrz, int len = -1);
    const wchar_t *GetData() const;

    void SetAt(int index, wchar_t ch);
    operator const wchar_t*() const;

    wchar_t operator[](int index) const;
    const QuiString& operator=(const QuiString& src);
    const QuiString& operator=(const wchar_t ch);
    const QuiString& operator=(const wchar_t* wstrz);
    const QuiString& operator=(const char* strz);
    const QuiString& operator+=(const char* strz);
    QuiString operator+(const QuiString& src) const;
    QuiString operator+(const wchar_t* wstrz) const;
    const QuiString& operator+=(const QuiString& src);
    const QuiString& operator+=(const wchar_t * wstrz);
    const QuiString& operator+=(const wchar_t ch);

    bool operator==(const wchar_t * wstrz) const { return (Compare(wstrz) == 0); }
    bool operator!=(const wchar_t * wstrz) const { return (Compare(wstrz) != 0); }
    bool operator<=(const wchar_t * wstrz) const { return (Compare(wstrz) <= 0); }
    bool operator<(const wchar_t * wstrz) const { return (Compare(wstrz) < 0); }
    bool operator>=(const wchar_t * wstrz) const { return (Compare(wstrz) >= 0); }
    bool operator>(const wchar_t * wstrz) const { return (Compare(wstrz) > 0); }

    int Compare(const wchar_t * wstrz) const;
    int CompareNoCase(const wchar_t * wstrz) const;

    void MakeUpper();
    void MakeLower();

    QuiString Left(int len) const;
    QuiString Mid(int pos, int len = -1) const;
    QuiString Right(int len) const;

    int Find(const wchar_t ch, int pos = 0) const;
    int Find(const wchar_t* wstrz, int pos = 0) const;
    int ReverseFind(const wchar_t ch) const;
    int Replace(const wchar_t* from, const wchar_t* to);

    int Format(const wchar_t* format_wstrz, ...);
    int SmallFormat(const wchar_t* format_wstrz, ...);

private:
    enum {
        MAX_LOCAL_STRING_LEN = 63
    };

private:
    wchar_t * wstr_;
    wchar_t buffer_[MAX_LOCAL_STRING_LEN + 1];
};


class QUI_EXPORT QuiStringPtrMap {
public:
    enum {
        DEFAULT_MAP_SIZE = 83
    };
    struct Element;

public:
    QuiStringPtrMap(int size = DEFAULT_MAP_SIZE);
    ~QuiStringPtrMap();

    void Resize(int size = DEFAULT_MAP_SIZE);
    void * Find(const wchar_t* key, bool optimize = true) const;
    bool Insert(const wchar_t* key, void* data);
    void * Set(const wchar_t* key, void* data);
    bool Remove(const wchar_t* key);
    void RemoveAll();
    int GetSize() const;
    const wchar_t * GetAt(int index) const;
    const wchar_t * operator[](int index) const;

private:
    void ClearElements();

private:
    Element ** elements_;
    int buckets_;
    int count_;
};

#endif   // QUI_UTILS_STRING_H_
