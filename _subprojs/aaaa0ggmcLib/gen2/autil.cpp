#include <alib/autil.h>
#include <time.h>
#include <malloc.h>
#include <stdio.h>
#include <windows.h>
#include <direct.h>
#include <io.h>
#include <sys/stat.h>
#include <stdint.h>
#include <psapi.h>
using namespace alib::ng;

int Util::io_printColor(dstring message,int color) {
    static CONSOLE_SCREEN_BUFFER_INFO info;
    [[maybe_unused]] static BOOL v = GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&info);

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),(WORD)color);
    int rt = printf("%s",message.c_str());
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),info.wAttributes);
    return rt;
}

std::string Util::ot_getTime() {
    time_t rawtime;
    struct tm *ptminfo;
    std::string rt = "";
    time(&rawtime);
    ptminfo = localtime(&rawtime);
    char * mdate = (char *)malloc(sizeof(char) * (512));
    memset(mdate,0,sizeof(char) * (512));
    sprintf(mdate,"%02d-%02d-%02d %02d:%02d:%02d",
            ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
            ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);
    rt = mdate;
    free(mdate);
    return rt;
}

std::string Util::sys_GetCPUId() {
    long lRet;
    HKEY hKey;
    CHAR tchData[1024];
    DWORD dwSize;

    lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("Hardware\\Description\\System\\CentralProcessor\\0"),0,KEY_QUERY_VALUE,&hKey);

    if(lRet == ERROR_SUCCESS) {
        dwSize = 1024;
        lRet = RegQueryValueEx(hKey,TEXT("ProcessorNameString"),0,0,(LPBYTE)tchData,&dwSize);

        tchData[dwSize] = '\0';
        RegCloseKey(hKey);
        if(lRet == ERROR_SUCCESS) {
            return std::string(tchData);
        } else {
            return "Unknown";
        }
    }
    return "Unknown";
}

void Util::io_traverseFiles(dstring path, std::vector<std::string>& files) {
    //文件句柄 注意：我发现有些文章代码此处是long类型，实测运行中会报错访问异常
    intptr_t hFile = 0;
    //文件信息
    struct _finddata_t fileinfo;
    std::string p;
    p.reserve(MAX_PATH);
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
        do {
            p.clear();
            //如果是目录,递归查找
            //如果不是,把文件绝对路径存入vector中
            if ((fileinfo.attrib & _A_SUBDIR)) {
                ///In this app,we do not need to do it
                //if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                //getFileNames(p.assign(path).append("\\").append(fileinfo.name), files);
            } else {
                files.push_back(p.append(path).append("\\").append(fileinfo.name));
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
}

std::string Util::str_unescape(dstring in) {
    std::string out = "";
    for(size_t i = 0; i < in.length(); i++) {
        if(in[i] == '\\') {
            if(++i == in.length()) return out; // 边界检查
            switch(in[i]) {
            case 'n'://New Line
                out += '\n';
                break;
            case '\\'://Backslash
                out += '\\';
                break;
            case 'v'://vertical
                out += '\v';
                break;
            case 't'://tab
                out += '\t';
                break;
            case 'r'://return
                out += '\r';
                break;
            case 'a'://alll
                out += '\007';
                break;
            case 'f'://换页符
                out += '\f';
                break;
            case 'b'://退格符
                out += '\b';
                break;
            case '0'://空字符
                out += '\0';
                break;
            case '?'://问号
                out += '\?';
                break;
            case '\"'://双引号
                out += '\"';
                break;
            default:
                i--;
                out += in[i];
                break;
            }
        } else {
            out += in[i];
        }
    }
    return out;
}


long Util::io_fileSize(dstring filePath) {
    struct stat statbuf;
    int ret;
    ret = stat(filePath.c_str(),&statbuf);//调用stat函数
    if(ret != 0) return ALIB_ERROR;//获取失败。
    return statbuf.st_size;//返回文件大小。
}

int Util::io_writeAll(dstring fth,dstring s) {
    std::ofstream of;
    of.open(fth);
    if(!of.is_open())return ALIB_ERROR;
    of.write(s.c_str(),s.length());
    of.close();
    return ALIB_SUCCESS;
}

int Util::io_readAll(std::ifstream & reader,string & ss) {
    unsigned long pos0 = reader.tellg();
    reader.seekg(0,std::ios::end);
    unsigned long pos1 = reader.tellg();
    reader.seekg(pos0,std::ios::beg);
    unsigned long size = pos1 - pos0+1;

    char * buf = new char[size];
    reader.read(buf,size-1);
    buf[size-1] = 0;
    ss.append(buf);

    delete [] buf;

    return ALIB_SUCCESS;
}

int Util::io_readAll(dstring fpath,std::string & ss) {
    unsigned long size = io_fileSize(fpath);

    std::ifstream reader(fpath);
    if(!reader.good())return ALIB_ERROR;

    char * buf = new char[size+1];

    reader.read(buf,size);
    buf[size] = 0;
    reader.close();

    ss.append(buf);

    delete [] buf;

    return ALIB_SUCCESS;
}

std::string Util::str_trim_rt(std::string & str) {
    std::string blanks("\f\v\r\t\n ");
    std::string temp;
    for(int i = 0; i < (int)str.length(); i++) {
        if(str[i] == '\0')
            str[i] = '\t';
    }
    str.erase(0,str.find_first_not_of(blanks));
    str.erase(str.find_last_not_of(blanks) + 1);
    temp = str;
    return temp;
}

void Util::str_trim_nrt(std::string & str) {
    std::string blanks("\f\v\r\t\n ");
    std::string temp;
    for(int i = 0; i < (int)str.length(); i++) {
        if(str[i] == '\0')
            str[i] = '\t';
    }
    str.erase(0,str.find_first_not_of(blanks));
    str.erase(str.find_last_not_of(blanks) + 1);
    temp = str;
}

void Util::str_split(dstring line,const char sep,std::vector<std::string> & vct) {
    const size_t size = line.size();
    const char* str = line.c_str();
    int start = 0,end = 0;
    for(int i = 0; i < (int)size; i++) {
        if(str[i] == sep) {
            string st = line.substr(start,end);
            str_trim(st);
            vct.push_back(st);
            start = i + 1;
            end = 0;
        } else
            end++;
    }
    if(end > 0) {
        string st = line.substr(start,end);
        str_trim(st);
        vct.push_back(st);
    }
}

void Util::str_split(dstring str,dstring splits, std::vector<std::string>& res) {
    if(!str.compare(""))return;
    //在字符串末尾也加入分隔符，方便截取最后一段
    std::string strs = str + splits;
    size_t pos = strs.find(splits);
    int step = splits.size();

    // 若找不到内容则字符串搜索函数返回 npos
    while (pos != strs.npos) {
        std::string temp = strs.substr(0, pos);
        res.push_back(temp);
        //去掉已分割的字符串,在剩下的字符串中进行分割
        strs = strs.substr(pos + step, strs.size());
        pos = strs.find(splits);
    }
}

bool Util::io_checkExistence(dstring name) {
    struct stat buffer;
    return(stat(name.c_str(), &buffer) == 0);
}

std::string Util::str_encAnsiToUTF8(dstring strAnsi) {
    std::string strOutUTF8 = "";
    WCHAR *str1;
    int n = MultiByteToWideChar(CP_ACP, 0, strAnsi.c_str(), -1, NULL, 0);
    str1 = new WCHAR[n];
    MultiByteToWideChar(CP_ACP, 0, strAnsi.c_str(), -1, str1, n);
    n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
    char *str2 = new char[n];
    WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
    strOutUTF8 = str2;
    delete [] str1;
    str1 = NULL;
    delete [] str2;
    str2 = NULL;
    return strOutUTF8;
}


std::string Util::str_encUTF8ToAnsi(dstring strUTF8) {
    int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(),  - 1, NULL, 0);
    WCHAR * wsz = new WCHAR[len + 1];
    memset(wsz, 0, (len+1)*sizeof(WCHAR));
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUTF8.c_str(),  - 1, wsz, len);
    len = WideCharToMultiByte(CP_ACP, 0, wsz,  - 1, NULL, 0, NULL, NULL);
    char *sz = new char[len + 1];
    memset(sz, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wsz,  - 1, sz, len, NULL, NULL);
    std::string strTemp(sz);
    delete [] sz;
    sz = NULL;
    delete [] wsz;
    wsz = NULL;
    return strTemp;
}

