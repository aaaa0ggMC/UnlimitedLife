///This file is not of c linkage
#ifndef AAAA_UTIL_H_INCLUDED
#define AAAA_UTIL_H_INCLUDED
#include <string>
#include <vector>
#include <fstream>

#ifndef DLL_EXPORT
#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif
#endif // DLL_EXPORT

// ǰ����ɫ�궨��
#define ALIB_PRINTCOLOR_BLACK_FRONT 0
#define ALIB_PRINTCOLOR_BLUE_FRONT 1
#define ALIB_PRINTCOLOR_GREEN_FRONT 2
#define ALIB_PRINTCOLOR_CYAN_FRONT 3
#define ALIB_PRINTCOLOR_RED_FRONT 4
#define ALIB_PRINTCOLOR_MAGENTA_FRONT 5
#define ALIB_PRINTCOLOR_YELLOW_FRONT 6
#define ALIB_PRINTCOLOR_WHITE_FRONT 7
#define ALIB_PRINTCOLOR_GRAY_FRONT 8
#define ALIB_PRINTCOLOR_LIGHT_BLUE_FRONT 9
#define ALIB_PRINTCOLOR_LIGHT_GREEN_FRONT 10
#define ALIB_PRINTCOLOR_LIGHT_CYAN_FRONT 11
#define ALIB_PRINTCOLOR_LIGHT_RED_FRONT 12
#define ALIB_PRINTCOLOR_LIGHT_MAGENTA_FRONT 13
#define ALIB_PRINTCOLOR_LIGHT_YELLOW_FRONT 14
#define ALIB_PRINTCOLOR_BRIGHT_WHITE_FRONT 15

// ������ɫ�궨��
#define ALIB_PRINTCOLOR_BLACK_BACK (0 << 4)
#define ALIB_PRINTCOLOR_BLUE_BACK (1 << 4)
#define ALIB_PRINTCOLOR_GREEN_BACK (2 << 4)
#define ALIB_PRINTCOLOR_CYAN_BACK (3 << 4)
#define ALIB_PRINTCOLOR_RED_BACK (4 << 4)
#define ALIB_PRINTCOLOR_MAGENTA_BACK (5 << 4)
#define ALIB_PRINTCOLOR_YELLOW_BACK (6 << 4)
#define ALIB_PRINTCOLOR_WHITE_BACK (7 << 4)
#define ALIB_PRINTCOLOR_GRAY_BACK (8 << 4)
#define ALIB_PRINTCOLOR_LIGHT_BLUE_BACK (9 << 4)
#define ALIB_PRINTCOLOR_LIGHT_GREEN_BACK (10 << 4)
#define ALIB_PRINTCOLOR_LIGHT_CYAN_BACK (11 << 4)
#define ALIB_PRINTCOLOR_LIGHT_RED_BACK (12 << 4)
#define ALIB_PRINTCOLOR_LIGHT_MAGENTA_BACK (13 << 4)
#define ALIB_PRINTCOLOR_LIGHT_YELLOW_BACK (14 << 4)
#define ALIB_PRINTCOLOR_BRIGHT_WHITE_BACK (15 << 4)


