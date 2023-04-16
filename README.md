# TCP_Chat
Клиент-серверный чат на протоколе TCP.
Необходимые библиотеки: GLog, GTest

Проект содержит две библиотеки и четыре исполняемых файла.
Библиотека ServerLib содержит реализацию серверной части приложения и использует файлы code/server.cpp, inc/serverFuncs.cpp, inc/server.h и inc/common.h.
Библиотека ClientLib содержит реализацию клиентской части приложения и использует файлы code/client.cpp, inc/clientFuncs.cpp, inc/client.h и inc/common.h.

Исполняемый файл Server использует код серверной части приложения и использует файл code/server.cpp.
Исполняемый файл ServerTests содержит юнит-тесты для серверной части приложения и использует файл test/server_test.cpp.
Исполняемый файл Client использует код клиентской части приложения и использует файл code/client.cpp.
Исполняемый файл ClientTests содержит юнит-тесты для клиентской части приложения и использует файл test/client_test.cpp.