std::string Util::ot_formatDuration(int msecs) {
    int sec = msecs%60;
    msecs /= 60;
    int min = msecs % 60;
    msecs /= 60;
    int hour = msecs % 60;
    msecs /= 60;
    int day = msecs % 24;
    msecs /= 24;
    int year = msecs % 356;
    std::string ret = "";
    if(year != 0)ret += to_string(year) + "y ";
    if(day != 0)ret += to_string(day) + "d ";
    if(hour != 0)ret += to_string(hour) + "h ";
    if(min != 0)ret += to_string(min) + "m ";
    if(sec != 0)ret += to_string(sec) + "s";
    return ret;
}

MemTp Util::sys_getProgramMemoryUsage() {
    mem_bytes mem = 0, vmem = 0;
    PROCESS_MEMORY_COUNTERS pmc;

    // get process hanlde by pid
    HANDLE process = GetCurrentProcess();
    if (GetProcessMemoryInfo(process, &pmc, sizeof(pmc))) {
        mem = pmc.WorkingSetSize;
        vmem = pmc.PagefileUsage;
    }
    CloseHandle(process);

    // use GetCurrentProcess() can get current process and no need to close handle

    return {mem,vmem};
}

GlMem Util::sys_getGlobalMemoryUsage() {
    MEMORYSTATUSEX statex;
    GlMem ret = {0};
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);

    ret.phy_all = statex.ullTotalPhys;
    ret.phy_used = ret.phy_all - statex.ullAvailPhys;
    ret.vir_all = statex.ullTotalVirtual;
    ret.vir_used = ret.vir_all - statex.ullAvailVirtual;
    ret.page_all = statex.ullTotalPageFile;
    ret.page_used = statex.ullTotalPageFile - statex.ullAvailPageFile;
    ret.percent = statex.dwMemoryLoad;

    return ret;
}

CPUInfo::CPUInfo() {
#ifdef BUILD_ON_WINDOWS
    this->CpuID = Util::sys_GetCPUId();
#endif // BUILD_ON_WINDOWS
}
