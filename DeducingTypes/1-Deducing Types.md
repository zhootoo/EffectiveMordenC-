# 类型推断
## Item1 理解模版类型推导
模版类型的推断方式是现代C++ **auto**这个功能的基石。

C++函数模板的定义方式：
```C++
template <typename T>
void f(ParamType param);//假设ParamType是const T&

f(expr)//调用方式
```
模版定义以*template*关键字开始，后面跟着模板参数列表。当调用模板函数时，编译器通常会根据函数的实参来确定绑定到模板参数T上的类型。例如，如果调用`f(1)`，实参类型为int，编译器推断模板实参为int，并绑定到T上。

编译期间，编译器会通过函数实参，推导出两个类型：T的类型和ParamType的类型，在这个例子中调用`f(1)`后，T推断为int，ParamType推断为const int&

一般来说模板参数T的类型就是expr的类型，但是这却并不总是成立的，还依赖ParamType的形式。分为三种情况讨论：

### case 1：ParamType是指针或者引用，但不是万能引用

推导规则：
1. 若 `expr` 的类型是引用，**忽略引用部分**
2. 用 `expr` 的剩余类型与 `ParamType` 进行模式匹配，确定 `T`

```cpp
template <typename T>
void f(T& param);

int x = 27;          // T = int,        param: int&
const int cx = x;    // T = const int,  param: const int&
const int& rx = x;   // T = const int,  param: const int&  （引用性被忽略）
```

**关键点：`const` 会被保留进 `T`**，因为调用方传入 `const` 对象时，承诺不修改它，编译器必须保持这个约束。

若 ParamType 为 `const T&`，则 `const` 已经在形参中，推导时实参的顶层 `const` 和引用性都会被忽略：

```cpp
template <typename T>
void f(const T& param);

int x = 27;          // T = int,  param: const int&
const int cx = x;    // T = int,  param: const int&  （const 无需再并入 T）
const int& rx = x;   // T = int,  param: const int&
```

指针的情况与引用类似：

```cpp
template <typename T>
void f(T* param);

int x = 27;
int* px = &x;        // T = int,        param: int*
const int* cpx = &x; // T = const int,  param: const int*  （底层 const 保留）
```

---

### case 2：ParamType 是万能引用

**万能引用（Universal Reference）** 写法为 `T&&`，其中 `T` 需要被推导（区别于普通右值引用）。它之所以"万能"，是因为既能绑定左值，也能绑定右值。

推导规则：
- 若 `expr` 是**左值**：`T` 推导为左值引用类型，再经引用折叠，`param` 最终为左值引用
- 若 `expr` 是**右值**：`T` 推导为非引用类型，`param` 为右值引用

引用折叠规则：只要含有左值引用，结果就是左值引用（`& &&` → `&`）；两个右值引用才折叠为右值引用（`&& &&` → `&&`）。

```cpp
template <typename T>
void f(T&& param);

int x = 27;
const int cx = x;
const int& rx = x;

f(x);    // 左值 → T = int&,        param: int&
f(cx);   // 左值 → T = const int&,  param: const int&
f(rx);   // 左值 → T = const int&,  param: const int&
f(27);   // 右值 → T = int,         param: int&&
```

**万能引用的意义**：配合 `std::forward<T>` 实现**完美转发**，将实参的左值/右值属性原封不动地传递给下一层函数：

```cpp
template <typename T>
void wrapper(T&& arg) {
    realFunc(std::forward<T>(arg)); // 左值传左值，右值传右值
}
```

---

### case 3：ParamType 既不是指针也不是引用（按值传递）

推导规则：忽略实参的**引用性**和**顶层 const**，剩余类型即为 `T`。

原因：按值传递意味着 `param` 是实参的一份独立拷贝，调用方的 `const` 或引用属性对这份拷贝没有约束意义。

```cpp
template <typename T>
void f(T param);

int x = 27;          // T = int,  param: int
const int cx = x;    // T = int,  param: int  （顶层 const 丢弃）
const int& rx = x;   // T = int,  param: int  （引用性和顶层 const 均丢弃）
```

注意：**底层 const 会被保留**，因为它是所指内容的属性，与拷贝无关：

```cpp
const char* str = "Hello"; // T = const char*，param: const char*
                            // const 修饰的是 char（底层），不是指针本身，保留
```

---

### 特殊情况：数组实参与函数实参

#### 数组实参

在按值传递时，数组会**退化（decay）为指针**：

```cpp
template <typename T>
void f(T param);

const char arr[] = "Hello";  // T = const char*，param: const char*
```

但在按引用传递时，数组**不退化**，`T` 推导为完整的数组类型：

```cpp
template <typename T>
void f(T& param);

const char arr[] = "Hello";  // T = const char[6]，param: const char (&)[6]
```

这一特性可用于在编译期获取数组长度：

```cpp
template <typename T, std::size_t N>
constexpr std::size_t arraySize(T (&)[N]) noexcept {
    return N;
}
```

#### 函数实参

**函数类型与函数指针的区别：**

- **函数类型**：描述函数签名本身，如 `void(int, double)`，不能赋值，不能存储
- **函数指针**：指向函数的指针，如 `void(*)(int, double)`，可以赋值、传递、存储

```cpp
void someFunc(int, double);

void (*fp)(int, double) = someFunc;   // 函数指针，存储函数地址
void (&fr)(int, double) = someFunc;   // 函数引用，绑定到函数
```

函数名在大多数上下文中会自动**退化为函数指针**（与数组退化为指针类似）：

```cpp
void (*fp)(int, double) = someFunc;   // someFunc 自动转换为指针
```

函数类型与数组类似，按值传递时退化为函数指针，按引用传递时保留函数类型：

```cpp
void someFunc(int, double);

template <typename T> void f1(T param);   // 按值
template <typename T> void f2(T& param);  // 按引用

f1(someFunc);  // T = void(*)(int, double)，退化为函数指针
f2(someFunc);  // T = void(int, double)，  保留函数类型，param: void (&)(int, double)
```

实际开发中几乎都使用函数指针，函数引用较为少见。但在泛型代码中理解两者的区别，有助于准确判断模板推导结果。
```