namespace alib {
namespace ng {
using namespace std;
using dstring = const std::string&;
using mem_bytes = __int64;


/** \brief Program Memory ����ʹ���ڴ�
 *  mem memory �ڴ�
 *  vmem virtual memory �����ڴ�
 */
struct DLL_EXPORT MemTp {
    mem_bytes mem;
    mem_bytes vmem;
};
/** \brief Global Memory Usage ȫ���ڴ�ʹ�����������(not)OpenGL!!!)
 * percent currently_being_used_mem/mem_all �ڴ�ʹ�ðٷֱ�
 * phy_all capacity of physical mem �����ڴ�����
 * vir_all ...
 * you can understand ���ܿ�����
 */
struct DLL_EXPORT GlMem {
    unsigned int percent;
    mem_bytes phy_all;
    mem_bytes vir_all;
    mem_bytes phy_used;
    mem_bytes vir_used;
    mem_bytes page_all;
    mem_bytes page_used;
};

/** \brief GetCPUInfo ��ȡCPU��Ϣ
 * Ŀǰֻ֧�ֻ�ȡCPU Id
 */
struct DLL_EXPORT CPUInfo {
    std::string CpuID;
    CPUInfo();
};

/** \brief Utility ������
 * �⻹��ʲôҪ���ģ���
 */
class DLL_EXPORT Util {
public:
///io
    //ͨ��const��������֧��const char*��std::string
    /** \brief print with colors ��ɫ���
    *
    * print something with a custom color
    * ������Զ�����ɫ���ַ���
    *
    * \param what u want to print ��Ҫ��ӡʲô
    * \param color ��ɫ
    * \return just like printf ��printfһ��
    */
    static int io_printColor(dstring message,int color);
    /** \brief traverse files �����ļ�
    *
    * traverse all files in a folder(not included sub-folders)
    * ����һ���ļ�������������ļ������������ļ��У�
    *
    * \param folder path �ļ���·��
    * \param a vector to store these file names һ��std::vector���ڴ������
    */
    static void io_traverseFiles(dstring path, std::vector<std::string>& files);
    /** \brief get file size ��ȡ�ļ���С
     *
     * get file size efficiently using direct.h (better than fstream::seekg&tellg[ChatGPT says])
     * ʹ��direct.h���ٻ�ȡ�ļ���С(��fstream::seekg&tellg��[ChatGPT˵��]��
     *
     * \param file path �ļ�·��
     * \return file size �ļ���С
     */
    static long io_fileSize(dstring filePath);
    /** \brief read a file
     *
     * read the rest content of a std::ifstream
     * ��ȡstd::ifstream��ʣ�µ���������
     *
     * \param ifstream
     * \return content ���ݣ��и�ʣ����
     */
    static std::string io_readAll(std::ifstream & reader);
    /** \brief read a file
     *
     *  read a file
     *  ��ȡ�ļ�
     *
     * \param path
     * \return content
     */
    static std::string io_readAll(dstring path);
    /** \brief write data д������
     *
     * \param file path
     * \param
     * \return 0 success,-1 error
     */
    static int io_writeAll(dstring path,dstring data);
    /** \brief check file/directory existence ����ļ������ļ��У�Ŀ¼���������
     * \param path
     * \return �Ƿ����
     *
     */
    static bool io_checkExistence(dstring path);

///other
    /** \brief returns a time formatted as string
     *
     * time!!!!
     * ʱ��!!!
     *
     * \return time as string,fmt: "YY-MM-DD HH:MM:SS" �����ַ�����ʱ�䣬��ʽ "����-����-���� ʱʱ:�ַ�:����"
     */
    static string ot_getTime();
    /** \brief format duration ��ʽ�����ʱ��
     * \param seconds ����
     * \return formatted string ��ʽ�����ַ���
     */
    static std::string ot_formatDuration(int msecs);

///system
    /** \brief GetCPUId
     *
     * \return CPUId
     */
    static string sys_GetCPUId();
    /** \brief get program memory usage(bytes) currently ��ȡ����Ŀǰ�ڴ�ʹ�����(��λ:B)
     * \return mem stats �ڴ�ʹ�����
     */
    static MemTp sys_getProgramMemoryUsage();
    /** \brief get global(bytes) ��ȡȫ���ڴ�ʹ�����(��λ:B)
     * \return usage
     */
    static GlMem sys_getGlobalMemoryUsage();

///data_string
    /** \brief unescaping strings ��ת���ַ���
     * \param data
     * \return unescaped string
     */
    static std::string str_unescapeString(dstring in);
    //�з���ֵ��û����ֵ������
    static void str_trim_nrt(std::string& str);
    static std::string str_trim_rt(std::string& str);
    /** \brief trim string �Ƴ��ַ���ǰ��հ��ַ�
     * \param template<bool returnACopy = false> �Լ���
     * \param string to be modified Ҫ�޸ĵ�string
     */
    template<bool returnACopy = false> static inline auto str_trim(std::string& str) {
        if constexpr(returnACopy)return str_trim_rt(str);
        else str_trim_nrt(str);
    }
    /** \brief split strings as small tokens �ָ��ַ���
     * \param source
     * \param a token
     * \param a storer
     */
    static void str_split(dstring source,const char separator,std::vector<std::string> & restorer);
    /** \brief split strings as small tokens �ָ��ַ���
     * \param source
     * \param a token as string
     * \param a storer
     */
    static void str_split(dstring source,dstring separatorString,std::vector<std::string>& restorer);

///data_string_encoding
    /** \brief transcode ת��
     * \param ansi
     * \return utf8
     */
    static std::string str_encAnsiToUTF8(dstring strAnsi);
    /** \brief transcode ת��
     * \param utf8
     * \return ansi
     */
    static std::string str_encUTF8ToAnsi(dstring strUTF8);

    operator =(Util&) = delete;
    Util() = delete;
};
}
}
#endif // AAAA_UTIL_H_INCLUDED
