#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <windows.h>
#include <WinBase.h>

char *GetIniKeyString(char *key);
char *SkipSpace(char *str);
unsigned long long TransType(char *Buffer);
char *DelTheComma(char *str);
char *Record(char *Buffer);
char* Record_s(const char* Buffer);
unsigned long long GetDiskFree(char *DiskStr);
char *DeleteLog(char *NewDiskStr, char *tempath);



/**********函数名: GetIniKeyString**************
作用:   获取ini文件中相对应字符串的值

flag:  标志位,确定section后开始循环查找key值
wTmp:  字符串地址,该函数中用于定位'='
Title: ini文件中section名
sLine: 临时存储读取的值

输入值: str
返回值: str
**********************************************/

char *GetIniKeyString(char *key)
{
    char section[] = "test";
    char filename[] = "record.ini";
    FILE *fp = NULL;
    int  flag = 0;
    char *wTmp;
    char Title[32];
    char sLine[1024];

    sprintf(Title, "[%s]", section);
    if (NULL == (fp = fopen(filename, "r")))
    {
        Record_s("Fail to open the 'ini' file!");
        return NULL;
    }
    else
    {
        while (NULL != fgets(sLine, 1024, fp))
        {

            //两种ini配置文件中注释语句,跳过到下一条
            if (0 == strncmp("//", sLine, 2)) continue;
            if ('#' == sLine[0])              continue;

            wTmp = strchr(sLine, '=');
            if ((NULL != wTmp) && (1 == flag))
            {
                //sLine是'='长度,取到'='前key的值
                if (0 == strncmp(key, sLine, wTmp - sLine))
                {
                    // 长度依文件读取的为准
                    // 后期获取整型对字符串进行类型转换,防止整型读取少一位,所以在函数外对字符串进行处理删除换行符'\n'
                    //sLine[strlen(sLine) - 1] = '\0';
                    fclose(fp);
                    //返回值到'='后对应的字符串
                    return wTmp + 1;
                }
            }
            else
            {
                if (strncmp(Title, sLine, strlen(sLine) - 1) == 0)
                {
                    // 长度依文件读取的为准
                    // 找到标题位置
                    flag = 1;
                }
            }
        }
    }
    fclose(fp);
    return NULL;
}


/**********函数名: SkipSpace********************
作用: 删除字符串前所有空格

输入值: str
返回值: str
**********************************************/

char *SkipSpace(char *str)
{
    char *s = str,*p = str;

    if(str == NULL)
        return 0;

    //跳过开头的空格
    while(*s != '\0' && *s == ' ')
        s++;

    //复制剩余字符到新位置,p总是前于s刚才跳过空格个数那么多位置
    while(*s != '\0')
        *p++ = *s++;

    //字串结束标识符
    *p = '\0';

    return str;
}


/**********函数名: TransType********************
作用: 对字符串类型转换为可数学计算整型

ByteNum: 字节数

输入值: ByteNum, Buffer
返回值: ByteNum
**********************************************/

unsigned long long TransType(char *Buffer)
{
    unsigned long long ByteNum;
    sscanf(Buffer,"%llu",&ByteNum);
    return ByteNum;
}


/**********函数名: DelTheComma******************
作用: 处理字符串，删除字符串间隔','

输入值: str
返回值: str
**********************************************/

