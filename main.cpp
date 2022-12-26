// Práctica 5 - Radix Sort con TBB
// Joan Pascual Alcaraz

#include <iostream>
#include <vector>
#include <algorithm>

#include <tbb/tbb.h>
#include "oneapi/tbb/blocked_range.h"
#include "oneapi/tbb/parallel_for.h"

using namespace std;
using namespace oneapi;

// Función que devuelve un vector con 1s en las posiciones donde
// el bit comparado con la máscara es 0 y 0s en las demás posiciones
void doMAP0(int mask,vector<int> &x, int n, vector<int> &out)
{
    tbb::parallel_for(
        tbb::blocked_range<int>(0, n),
        
        // lambda function
        [&](tbb::blocked_range<int> r) {
            for (auto i = r.begin(); i != r.end(); i++) {
                if((x[i]&mask) == 0){
                    out[i] = 1;
                }else{
                    out[i] = 0;
                }
            }
        }
    );
}

// Función que devuelve un vector con 1s en las posiciones donde
// el bit comparado con la máscara es 1 y 0s en las demás posiciones
void doMAP1(int mask,vector<int> &x, int n, vector<int> &out)
{
    tbb::parallel_for(
        tbb::blocked_range<int>(0, n),
        
        // lambda function
        [&](tbb::blocked_range<int> r) {
            for (auto i = r.begin(); i != r.end(); i++) {
                if((x[i]&mask) == 0){
                    out[i] = 0;
                }else{
                    out[i] = 1;
                }
            }
        }
    );
}

// Función que lleva a cabo un parallel_scan con un vector d 1s y 0s
int doSCAN(vector<int> &out, const vector<int> &in, int n){
    int total_sum = tbb::parallel_scan(
        tbb::blocked_range<int>(0, n), //range
        0, //id
        // lambda function
        [&](tbb::blocked_range<int> r, int sum, bool is_final_scan){        
            int tmp = sum;
            for (int i = r.begin(); i < r.end(); ++i) {
                tmp = tmp + in[i];
                if (is_final_scan)
                    out[i] = tmp;
            }
            return tmp;
        },
        [&]( int left, int right ) {
            return left + right;
        }
    );
    return total_sum;
}

// Función que, con el vector de 1s y 0s y el vector acumulativo resultado
// del scan, devuelve un vector de tamaño reducido con sólo las posiciones
// que tenían 1, de forma ordenada
void doMAPFilter(vector<int> &bolMatch, vector<int> &ixMatch, vector<int> &x, vector<int> &out, int n){
    tbb::parallel_for(
        tbb::blocked_range<int>(0, n),
        // lambda function
        [&](tbb::blocked_range<int> r) {
            for (auto i = r.begin(); i < r.end(); i++) {
                if (bolMatch[i]){
                    out[ixMatch[i]-1] = x[i];    
                }else{ 
                    int last_pos = ixMatch[ixMatch.size() - 1];
                    int count = i - ixMatch[i];
                    out[last_pos + count] = x[i];    
                }
            }
        }
    );
}

int main(){
    // Vector de enteros a ordenar (el tamaño no puede ser mayor a 6)
    static vector<int> x{7, 0, 2, 3, 2, 1};

    // Valor máximo del vector
    const int MAX_VALUE = 7;

    // Se imprime el vector inicial
    cout << endl;
    cout << "Vector inicial: ";
    for (int i: x){
        cout << i << ',';
    }
    cout << endl << endl;

    vector<int> vect = x;
    int mask = 0b001; // Máscara inicial (primer bit)
	tbb::tick_count t0 = tbb::tick_count::now();

    // Dependiendo del valor máximo, se calculan las iteraciones a realizar
    // (número de bits que hay que comprobar)
 	for(int i = 0; i < int(log2(MAX_VALUE)) + 1; i++){
        // MAP operation
        vector<int> bolMatch0(vect.size());
        vector<int> bolMatch1(vect.size());
        doMAP0(mask, vect, vect.size(), bolMatch0); // Vector con 1s si el bit es 0
        doMAP1(mask, vect, vect.size(), bolMatch1); // Vector con 1s si el bit es 1

        // SCAN
        vector<int> ixMatch0(vect.size());
        vector<int> ixMatch1(vect.size());
        int sum0 = doSCAN(ixMatch0, bolMatch0, vect.size()); // Scan para 0s
        int sum1 = doSCAN(ixMatch1, bolMatch1, vect.size()); // Scan para 1s

        // JOIN
        vector<int> filtered_results0(sum0);
        vector<int> filtered_results1(sum1);
        // Aquí se obtienen los dos vectores reducidos ordenados (uno de 0s y otro de 1s)
        doMAPFilter(bolMatch0,ixMatch0,vect,filtered_results0, vect.size());
        doMAPFilter(bolMatch1,ixMatch1,vect,filtered_results1, vect.size());

        // Se juntan los dos vectores resultantes (primero 0s, después 1s)
        filtered_results0.insert(filtered_results0.end(), filtered_results1.begin(), filtered_results1.end());

        // Se imprime cada iteración del Radix sort
        cout << "Iteración nº " << i+1 << ": ";
        for (int i: filtered_results0){
            cout << i << ',';
        }
        cout << endl;

        // Actualización del vector para la próxima iteración
        vect = filtered_results0;
        // Actualización de la máscara para el siguiente bit
        mask = mask << 1;
    }

    // Se imprime el vector ordenado
    cout << endl;
    cout << "RESULTADO DE RADIX SORT: ";
    for (int i: vect){
        cout << i << ',';
    }
    cout << endl;

 	cout << "\nTime: " << (tbb::tick_count::now()-t0).seconds() << "seconds" << endl;
 	
    return 0;
}