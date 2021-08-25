# Delete-Catelog
It's a simple delete program.
Through the .ini document, you can change two values which are "path" and "size". Path is the folder path, the size is depends on your personal preference, like 100GB Remaining in the disk, if the disk storage less the the size value, execute the program, your path folder will be clean chronologically, earliest file will be remove first until the disk storage more the you set.
The file name and delete time will be record in record.txt
Example: 2021/08/20 Friday 14:32:00.70 Adobe After Effects 2020
# Features
## GetIniKeyString
get value in .ini file, some other functions in this function.
  SkipSpace
  TransType
  DelTheComma
  (These three functions used to operate on strings)
## Record
Record time and file name
## GetDiskFreeSpace
Read disk storage from win system "dir", return a int value from this function
## DeleteLog
Execute delete operation.
## ThreadFun
Thread function
