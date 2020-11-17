#include <omp.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

/// <summary>
/// Структура для вектора, состоящая из трех координат.
/// </summary>
typedef struct Vector {
	double x, y, z;
} Vector;

/// <summary>
/// Глобальные переменные для массива считанных векторов и их количества.
/// </summary>
Vector* vectors;
size_t numberOfVectors;

/// <summary>
/// Метод для чтения из файла, в котором должно быть записано 
/// число векторов, а далее векторы покоординатно.
/// В случае неверного формата происходит обработка ошибок.
/// </summary>
void read(char* filename) {
	FILE* file = fopen(filename, "r");
	if (!file) {
		perror(filename);
		exit(1);
	}

	if (fscanf(file, "%ld", &numberOfVectors) != 1) {
		printf("wrong number in the file\n");
		fclose(file);
		exit(1);
	}

	/// Проверка на то, что количество векторов не меньше 3.
	/// Если меньше, то искать компланарные тройки бесполезно.
	/// Завершаем программу.
	if (numberOfVectors < 3)
	{
		printf("it's not interesting to check for complanarity less than 3 vectors\n");
		fclose(file);
		exit(0);
	}

	/// Проверка на то, что количество векторов не превышает 50.
	/// Если превышает, мы все же не будем считать больше 50 векторов.
	if (numberOfVectors > 50)
	{
		printf("that's just too much... believe me, 50 will do just fine\n");
		numberOfVectors = 50;
	}

	/// Выделяем память для векторов в глобальной переменной и считываем.
	vectors = (Vector*)calloc(numberOfVectors, sizeof(Vector));
	for (size_t i = 0; i < numberOfVectors; i++) {

		if (fscanf(file, "%lf", &vectors[i].x) != 1) {
			numberOfVectors = i;
			printf("something in the file is wrong\n");
			break;
		}
		if (fscanf(file, "%lf", &vectors[i].y) != 1) {
			numberOfVectors = i;
			printf("something in the file is wrong\n");
			break;
		}
		if (fscanf(file, "%lf", &vectors[i].z) != 1) {
			numberOfVectors = i;
			printf("something in the file is wrong\n");
			break;
		}
	}

	fclose(file);
}

/// <summary>
/// Метод для печати считанных векторов в консоль. 
/// </summary>
void printVectors(Vector* vectors) {
	printf("your vectors:\n");

	for (size_t i = 0; i < numberOfVectors; i++)
	{
		printf("{%g, %g, %g}\n", vectors[i].x, vectors[i].y, vectors[i].z);
	}

	printf("\n");
}

/// <summary>
/// Метод для проверки, является ли тройка векторов компланарной через 
/// равенство смешанного произведения нулю.
/// </summary>
bool coplanar(Vector a, Vector b, Vector c) {
	int value = (a.x * b.y * c.z) + (a.y * b.z * c.x) + (a.z * b.x * c.y) -
		(a.z * b.y * c.x) - (a.x * b.z * c.y) - (b.x * a.y * c.z);
	return value == 0;
}

/// <summary>
/// Функция, выполняемая потоком. Каждый поток прикреплен к
/// первому вектору в тройке и внутри него происходит подбор
/// второго и третьего векторов, проверка их на компланарность
/// и вывод результата.
/// </summary>
void threadFunction(size_t i) {

	for (size_t j = i + 1; j < numberOfVectors; ++j) {
		for (size_t k = j + 1; k < numberOfVectors; ++k) {
			if (coplanar(vectors[i], vectors[j], vectors[k])) {
				printf("coplanar are {%g, %g, %g}, {%g, %g, %g}, {%g, %g, %g}\n",
					vectors[i].x, vectors[i].y, vectors[i].z,
					vectors[j].x, vectors[j].y, vectors[j].z,
					vectors[k].x, vectors[k].y, vectors[k].z);
			}
			else { printf("not coplanar\n"); }
		}
	}
}

/// <summary>
/// Метод для организации потоковой работы. Потоки создаются
/// с помощью директивы параллельного цикла, в котором вызывается
/// функция. Программа сама выбирает оптимальное количество потоков
/// для распараллеливания цикла.
/// </summary>
void threadWork() {

#pragma omp parallel for
	for (size_t i = 0; i < numberOfVectors; i++) {
		threadFunction(i);
	}
}

/// <summary>
/// Точка входа, если аргументы входной строки верны,
/// отсюда вызываются методы чтения, вывода
/// считанных векторов, проверки компланарности.
/// </summary>
int main(int argc, char** argv) {

	if (argc != 2) {
		printf("wrong number of args %d\n", argc);
		return 1;
	}

	char* input = argv[1];

	printf("reading...\n");
	read(input);

	printVectors(vectors);

	threadWork();

	printf("the end\n");

	/// Освобождение памяти, выделенной для векторов.
	free(vectors);
	return 0;
}