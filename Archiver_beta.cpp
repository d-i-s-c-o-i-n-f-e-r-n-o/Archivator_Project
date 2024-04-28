#define _CRT_SECURE_NO_WARNINGS
#define PATH_LENGTH         500
#define USER_INPUT_LENGTH   85
//обычные библиотеки
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <locale.h>
#include <string.h>
#include <sys/stat.h>
//архиваторские библиотеки
#include "Archivator_Deflate.h"
#include "Archivator_Inflate.h"
#include "Archivator_Deflate.cpp"
#include "Archivator_Inflate.cpp"
FILE *f;

/*----------Создать архив---------- */
int Create(char* path, char* name_zip, char* mode) {
    //the character of the beginning of the name/path/format
    char sym[] = "'";

    int i = 0;
    int start = 0, end = 0;
    char name[200];
    memset(name, 0, 200);

    //getting the archive name
    while (strchr(sym, name_zip[i]) == NULL)
    {
        i++;
    }
    start = i + 1;
    i = start;
    while (strchr(sym, name_zip[i]) == NULL)
    {
        i++;
    }
    end = i - 1;
    strncpy(name, name_zip + start, end - start + 1);

    //defining the archiving format
    if (strstr(mode, "ZIP"))
    {
        strcat(name, ".zip");
    }
    else if (strstr(mode, "TAR"))
    {
        strcat(name, ".tar");
    }

    //output the archive name
    printf("%s//\n", name);

    //creating a new archive
    f = fopen(name, "wb");
    if (f != NULL)
    {
        fclose(f);
        return 0;
    }
    else return 1;
}
/*----------Извлечь архив----------*/
int Extract(char* path) {
    /*
    Function: извлечение архива по указанному адресу, передаваемому в функцию
                если адреса нет, address == 0, тогда сохранять по default

    Debug:  return 0, если успешно
            return 1, если завершилось ошибкой
    */
    return 0;
}
/*----------Просмотреть содержимое архива----------*/
int List() {
    /* Function: просмотр содержимого архива. Реализация на своё усмотрение, передаваемые переменные тоже.
       Debug:  return 0 если успешно, return 1 если завершилось ошибкой */
    return 0;
}
/*----------Добавить файлы в архив----------*/
int Add() {
    /* Function: добавление файлов в архив. Реализация на своё усмотрение, передаваемые переменные тоже.
       Debug:  return 0 если успешно, return 1 если завершилось ошибкой */
    return 0;
}
/*----------Удалить файлы из архива----------*/
int Delete() {
    /* Function: удаление файлов из архива. Реализация на своё усмотрение, передаваемые переменные тоже.
       Debug:  return 0 если успешно, return 1 если завершилось ошибкой */
    return 0;
}
/*----------Указать путь к архиву или к файлам для архивации----------*/
char* File() {
    /* Function: указание пути. возвращает адрес, по которому находятся файлы. Должна быть проверка на корректность
       Debug: отсутствует. */
    return 0;
}
/*----------Указать путь для сохранения архива или извлеченных файлов----------*/
int Output(bool isVerbose, char* inputPath, int mode) {
    bool noDir;
    char command[5];

    if (mode) {
        if (mode == 1)      //mode = 1 - сохранение архива
            printf("\nСохранить архив в папке по умолчанию (-yes) или ввести новый путь для сохранения(-no)?: ");
        if (mode == 2)      //mode = 2 - сохранение извлечённых файлов
            printf("\nСохранить извлечённыйе файлы в папке по умолчанию (-yes) или ввести новый путь для сохранения(-no)?: ");

        while (1) {
            gets_s(command, sizeof(command));
            if (strcmp(command, "-yes") == NULL) {          //если по умолчанию
                if (isVerbose) printf("Verbose: Путь сохранения установлен на путь по умолчанию.\n");
                return 1;
            }
            else if (strcmp(command, "-no") == NULL) break;        //если ввести новый путь
            else printf("Ошибка. Вы ввели недопустимый ответ. Повторите ввод.\n(yes - это по умолчанию, no - это ввести путь самостоятельно)\n");
        }
    }

    printf("Помощь: Если не знаете, какой адрес вставить, скопируйте в ввод этот:\n");
    system("break"); system("cd");
    do {
        //const struct stat* dir;
        struct stat* dir;
        dir = (struct stat*)malloc(sizeof(struct stat));
        printf("Введите путь: ");
        scanf("%s", inputPath);
        noDir = stat(inputPath, dir);
        free(dir);
        if (noDir) {
            printf("Ошибка. Неправильный путь к папке. Исправьте путь самостоятельно или воспользуйтесь подсказкой.\n");
            printf("Помощь: Если не знаете, какой адрес вставить, скопируйте в ввод этот:\n");
            system("cd");
            if (isVerbose) printf("Verbose: Путь не сохранён. Запрос на повторный ввод.\n");
        }
    } while (noDir);
    if (isVerbose) printf("Verbose: Путь сохранён успешно.\n");
    system("break");
}
/*----------Указание уровня сжатия (только для архивации)----------*/
int Size(int* defaultSize) {
    /*
    Function: указание уровня сжатия. больше уровень -> больше сжатие.
              проверка ввода на вхождение в допустимый диапазон в do...while()
              если пользователь соглашается установить путь в качестве по умолчанию, то меняем defaultSize

    Debug:  отсутствует.
    */

    int lvl;
    do {
        printf("Введите степень сжатия (0 < x < 8):");
        scanf_s("%i", &lvl);
        if (lvl < DEFLATE_LVL_MIN || lvl >= DEFLATE_LVL_MAX)
            printf("Ошибка. Недопустимая степень сжатия. Допустимы целые значения от 0 до 8\n"); //я не знаю какой диапазон. точка.
    } while (lvl < DEFLATE_LVL_MIN || lvl >= DEFLATE_LVL_MAX);
    return lvl;
}
/*----------Посмотреть справку по командам (сделано)----------*/
void Help() {
    printf("----------------------------------------------------------------------------------\n");
    printf("|                       Справка по поддерживаемым командам:                      |\n");
    printf("----------------------------------------------------------------------------------\n");
    printf("| Cоздать архив:                                                -c или -create   |\n");
    printf("| Извлечь архив:                                                -e или -extract  |\n");
    printf("| Просмотреть содержимое архива:                                -l или -list     |\n");
    printf("| Добавить файлы в архив:                                       -a или -add      |\n");
    printf("| Удалить файлы из архива:                                      -d или -delete   |\n");
    printf("| Указать путь к архиву или к файлам для архивации:             -f или -file     |\n");
    printf("| Указать путь для сохранения архива или извлеченных файлов:    -o или -output   |\n");
    printf("| Режим с детальным выводом процесса работы:                    -v или -verbose  |\n");
    printf("| Указание уровня сжатия (только для архивации):                -s или -size     |\n");
    printf("| Посмотреть справку по командам:                               -h или -help     |\n");
    printf("| Выход из программы:                                           -r или -return   |\n");
    printf("----------------------------------------------------------------------------------\n");
    printf("| Рекомендуется вводить команды без разделений или используя один пробел.        |\n");
    printf("| Без указания дополнительных параметров используются настройки по умолчанию.    |\n");
    printf("| Выход из программы выполняется после завершения всех остальных операций.       |\n");
    printf("----------------------------------------------------------------------------------\n");
}
/*----------Выход из программы (сделано)----------*/
int Return() {
    char command[5];
    do {
        printf("Вы хотите выйти из программы? (-yes / -no)\n");
        gets_s(command, sizeof(command));
        if (strcmp(command, "-yes") == NULL) return 1;      //если yes
        else if (strcmp(command, "-no") == NULL) return 0;  //если no
        else printf("Ошибка. Вы ввели недопустимый ответ. Повторите ввод.\n(yes - это да, no - это нет)\n");
    } while (strcmp(command, "-yes") != NULL || strcmp(command, "-no") != NULL);
}
/*----------Главная функция----------*/
int main() {
    char* currentPath, * defaultPath;
    char* userInput;

    bool noDir;
    int defaultSize = DEFLATE_LVL_DEF,
        currentSize;
    currentPath = (char*)malloc(PATH_LENGTH * sizeof(char));
    defaultPath = (char*)malloc(PATH_LENGTH * sizeof(char));
    userInput = (char*)malloc(USER_INPUT_LENGTH * sizeof(char));

    setlocale(LC_ALL, "rus");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    //Инициализация пути сохранения файлов по умолчанию
    printf("Вас приветствует программа Archivator.\n\nИнициализация пути сохранения файлов и извлечения архивов по умолчанию.\n");
    Output(0, currentPath, 0);
    defaultPath = currentPath;
    //Старт
    printf("\nПрограмма готова приступить к работе.\nВот справка по поддерживаемым командам.\n");
    Help();

    //Обработчик команд
    while (1) {
        printf("\nВведите свою команду: ");
        getchar();
        gets_s(userInput, sizeof(userInput));

        boolean isCreate = (strstr(userInput, "-c") != NULL),
            isExtract = (strstr(userInput, "-e") != NULL),
            isFile = (strstr(userInput, "-f") != NULL),
            isOutput = (strstr(userInput, "-o") != NULL),
            isSize = (strstr(userInput, "-s") != NULL),
            isVerbose = (strstr(userInput, "-v") != NULL);
        currentSize = defaultSize;

        /*----------Verbose----------*/
        if (isVerbose) {
            printf("Verbose: Ввод пользователя принят.\n");
            printf("Verbose: Режим с детальным выводом процесса работы (Verbose) включён.\n");
        }
        /*----------Exeptions Of Users Line----------*/
        if (isCreate && isExtract) {
            printf("Невозможно, чувак :/");
            if (isVerbose) printf("Verbose: Ввод пользователя некорректен.\n");
            break;
        }
        /*----------Help----------*/
        if (strstr(userInput, "-h") != NULL) {
            if (isVerbose) {
                printf("Verbose: Команда \"Help\" считана.\n");
                Help();
                printf("Verbose: Справка выведена.\n");
            }
            else Help();
        }
        /*----------Output----------*/
        if (isOutput) {
            int mode;
            if (isVerbose) printf("Verbose: Команда \"Output\" считана.\n");

            if (strstr(userInput, "-c") != NULL) {
                isCreate = 1; mode = 1;
            }
            if (strstr(userInput, "-e") != NULL) {
                isExtract = 1; mode = 2;
            }

            Output(isVerbose, currentPath, mode);

            if (isVerbose) printf("Verbose: Команда \"Output\" выполнена.\n");
        }
        /*----------Create----------*/
        if (isCreate) {
            if (isVerbose) printf("Verbose: Команда \"Create\" считана.\n");
            char a_name[80];
            gets_s(a_name);
            char* arch_name = a_name;
            char comp_mode[3];
            gets_s(comp_mode);
            char* c_m = comp_mode;
            /*---DEBUG---*/
            if (Create(currentPath, arch_name, c_m)) 
            {
                if (isVerbose) printf("Verbose: Архив с именем %s не может быть создан или перезаписан по адресу %s\n", arch_name, currentPath); //error
            }             
            else
                if (isVerbose) 
                    printf("Verbose: Архив с именем %s создан по адресу %s\n", arch_name, currentPath);                                 //ne error
            /*---/DEBUG---*/

            if (isVerbose) printf("Verbose: Команда \"Create\" выполнена.\n");
        }
        /*----------Add----------*/
        if (strstr(userInput, "-a") != NULL) {
            if (isVerbose) printf("Verbose: Команда \"Add\" считана.\n");

            /*---DEBUG---*/
            if (Add()) printf("Невозможно добавить файл в архив");                           //error
            else printf("Файл добавлен в архив")/*something*/;                                 //ne error
            /*---/DEBUG---*/

            if (isVerbose) printf("Verbose: Команда \"Add\" выполнена.\n");
        }
        /*----------Delete----------*/
        if (strstr(userInput, "-d") != NULL) {
            if (isVerbose) printf("Verbose: Команда \"Delete\" считана.\n");

            /*---DEBUG---*/
            if (Delete()) /*something*/;                        //error
            else /*something*/;                                 //ne error
            /*---/DEBUG---*/

            if (isVerbose) printf("Verbose: Команда \"Delete\" выполнена.\n");
        }
        /*----------List----------*/
        if (strstr(userInput, "-l") != NULL) {
            if (isVerbose) printf("Verbose: Команда \"List\" считана.\n");

            /*---DEBUG---*/
            if (List()) /*something*/;                          //error
            else /*something*/;                                 //ne error
            /*---/DEBUG---*/

            if (isVerbose) printf("Verbose: Команда \"List\" выполнена.\n");
        }
        /*----------File----------*/
        if (isFile) {
            if (isVerbose) printf("Verbose: Команда \"File\" считана.\n");

            /*---DEBUG---*/
            if (File()) /*something*/;                          //error
            else /*something*/;                                 //ne error
            /*---/DEBUG---*/

            if (isVerbose) printf("Verbose: Команда \"File\" выполнена.\n");
        }
        /*----------Size----------*/
        if (isSize) {
            if (isVerbose) printf("Verbose: Команда \"Size\" считана.\n");

            currentSize = Size(&defaultSize);

            if (isVerbose) printf("Verbose: Команда \"Size\" выполнена.\n");
        }
        /*----------Extract----------*/
        if (isExtract) {
            if (isVerbose) printf("Verbose: Команда \"Extract\" считана.\n");

            /*---DEBUG---*/
            if (Extract(currentPath)) /*something*/;            //error
            else /*something*/;                                 //ne error
            /*---/DEBUG---*/

            if (isVerbose) printf("Verbose: Команда \"Extract\" выполнена.\n");
        }
        /*----------Return(done)----------*/
        if (strstr(userInput, "-r") != NULL) {
            if (isVerbose) printf("Verbose: Команда \"Return\" считана.\n");
            if (Return()) {
                if (isVerbose) printf("Verbose: Завершение программы по запросу пользователя.\n");
                return 0;
            }
            else if (isVerbose) printf("Verbose: Завершение программы прервано пользователем.\n");
        }
        /*----------End Of Users Line----------*/
        if (isVerbose) printf("Verbose: Ввод пользователя выполнен.\n");
    }
}