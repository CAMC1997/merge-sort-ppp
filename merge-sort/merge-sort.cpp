#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <iomanip>
#include <stdlib.h>
#include <thread>

using namespace std;

void merge(vector<int> &arr, int left, int mid, int right)
{
  // Create two temporary vectors
  vector<int> leftVector(mid - left + 1);
  vector<int> rightVector(right - mid);

  // Copy data to temporary vectors
  for (int i = 0; i < leftVector.size(); i++)
  {
    leftVector[i] = arr[left + i];
  }
  for (int j = 0; j < rightVector.size(); j++)
  {
    rightVector[j] = arr[mid + 1 + j];
  }

  // Merge temporary vectors back into original array
  int leftIndex = 0, rightIndex = 0, resultIndex = left;

  while (leftIndex < leftVector.size() && rightIndex < rightVector.size())
  {
    if (leftVector[leftIndex] <= rightVector[rightIndex])
    {
      arr[resultIndex] = leftVector[leftIndex];
      leftIndex++;
    }
    else
    {
      arr[resultIndex] = rightVector[rightIndex];
      rightIndex++;
    }
    resultIndex++;
  }

  // Copy remaining elements of left vector
  while (leftIndex < leftVector.size())
  {
    arr[resultIndex] = leftVector[leftIndex];
    leftIndex++;
    resultIndex++;
  }

  // Copy remaining elements of right vector
  while (rightIndex < rightVector.size())
  {
    arr[resultIndex] = rightVector[rightIndex];
    rightIndex++;
    resultIndex++;
  }
}

void mergeSortSequential(vector<int> &arr, int leftIndex, int rightIndex)
{
  // Base case: Subarray with one or zero elements is already sorted
  if (leftIndex >= rightIndex)
  {
    return;
  }

  // Divide and conquer: Find the middle index and sort subarrays
  int middleIndex = leftIndex + (rightIndex - leftIndex) / 2;
  mergeSortSequential(arr, leftIndex, middleIndex);
  mergeSortSequential(arr, middleIndex + 1, rightIndex);

  // Merge the sorted subarrays
  merge(arr, leftIndex, middleIndex, rightIndex);
}

void mergeSortParallel(vector<int> &arr, int leftIndex, int rightIndex)
{
  // Base case: Subarray with one or zero elements is already sorted
  if (leftIndex >= rightIndex)
  {
    return;
  }

  // Divide the array into two subarrays
  int middleIndex = leftIndex + (rightIndex - leftIndex) / 2;

  // Use OpenMP task for parallel recursion
#pragma omp task
  mergeSortParallel(arr, leftIndex, middleIndex); // Sort first subarray

#pragma omp task
  mergeSortParallel(arr, middleIndex + 1, rightIndex); // Sort second subarray

  // Synchronize here to ensure that the tasks are completed before merging
#pragma omp taskwait

  // Merge the sorted subarrays
#pragma omp critical
  merge(arr, leftIndex, middleIndex, rightIndex);
}

void printMenu()
{
  cout << "\t\tOrdenamiento por mezcla";
  cout << "\n\t\t=========================" << endl;
  cout << "\nElija una opción:" << endl;
  cout << "1. Usar un arreglo predefinido" << endl;
  cout << "2. Ingresar un tamaño personalizado" << endl;
  cout << "Opción: ";
}

void measureTimeAndPerformance(vector<int> &arr, int leftIndex, int rightIndex)
{
  // Medir el tiempo de ordenamiento secuencial
  chrono::time_point<chrono::system_clock> start_seq = chrono::system_clock::now();
  mergeSortSequential(arr, leftIndex, rightIndex);
  chrono::time_point<chrono::system_clock> end_seq = chrono::system_clock::now();
  chrono::duration<double> elapsed_seconds_seq = end_seq - start_seq;

  // Medir el tiempo de ordenamiento paralelo con OpenMP tasks
  chrono::time_point<chrono::system_clock> start_par = chrono::system_clock::now();
#pragma omp parallel
#pragma omp single
  {
    mergeSortParallel(arr, leftIndex, rightIndex);
  }
  chrono::time_point<chrono::system_clock> end_par = chrono::system_clock::now();
  chrono::duration<double> elapsed_seconds_par = end_par - start_par;

  // Imprimir el tiempo de ordenamiento
  cout << "Tiempo de ordenamiento secuencial: " << fixed << setprecision(4) << elapsed_seconds_seq.count() * 1000 << " ms" << endl;
  cout << "Tiempo de ordenamiento paralelo: " << fixed << setprecision(4) << elapsed_seconds_par.count() * 1000 << " ms" << endl;
  printf("Speedup: %.2f\n", (elapsed_seconds_seq/elapsed_seconds_par));
  printf("Efficiency %.2f%%\n", 100*(elapsed_seconds_seq/elapsed_seconds_par)/std::thread::hardware_concurrency());
}

void option1()
{
  // Arreglo predefinido
  vector<int> arr = {10, 8, 3, 1, 5, 2, 7, 6, 9, 4};

  measureTimeAndPerformance(arr, 0, arr.size() - 1); // Pass vector as reference to measureTime function
}

void option2()
{
  int n;
  cout << "Ingrese el tamaño del arreglo: ";
  cin >> n;

  // Generar un arreglo aleatorio
  vector<int> arr(n);
  srand(time(NULL));
  for (int i = 0; i < arr.size(); i++)
  {
    arr[i] = rand() % 100;
  }

  measureTimeAndPerformance(arr, 0, arr.size() - 1);
}

int main()
{

  /* Imprimir el vector original
  cout << "Vector original: ";
  for (int i = 0; i < arr.size(); i++) {
    cout << arr[i] << " ";
  }
  cout << endl;
  */
  int option;
  char continuar;

  do
  {
    system("clear");
    printMenu();
    cin >> option;

    switch (option)
    {
    case 1:
      option1();
      break;
    case 2:
      option2();
      break;
    default:
      cout << "Opción inválida." << endl;
    }

    // Preguntar si desea continuar
    cout << "¿Desea elegir otra opción? (s/n): ";
    cin >> continuar;
  } while (continuar == 's');

  // wait one minute before exit
  std::this_thread::sleep_for(std::chrono::seconds(20));

  // Press a key to exit
  cin.get();

  return 0;
}