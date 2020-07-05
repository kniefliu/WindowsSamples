#include "qui/utils/qui_string.h"

#include <Windows.h>
#include <stdlib.h>

QuiString::QuiString()
    : wstr_(buffer_)
{
    buffer_[0] = L'\0';
}
QuiString::QuiString(const wchar_t ch)
    : wstr_(buffer_)
{
    buffer_[0] = ch;
    buffer_[1] = L'\0';
}
QuiString::QuiString(const QuiString& rhs)
    : wstr_(buffer_)
{
    buffer_[0] = L'\0';
    Assign(rhs.wstr_);
}
QuiString::QuiString(const wchar_t * wstrz, int len/* = -1*/)
{
    QuiASSERT(!::IsBadStringPtr(wstrz, (UINT_PTR)-1) || wstrz == NULL);
    buffer_[0] = L'\0';
    Assign(wstrz, len);
}
QuiString::~QuiString()
{
    if (buffer_ != wstr_) {
        free(wstr_);
    }
}
std::wstring QuiString::ToStdWString()
{
    // TODO
    return L"";
}
std::string QuiString::ToStdString()
{
    // TODO
    return "";
}

void QuiString::Empty()
{
    if (wstr_ != buffer_) {
        free(wstr_);
    }
    wstr_ = buffer_;
    buffer_[0] = L'\0';
}
int QuiString::GetLength() const
{
    return (int)wcslen(wstr_);
}
bool QuiString::IsEmpty() const
{
    return wstr_[0] == L'\0';
}
wchar_t QuiString::GetAt(int index) const
{
    return wstr_[index];
}
void QuiString::Append(const wchar_t * wstrz)
{
    int new_len = GetLength() + (int)wcslen(wstrz);
    if (new_len >= MAX_LOCAL_STRING_LEN) {
        if (wstr_ == buffer_) {
            wstr_ = static_cast<wchar_t *>(malloc((new_len + 1) * sizeof(wchar_t)));
            wcscpy(wstr_, buffer_);
            wcscat(wstr_, wstrz);
        }
        else {
            wstr_ = static_cast<wchar_t *>(realloc(wstr_, (new_len + 1) * sizeof(wchar_t)));
            wcscat(wstr_, wstrz);
        }
    }
    else {
        if (wstr_ != buffer_) {
            free(wstr_);
            wstr_ = buffer_;
        }
        wcscat(buffer_, wstrz);
    }
}
void QuiString::Assign(const wchar_t * wstrz, int len/* = -1*/)
{
    if (wstrz == NULL) {
        wstrz = L"";
    }
    len = (len < 0 ? (int)wcslen(wstrz) : len);
    if (len < MAX_LOCAL_STRING_LEN) {
        if (wstr_ != buffer_) {
            free(wstr_);
            wstr_ = buffer_;
        }
    }
    else if (len > GetLength() || wstr_ == buffer_) {
        if (wstr_ == buffer_) {
            wstr_ = NULL;
        }
        wstr_ = static_cast<wchar_t *>(realloc(wstr_, (len + 1) * sizeof(wchar_t)));
    }
    wcsncpy(wstr_, wstrz, len);
    wstr_[len] = L'\0';
}
const wchar_t *QuiString::GetData() const
{
    return wstr_;
}

void QuiString::SetAt(int index, wchar_t ch)
{
    QuiASSERT(index >= 0 && index < GetLength());
    wstr_[index] = ch;
}
QuiString::operator const wchar_t*() const
{
    return wstr_;
}

wchar_t QuiString::operator[](int index) const
{
    return wstr_[index];
}
const QuiString& QuiString::operator=(const QuiString& src)
{
    Assign(src);
    return *this;
}
const QuiString& QuiString::operator=(const wchar_t ch)
{
    Empty();
    buffer_[0] = ch;
    buffer_[1] = L'\0';
    return *this;
}
const QuiString& QuiString::operator=(const wchar_t* wstrz)
{
    if (wstrz) {
        QuiASSERT(!::IsBadStringPtr(wstrz, (UINT_PTR)-1));
        Assign(wstrz);
    }
    else {
        Empty();
    }
    return *this;
}
const QuiString& QuiString::operator=(const char* strz)
{
    if (strz) {
        QuiASSERT(!::IsBadStringPtrA(strz, (UINT_PTR)-1));
        int str_size = (int)strlen(strz) + 1;
        int wstr_size = str_size * sizeof(wchar_t);
        wchar_t * wstrz = (wchar_t*)_alloca(wstr_size);
        if (wstrz) {
            ::MultiByteToWideChar(::GetACP(), 0, strz, -1, wstrz, wstr_size);           
        }
        Assign(wstrz);
    }
    else {
        Empty();
    }
    return *this;
}
const QuiString& QuiString::operator+=(const char* strz)
{
    if (strz) {
        QuiASSERT(!::IsBadStringPtrA(strz, (UINT_PTR)-1));
        int str_size = (int)strlen(strz) + 1;
        int wstr_size = str_size * sizeof(wchar_t);
        wchar_t * wstrz = (wchar_t*)_alloca(wstr_size);
        if (wstrz) {
            ::MultiByteToWideChar(::GetACP(), 0, strz, -1, wstrz, wstr_size);
        }
        Append(wstrz);
    }
    return *this;
}
QuiString QuiString::operator+(const QuiString& src) const
{
    QuiString temp = *this;
    temp.Append(src);
    return temp;
}
QuiString QuiString::operator+(const wchar_t* wstrz) const
{
    if (wstrz) {
        QuiASSERT(!::IsBadStringPtr(wstrz, (UINT_PTR)-1));
        QuiString temp = *this;
        temp.Append(wstrz);
        return temp;
    }
    return *this;
}
const QuiString& QuiString::operator+=(const QuiString& src)
{
    Append(src);
    return *this;
}
const QuiString& QuiString::operator+=(const wchar_t * wstrz)
{
    if (wstrz) {
        QuiASSERT(!::IsBadStringPtr(wstrz, (UINT_PTR)-1));
        Append(wstrz);
    }
    return *this;
}
const QuiString& QuiString::operator+=(const wchar_t ch)
{
    wchar_t wstrz[] = { ch, L'\0' };
    Append(wstrz);
    return *this;
}

