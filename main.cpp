/**********************************************************
*                    КАФЕДРА № 304 1 КУРС                       *
*---------------------------------------------------------------*
* Project Type : Win64 Console Application                      *
* Project Name : two-dimensional arrays                         *
* File Name    : main.cpp                                       *
* Language     : C/C++                                          *
* Programmer(s): Коркина К.В.                                   *
* Modified By  :                                                *
* Lit source   :                                                *
* Created      : 22/05/25                                       *
* Last Revision: 29/05/25                                       *
* Comment(s)   : Структуры                                      *
****************************************************************/
#include <iostream>
#include <fstream>
#include <iomanip> 

using namespace std;

const int M = 100;
//const char* FName = "test_1.txt";         //проверка открытия файла
//const char* FName = "test_2.txt";         //проверка на пустой файл
//const char* FName = "test_3.txt";         //проверка бортового номера (первый сивол не буква)
//const char* FName = "test_8.txt";         //проверка бортового номера (разделитель не -)
//const char* FName = "test_4.txt";         //проверка номера рейса
const char* FName = "test_5.txt";         //проверка времени (некорректное количество минут)
//const char* FName = "test_6.txt";         //проверка времени (некорректное количество часов)
//const char* FName = "test_7.txt";           //проверка времени (разделитель не :)
//const char* FName = "test_9.txt";         //тест с корректными данными (хаотично)
//const char* FName = "test_10.txt";          //тест с корректными данными (изначально отсортирован)
//const char* FName = "test_11.txt";          //тест с корректными данными (отсортирован в обратном порядке)

//данные о самолете
struct Splane {
    char* board_number;    // бортовой номер
    char* flight_number;   // номер рейса
    char* plane_model;     // марка ЛА
    char* time;            // время входа в зону
};//struct

//Структура вида: ключ-значение для индексной сортировки
struct Sindex_Sort {
    int id;             //индекс
    int board_number;   //значение
}; //Sindex_Sort

// Функция для вычисления длины строки 
int stringLength(const char* str);

// Проверка бортового номера
bool validateBoardNumber(const char* number);     //строка, содержащая бортовой номер

// Проверка номера рейса
bool validateFlightNumber(const char* number);   //строка, содержащая номер рейса

// Проверка времени
bool validateTime(const char* time);             //строка, содержащая время

//вывод ошибок
void printError(int ErrorCode);                  //код ошибки

//разделение строки на "слова"
void splitString(const char* input      //строка для разделения
    , char** words          //результат
    , int wordCount         //макс. кол-во слов для выделения
    , int maxWordLength);    //макс. длина каждого слова

//удаление памяти
void freeMemory(Splane planes[]         //массив структур
    , int AM);               //количество записей

// Чтение данных из файла
int readFromFile(Splane planes[]        //массив структур
    , int& AM);              //количество считанных записей

// Вывод данных
void printData(const Splane planes[]            //массив структур
    , Sindex_Sort new_planes[]      //структура с индексной сортировкой
    , int AM);                      //количество считанных записей

// Функция для выделения числа из бортового номера
int parseBoardNumber(const char* board_num);

//индексная сортировка подсчетом
void index_Sort(Sindex_Sort new_planes[]          //массив структур (пара: ключ-значение)
    , int count);                         //количество считанных записей

int main() {
    setlocale(LC_ALL, "rus");

    int AM = 0;
    int errCode;

    Splane* airplanes = new Splane[M];

    // Чтение данных из файла
    errCode = readFromFile(airplanes, AM);

    //создание структуры для индексной сортировки
    Sindex_Sort air[100];
    for (int i = 0; i < AM; i++) {
        air[i].id = i;
        air[i].board_number = parseBoardNumber(airplanes[i].board_number);
    }

    if (errCode == 0) {
        printData(airplanes, air, AM);

        cout << endl;

        index_Sort(air, AM);

        printData(airplanes, air, AM);

        freeMemory(airplanes, AM);
    }
    else
        printError(errCode);
}
//найти длину строки
int stringLength(const char* str) {
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}//stringLength()

// Проверка бортового номера
bool validateBoardNumber(const char* number)     //строка, содержащая бортовой номер
{
    int len = stringLength(number);
    if (len < 6)                                        //не менее 6 знаков
        return false;
    if (!(number[0] >= 'A' && number[0] <= 'Z'))        //начинается с буквы
        return false;
    if (number[1] != '-')                               //после буквы точно -
        return false;
    for (int i = 2; i < len; i++) {                     //следующие символы - цифры
        if (!(number[i] >= '0' && number[i] <= '9'))
            return false;
    }//for
    return true;
}//validateBoardNumber()

