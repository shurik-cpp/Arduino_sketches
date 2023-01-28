#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <ESP8266FtpServer.h>


const uint8_t led = 2;
const char* ssid = "Raspberry";
const char* ftp_login = "rasp";
const char* ftp_password = "berry";

ESP8266WebServer HTTP_server(80);
FtpServer FTP_server; // по умолчанию используется 21 порт


void setup() {
// put your setup code here, to run once:
pinMode(led, OUTPUT);

Serial.begin(9600);

WiFi.softAP(ssid);

SPIFFS.begin();
HTTP_server.begin();
FTP_server.begin(ftp_login, ftp_password);

Serial.print("\nMy IP to connect via Web-Browser or FTP: ");
Serial.println(WiFi.softAPIP());
Serial.println('\n');

// Обработка HTTP-запросов
HTTP_server.on("/LedSwitch", []()
					{                                        // При HTTP запросе вида http://192.168.4.1/LedSwitch
						HTTP_server.send(200, "text/plain", LedSwitch());                     // Отдаём клиенту код успешной обработки запроса, сообщаем, что формат ответа текстовый и возвращаем результат выполнения функции relay_switch 
					});
HTTP_server.on("/LedStatus", []()
					{                                        // При HTTP запросе вида http://192.168.4.1/LedStatus
						HTTP_server.send(200, "text/plain", LedStatus());                     // Отдаём клиенту код успешной обработки запроса, сообщаем, что формат ответа текстовый и возвращаем результат выполнения функции LedStatus 
					});
HTTP_server.onNotFound([]()
					{                                                 // Описываем действия при событии "Не найдено"
						if (!handleFileRead(HTTP_server.uri()))                                       // Если функция HandleFileRead (описана ниже) возвращает значение false в ответ на поиск файла в файловой системе
							HTTP_server.send(404, "text/plain", "Not Found");                        // возвращаем на запрос текстовое сообщение "File isn't found" с кодом 404 (не найдено)
					});
}

String LedSwitch() {                                                 // Функция переключения реле 
	bool state = !digitalRead(led);
	digitalWrite(led, state);                                           // меняем значение на пине подключения реле
	return String(state);                                                 // возвращаем результат, преобразовав число в строку
}

String LedStatus() {                                                 // Функция для определения текущего статуса реле                        
	return String(!digitalRead(led));                                                 // возвращаем результат, преобразовав число в строку
}

bool handleFileRead(const String& path) {                                       // Функция работы с файловой системой
	if (path.endsWith("/")) 											// Если устройство вызывается по корневому адресу, то должен вызываться файл index.html (добавляем его в конец адреса)
		path += "index.html";                          
	String contentType = getContentType(path);                            // С помощью функции GetContentType (описана ниже) определяем по типу файла (в адресе обращения) какой заголовок необходимо возвращать по его вызову

	if (SPIFFS.exists(path)) {                                              // Если в файловой системе существует файл по адресу обращения
		File file = SPIFFS.open(path, "r");                                 //  Открываем файл для чтения
		size_t sent = HTTP_server.streamFile(file, contentType);                   //  Выводим содержимое файла по HTTP, указывая заголовок типа содержимого contentType
		file.close();                                                       //  Закрываем файл
		return true;                                                        //  Завершаем выполнение функции, возвращая результатом ее исполнения true (истина)
	}
	return false;                                                         // Завершаем выполнение функции, возвращая результатом ее исполнения false (если не обработалось предыдущее условие)
}

String getContentType(const String& filename){                                 // Функция, возвращающая необходимый заголовок типа содержимого в зависимости от расширения файла
	if (filename.endsWith(".html")) return "text/html";                   // Если файл заканчивается на ".html", то возвращаем заголовок "text/html" и завершаем выполнение функции
	else if (filename.endsWith(".css")) return "text/css";                // Если файл заканчивается на ".css", то возвращаем заголовок "text/css" и завершаем выполнение функции
	else if (filename.endsWith(".js")) return "application/javascript";   // Если файл заканчивается на ".js", то возвращаем заголовок "application/javascript" и завершаем выполнение функции
	else if (filename.endsWith(".png")) return "image/png";               // Если файл заканчивается на ".png", то возвращаем заголовок "image/png" и завершаем выполнение функции
	else if (filename.endsWith(".jpg")) return "image/jpeg";              // Если файл заканчивается на ".jpg", то возвращаем заголовок "image/jpg" и завершаем выполнение функции
	else if (filename.endsWith(".gif")) return "image/gif";               // Если файл заканчивается на ".gif", то возвращаем заголовок "image/gif" и завершаем выполнение функции
	else if (filename.endsWith(".ico")) return "image/x-icon";            // Если файл заканчивается на ".ico", то возвращаем заголовок "image/x-icon" и завершаем выполнение функции
	return "text/plain";                                                  // Если ни один из типов файла не совпал, то считаем что содержимое файла текстовое, отдаем соответствующий заголовок и завершаем выполнение функции
}

void loop() {
	HTTP_server.handleClient();                                             // Обработчик HTTP-событий (отлавливает HTTP-запросы к устройству и обрабатывает их в соответствии с выше описанным алгоритмом)
	FTP_server.handleFTP();                                                 // Обработчик FTP-соединений  
}
