#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <iomanip>
#include <stdlib.h>
#include <thread>

using namespace std;

void merge(vector<int> &arr, int p, int q, int r)
{
  // Crear dos vectores temporales
  vector<int> L(q - p + 1);
  vector<int> R(r - q);

  // Copiar los datos en los vectores temporales
  for (int i = 0; i < L.size(); i++)
  {
    L[i] = arr[p + i];
  }
  for (int j = 0; j < R.size(); j++)
  {
    R[j] = arr[q + 1 + j];
  }

  // Fusionar los vectores temporales en el vector original
  int i = 0, j = 0, k = p;

  while (i < L.size() && j < R.size())
  {
    if (L[i] <= R[j])
    {
      arr[k] = L[i];
      i++;
    }
    else
    {
      arr[k] = R[j];
      j++;
    }
    k++;
  }

  // Copiar los elementos restantes de los vectores temporales
  while (i < L.size())
  {
    arr[k] = L[i];
    i++;
    k++;
  }
  while (j < R.size())
  {
    arr[k] = R[j];
    j++;
    k++;
  }
}

void mergeSortSequential(vector<int> &arr, int l, int r)
{
  if (l < r)
  {
    int m = l + (r - l) / 2;

    mergeSortSequential(arr, l, m);
    mergeSortSequential(arr, m + 1, r);

    merge(arr, l, m, r);
  }
}

void mergeSortParallel(vector<int> &arr, int l, int r)
{
  if (l < r)
  {
    int m = l + (r - l) / 2;

// Ordenar los subconjuntos de forma paralela
#pragma omp parallel sections
    {
#pragma omp section
      {
        mergeSortParallel(arr, l, m);
      }
#pragma omp section
      {
        mergeSortParallel(arr, m + 1, r);
      }
    }

    merge(arr, l, m, r);
  }
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

void measureTime(vector<int> &arr, int l, int r)
{
  // Medir el tiempo de ordenamiento secuencial
  chrono::time_point<chrono::system_clock> start_seq = chrono::system_clock::now();
  mergeSortSequential(arr, l, r);
  chrono::time_point<chrono::system_clock> end_seq = chrono::system_clock::now();
  chrono::duration<double> elapsed_seconds_seq = end_seq - start_seq;

  // Medir el tiempo de ordenamiento paralelo
  chrono::time_point<chrono::system_clock> start_par = chrono::system_clock::now();
  mergeSortParallel(arr, l, r);
  chrono::time_point<chrono::system_clock> end_par = chrono::system_clock::now();
  chrono::duration<double> elapsed_seconds_par = end_par - start_par;

  // Imprimir el tiempo de ordenamiento
  cout << "Tiempo de ordenamiento secuencial: " << fixed << setprecision(4) << elapsed_seconds_seq.count() * 1000 << " ms" << endl;
  cout << "Tiempo de ordenamiento paralelo: " << fixed << setprecision(4) << elapsed_seconds_par.count() * 1000 << " ms" << endl;
}

void option1()
{
  // Arreglo predefinido
  vector<int> arr = {10, 8, 3, 1, 5, 2, 7, 6, 9, 4};

  measureTime(arr, 0, arr.size() - 1); // Pass vector as reference to measureTime function
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

  measureTime(arr, 0, arr.size() - 1);
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