#include <windows.h>
#include <iostream>

// Глобальный мьютекс
HANDLE hMutex;

// Вспомогательная функция для вычисления факториала
unsigned long long factorial(int n) {
    if (n <= 1) return 1;
    else return n * factorial(n - 1);
}

// Функция потока, выполняющая сложные вычисления и изменяющая общий ресурс
DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    int* sharedResource = (int*)lpParam;
    int operationsPerformed = 0;

    for (int i = 0; i < 5; ++i) {
        WaitForSingleObject(hMutex, INFINITE); // Захват мьютекса

        // Начало критической секции
        //std::cout << "Thread " << GetCurrentThreadId() << " is entering critical section." << std::endl;

        // Выполнение сложной операции: расчет факториала
        unsigned long long factorialResult = factorial(*sharedResource);
        std::cout << "Factorial of " << *sharedResource << " is: " << factorialResult << std::endl;
        (*sharedResource)++;
        operationsPerformed++;

        // Конец критической секции
        //std::cout << "Thread " << GetCurrentThreadId() << " is leaving critical section." << std::endl;
        ReleaseMutex(hMutex); // Освобождение мьютекса

        Sleep(1000); // Имитация работы вне критической секции
    }

    std::cout << "Thread " << GetCurrentThreadId() << " performed " << operationsPerformed << " operations." << std::endl;
    return 0;
}

int main() {
    int sharedResource = 1; // Начальное значение ресурса установим в 1 для корректного вычисления факториала

    // Создание мьютекса
    hMutex = CreateMutex(NULL, FALSE, NULL);
    if (hMutex == NULL) {
        std::cerr << "CreateMutex error: " << GetLastError() << std::endl;
        return 1;
    }

    // Создание трех потоков
    const int threadCount = 3;
    HANDLE hThreads[threadCount];
    for (int i = 0; i < threadCount; ++i) {
        hThreads[i] = CreateThread(NULL, 0, ThreadFunction, &sharedResource, 0, NULL);
        if (hThreads[i] == NULL) {
            std::cerr << "CreateThread error: " << GetLastError() << std::endl;
            return 1;
        }
    }

    // Ожидание завершения потоков
    WaitForMultipleObjects(threadCount, hThreads, TRUE, INFINITE);

    // Закрытие дескрипторов потоков
    for (int i = 0; i < threadCount; ++i) {
        CloseHandle(hThreads[i]);
    }

    // Вывод результата
    std::cout << "Final Shared Resource Value: " << sharedResource << std::endl;

    // Закрытие мьютекса
    CloseHandle(hMutex);

    return 0;
}
