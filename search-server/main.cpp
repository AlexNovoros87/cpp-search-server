// Решите загадку: Сколько чисел от 1 до 1000 содержат как минимум одну цифру 3?
// Напишите ответ здесь:
#include <iostream>
using namespace std;
const int BORDER_COUNT = 1000;
int main() {
    int counter=0;
    
    for (int i = 0; i < BORDER_COUNT; ++i)
    {
        if (i % 10 == 3 || (i % 100) / 10 == 3) { ++counter;}
    }
    cout << counter << endl;
}
// Закомитьте изменения и отправьте их в свой репозиторий.
