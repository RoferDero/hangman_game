#include <iostream>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

namespace {

    // Понижение регистра для UTF-8 строки (кириллица)
    std::string toLowerUtf8(const std::string& s) {
        std::string result;
        result.reserve(s.size());
        size_t i = 0;
        while (i < s.size()) {
            unsigned char c = static_cast<unsigned char>(s[i]);
            size_t len = 1;
            if ((c & 0x80) == 0)         len = 1;
            else if ((c & 0xE0) == 0xC0) len = 2;
            else if ((c & 0xF0) == 0xE0) len = 3;
            else if ((c & 0xF8) == 0xF0) len = 4;

            if (i + len > s.size()) len = s.size() - i;
            std::string ch = s.substr(i, len);
            i += len;

            if (len == 1) {
                result += static_cast<char>(std::tolower(static_cast<unsigned char>(ch[0])));
            }
            else if (len == 2) {
                unsigned char b0 = static_cast<unsigned char>(ch[0]);
                unsigned char b1 = static_cast<unsigned char>(ch[1]);
                if (b0 == 0xD0 && b1 >= 0x90 && b1 <= 0xAF) {  // А-Я
                    result += static_cast<char>(0xD0);
                    result += static_cast<char>(b1 + 0x20);
                }
                else if (b0 == 0xD0 && b1 == 0x81) {         // Ё
                    result += static_cast<char>(0xD1);
                    result += static_cast<char>(0x91);
                }
                else {
                    result += ch;
                }
            }
            else {
                result += ch;
            }
        }
        return result;
    }




}

// Пара «слово — подсказка (определение)»
struct WordEntry {
    std::string word;
    std::string hint;
};

class HangmanGame {
public:
    HangmanGame() : rng(std::random_device{}()), maxAttempts(6), mistakes(0) {
        words = {
            { "программирование", "процесс создания компьютерных программ" },
            { "алгоритм",         "описание последовательности действий, необходимых для решения задачи" },
            { "компилятор",       "программа, переводящая исходный код в машинный код" },
            { "разработчик",      "специалист, создающий программы" },
            { "интерфейс",        "заголовок функции, в котором указывается название функции, список её параметров и тип возвращаемого значения" },
            { "операция",         "действие, производимое над операндами" },
            { "переменная",       "именованная область памяти" },
            { "библиотека",       "набор готовых функций и классов, которые можно подключать к программе" },
            { "выражение",        "синтаксическая единица языка, определяющая способ вычисления некоторого значения" },
            { "аллокатор",        "часть программы, которая запрашивает память большими кусками напрямую у ОС через системные вызовы" },
            { "отладка",          "процесс поиска, локализации и исправления ошибок в программе" },
            { "структура",        "тип данных, объединяющий несколько переменных под одним именем" },
            { "константа",        "переменная, значение которой нельзя изменить после инициализации" },
            { "функция",          "именованный блок кода, выполняющий определённую задачу" },
            { "массив",           "набор элементов одного типа, расположенных в памяти подряд" },
            { "указатель",        "переменная, хранящая адрес другой переменной или объекта" },
            { "ссылка",           "псевдоним для существующей переменной" },
            { "операнд",          "значение или переменная, над которой выполняется операция" }
        };
    }

    void play() {
        std::cout << "=================================\n";
        std::cout << "     ИГРА «ВИСЕЛИЦА»\n";
        std::cout << "=================================\n\n";
        std::cout << "Введите букву — или '?' (подсказка),\n";
        std::cout << "чтобы увидеть определение слова.\n\n";
        
        bool playAgain = true;
        while (playAgain) {
            startNewGame();
            gameLoop();
            playAgain = askPlayAgain();
        }
        std::cout << "\nСпасибо за игру!\n";
    }
private:
    std::vector<WordEntry> words;
    std::string secretWord;
    std::string guessedWord;
    std::string usedLetters;
    std::string currentHint;
    int maxAttempts;
    int mistakes;
    std::mt19937 rng
