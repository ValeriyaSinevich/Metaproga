#include <iostream>
#include "TypeManip.h"

struct EmptyType {};

template<class T1, class T2>
struct TwoHolder {
    T1 value1;
    T2 value2;
};

template<int n, class A, class B>
struct Fibonacchi {
    static const int result = Fibonacchi<-1, Fibonacchi<n - 1, A, B>, Fibonacchi<n - 2, A, B>>::result;
};

template<int n>
struct Fibonacchi<n, EmptyType, EmptyType> {
    static const int result =
            Fibonacchi<-1,
            Fibonacchi<n - 1, EmptyType, EmptyType>,
                    Fibonacchi<n - 2, EmptyType, EmptyType>>::result;
};

template<class A, class B>
struct Fibonacchi<-1, A, B> {
    static const int result = A::result + B::result;
};

template<>
struct Fibonacchi<0, EmptyType, EmptyType> {
    static const int result = 1;
};

template<>
struct Fibonacchi<1, EmptyType, EmptyType> {
    static const int result = 1;
};


// Линейная иерархия для каждой ветки.
// i - текущий номер класса (в общей иерархии)
// n - сколько классов осталось сгенерировать в текущей ветке
template<int i, int n, template<int m> class T,  template <class, class> class Unit, class Root>
class CustomGenLinearHierarchy : public Unit<T<i>, CustomGenLinearHierarchy<i + 1, n - 1, T, Unit, Root> >{
public:
    typedef Unit<T<i>, CustomGenLinearHierarchy<i + 1, n - 1, T, Unit, Root>> ChildBase;
    typedef CustomGenLinearHierarchy<i + 1, n - 1, T, Unit, Root> Child;
};

template<int i, template<int m> class T, template <class, class> class Unit, class Root>
class CustomGenLinearHierarchy<i, 1, T, Unit, Root> : public Unit<T<i>, Root> {
public:
    typedef Unit<T<i>, Root> ChildBase;
};


// i - текущее количество сгенерированных классов
// n - сколько веток еще нужно сгенерировать
// k - номер текущей ветки
template <int i, int n, int k>
class CustomGenScatterHierarchy
: public CustomGenLinearHierarchy<i, Fibonacchi<k, EmptyType, EmptyType>::result, Loki::Int2Type, TwoHolder, EmptyType>
, public CustomGenScatterHierarchy<i + Fibonacchi<k, EmptyType, EmptyType>::result, n - 1, k + 1>  {
public:
    typedef CustomGenLinearHierarchy<i, Fibonacchi<k, EmptyType, EmptyType>::result, Loki::Int2Type, TwoHolder, EmptyType>
    LeftBase;
    typedef CustomGenScatterHierarchy<i + Fibonacchi<k, EmptyType, EmptyType>::result, n - 1, k + 1> RightBase;
};

// Спецификация для случая, когда осталась одна ветка
template <int i, int k>
class CustomGenScatterHierarchy<i, 1, k> :
        public CustomGenLinearHierarchy<i,
        Fibonacchi<k, EmptyType, EmptyType>::result,
        Loki::Int2Type,
        TwoHolder,
        EmptyType>
{
public:
    typedef CustomGenLinearHierarchy<i, Fibonacchi<k, EmptyType, EmptyType>::result, Loki::Int2Type, TwoHolder, EmptyType>
            LeftBase;
};

// Поисковик по ветке
// n - номер текущего элемента
// i - индекс, по которому ищем
template <class H, int i>
struct FieldLinearHelper {
    int value;
    explicit FieldLinearHelper(H& obj) {
        auto subobj = static_cast<typename H::ChildBase>(obj);
        value = FieldLinearHelper<typename H::Child, i - 1>(subobj.value2).value;
    }
};

template <class H>
struct FieldLinearHelper<H, 0> {
    int value;
    explicit FieldLinearHelper(H& obj) {
        auto subobj = static_cast<typename H::ChildBase>(obj);
        value = subobj.value1.value;
    }
};


// Поисковик по корневому классу. Ищет нужную ветку и вызывает поисковик по ветке
// k - номер текущей ветки
// i - индекс класса, по которому ищем
template <class H, int i, int k, bool left=(i < Fibonacchi<k, EmptyType, EmptyType>::result)>
struct FieldScatterHelper {
    //int value;
    //explicit FieldScatterHelper(H& obj) {}
};


template <class H, int i, int k>
struct FieldScatterHelper<H, i, k, true> {
    int value;
    explicit FieldScatterHelper(H& obj) {
        auto subobj = static_cast<typename H::LeftBase>(obj);
        value = FieldLinearHelper<typename H::LeftBase, i>(subobj).value;
    }
};

template <class H, int i, int k>
struct FieldScatterHelper<H, i, k, false> {
    int value;
    explicit FieldScatterHelper(H& obj) {
        auto subobj = static_cast<typename H::RightBase>(obj);
        value = FieldScatterHelper<typename H::RightBase, i - Fibonacchi<k, EmptyType, EmptyType>::result, k + 1>(subobj).value;
    }
};

template <int k>
using Hierarchy = CustomGenScatterHierarchy<0, k, 0>;

template <class H, int i>
using Field = FieldScatterHelper<H, i, 0>;

#define SIZE 3

/*
 * Корневой класс Hierarchy<N>, реализованный через CustomGenScatterHierarchy, одновременно наследуется от N веток типа
 * CustomGenLinearHierarchy<n>, где n - вычисленное во время компиляции i-ое число Фибоначчи, где i - номер ветки.
 * Каждая из веток линейно содержит в себе n классов типа Int2Type<k>, где k - номер класса в общей иерархии
 * (сначала первая веткаи все ее дети, потом вторая и т.д.).
 * Структура Field<Hierarchy<N>, k> по номеру k находит класс номер k в инстансе типа Hierarchy<N> и возвращает его
 * инстанс. Совпадение номера класса и его значения говорит о том, что все верно.
 *
 * Как-то без Typelistов тут все, хотя идея полностью взята из книги Alexandrescu.
 */

int main() {
    Hierarchy<SIZE> hometask;
    std::cout << "Hierarchy size: " << SIZE << std::endl;
    std::cout << "class number 0: Int2Type<" << Field<Hierarchy<3>, 0>(hometask).value << '>' << std::endl;
    std::cout << "class number 1: Int2Type<" << Field<Hierarchy<3>, 1>(hometask).value << '>' << std::endl;
    std::cout << "class number 2: Int2Type<" << Field<Hierarchy<3>, 2>(hometask).value << '>' << std::endl;
    std::cout << "class number 3: Int2Type<" << Field<Hierarchy<3>, 3>(hometask).value << '>' << std::endl;
}