char *DelTheComma(char *str)
{
    int j = 0;
    int tmp = strlen(str);
    for(int i = 0; i < strlen(str); i++)
    {
        if(str[i] != ',')
        {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';

    return str;
}


/**********函数名: Record***********************
作用: 记录时间和文件名

time: 存储获得的日期时间

输入值: Buffer
返回值: NULL
**********************************************/

char *Record(char *Buffer)
{
    FILE *fp;
    char time[255] = { 0 };


    if ((fp = _popen("echo %date% %time%", "r")) == NULL)
    {
        Record_s("Fail to store log, Please check whether the catalog has been emptied. \n");
        exit(1);
    }


    fgets(time, 255, fp);
    time[strlen(time) - 1] = '\0';           //替换换行符
    fp = fopen("record.txt", "a+");

    fprintf(fp, "%s     %s\n", time, Buffer);
    fprintf(fp, "\n");

    fclose(fp);

    _pclose(fp);

    return NULL;
}


char* Record_s(const char* Buffer)
{
    FILE* fp;
    char time[255] = { 0 };


    if ((fp = _popen("echo %date% %time%", "r")) == NULL)
    {
        Record_s("Fail to store log, Please check whether the catalog has been emptied. \n");
        exit(1);
    }


    fgets(time, 255, fp);
    time[strlen(time) - 1] = '\0';           //替换换行符
    fp = fopen("record_s.txt", "a+");

    fprintf(fp, "%s     %s\n", time, Buffer);
    fprintf(fp, "\n");

    fclose(fp);

    _pclose(fp);

    return NULL;
}


/**********函数名: GetDiskFreeSpace*************
作用:  获得磁盘剩余空间

DiskStr:          第一管道的操作代码
NewDiskStr:       第二管道的操作代码
Buffer:           存放目录
token:            存放','
StrLine[1024]:    每行最大读取的字符数,可根据实际情况扩大
ByteNum:          磁盘空间剩余字节数(单位/比特)
GNum:             磁盘空间剩余字节数(单位/G比特)

输入值: DiskStr
返回值: ByteNum
**********************************************/

unsigned long long GetDiskFree(char *DiskStr)
{
    FILE *fp;

    char *Buffer;
    char *token;
    char buf[4] = " ";

    static char StrLine[1024];

    unsigned long long ByteNum = 0;
    unsigned long long GNum = 0;


    if ((fp = _popen(DiskStr, "r")) == NULL)
        return -1;

    while (!feof(fp))
    {
        fgets(StrLine,1024,fp);              //读取一行,并定位到下一行

        if (strstr(StrLine, "可用字节") != NULL)
            break;    
    }
    _pclose(fp);
    
    SkipSpace(StrLine);
    token = strtok(StrLine,buf);
    if (token == NULL)
        return -1;
    else if(atoi(token) == 2)
        return -1;


    for(int temp = 0;(void)(token != NULL), temp < 2; temp++)
    {
        token = strtok(NULL,buf);
    }

    Buffer = token;
    DelTheComma(Buffer);
    ByteNum = TransType(Buffer);
    GNum = ByteNum / 1024 / 1024 / 1024;


    return GNum;
}


/**********函数名: DeleteLog********************

作用:  执行日志删除操作

path[64]:        存储路径
Buffer:          存储目录
StrLine[1024]:   行读取
tempath:         输出目录

输入值: NewDiskStr
返回值: Buffer
**********************************************/

char *DeleteLog(char *NewDiskStr, char *tempath)
{
    FILE *fp;

    char path[1024];
    char *Buffer;

    char *StrLine;
    StrLine = (char*)malloc(1024);
    if((fp = _popen(NewDiskStr, "r")) == NULL)       //判断是否存在及可读
    {
        Record_s("error!");
        return NULL;
    }

    fgets(StrLine,1024,fp);                                //读取一行
    StrLine[strlen(StrLine) - 1] = '\0';               //替换换行符
    sprintf(path,"rmdir /s/q %s\\\"%s\"", tempath, StrLine);

    system(path);                                               //删除目录
    printf("delete success!   %s\n", StrLine);

    _pclose(fp);

    return StrLine;
}


/**********线程函数*****************************
DiskStr[64]:       时间顺序输出目录详细信息（包括磁盘剩余空间）
NewDiskStr[64]:    只保留目录
tempath[64]:       临时保存主目录
GNum:              磁盘空间剩余字节数(单位/G比特)
Buffer:            存储目录
tmp:               限定值,当磁盘剩余空间小于限定值时开始循环
interval:          磁盘检查间隔时间(单位/分钟)
intervals:         磁盘检查间隔时间(单位/毫秒)
tmpath1[] = "diskstr";    文件路径
tmpath2[] = "size";         磁盘剩余空间
**********************************************/

DWORD WINAPI ThreadFun()
{
    char DiskStr[1024] = { 0 };
    char NewDiskStr[1024] = { 0 };
    char tempath[1024] = { 0 };

    //替换字符串末尾的换行符'\n'
    //Record_s("to do");
    char tmpath1[] = "diskstr";
    char* tempbuffer = GetIniKeyString(tmpath1);

    if (tempbuffer == NULL)
        return 0;
    else
        strcpy(tempath, GetIniKeyString(tmpath1));
    
    tempath[strlen(tempath) - 1] = '\0';
    sprintf(DiskStr,"dir %s/od", tempath);
    DiskStr[strlen(DiskStr)] = '\0';

    sprintf(NewDiskStr,"dir %s/od/b", tempath);
    NewDiskStr[strlen(NewDiskStr)] = '\0';
    
    //Record_s("to do");
    int tmp = 0;
    char tmpath2[] = "size";
    
    if (GetIniKeyString(tmpath2) == NULL)
    {
        Record_s("set storage = 0");
        return 0;
    }
    else
        tmp = atoi(GetIniKeyString(tmpath2));
    
    int tmpFree = GetDiskFree(DiskStr);
    if (tmpFree == -1)
        Record_s("The content is empty!");
    Record_s("start");
    if (tmpFree > tmp)
    {   
        char tmprecord[1024];
        sprintf(tmprecord, "Last storage %d > %d", GetDiskFree(DiskStr), tmp);
        Record_s(tmprecord);
        return 0;
    }

    while(GetDiskFree(DiskStr) < tmp && GetDiskFree(DiskStr) > 0)
        {
            int temp = GetDiskFree(DiskStr);
            char *Buffer = DeleteLog(NewDiskStr, tempath);
            Record(Buffer);
            delete Buffer;
            Sleep(1000);         
        }
    Record_s("end");

    return 0;
}



/*------------------------主函数------------------------*/
/*------------------------主函数------------------------*/
/*------------------------主函数------------------------*/



int main()
{
    HANDLE handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFun, 0, 0, 0);
    WaitForSingleObject(handle, INFINITE);

    return 0;
}
