#include <iostream>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <windows.h>
using namespace std;
const int N = 500;

// Функция для вывода двумерного массива
void printArray(double** array, int rows, int cols, const string& name) {
    cout << "Массив " << name << " размером " << rows << "x" << cols << ":\n";
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << array[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// Функция для умножения матрицы W на скалярное значение u[a]
void multiplyMatrixScalar(double** W, double u_a, double** result, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        result[i][0] = 0;  // Результат - это одномерный вектор
        for (int j = 0; j < cols; j++) {
            result[i][0] += W[i][j] * u_a;  // Умножаем строку W на элемент u[a]
        }
    }
}

// Функция для создания двумерного массива
double** createMatrix(int rows, int cols) {
    double** matrix = new double*[rows];
    for (int i = 0; i < rows; i++) {
        matrix[i] = new double[cols];
    }
    return matrix;
}

// Функция для освобождения памяти, выделенной под двумерный массив
void deleteMatrix(double** matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

// Функция для транспонирования матрицы
double** transpose(double** matrix, int rows, int cols) {
    double** result = createMatrix(cols, rows);  // Создаем транспонированную матрицу
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[j][i] = matrix[i][j];  // Меняем местами строки и столбцы
        }
    }

    return result;
}

// Функция для умножения двух матриц с проверкой размерностей
double** multiplyMatrices(double** mat1, int rows1, int cols1, double** mat2, int rows2, int cols2) {
    // Проверка совместимости матриц для умножения
    if (cols1 != rows2) {
        cout << "Ошибка: Невозможно умножить матрицы. Несовместимые размерности: "
             << cols1 << " != " << rows2 << endl;
        exit(1);  // Прерываем выполнение программы
    }

    // Создаем результирующую матрицу (rows1 x cols2)
    double** result = createMatrix(rows1, cols2);

    // Умножение матриц
    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++) {
            result[i][j] = 0;  // Инициализируем элемент результирующей матрицы
            for (int k = 0; k < cols1; k++) {
                // Суммируем произведение соответствующих элементов
                result[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }

    return result;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    //Создание и инициализация массива u
    double u[N];
    double d[N];
    int l = 0;
    for (double k = -1; k <= 5.0;) {
        u[l] = k;
        d[l] = pow(k,2);
        l++;
        k = k + 0.012;
    }
    /*for (int i = 0; i < l; i++) {
        cout << u[i] << endl;
    }*/

    //Здесь можно будет задать количество слоев

    int count_layer = 4; // Количество слоев
    int size[count_layer] = {1, 5, 3, 1};

    // Создаем массив для хранения массивов w,s,y, delta, epsilon
    double*** arrayOfW = new double**[count_layer - 1];
    double*** arrayOfS = new double**[count_layer - 1]; //Входная сумма нейронов
    double*** arrayOfY = new double**[count_layer - 1];
    double*** arrayOfDelta = new double**[count_layer - 1];
    double*** arrayOfEpsilon = new double**[count_layer - 1];

    for (int a = 0; a < N; a++) {
        for (int i = 1; i < count_layer; i++) {
            int rows = size[i]; //строки
            int last_cols = size[i - 1]; // Столбцы (предыдущий слой)
            int cols = 1; //столбцы

            // Создаем массив w и инициализируем его случайными значениями
            double** w = createMatrix(rows, last_cols);
            for (int j = 0; j < rows; j++) {
                for (int k = 0; k < last_cols; k++) {
                    w[j][k] = -1 + static_cast<double>(rand()) / (RAND_MAX / 2);// генерация от -1 до 1
                }
            }
            arrayOfW[i - 1] = w; // Сохраняем массив w в arrayOfW

            // Выводим массив w
            //printArray(w, rows, last_cols, "w");

            // Создаем массив b и инициализируем случайными значениями
            double** b = createMatrix(rows, 1);
            for (int j = 0; j < rows; j++) {
                b[j][0] = -1 + static_cast<double>(rand()) / (RAND_MAX / 2);// генерация от -1 до 1
            }

            // Выводим массив b
            //printArray(b, rows, cols, "b");

            // Создаем и инициализируем массив s
            double** s = createMatrix(rows, 1);
            multiplyMatrixScalar(w, u[a], s, rows, last_cols);
            for (int j = 0; j < rows; j++) {
                s[j][0] += b[j][0];
            }
            arrayOfS[i - 1] = s; // Сохраняем массив s в arrayOfS
            //printArray(s, rows, cols, "s");

            // Создаем и инициализируем массив y (сигмоидная функция активации)
            double** y = createMatrix(rows, 1);
            for (int j = 0; j < rows; j++) {
                y[j][0] = 1 / (1 + exp(-s[j][0]));
            }
            arrayOfY[i - 1] = y;// Сохраняем массив y в arrayOfY
            //printArray(y, rows, cols, "y");

            //НАЧИНАЕМ ОБРАТНЫЙ ПРОХОД
            //Создаем массив newEpsilon для последнего слоя
            if (i == count_layer - 1) {
                double **newEpsilon = createMatrix(rows, 1);
                for (int j = 0; j < rows; j++) {
                    newEpsilon[j][0] = y[j][0] - d[a]; // Разница между прогнозом и реальным значением
                }
                arrayOfDelta[i - 1] = newEpsilon;
                arrayOfEpsilon[i - 1] = newEpsilon;
                printArray(newEpsilon, rows, cols, "epsilon");
            }
            /*else {
                //здесь написать код для подсчета эпсилонов 1 и 2 слоя
                double** trans_w = transpose(arrayOfW[i], rows, last_cols);
                printArray(trans_w, rows, last_cols, "trans_w");
            }*/

            deleteMatrix(b, rows);
        }
    }
    return 0;
}
