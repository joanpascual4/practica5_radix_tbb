Práctica 5 - Radix Sort implementado con TBB
Joan Pascual Alcaraz

Esta práctica consiste en la implementación del algoritmo de ordenación de 
Radix en paralelo. Para ello hacemos uso de la librería TBB y de la herramienta 
Bazel.

El algoritmo de ordenación de Radix lleva a cabo tantas iteraciones como número 
de bits tenga el mayor número del vector, y ordena los números bit a bit, dando 
como resultado final el vector ordenado.

Para llevarlo a cabo me he inspirado principalmente en uno de los ejemplos a 
nuestra disposición, 6_Example_PackingProblem, donde se usa el parallel_scan que 
he aprovechado para implementar Radix. La idea es llevar a cabo dos map distintos, 
uno donde nos devuelva un vector con 1s en las posiciones donde el bit comprobado 
del número sea 0, y otro a la inversa, con 1s cuando es 1. Después se obtienen los 
vectores acumulativos para conocer el orden de los números, y finalmente tenemos 
dos vectores reducidos: el primero con los números que tenían el bit a 0 
ordenados, y el segundo con los números con el bit a 1 también ordenados. Tan 
sólo queda unir estos dos vectores, primero el de 0s y luego el de 1s (para 
ordenarlos de menor a mayor) y se obtiene un vector intermedio resultado de la 
iteración. Al final de la última iteración se tiene el vector ordenado 
correctamente de menor a mayor.