int QuiString::Compare(const wchar_t * wstrz) const
{
    return wcscmp(wstr_, wstrz);
}
int QuiString::CompareNoCase(const wchar_t * wstrz) const
{
    return wcsicmp(wstr_, wstrz);
}

void QuiString::MakeUpper()
{
    wcsupr(wstr_);
}
void QuiString::MakeLower()
{
    wcslwr(wstr_);
}

QuiString QuiString::Left(int len) const
{
    if (len < 0) len = 0;
    if (len > GetLength()) len = GetLength();
    return QuiString(wstr_, len);
}
QuiString QuiString::Mid(int pos, int len/* = -1*/) const
{
    if (pos < 0) pos = 0;
    if (len < 0) len = GetLength() - pos;
    if (pos + len > GetLength()) len = GetLength() - pos;
    if (len <= 0) return QuiString();
    return QuiString(wstr_ + pos, len);
}
QuiString QuiString::Right(int len) const
{
    int pos = GetLength() - len;
    if (pos < 0) {
        pos = 0;
        len = GetLength();
    }
    return QuiString(wstr_ + pos, len);
}

int QuiString::Find(const wchar_t ch, int pos/* = 0*/) const
{
    QuiASSERT(pos >= 0 && pos <= GetLength());
    if (pos != 0 && (pos < 0 || pos > GetLength())) {
        return -1;
    }
    const wchar_t * p = wcschr(wstr_ + pos, ch);
    if (!p) {
        return -1;
    }
    return (int)(p - wstr_);
}
int QuiString::Find(const wchar_t* sub_wstr, int pos/* = 0*/) const
{
    QuiASSERT(!::IsBadStringPtr(sub_wstr, (UINT_PTR)-1));
    QuiASSERT(pos >= 0 && pos <= GetLength());
    if (pos != 0 && (pos < 0 || pos > GetLength())) {
        return -1;
    }
    const wchar_t * p = wcsstr(wstr_ + pos, sub_wstr);
    if (!p) {
        return -1;
    }
    return (int)(p - wstr_);
}
int QuiString::ReverseFind(const wchar_t ch) const
{
    const wchar_t * p = wcsrchr(wstr_, ch);
    if (!p) {
        return -1;
    }
    return (int)(p - wstr_);
}
int QuiString::Replace(const wchar_t* from, const wchar_t* to)
{
    QuiString temp;
    int count = 0;
    int pos = Find(from);
    if (pos < 0) {
        return 0;
    }
    int from_len = (int)wcslen(from);
    int to_len = (int)wcslen(to);
    while (pos > 0) {
        temp = Left(pos);
        temp += to;
        temp += Mid(pos + from_len);
        Assign(temp);
        pos = Find(from, pos + to_len);
        count++;
    }
    return count;
}

int QuiString::Format(const wchar_t* format_wstrz, ...)
{
    wchar_t * sprintf_wstr = nullptr;
    va_list arg_list;
    int len;
    va_start(arg_list, format_wstrz);
    len = _vsnwprintf(nullptr, 0, format_wstrz, arg_list);
    sprintf_wstr = (wchar_t *)malloc((len + 1) * sizeof(wchar_t));
    ::ZeroMemory(sprintf_wstr, (len + 1) * sizeof(wchar_t));
    int ret = _vsnwprintf(sprintf_wstr, len + 1, format_wstrz, arg_list);
    va_end(arg_list);
    Assign(sprintf_wstr);
    free(sprintf_wstr);
    return ret;
}
int QuiString::SmallFormat(const wchar_t* format_wstrz, ...)
{
    QuiString format = format_wstrz;
    wchar_t buffer[MAX_LOCAL_STRING_LEN] = { 0 };
    va_list arg_list;
    va_start(arg_list, format_wstrz);
    int ret = ::wvsprintf(buffer, format, arg_list);
    va_end(arg_list);
    Assign(buffer);
    return ret;
}