// Проверка номера рейса
bool validateFlightNumber(const char* number)   //строка, содержащая номер рейса
{
    // Проверяем первые 4 символа вручную
    if (number[0] != 'R' ||
        number[1] != 'E' ||
        number[2] != 'I' ||
        number[3] != 'S') {
        return false;
    }//if

    // Проверяем остальные символы на цифры
    int len = stringLength(number);
    for (int i = 4; i < len; i++) {
        if (!(number[i] >= '0' && number[i] <= '9')) {
            return false;
        }//if
    }//for
    return true;
}//validateFlightNumber()

//проверка времени
bool validateTime(const char* time)             //строка, содержащая время
{
    int len = stringLength(time);
    if (len != 5)                                // Точно 5 символов
        return false;
    if (time[2] != ':')                          // проверка разделителя
        return false;

    // Часы 
    if (!(time[0] >= '0' && time[0] <= '2') ||  // Первая цифра часов 0-2
        !(time[1] >= '0' && time[1] <= '9'))    // Вторая цифра часов 0-9
        return false;

    int hours = (time[0] - '0') * 10 + (time[1] - '0');
    if (hours > 23) return false;               // Проверка диапазона 0-23

    // Минуты
    if (!(time[3] >= '0' && time[3] <= '5') ||  // Первая цифра минут 0-5
        !(time[4] >= '0' && time[4] <= '9'))    // Вторая цифра минут 0-9
        return false;
    return true;
}//validateTime()

//вывод ошибок
void printError(int ErrorCode)                  //код ошибки
{
    switch (ErrorCode) {
    case 1:
        cout << "Ошибка чтения файла!" << endl;
        break;
    case 2:
        cout << "Ошибка: файл пуст!" << endl;
        break;
    case 3:
        cout << "Ошибка: неверный бортовой номер!" << endl;
        break;
    case 4:
        cout << "Ошибка: неверный номер рейса!" << endl;
        break;
    case 5:
        cout << "Ошибка: неверное время" << endl;
        break;
    default:
        cout << "Неопознанная ошибка!" << endl;
    }
}//printError()

//разделение строки на "слова"
void splitString(const char* input      //строка для разделения
    , char** words          //результат
    , int wordCount         //макс. кол-во слов для выделения
    , int maxWordLength)    //макс. длина каждого слова
{
    int wordIndex = 0;       //индекс текущего слова в массиве words
    int charIndex;           //индекс текущего символа в строке

    // Инициализация массива слов нулевыми строками 
    for (int i = 0; i < wordCount; i++)
    {
        words[i] = new char[maxWordLength];     //выделяем память для каждого слова
        words[i][0] = '\0';
    }//for

    int i = 0;               //индекс для прохода по входной строке
    //пока дошли до конца строки и не "разобрали" все слова
    while (input[i] != '\0' && wordIndex < wordCount)
    {
        // Пропускаем пробелы
        while (input[i] == ' ') i++;

        // (проверка) если конец строки после пробелов
        if (input[i] == '\0') break;

        charIndex = 0;
        // Копируем слово пока не: закончилась строка, не пробел, не превысили макс. длину слова 
        while (input[i] != '\0' && input[i] != ' ' && charIndex < maxWordLength - 1)
        {
            words[wordIndex][charIndex] = input[i];     //копируем посимвольно
            charIndex++;
            i++;
        }//while
        words[wordIndex][charIndex] = '\0';     //завершаем слово 
        wordIndex++;                            //переходим к след.слову
    }//while
}//splitString()

// Освобождение памяти
void freeMemory(Splane planes[]         //массив структур
    , int AM)               //количество записей
{
    for (int i = 0; i < AM; i++) {
        delete[] planes[i].board_number;
        delete[] planes[i].flight_number;
        delete[] planes[i].plane_model;
        delete[] planes[i].time;
    }//for
}//freeMemory()

