#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>

/// <summary>
/// ��������� ��� �������, ��������� �� ���� ���������.
/// </summary>
typedef struct Vector {
	double x, y, z;
} Vector;

/// <summary>
/// ���������� ���������� ��� ������� ��������� �������� � �� ����������.
/// </summary>
Vector* vectors;
size_t numberOfVectors;

/// <summary>
/// ����� ��� ������ �� �����, � ������� ������ ���� �������� 
/// ����� ��������, � ����� ������� �������������.
/// � ������ ��������� ������� ���������� ��������� ������.
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

	/// �������� �� ��, ��� ���������� �������� �� ������ 3.
	/// ���� ������, �� ������ ������������ ������ ����������.
	/// ��������� ���������.
	if (numberOfVectors < 3)
	{
		printf("it's not interesting to check for complanarity less than 3 vectors\n");
		fclose(file);
		exit(0);
	}

	/// �������� �� ��, ��� ���������� �������� �� ��������� 50.
	/// ���� ���������, �� ��� �� �� ����� ������� ������ 50 ��������.
	if (numberOfVectors>50)
	{
		printf("that's just too much... believe me, 50 will do just fine\n");
		numberOfVectors = 50;
	}

	/// �������� ������ ��� �������� � ���������� ���������� � ���������.
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
/// ����� ��� ������ ��������� �������� � �������. 
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
/// ����� ��� ��������, �������� �� ������ �������� ������������ ����� 
/// ��������� ���������� ������������ ����.
/// </summary>
bool coplanar(Vector a, Vector b, Vector c) {
	int value = (a.x * b.y * c.z) + (a.y * b.z * c.x) + (a.z * b.x * c.y) -
		(a.z * b.y * c.x) - (a.x * b.z * c.y) - (b.x * a.y * c.z);
	return value == 0;
}

/// <summary>
/// �������, ����������� �������. ������ ����� ���������� �
/// ������� ������� � ������ � ������ ���� ���������� ������
/// ������� � �������� ��������, �������� �� �� ��������������
/// � ����� ����������.
/// </summary>
void* threadFunction(void* index) {

	size_t i = (size_t)index;

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

	return NULL;
}

/// <summary>
/// ����� ��� ����������� ��������� ������. ���������� � �����
/// ������������� ������ ��� ������, �� ������ �� ������ ������.
/// ����� ������ ����������� ��������, � ������� ���� ���������
/// ���� �������. ����������� ������ �������.
/// </summary>
void threadWork() {
	pthread_t* threads = malloc(numberOfVectors * sizeof(pthread_t));
	for (size_t i = 0; i < numberOfVectors; i++) {
		pthread_t thread;
		pthread_create(&thread, NULL, threadFunction, (void*)i);
		threads[i] = thread;
	}

	for (size_t i = 0; i < numberOfVectors; ++i) {
		pthread_join(threads[i], NULL);
	}

	free(threads);
}

/// <summary>
/// ����� �����, ���� ��������� ������� ������ �����,
/// ������ ���������� ������ ������, ������
/// ��������� ��������, �������� ��������������.
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

	/// ������������ ������, ���������� ��� ��������.
	free(vectors);
	return 0;
}