struct QuiStringPtrMap::Element {
    QuiString key;
    void* data;
    struct QuiStringPtrMap::Element *prev;
    struct QuiStringPtrMap::Element *next;
};

static uint32_t HashKey(const wchar_t * key)
{
    uint32_t i = 0;
    size_t len = wcslen(key);
    while (len-- > 0) {
        i = (i << 5) + i + key[len];
    }
    return i;
}
//static uint32_t HashKey(const QuiString& key)
//{
//    return HashKey((const wchar_t *)key);
//}

QuiStringPtrMap::QuiStringPtrMap(int size/* = DEFAULT_MAP_SIZE*/)
    : count_(0)
{
    if (size < 16) size = 16;
    buckets_ = size;
    elements_ = new Element*[size];
    memset(elements_, 0, size * sizeof(Element *));
}
QuiStringPtrMap::~QuiStringPtrMap()
{
    ClearElements();
}

void QuiStringPtrMap::Resize(int size/* = DEFAULT_MAP_SIZE*/)
{
    ClearElements();

    if (size < 0) size = 0;
    if (size > 0) {
        elements_ = new Element*[size];
        memset(elements_, 0, size * sizeof(Element *));
    }
    buckets_ = size;
    count_ = 0;
}
void * QuiStringPtrMap::Find(const wchar_t* key, bool optimize/* = true*/) const
{
    if (buckets_ == 0 || GetSize() == 0) return nullptr;
    uint32_t slot = HashKey(key) % buckets_;
    for (Element * item = elements_[slot]; item; item = item->next) {
        if (item->key == key) {
            if (optimize && item != elements_[slot]) {
                if (item->next) {
                    item->next->prev = item->prev;
                }
                item->prev->next = item->next;
                item->prev = nullptr;
                item->next = elements_[slot];
                item->next->prev = item;
                // 
                elements_[slot] = item;
            }
            return item->data;
        }
    }
    return nullptr;
}
bool QuiStringPtrMap::Insert(const wchar_t* key, void* data)
{
    if (buckets_ == 0) return false;
    if (Find(key)) return false;

    uint32_t slot = HashKey(key) % buckets_;
    Element *item = new Element();
    item->key = key;
    item->data = data;
    item->prev = nullptr;
    item->next = elements_[slot];
    if (item->next)
        item->next->prev = item;
    elements_[slot] = item;
    count_++;
    return true;
}
void * QuiStringPtrMap::Set(const wchar_t* key, void* data)
{
    if (buckets_ == 0) return data;

    if (GetSize() > 0) {
        uint32_t slot = HashKey(key) % buckets_;
        for (Element * item = elements_[slot]; item; item = item->next) {
            if (item->key == key) {
                void * old_data = item->data;
                item->data = data;
                return old_data;
            }
        }
    }

    Insert(key, data);
    return nullptr;
}
bool QuiStringPtrMap::Remove(const wchar_t* key)
{
    if (buckets_ == 0 || GetSize() == 0) return false;

    uint32_t slot = HashKey(key) % buckets_;
    Element** items = &elements_[slot];
    while (*items) {
        if ((*items)->key == key) {
            Element * kill_item = *items;
            *items = (*items)->next;
            if (*items) {
                (*items)->prev = kill_item->prev;
            }
            delete kill_item;
            count_--;
            return true;
        }
        items = &((*items)->next);
    }
    return false;
}
void QuiStringPtrMap::RemoveAll()
{
    this->Resize(buckets_);
}
int QuiStringPtrMap::GetSize() const
{
#if QUI_DEBUG
    int count = 0;
    int len = buckets_;
    while (len--) {
        for (const Element* item = elements_[len]; item; item = item->next) {
            count++;
        }
    }
    QuiASSERT(count == count_);
#endif
    return count_;
}
const wchar_t * QuiStringPtrMap::GetAt(int index) const
{
    if (buckets_ == 0 || GetSize() == 0) return false;
    int pos = 0;
    int len = buckets_;
    while (len--) {
        for (Element *item = elements_[len]; item; item = item->next) {
            if (pos++ == index) {
                return item->key.GetData();
            }
        }
    }

    return nullptr;
}
const wchar_t * QuiStringPtrMap::operator[](int index) const
{
    return GetAt(index);
}

void QuiStringPtrMap::ClearElements()
{
    if (elements_) {
        int len = buckets_;
        while (len--) {
            Element* item = elements_[len];
            while (item) {
                Element * kill_item = item;
                item = item->next;
                delete kill_item;
            }
        }
        delete[] elements_;
        elements_ = nullptr;
    }
}