// Чтение данных из файла
int readFromFile(Splane planes[]        //массив структур
    , int& AM)              //количество считанных записей
{
    ifstream in(FName);     //считывание из файла
    if (!in.is_open())
    {
        return 1;           // Ошибка открытия файла
    }//if
    //проверка, что файл не пуст (пытаемся прочитать первый символ)
    if (in.get() == EOF)
    {
        in.close();
        return 2;           // Файл пуст
    }//if
    //вернуть указатель чтения в начало файла (для корректной обработки после get())
    in.seekg(0);

    char* line = new char[256];             //строка для чтения из файла
    int count = 0;                          //подсчет считанных записей 

    while (in.getline(line, 256))           //построчно читаем
    {
        char* temp[4];                      //для каждой строки создаем массив на 4 эл-та(структуры самолёта)
        for (int j = 0; j < 4; j++)
            temp[j] = new char[50];         //выделить память для каждого элемента

        splitString(line, temp, 4, 50);     //разделение строки на отдельные элементы

        bool valid = true;                  //проверка на бортовой номер
        if (!validateBoardNumber(temp[0]))
        {
            valid = false;
            return 3;
        }//if
        if (!validateFlightNumber(temp[1])) //проверка рейса
        {
            valid = false;
            return 4;
        }//if
        if (!validateTime(temp[3]))         //проверка времени
        {
            valid = false;
            cout << "ошибка в строке: " << count << endl << "неверное значение: " << temp[3] << endl;
            return 5;
        }//if
        //если все проверки пройдены
        if (valid) {
            planes[count].board_number = temp[0];
            planes[count].flight_number = temp[1];
            planes[count].plane_model = temp[2];
            planes[count].time = temp[3];
            count++;
        }//if
        else
        {   //в случае ошибки - освободить память
            for (int j = 0; j < 4; j++) delete[] temp[j];
        }//else
    }//while

    AM = count;       //сохранить кол-во успешно прочитанных записей
    in.close();       //закрыть файл и освободить память
    delete[] line;
    return 0;         // Успешное чтение
}//readFromFile()

// Вывод данных
void printData(const Splane planes[]            //массив структур
    , Sindex_Sort new_planes[]      //структура с индексной сортировкой
    , int AM)                       //количество считанных записей
{
    // Шапка таблицы
    cout << left << "|"
        << setw(15) << "Бортовой номер" << "|"
        << setw(15) << "Номер рейса" << "|"
        << setw(15) << "Марка ЛА" << "|"
        << setw(10) << "Время" << "|" << endl;

    // Данные таблицы
    for (int i = 0; i < AM; i++)
    {
        cout << left << "|"
            << setw(15) << planes[new_planes[i].id].board_number << "|"
            << setw(15) << planes[new_planes[i].id].flight_number << "|"
            << setw(15) << planes[new_planes[i].id].plane_model << "|"
            << setw(10) << planes[new_planes[i].id].time << "|" << endl;
    }//for
}//printData()

// Функция для выделения числа из бортового номера
int parseBoardNumber(const char* board_num) {
    const char* p = board_num + 2;      //пропускаем первые 2 символа (букву и '-')
    int num = 0, mnoj = 1, len_s = 0;

    len_s = stringLength(p);
    for (int i = len_s - 1; i >= 0; i--)
    {
        num += (p[i] - '0') * mnoj;
        mnoj *= 10;
    }//for
    return num;
}//parseBoardNumber()

//индексная сортировка подсчетом
void index_Sort(Sindex_Sort new_planes[]          //массив структур (пара: ключ-значение)
    , int count)                         //количество считанных записей
{
    setlocale(LC_ALL, "RUS");
    cout << endl << "Структура до сортировки" << endl;
    for (int i = 0; i < count; i++) cout << new_planes[i].id << " " << new_planes[i].board_number << endl;

    //найти максимум
    int mmax = new_planes[0].board_number;
    for (int i = 1; i < count; i++) {
        if (new_planes[i].board_number > mmax) {
            mmax = new_planes[i].board_number;
        }
    }

    // Создаем массив счетчиков размером mmax+1
    int* cnt = new int[mmax + 1](); // Инициализация нулями

    // Подсчет элементов
    for (int i = 0; i < count; i++) {
        cnt[new_planes[i].board_number]++;
    }

    // Преобразуем в префиксные суммы
    for (int i = 1; i <= mmax; i++) {
        cnt[i] += cnt[i - 1];
    }

    // Создаем временный массив для результата
    Sindex_Sort* temp = new Sindex_Sort[count];

    // Заполняем в обратном порядке 
    for (int i = count - 1; i >= 0; i--) {

        // получаем значение board_number из текущего элемента массива
        int num = new_planes[i].board_number;
        cnt[num] = cnt[num] - 1; // Уменьшаем счетчик
        int new_pos = cnt[num];  // Получаем новую позицию
        temp[new_pos] = new_planes[i]; // Копируем элемент
    }

    // Копируем обратно
    for (int i = 0; i < count; i++) {
        new_planes[i] = temp[i];
    }

    // Освобождаем память
    delete[] cnt;
    delete[] temp;

    cout << endl << "Структура после сортировки" << endl;
    for (int i = 0; i < count; i++) cout << new_planes[i].id << " " << new_planes[i].board_number << endl;
    cout << endl;
}//indexsort()